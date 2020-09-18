#ifdef __linux__
#include "platform.h"
#include <fcntl.h>
#include <linux/videodev2.h>
#include <unistd.h>



auto list_video_devices() -> std::vector<Device> {
	struct video_device* device = ::video_device_alloc();
	int idx = 0;

	if (::ioctl)
}

#endif