#include <ntifs.h>

#include "driver/ioctl.h"

#include <intrin.h>

#include "common/utils.h"
#include "driver/utils.h"

// ===========================================================================
// Offsets 24h2
// ===========================================================================

#define KI_KERNEL_CET_AUDIT_MODE_ENABLED_OFFSET_24H2 0x01201843
#define KI_KERNEL_CET_ENABLED_OFFSET_24H2            0x01201841
#define KI_KERNEL_CET_LOGGING_OFFSET_24H2            0x00fc6050

// ===========================================================================
// Offsets 24h2
// ===========================================================================

#define KI_KERNEL_CET_AUDIT_MODE_ENABLED_OFFSET KI_KERNEL_CET_AUDIT_MODE_ENABLED_OFFSET_24H2
#define KI_KERNEL_CET_ENABLED_OFFSET            KI_KERNEL_CET_ENABLED_OFFSET_24H2
#define KI_KERNEL_CET_LOGGING_OFFSET            KI_KERNEL_CET_LOGGING_OFFSET_24H2

int32_t DoRun = 1;

// ===========================================================================
// Function definitions
// ===========================================================================

int32_t _DivZeroFilter()
{
  DbgPrint(__FUNCTION__ "\n");

  return EXCEPTION_EXECUTE_HANDLER;
}

NTSTATUS IoctlDivInteger(void* Buffer, uint32_t Length)
{
  IoctlDivInteger_t* di     = NULL;
  NTSTATUS           Status = STATUS_INVALID_PARAMETER;

  DbgPrintEnter();

  if (!Buffer)
  {
    DbgPrint("Invalid buffer\n");
    goto EXIT;
  }

  if (sizeof(IoctlDivInteger_t) > Length)
  {
    DbgPrint("Invalid length\n");
    Status = STATUS_BUFFER_TOO_SMALL;
    goto EXIT;
  }
  di = (IoctlDivInteger_t*)Buffer;

  try
  {
    Status = DivInteger(di->Dividend, di->Divisor);
    if (!NT_SUCCESS(Status))
    {
      DbgPrint("[-] " __FUNCTION__ ": DivInteger\n");
      goto EXIT;
    }
  }
  except(_DivZeroFilter())
  {
    DbgPrint("A division by zero occurred\n");
    Status = STATUS_INTEGER_DIVIDE_BY_ZERO;
    goto EXIT;
  }

  Status = STATUS_SUCCESS;

EXIT:
  DbgPrintLeave();

  return Status;
}

NTSTATUS IoctlIncRetAddr()
{
  DbgPrintEnter();

  IncRetAddr();

  DbgPrintLeave();

  return STATUS_SUCCESS;
}

NTSTATUS IoctlWriteCr4(void* Buffer, uint32_t Length)
{
  Cr4Flags_t* Flags  = NULL;
  NTSTATUS    Status = STATUS_INVALID_PARAMETER;
  uint8_t*    Value  = NULL;
  uint64_t    cr4    = 0;

  DbgPrintEnter();

  if (!Buffer)
  {
    DbgPrint("Invalid buffer\n");
    goto EXIT;
  }

  if (sizeof(uint8_t) > Length)
  {
    DbgPrint("Invalid length\n");
    Status = STATUS_BUFFER_TOO_SMALL;
    goto EXIT;
  }
  Value = (uint8_t*)Buffer;

  Flags = (Cr4Flags_t*)&cr4;
  cr4   = __readcr4();

  DbgPrint("Reading CR4 = 0x%016llx\n", cr4);

  Flags->CET = *Value & 1;

  DbgPrint("Writing CR4 = 0x%016llx\n", cr4);

  __writecr4(cr4);

  Status = STATUS_SUCCESS;

EXIT:
  DbgPrintLeave();

  return Status;
}

NTSTATUS IoctlSkipNextFrame()
{
  DbgPrintEnter();

  SkipNextFrame();

  DbgPrintLeave();

  return STATUS_SUCCESS;
}

NTSTATUS IoctlWriteMsr(void* Buffer, uint32_t Length)
{
  Msr_t*   Msr    = NULL;
  NTSTATUS Status = STATUS_INVALID_PARAMETER;

  DbgPrintEnter();

  if (!Buffer)
  {
    DbgPrint("Invalid buffer\n");
    goto EXIT;
  }

  if (sizeof(Msr_t) > Length)
  {
    DbgPrint("Invalid length\n");
    Status = STATUS_BUFFER_TOO_SMALL;
    goto EXIT;
  }
  Msr = (Msr_t*)Buffer;

  DbgPrint("Reading MSR[0x%04x] = 0x%016llx\n",
           Msr->Register,
           __readmsr(Msr->Register));
  DbgPrint("Writing MSR[0x%04x] = 0x%016llx\n",
           Msr->Register,
           Msr->Value);

  __writemsr(Msr->Register, Msr->Value);

EXIT:
  DbgPrintLeave();

  return Status;
}

NTSTATUS IoctlWriteShadowStack(void* Buffer, uint32_t Length)
{
  IoctlWriteShadowStack_t* wss            = NULL;
  NTSTATUS                 Status         = STATUS_INVALID_PARAMETER;
  uint8_t*                 u8ShadowStack  = NULL;
  uint64_t*                u64ShadowStack = NULL;

  DbgPrintEnter();

  if (!Buffer)
  {
    DbgPrint("Invalid buffer\n");
    goto EXIT;
  }

  if (sizeof(IoctlWriteShadowStack_t) > Length)
  {
    DbgPrint("Invalid length\n");
    Status = STATUS_BUFFER_TOO_SMALL;
    goto EXIT;
  }
  wss = (IoctlWriteShadowStack_t*)Buffer;

  u8ShadowStack  = GetThreadKernelShadowStack(KeGetCurrentThread());
  u64ShadowStack = (uint64_t*)(u8ShadowStack + wss->Offset);

  DbgPrint("Reading -> %p = %p\n", u64ShadowStack, *u64ShadowStack);
  DbgPrint("Writing -> %p = %p\n", u64ShadowStack, wss->Address);

  *u64ShadowStack = wss->Address;

EXIT:
  DbgPrintLeave();

  return Status;
}

NTSTATUS IoctlWriteShadowStackPte(void* Buffer, uint32_t Length)
{
  NTSTATUS       Status          = STATUS_INVALID_PARAMETER;
  PageEntry64_t* PageEntry       = NULL;
  PageEntry64_t* ShadowPageEntry = NULL;
  void*          ShadowStack     = NULL;

  DbgPrintEnter();

  if (!Buffer)
  {
    DbgPrint("Invalid buffer\n");
    goto EXIT;
  }

  if (sizeof(PageEntry64_t) > Length)
  {
    DbgPrint("Invalid length\n");
    Status = STATUS_BUFFER_TOO_SMALL;
    goto EXIT;
  }
  PageEntry = (PageEntry64_t*)Buffer;

  ShadowStack     = GetThreadKernelShadowStack(KeGetCurrentThread());
  ShadowPageEntry = (PageEntry64_t*)GetShadowPteAddress(ShadowStack);

  DbgPrint("Shadow Page Entry:     %p -> %p\n", ShadowPageEntry, *ShadowPageEntry);
  ShadowPageEntry->Write = PageEntry->Write;

  for (uint64_t i = 0; i < 0xfffffffffffffff0; ++i)
  {
  }

  DbgPrint("New Shadow Page Entry: %p -> %p\n", ShadowPageEntry, *ShadowPageEntry);

  Status = STATUS_SUCCESS;

EXIT:
  DbgPrintLeave();

  return Status;
}
