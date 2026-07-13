#include "driver.h"

// Read callback
VOID
VirtualDriverEvtIoRead(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t     Length
)
{
    NTSTATUS status;
    PVOID buffer;
    size_t bufferSize;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;

    UNREFERENCED_PARAMETER(Length);

    device = WdfIoQueueGetDevice(Queue);
    deviceContext = DeviceGetContext(device);

    // Get the read buffer
    status = WdfRequestRetrieveOutputBuffer(
        Request,
        sizeof(DEVICE_INFO),
        &buffer,
        &bufferSize
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfRequestRetrieveOutputBuffer failed 0x%x\n", status));
        WdfRequestComplete(Request, status);
        return;
    }

    // Fill buffer with device info
    PDEVICE_INFO info = (PDEVICE_INFO)buffer;
    info->DeviceId = deviceContext->DeviceId;
    info->ReadCount = deviceContext->ReadCount;
    info->WriteCount = deviceContext->WriteCount;
    RtlStringCchCopyA(info->DeviceName, 32, deviceContext->DeviceName);

    // Update read count
    deviceContext->ReadCount++;

    // Complete the request
    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, sizeof(DEVICE_INFO));

    KdPrint(("VirtualDriver: Read completed, count: %d\n", deviceContext->ReadCount));
}

// Write callback
VOID
VirtualDriverEvtIoWrite(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t     Length
)
{
    NTSTATUS status;
    PVOID buffer;
    size_t bufferSize;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;

    device = WdfIoQueueGetDevice(Queue);
    deviceContext = DeviceGetContext(device);

    // Get the write buffer
    status = WdfRequestRetrieveInputBuffer(
        Request,
        sizeof(DEVICE_INFO),
        &buffer,
        &bufferSize
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
        WdfRequestComplete(Request, status);
        return;
    }

    // Update device info from buffer
    PDEVICE_INFO info = (PDEVICE_INFO)buffer;
    deviceContext->DeviceId = info->DeviceId;
    RtlStringCchCopyA(deviceContext->DeviceName, 32, info->DeviceName);

    // Update write count
    deviceContext->WriteCount++;

    // Complete the request
    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, Length);

    KdPrint(("VirtualDriver: Write completed, count: %d\n", deviceContext->WriteCount));
}

// Device control callback (IOCTL handling)
VOID
VirtualDriverEvtIoDeviceControl(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t     OutputBufferLength,
    _In_ size_t     InputBufferLength,
    _In_ ULONG      IoControlCode
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PVOID inputBuffer = NULL;
    PVOID outputBuffer = NULL;
    size_t inputBufferSize = 0;
    size_t outputBufferSize = 0;
    WDFDEVICE device;
    PDEVICE_CONTEXT deviceContext;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    deviceContext = DeviceGetContext(device);

    switch (IoControlCode) {
    case IOCTL_VIRTUALDRIVER_GET_INFO:
        // Get device info
        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(DEVICE_INFO),
            &outputBuffer,
            &outputBufferSize
        );

        if (NT_SUCCESS(status)) {
            PDEVICE_INFO info = (PDEVICE_INFO)outputBuffer;
            info->DeviceId = deviceContext->DeviceId;
            info->ReadCount = deviceContext->ReadCount;
            info->WriteCount = deviceContext->WriteCount;
            RtlStringCchCopyA(info->DeviceName, 32, deviceContext->DeviceName);
            WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, sizeof(DEVICE_INFO));
        } else {
            WdfRequestComplete(Request, status);
        }
        break;

    case IOCTL_VIRTUALDRIVER_SET_INFO:
        // Set device info
        status = WdfRequestRetrieveInputBuffer(
            Request,
            sizeof(DEVICE_INFO),
            &inputBuffer,
            &inputBufferSize
        );

        if (NT_SUCCESS(status)) {
            PDEVICE_INFO info = (PDEVICE_INFO)inputBuffer;
            deviceContext->DeviceId = info->DeviceId;
            RtlStringCchCopyA(deviceContext->DeviceName, 32, info->DeviceName);
            WdfRequestComplete(Request, STATUS_SUCCESS);
        } else {
            WdfRequestComplete(Request, status);
        }
        break;

    case IOCTL_VIRTUALDRIVER_RESET_STATS:
        // Reset statistics
        deviceContext->ReadCount = 0;
        deviceContext->WriteCount = 0;
        WdfRequestComplete(Request, STATUS_SUCCESS);
        KdPrint(("VirtualDriver: Statistics reset via IOCTL\n"));
        break;

    case IOCTL_VIRTUALDRIVER_PING:
        // Ping test
        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(ULONG),
            &outputBuffer,
            &outputBufferSize
        );

        if (NT_SUCCESS(status)) {
            *(PULONG)outputBuffer = 0xDEADBEEF;
            WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, sizeof(ULONG));
        } else {
            WdfRequestComplete(Request, status);
        }
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        WdfRequestComplete(Request, status);
        KdPrint(("VirtualDriver: Invalid IOCTL 0x%x\n", IoControlCode));
        break;
    }
}
