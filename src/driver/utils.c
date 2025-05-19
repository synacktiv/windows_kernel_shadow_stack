#include "driver/utils.h"

#include <ntstrsafe.h>

#define KERNEL_ADDR_MASK       (0xfffffffffff00000)
#define MASK_KERNEL_ADDR(addr) ((addr)&KERNEL_ADDR_MASK)

// ===========================================================================
// Offsets 24h2 - KTHREAD
// ===========================================================================

static uint32_t KernelShadowStack_offset        = 0x408;
static uint32_t KernelShadowStackBase_offset    = 0x418;
static uint32_t KernelShadowStackInitial_offset = 0x410;
static uint32_t KernelShadowStackLimit_offset   = 0x420;
static uint32_t MiscFlags_offset                = 0x074;
static uint32_t Process_offset                  = 0x220;
static uint32_t ThreadListEntry_offset          = 0x2f8;

// ===========================================================================
// Offsets 24h2 - EPROCESS
// ===========================================================================

static uint32_t ImageFileName_offset  = 0x338;
static uint32_t ThreadListHead_offset = 0x30;
// static uint32_t ThreadListHead_offset = 0x370;

// ===========================================================================
// Extern declarations
// ===========================================================================

extern void setRsp(void*);

// ===========================================================================
// Function definitions
// ===========================================================================

NTSTATUS CreateExposedDevice(DRIVER_OBJECT* DriverObject)
{
  NTSTATUS Status = STATUS_INVALID_PARAMETER;

  DbgPrintEnter();

  if (!DriverObject)
  {
    DbgPrint("Invalid drive object\n");
    goto EXIT;
  }

  Status = CreateExposedDeviceEx(
    DriverObject,
    DRIVER_DEVICE_NAME,
    DOS_DEVICE_NAME);
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("CreateExposedDeviceEx failed\n");
    goto EXIT;
  }

  Status = STATUS_SUCCESS;

EXIT:
  DbgPrintLeave();

  return Status;
}

NTSTATUS CreateExposedDeviceEx(
  DRIVER_OBJECT* DriverObject,
  wchar_t*       DeviceName,
  wchar_t*       SymlinkName)
{
  DEVICE_OBJECT* DeviceObject = NULL;
  NTSTATUS       Status       = STATUS_INVALID_PARAMETER;
  UNICODE_STRING uDeviceName  = { 0 };
  UNICODE_STRING uSymlinkName = { 0 };

  DbgPrintEnter();

  if (!DriverObject)
  {
    DbgPrint("Invalid drive object\n");
    goto EXIT;
  }

  if (!DeviceName)
  {
    DbgPrint("Invalid device name\n");
    goto EXIT;
  }

  if (!SymlinkName)
  {
    DbgPrint("Invalid symbolic link name\n");
    goto EXIT;
  }

  RtlInitUnicodeString(&uDeviceName, DeviceName);
  RtlInitUnicodeString(&uSymlinkName, SymlinkName);

  Status = IoCreateDevice(
    DriverObject,
    0,
    &uDeviceName,
    FILE_DEVICE_UNKNOWN,
    0,
    FALSE,
    &DeviceObject);
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to create device object\n");
    goto EXIT;
  }

  Status = IoCreateSymbolicLink(&uSymlinkName, &uDeviceName);
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to create symbolic link\n");
    goto EXIT;
  }

  Status = STATUS_SUCCESS;

EXIT:
  if (!NT_SUCCESS(Status) && DeviceObject)
  {
    IoDeleteDevice(DeviceObject);
  }

  DbgPrintLeave();

  return Status;
}

void DeleteExposedDevice(DRIVER_OBJECT* DriverObject)
{
  DbgPrintEnter();

  if (!DriverObject)
  {
    DbgPrint("Invalid driver object\n");
    goto EXIT;
  }

  DeleteExposedDeviceEx(DriverObject, DOS_DEVICE_NAME);

EXIT:
  DbgPrintLeave();
}

void DeleteExposedDeviceEx(DRIVER_OBJECT* DriverObject, wchar_t* SymlinkName)
{
  UNICODE_STRING uSymlinkName = { 0 };

  DbgPrintEnter();

  if (!DriverObject)
  {
    DbgPrint("Invalid driver object\n");
    goto EXIT;
  }

  if (!SymlinkName)
  {
    DbgPrint("Invalid symbolic link name\n");
    goto EXIT;
  }

  RtlInitUnicodeString(&uSymlinkName, SymlinkName);

  IoDeleteSymbolicLink(&uSymlinkName);

  if (DriverObject->DeviceObject)
  {
    IoDeleteDevice(DriverObject->DeviceObject);
  }

EXIT:
  DbgPrintLeave();
}

int32_t DivInteger(int32_t Dividend, int32_t Divisor)
{
  int32_t Quotient = 0;

  DbgPrintEnter();

  Quotient = Dividend / Divisor;

  DbgPrintLeave();

  return Quotient;
}

void IncRetAddr()
{
  uintptr_t* placeholder = NULL;

  DbgPrintEnter();

  placeholder = (uintptr_t*)&placeholder;
  DbgPrint("Legitimate return address:  %p\n", placeholder[3]);
  placeholder[3]++;
  DbgPrint("Incremented return address: %p\n", placeholder[3]);

  DbgPrintLeave();
}

void* GetCurrentThreadKernelShadowStack()
{
  return GetThreadKernelShadowStack(KeGetCurrentThread());
}

void* GetCurrentThreadKernelShadowStackBase()
{
  return GetThreadKernelShadowStackBase(KeGetCurrentThread());
}

void* GetCurrentThreadKernelShadowStackInitial()
{
  return GetThreadKernelShadowStackInitial(KeGetCurrentThread());
}

void* GetCurrentThreadKernelShadowStackLimit()
{
  return GetThreadKernelShadowStackLimit(KeGetCurrentThread());
}

MiscFlags_t GetCurrentThreadMiscFlags()
{
  return GetThreadMiscFlags(KeGetCurrentThread());
}

void* GetKernelBase()
{
  return GetModuleBase((void*)ExAllocatePool2);
}

void* GetMmShadowPfnDatabase()
{
  UNICODE_STRING ustr                    = { 0 };
  uintptr_t      MmGetVirtualForPhysical = 0;
  uintptr_t      MmShadowPfnDatabase     = 0;

  RtlInitUnicodeString(&ustr, L"MmGetVirtualForPhysical");

  MmGetVirtualForPhysical = (uintptr_t)MmGetSystemRoutineAddress(&ustr);

  MmShadowPfnDatabase = *(uintptr_t*)(MmGetVirtualForPhysical + 0x22);

  MmShadowPfnDatabase &= ~(uintptr_t)0xff;

  return (void*)MmShadowPfnDatabase;
}

void* GetModuleBase(void* Address)
{
  char* addr = Address;

  for (addr = (char*)((uintptr_t)addr & 0xffffffffffff0000);; addr -= 0x10000)
  {
    if (addr[0] == 'M' && addr[1] == 'Z')
    {
      DbgPrint("Module found: %p\n", addr);
      break;
    }
  }

  return addr;
}

void* GetShadowPteAddress(void* Address)
{
  uintptr_t Address_            = 0;
  uintptr_t MmShadowPfnDatabase = 0;

  uintptr_t PteAddress = 0;
  // uintptr_t PdeAddress  = 0;
  // uintptr_t PdpeAddress = 0;
  // uintptr_t Pml4Address = 0;

  if (Address)
  {
    Address_            = (uintptr_t)Address;
    MmShadowPfnDatabase = (uintptr_t)GetMmShadowPfnDatabase();

    PteAddress = ((Address_ >> 9) & 0x7FFFFFFFF8LL) + MmShadowPfnDatabase;
    // PdeAddress  = ((PteAddress >> 9) & 0x7FFFFFFFF8LL) + MmShadowPfnDatabase;
    // PdpeAddress = ((PdeAddress >> 9) & 0x7FFFFFFFF8LL) + MmShadowPfnDatabase;
    // Pml4Address = ((PdpeAddress >> 9) & 0x7FFFFFFFF8LL) + MmShadowPfnDatabase;
  }

  return (void*)PteAddress;
}

MiscFlags_t GetThreadMiscFlags(PKTHREAD Thread)
{
  MiscFlags_t Flags = { 0 };

  if (!Thread)
  {
    DbgPrint(__FUNCTION__ ": Invalid thread\n");
    goto EXIT;
  }

  Flags = *(MiscFlags_t*)((char*)Thread + MiscFlags_offset);

EXIT:
  return Flags;
}

void* GetThreadKernelShadowStack(PKTHREAD Thread)
{
  void* Stack = NULL;

  if (!Thread)
  {
    DbgPrint("Invalid thread\n");
    goto EXIT;
  }
  Stack = *(void**)((char*)Thread + KernelShadowStack_offset);

  DbgPrint("Kernel Shadow Stack: %p\n", Stack);

EXIT:
  return Stack;
}

void* GetThreadKernelShadowStackBase(PKTHREAD Thread)
{
  void* Stack = NULL;

  if (!Thread)
  {
    DbgPrint("Invalid thread\n");
    goto EXIT;
  }
  Stack = *(void**)((char*)Thread + KernelShadowStackBase_offset);

  DbgPrint("Kernel Shadow Stack Base: %p\n", Stack);

EXIT:
  return Stack;
}

void* GetThreadKernelShadowStackInitial(PKTHREAD Thread)
{
  void* Stack = NULL;

  if (!Thread)
  {
    DbgPrint("Invalid thread\n");
    goto EXIT;
  }
  Stack = *(void**)((char*)Thread + KernelShadowStackInitial_offset);

  DbgPrint("Kernel Shadow Stack Initial: %p\n", Stack);

EXIT:
  return Stack;
}

void* GetThreadKernelShadowStackLimit(PKTHREAD Thread)
{
  void* Stack = NULL;

  if (!Thread)
  {
    DbgPrint("Invalid thread\n");
    goto EXIT;
  }
  Stack = *(void**)((char*)Thread + KernelShadowStackLimit_offset);

  DbgPrint("Kernel Shadow Stack Limit: %p\n", Stack);

EXIT:
  return Stack;
}

void PrintCetKernelShadowStack(PKTHREAD Thread)
{
  MiscFlags_t* Flags = NULL;

  Flags = (MiscFlags_t*)((char*)Thread + MiscFlags_offset);

  DbgPrint("Thread: %p\n", Thread);

  DbgPrint("  CetUserShadowStack: %s\n",
           Flags->CetUserShadowStack ? "TRUE" : "FALSE");
  DbgPrint("  CetKernelShadowStack: %s\n",
           Flags->CetKernelShadowStack ? "TRUE" : "FALSE");
}

NTSTATUS PrintCetKernelShadowStackCallback(PKTHREAD Thread, void* Context)
{
  UNREFERENCED_PARAMETER(Context);

  PrintCetKernelShadowStack(Thread);

  return STATUS_SUCCESS;
}

void PrintThreadProcessName(PKTHREAD Thread)
{
  PKPROCESS Process = NULL;

  if (Thread)
  {
    Process = *(void**)((char*)Thread + Process_offset);

    DbgPrint("Thread: %p >> %s\n",
             Thread,
             (char*)Process + ImageFileName_offset);
  }
}

NTSTATUS PrintThreadProcessNameCallback(PKTHREAD Thread, void* Context)
{
  UNREFERENCED_PARAMETER(Context);

  PrintThreadProcessName(Thread);

  return STATUS_SUCCESS;
}

void SetThreadCetKernelShadowStack(PKTHREAD Thread, uint8_t Value)
{
  MiscFlags_t* Flags = NULL;

  if (Thread)
  {
    DbgPrint("Thread: %p\n", Thread);

    Flags = (MiscFlags_t*)((char*)Thread + MiscFlags_offset);

    Flags->CetKernelShadowStack = Value & 1;
  }
}

NTSTATUS SetThreadCetKernelShadowStackCallback(PKTHREAD Thread, void* Context)
{
  if (!Thread)
  {
    DbgPrint(__FUNCTION__ ": Invalid thread\n");
    return STATUS_INVALID_PARAMETER;
  }

  SetThreadCetKernelShadowStack(Thread, (uintptr_t)Context & 0xff);

  return STATUS_SUCCESS;
}

void SetThreadKernelShadowStack(PKTHREAD Thread, void* ShadowStack)
{
  if (Thread)
  {
    *(void**)((char*)Thread + KernelShadowStack_offset) = ShadowStack;
  }
}

void SetThreadKernelShadowStackBase(PKTHREAD Thread, void* ShadowStackBase)
{
  if (Thread)
  {
    *(void**)((char*)Thread + KernelShadowStackBase_offset) = ShadowStackBase;
  }
}

void SetThreadKernelShadowStackInitial(
  PKTHREAD Thread,
  void*    ShadowStackInitial)
{
  if (Thread)
  {
    *(void**)((char*)Thread + KernelShadowStackInitial_offset) =
      ShadowStackInitial;
  }
}

void SetThreadKernelShadowStackLimit(PKTHREAD Thread, void* ShadowStackLimit)
{
  if (Thread)
  {
    *(void**)((char*)Thread + KernelShadowStackLimit_offset) = ShadowStackLimit;
  }
}

void SkipNextFrame()
{
  uintptr_t  module  = 0;
  uintptr_t* new_rsp = NULL;

  DbgPrintEnter();

  module  = (uintptr_t)GetModuleBase((void*)SkipNextFrame);
  new_rsp = (uintptr_t*)&new_rsp;

  for (new_rsp += 2;; ++new_rsp)
  {
    if (MASK_KERNEL_ADDR(module) == MASK_KERNEL_ADDR(*new_rsp))
    {
      break;
    }
  }

  setRsp(new_rsp);

  DbgPrintLeave();
}

NTSTATUS ThreadWalker(ThreadCallback_t Callback, void* Context)
{
  LIST_ENTRY* Entry   = NULL;
  LIST_ENTRY* Head    = NULL;
  NTSTATUS    Status  = STATUS_INVALID_PARAMETER;
  PEPROCESS   Process = NULL;
  PKTHREAD    Thread  = NULL;

  if (!Callback)
  {
    DbgPrint(__FUNCTION__ ": Invalid parameter\n");
    goto EXIT;
  }

  Process = PsGetCurrentProcess();
  Head    = (LIST_ENTRY*)((char*)Process + ThreadListHead_offset);

  for (Entry = Head->Flink; Entry != Head; Entry = Entry->Flink)
  {
    Thread = (PKTHREAD)((uintptr_t)Entry - ThreadListEntry_offset);

    Status = Callback(Thread, Context);
    if (NT_ERROR(Status))
    {
      DbgPrint(__FUNCTION__ ": Callback failed\n");
      break;
    }
  }

EXIT:
  DbgPrintLeave();

  return Status;
}
