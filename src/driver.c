#include <wdm.h>

#include "common/utils.h"

#include "common/utils.h"
#include "driver/ioctl.h"
#include "driver/utils.h"

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
  UNREFERENCED_PARAMETER(DriverObject);

  DoRun = 0;

  DeleteExposedDevice(DriverObject);

  DbgPrint("Driver unloaded!\n");
}

NTSTATUS DispatchCreateClose(DEVICE_OBJECT* DeviceObject, IRP* Irp)
{
  NTSTATUS Status = STATUS_SUCCESS;

  UNREFERENCED_PARAMETER(DeviceObject);

  switch (Irp->Tail.Overlay.CurrentStackLocation->MajorFunction)
  {
    case IRP_MJ_CREATE:
      DbgPrint("Device opened\n");
      break;

    case IRP_MJ_CLOSE:
      DbgPrint("Device closed\n");
      break;

    default:
      Status = STATUS_UNSUCCESSFUL;
      break;
  }

  Irp->IoStatus.Status      = Status;
  Irp->IoStatus.Information = 0;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return Status;
}

NTSTATUS DispatchDeviceControl(DEVICE_OBJECT* DeviceObject, IRP* Irp)
{
  IO_STACK_LOCATION* csl          = NULL;
  NTSTATUS           Status       = STATUS_SUCCESS;
  uint32_t           WrittenBytes = 0;
  void*              Buffer       = NULL;

  DbgPrint("Entering " __FUNCTION__ "\n");

  UNREFERENCED_PARAMETER(DeviceObject);

  csl    = Irp->Tail.Overlay.CurrentStackLocation;
  Buffer = Irp->AssociatedIrp.SystemBuffer;

  DbgPrint("IOCTL: 0x%08x\n", csl->Parameters.DeviceIoControl.IoControlCode);

  switch (csl->Parameters.DeviceIoControl.IoControlCode)
  {
    case IOCTL_INC_RET_ADDR:
      Status = IoctlIncRetAddr();
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlIncRetAddr failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_SKIP_NEXT_FRAME:
      Status = IoctlSkipNextFrame();
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlSkipNextFrame failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_WRITE_CURRENT_SHADOW_STACK:
      Status = IoctlWriteShadowStack(
        Buffer,
        csl->Parameters.DeviceIoControl.InputBufferLength);
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlWriteShadowStack failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_WRITE_CR4:
      Status = IoctlWriteCr4(
        Buffer,
        csl->Parameters.DeviceIoControl.InputBufferLength);
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlWriteCr4 failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_WRITE_MSR:
      Status = IoctlWriteMsr(
        Buffer,
        csl->Parameters.DeviceIoControl.InputBufferLength);
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlWriteMsr failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_WRITE_CURRENT_SHADOW_STACK_PTE:
      Status = IoctlWriteShadowStackPte(
        Buffer,
        csl->Parameters.DeviceIoControl.InputBufferLength);
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlWriteShadowStackPte failed\n");
        goto EXIT;
      }
      break;

    case IOCTL_DIV_INTEGER:
      if (sizeof(IoctlDivInteger_t) >
          csl->Parameters.DeviceIoControl.OutputBufferLength)
      {
        DbgPrint("IoctlDivInteger output buffer to small\n");
        Status = STATUS_BUFFER_TOO_SMALL;
        goto EXIT;
      }
      Status = IoctlDivInteger(
        Buffer,
        csl->Parameters.DeviceIoControl.InputBufferLength);
      if (!NT_SUCCESS(Status))
      {
        DbgPrint("IoctlDivInteger failed\n");
        goto EXIT;
      }
      WrittenBytes = csl->Parameters.DeviceIoControl.OutputBufferLength;
      break;

    default:
      Status = STATUS_UNSUCCESSFUL;
      break;
  }

EXIT:
  Irp->IoStatus.Information = WrittenBytes;
  Irp->IoStatus.Status      = Status;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  DbgPrint("Leaving " __FUNCTION__ "\n");

  return Status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
  NTSTATUS Status = STATUS_SUCCESS;

  UNREFERENCED_PARAMETER(RegistryPath);

  Status = DbgSetDebugFilterState(0x100, 3, TRUE);
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to set debug filter state\n");
    goto EXIT;
  }

  DbgPrint("Driver loading...\n");

  DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
  DriverObject->DriverUnload                         = DriverUnload;

  Status = CreateExposedDevice(DriverObject);
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("CreateExposedDevice failed\n");
    goto EXIT;
  }

EXIT:
  return Status;
}
