#include "driver.h"

// Device context structure
typedef struct _DEVICE_CONTEXT {
    ULONG DeviceId;
    ULONG ReadCount;
    ULONG WriteCount;
    CHAR DeviceName[32];
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

// Create device context
NTSTATUS
VirtualDriverDeviceContextCreate(
    _In_ WDFDEVICE Device
)
{
    PDEVICE_CONTEXT deviceContext;
    NTSTATUS status;

    deviceContext = DeviceGetContext(Device);
    
    // Initialize device context
    deviceContext->DeviceId = 0;
    deviceContext->ReadCount = 0;
    deviceContext->WriteCount = 0;
    RtlStringCchCopyA(deviceContext->DeviceName, 32, "VirtualDriver");

    KdPrint(("VirtualDriver: Device context created\n"));
    return STATUS_SUCCESS;
}

// Get device information
NTSTATUS
VirtualDriverGetDeviceInfo(
    _In_ WDFDEVICE Device,
    _Out_ PDEVICE_INFO Info
)
{
    PDEVICE_CONTEXT deviceContext;

    deviceContext = DeviceGetContext(Device);
    
    Info->DeviceId = deviceContext->DeviceId;
    Info->ReadCount = deviceContext->ReadCount;
    Info->WriteCount = deviceContext->WriteCount;
    RtlStringCchCopyA(Info->DeviceName, 32, deviceContext->DeviceName);

    return STATUS_SUCCESS;
}

// Set device information
NTSTATUS
VirtualDriverSetDeviceInfo(
    _In_ WDFDEVICE Device,
    _In_ PDEVICE_INFO Info
)
{
    PDEVICE_CONTEXT deviceContext;

    deviceContext = DeviceGetContext(Device);
    
    deviceContext->DeviceId = Info->DeviceId;
    RtlStringCchCopyA(deviceContext->DeviceName, 32, Info->DeviceName);

    KdPrint(("VirtualDriver: Device info updated\n"));
    return STATUS_SUCCESS;
}

// Reset device statistics
NTSTATUS
VirtualDriverResetDeviceStats(
    _In_ WDFDEVICE Device
)
{
    PDEVICE_CONTEXT deviceContext;

    deviceContext = DeviceGetContext(Device);
    
    deviceContext->ReadCount = 0;
    deviceContext->WriteCount = 0;

    KdPrint(("VirtualDriver: Device statistics reset\n"));
    return STATUS_SUCCESS;
}
