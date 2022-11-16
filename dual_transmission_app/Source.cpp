// dual_transmission.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>

#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <guiddef.h>
#include <cstddef>
#include <system_error>

#include <hidclass.h>
#include <tchar.h>

#include "Public.h"

void error(DWORD err)
{
	WCHAR buf[0x200];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, 0x200, NULL);
	wprintf(L"%x: %s\n", err, buf);
}

int main() {

	HDEVINFO devInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_dualtransmissionkmdf2, NULL, NULL, DIGCF_DEVICEINTERFACE);
	
	if (devInfo == INVALID_HANDLE_VALUE) {
		error(GetLastError());
		goto EXIT;
	}

	int DeviceIndex = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	if (!SetupDiEnumDeviceInfo(devInfo, DeviceIndex++, &DeviceInfoData)) {
		error(GetLastError());
		goto EXIT;
	}
		//DeviceInfoData.DevInst
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVINFO_DATA));
	DeviceInterfaceData.cbSize = sizeof(SP_DEVINFO_DATA);

	SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;
	ZeroMemory(&DeviceInterfaceDetailData, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA));
	DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	PDWORD RequiredSize = (PDWORD)(offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath) + sizeof(TCHAR));

	SetupDiGetDeviceInterfaceDetail(devInfo, &DeviceInterfaceData, &DeviceInterfaceDetailData, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA), RequiredSize, &DeviceInfoData);
	
	LPCWSTR name = DeviceInterfaceDetailData.DevicePath;
	HANDLE hDriver = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hDriver == INVALID_HANDLE_VALUE) 
	{
		error(GetLastError());
		goto EXIT;
	}
	
	DWORD dwBytesReturned = 0;
	DWORD nDataFromDriver = 0;

	DeviceIoControl(hDriver, IOCTL_HID_GET_INPUT_REPORT, NULL, 0, &nDataFromDriver, sizeof(DWORD), &dwBytesReturned, NULL);

	printf("Data from driver: %04X\n", nDataFromDriver);

	CloseHandle(hDriver);

EXIT:
	system("pause");
	return 0;

}