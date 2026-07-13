#include "driver.h"

// Driver entry point
extern "C"
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    // Initialize driver config to call the driver's EvtDeviceAdd
    WDF_DRIVER_CONFIG_INIT(&config, VirtualDriverEvtDeviceAdd);

    // Set the driver's unload routine
    config.EvtDriverUnload = VirtualDriverEvtDriverUnload;

    // Create the WDF driver object
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfDriverCreate failed 0x%x\n", status));
        return status;
    }

    KdPrint(("VirtualDriver: Driver loaded successfully\n"));
    return STATUS_SUCCESS;
}

// Driver unload routine
VOID
VirtualDriverEvtDriverUnload(
    _In_ WDFDRIVER Driver
)
{
    UNREFERENCED_PARAMETER(Driver);
    KdPrint(("VirtualDriver: Driver unloaded\n"));
}

// Device add routine - called when a device instance is added
NTSTATUS
VirtualDriverEvtDeviceAdd(
    _In_ WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    UNREFERENCED_PARAMETER(Driver);
    NTSTATUS status;
    WDFDEVICE device;
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
    WDF_IO_QUEUE_CONFIG queueConfig;

    // Initialize PnP and Power callbacks
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = VirtualDriverEvtDevicePrepareHardware;
    pnpPowerCallbacks.EvtDeviceReleaseHardware = VirtualDriverEvtDeviceReleaseHardware;
    pnpPowerCallbacks.EvtDeviceD0Entry = VirtualDriverEvtDeviceD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit = VirtualDriverEvtDeviceD0Exit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    // Set device attributes
    WDF_OBJECT_ATTRIBUTES_INIT(&deviceAttributes);
    deviceAttributes.SynchronizationScope = WdfSynchronizationScopeNone;

    // Create the device object
    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfDeviceCreate failed 0x%x\n", status));
        return status;
    }

    // Create the default I/O queue
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchParallel
    );

    queueConfig.EvtIoRead = VirtualDriverEvtIoRead;
    queueConfig.EvtIoWrite = VirtualDriverEvtIoWrite;
    queueConfig.EvtIoDeviceControl = VirtualDriverEvtIoDeviceControl;

    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    // Create a device interface for communication
    status = WdfDeviceCreateDeviceInterface(
        device,
        &GUID_DEVINTERFACE_VIRTUALDRIVER,
        NULL
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("VirtualDriver: WdfDeviceCreateDeviceInterface failed 0x%x\n", status));
        return status;
    }

    KdPrint(("VirtualDriver: Device created successfully\n"));
    return STATUS_SUCCESS;
}

// Prepare hardware callback
NTSTATUS
VirtualDriverEvtDevicePrepareHardware(
    _In_ WDFDEVICE Device,
    _In_ WDFCMRESLIST ResourcesRaw,
    _In_ WDFCMRESLIST ResourcesTranslated
)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    KdPrint(("VirtualDriver: Device prepared\n"));
    return STATUS_SUCCESS;
}

// Release hardware callback
NTSTATUS
VirtualDriverEvtDeviceReleaseHardware(
    _In_ WDFDEVICE Device,
    _In_ WDFCMRESLIST ResourcesTranslated
)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    KdPrint(("VirtualDriver: Hardware released\n"));
    return STATUS_SUCCESS;
}

// D0 entry callback (device entering working state)
NTSTATUS
VirtualDriverEvtDeviceD0Entry(
    _In_ WDFDEVICE Device,
    _In_ WDF_POWER_DEVICE_STATE PreviousState
)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(PreviousState);

    KdPrint(("VirtualDriver: Device entering D0 state\n"));
    return STATUS_SUCCESS;
}

// D0 exit callback (device leaving working state)
NTSTATUS
VirtualDriverEvtDeviceD0Exit(
    _In_ WDFDEVICE Device,
    _In_ WDF_POWER_DEVICE_STATE TargetState
)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(TargetState);

    KdPrint(("VirtualDriver: Device leaving D0 state\n"));
    return STATUS_SUCCESS;
}
