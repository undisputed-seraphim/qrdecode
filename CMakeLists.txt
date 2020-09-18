cmake_minimum_required(VERSION 3.13)
project(qrdecode)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS ON)
set(OpenCV_STATIC ON)
find_package(Boost 1.70 REQUIRED COMPONENTS program_options)
find_package(OpenCV REQUIRED COMPONENTS core imgproc objdetect video videoio)

file(GLOB src "${CMAKE_SOURCE_DIR}/src/*.*")
add_executable(qrdecode ${src})
target_link_libraries(qrdecode PUBLIC
	Boost::headers
	Boost::program_options
	opencv_core
	opencv_imgproc
	opencv_objdetect
	opencv_video
	opencv_videoio
)
