#include <boost/program_options.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include "platform.h"

namespace po = boost::program_options;

constexpr char BELL = '\a';

void print_video_devices() {
	auto devices = list_video_devices();
	for (const auto& dev : devices) {
		std::cout 
			<< "Device #" << dev.id
			<< ": " << dev.name
			<< "\r\n\t" << dev.path
			<< std::endl;
	}
}

void watch_loop(cv::VideoCapture& vc, int fd, bool stop_on_blank = true) {
	auto img = cv::Mat();
	auto output = std::string();
	auto previous = std::string();
	auto qr = cv::QRCodeDetector();
	while (vc.read(img)) {
		if (img.empty()) {
			if (stop_on_blank) {
				break;
			}
			else {
				continue;
			}
		}
		output = qr.detectAndDecode(img);
		if (output.empty()) {
			continue;
		}
		if (output != previous) {
			std::cout << BELL << std::endl;
			if (fd > 0) {
				write_to_fd(fd, output);
				write_to_fd(fd, "\n");
			} else {
				std::cout << output << std::endl;
			}
			previous = std::move(output);
		}
	}
}

int main(int argc, char* argv[]) {
	auto opts = po::options_description("qrdecode options");
	int fd = -1;
	int input_idx = 0;
	auto src = std::string();
	auto url = std::string();
	opts.add_options()
		("fd,f", po::value<>(&fd), "Output to file descriptor. Without this flag, this program outputs to stdout.\n")
		("help,h", "Show this help message and exit.\n")
		("list,l", "List available video input devices on this machine.\n")
		("input,i", po::value<>(&input_idx), "Use input camera at index (Use list option to show all available).\nDefaults to index 0.\n")
		("source,s", po::value<>(&src), "Use video or image file at path.\nThis will read the video or image as fast as the machine allows until no more frames are received.\n")
		("url,u", po::value<>(&url), "Listen on a URL for a video stream continuously.\nThis will NOT terminate the program even if empty frames are reached.\n");
	auto vars = po::variables_map();
	po::store(po::parse_command_line(argc, argv, opts), vars);
	po::notify(vars);
	if (vars.count("help")) {
		std::cout << "qrdecode is a command-line QR code viewer and decoder based on OpenCV.\n" << std::endl;
		std::cout << opts << std::endl;
		return EXIT_FAILURE;
	}
	if (vars.count("list")) {
		print_video_devices();
		return EXIT_SUCCESS;
	}
	if (vars.count("source")) {
		std::cout << "Reading from " << src << std::endl;
		auto vc = cv::VideoCapture(src);
		watch_loop(vc, fd);
	}
	if (vars.count("input")) {
		std::cout << "Using camera #" << input_idx << std::endl;
		auto vc = cv::VideoCapture(input_idx);
		watch_loop(vc, fd);
	}
	if (vars.count("url")) {
		std::cout << "Listening on " << url << std::endl;
		auto vc = cv::VideoCapture(url);
		watch_loop(vc, fd, false);
	}
	return EXIT_SUCCESS;
}