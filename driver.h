#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <ntddk.h>
#include <wdf.h>
#include <ntstrsafe.h>

// Device GUID
// {A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
DEFINE_GUID(GUID_DEVINTERFACE_VIRTUALDRIVER, 
    0xA1B2C3D4, 0xE5F6, 0x7890, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90);

// Device information structure
typedef struct _DEVICE_INFO {
    ULONG DeviceId;
    ULONG ReadCount;
    ULONG WriteCount;
    CHAR DeviceName[32];
} DEVICE_INFO, *PDEVICE_INFO;

// IOCTL definitions
#define IOCTL_VIRTUALDRIVER_GET_INFO \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIRTUALDRIVER_SET_INFO \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIRTUALDRIVER_RESET_STATS \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIRTUALDRIVER_PING \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Driver entry point
extern "C"
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);

// Driver unload routine
EVT_WDF_DRIVER_UNLOAD VirtualDriverEvtDriverUnload;

// Device add routine
EVT_WDF_DEVICE_PREPARE_HARDWARE VirtualDriverEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE VirtualDriverEvtDeviceReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY VirtualDriverEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT VirtualDriverEvtDeviceD0Exit;

EVT_WDF_DEVICE_ADD VirtualDriverEvtDeviceAdd;

// I/O queue callbacks
EVT_WDF_IO_QUEUE_IO_READ VirtualDriverEvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE VirtualDriverEvtIoWrite;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL VirtualDriverEvtIoDeviceControl;

// Device context functions
NTSTATUS
VirtualDriverDeviceContextCreate(
    _In_ WDFDEVICE Device
);

NTSTATUS
VirtualDriverGetDeviceInfo(
    _In_ WDFDEVICE Device,
    _Out_ PDEVICE_INFO Info
);

NTSTATUS
VirtualDriverSetDeviceInfo(
    _In_ WDFDEVICE Device,
    _In_ PDEVICE_INFO Info
);

NTSTATUS
VirtualDriverResetDeviceStats(
    _In_ WDFDEVICE Device
);

#endif // _DRIVER_H_
