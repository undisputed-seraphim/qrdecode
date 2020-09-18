#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "platform.h"
#include <Windows.h>
#include <dshow.h>
#include <io.h>

#pragma comment(lib, "strmiids.lib")

std::string ConvertWCSToMBS(const wchar_t* pstr, int wslen) {
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);
	std::string str(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, str.data(), len, NULL, NULL);
	return str;
}

std::string ConvertBSTRToMBS(BSTR bstr) {
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

auto getDevicesList(const GUID deviceClass) -> std::vector<Device> {
	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr)) {
		return {};
	}
	// Create the System Device Enumerator
	ICreateDevEnum* pDevEnum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	// If succeeded, create an enumerator for the category
	IEnumMoniker* pEnum = NULL;
	if (SUCCEEDED(hr)) {
		hr = pDevEnum->CreateClassEnumerator(deviceClass, &pEnum, 0);
		if (hr == S_FALSE) {
			hr = VFW_E_NOT_FOUND;
		}
		pDevEnum->Release();
	}

	auto device_list = std::vector<Device>();
	int index = 0;
	if (SUCCEEDED(hr)) {
		IMoniker* pMoniker = NULL;
		while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
			IPropertyBag* pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
			if (FAILED(hr)) {
				pMoniker->Release();
				continue;
			}
			Device device;
			VARIANT var;
			VariantInit(&var);
			// Read FriendlyName or Description
			hr = pPropBag->Read(L"Description", &var, 0);
			if (FAILED(hr)) {
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
			}
			if (FAILED(hr)) {
				VariantClear(&var);
				continue;
			} else {
				device.name = ConvertBSTRToMBS(var.bstrVal);
			}
			VariantClear(&var);
			hr = pPropBag->Read(L"DevicePath", &var, 0);
			if (FAILED(hr)) {
				VariantClear(&var);
				continue;
			} else {
				device.path = ConvertBSTRToMBS(var.bstrVal);
			}
			device.id = index;
			device_list.push_back(device);
			index++;
		}
		pEnum->Release();
	}
	CoUninitialize();
	return device_list;
}

auto list_video_devices() -> std::vector<Device> {
	return getDevicesList(CLSID_VideoInputDeviceCategory);
}

size_t write_to_fd(int fd, const std::string& data) {
	return _write(fd, data.data(), data.size());
}

#endif // _WIN32