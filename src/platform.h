#pragma once

#include <string>
#include <vector>

struct Device {
	int id;
	std::string path;
	std::string name;
};

auto list_video_devices()->std::vector<Device>;

size_t write_to_fd(int fd, const std::string& data);