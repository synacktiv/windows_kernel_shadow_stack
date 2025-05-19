#include "client/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// ===========================================================================
// Inner global declarations
// ===========================================================================

static HANDLE gDevice = INVALID_HANDLE_VALUE;

// ===========================================================================
// Function definitions
// ===========================================================================

void CloseExposedDevice()
{
  if (INVALID_HANDLE_VALUE != gDevice)
  {
    CloseHandle(gDevice);
    gDevice = INVALID_HANDLE_VALUE;
  }
}

int32_t DivInteger(int32_t Dividend, int32_t Divisor)
{
  int32_t           ExitCode = EXIT_FAILURE;
  IoctlDivInteger_t di       = { 0 };

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  __try
  {
    di.Quotient = Dividend / Divisor;
  }
  __except (1)
  {
    printf("[-] Divided by zero\n");
  }

  di.Dividend = Dividend;
  di.Divisor  = Divisor;

  if (!DeviceIoControl(
        gDevice,
        IOCTL_DIV_INTEGER,
        &di,
        sizeof(IoctlDivInteger_t),
        &di,
        sizeof(IoctlDivInteger_t),
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  printf("Division: %i / %i => %i\n", di.Dividend, di.Divisor, di.Quotient);

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t IncRetAddr()
{
  int32_t ExitCode = EXIT_FAILURE;

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  if (!DeviceIoControl(
        gDevice,
        IOCTL_INC_RET_ADDR,
        NULL,
        0,
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  printf("Stack incremented\n");

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t OpenExposedDevice()
{
  int32_t ExitCode = EXIT_FAILURE;

  if (EXIT_SUCCESS != OpenExposedDeviceEx(CLIENT_DEVICE_NAME))
  {
    printf("[!] " __FUNCTION__ " failed\n");
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t OpenExposedDeviceEx(char* DeviceName)
{
  int32_t ExitCode = EXIT_FAILURE;

  if (!DeviceName)
  {
    printf("[-] " __FUNCTION__ ": Invalid device name\n");
    goto EXIT;
  }

  if (INVALID_HANDLE_VALUE != gDevice)
  {
    printf("[!] " __FUNCTION__ ": Device already opened\n");
    goto EXIT;
  }

  gDevice = CreateFile(
    DeviceName,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": CreateFile failed (0x%08x)\n", GetLastError());
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

void PrintCr4Flags(Cr4Flags_t Flags)
{
  printf("CR4 (0x%llx):\n", *(uint64_t*)&Flags);
  printf("  VME: %s\n", Flags.VME ? "TRUE" : "FALSE");
  printf("  PVI: %s\n", Flags.PVI ? "TRUE" : "FALSE");
  printf("  TSD: %s\n", Flags.TSD ? "TRUE" : "FALSE");
  printf("  DE: %s\n", Flags.DE ? "TRUE" : "FALSE");
  printf("  PSE: %s\n", Flags.PSE ? "TRUE" : "FALSE");
  printf("  PAE: %s\n", Flags.PAE ? "TRUE" : "FALSE");
  printf("  MCE: %s\n", Flags.MCE ? "TRUE" : "FALSE");
  printf("  PGE: %s\n", Flags.PGE ? "TRUE" : "FALSE");
  printf("  PCE: %s\n", Flags.PCE ? "TRUE" : "FALSE");
  printf("  OSFXSR: %s\n", Flags.OSFXSR ? "TRUE" : "FALSE");
  printf("  OSXMMEXCPT: %s\n", Flags.OSXMMEXCPT ? "TRUE" : "FALSE");
  printf("  UMIP: %s\n", Flags.UMIP ? "TRUE" : "FALSE");
  printf("  LA57: %s\n", Flags.LA57 ? "TRUE" : "FALSE");
  printf("  VMXE: %s\n", Flags.VMXE ? "TRUE" : "FALSE");
  printf("  SMXE: %s\n", Flags.SMXE ? "TRUE" : "FALSE");
  printf("  Reserved0: %s\n", Flags.Reserved0 ? "TRUE" : "FALSE");
  printf("  FSGSBASE: %s\n", Flags.FSGSBASE ? "TRUE" : "FALSE");
  printf("  PCIDE: %s\n", Flags.PCIDE ? "TRUE" : "FALSE");
  printf("  OSXSAVE: %s\n", Flags.OSXSAVE ? "TRUE" : "FALSE");
  printf("  KL: %s\n", Flags.KL ? "TRUE" : "FALSE");
  printf("  SMEP: %s\n", Flags.SMEP ? "TRUE" : "FALSE");
  printf("  SMAP: %s\n", Flags.SMAP ? "TRUE" : "FALSE");
  printf("  PKE: %s\n", Flags.PKE ? "TRUE" : "FALSE");
  printf("  CET: %s\n", Flags.CET ? "TRUE" : "FALSE");
  printf("  PKS: %s\n", Flags.PKS ? "TRUE" : "FALSE");
  printf("  UINTR: %s\n", Flags.UINTR ? "TRUE" : "FALSE");
  printf("  Reserved1: 0x%llx\n", Flags.Reserved1);
}

void PrintMiscFlags(MiscFlags_t Flags)
{
  printf("Misc flags (0x%x):\n", *(uint32_t*)&Flags);
  printf("  AutoBoostActive: %s\n", Flags.AutoBoostActive ? "TRUE" : "FALSE");
  printf("  ReadyTransition: %s\n", Flags.ReadyTransition ? "TRUE" : "FALSE");
  printf("  WaitNext: %s\n", Flags.WaitNext ? "TRUE" : "FALSE");
  printf("  SystemAffinityActive: %s\n",
         Flags.SystemAffinityActive ? "TRUE" : "FALSE");
  printf("  Alertable: %s\n", Flags.Alertable ? "TRUE" : "FALSE");
  printf("  UserStackWalkActive: %s\n",
         Flags.UserStackWalkActive ? "TRUE" : "FALSE");
  printf("  ApcInterruptRequest: %s\n",
         Flags.ApcInterruptRequest ? "TRUE" : "FALSE");
  printf("  QuantumEndMigrate: %s\n",
         Flags.QuantumEndMigrate ? "TRUE" : "FALSE");
  printf("  SecureThread: %s\n", Flags.SecureThread ? "TRUE" : "FALSE");
  printf("  TimerActive: %s\n", Flags.TimerActive ? "TRUE" : "FALSE");
  printf("  SystemThread: %s\n", Flags.SystemThread ? "TRUE" : "FALSE");
  printf("  ProcessDetachActive: %s\n",
         Flags.ProcessDetachActive ? "TRUE" : "FALSE");
  printf("  CalloutActive: %s\n", Flags.CalloutActive ? "TRUE" : "FALSE");
  printf("  ScbReadyQueue: %s\n", Flags.ScbReadyQueue ? "TRUE" : "FALSE");
  printf("  ApcQueueable: %s\n", Flags.ApcQueueable ? "TRUE" : "FALSE");
  printf("  ReservedStackInUse: %s\n",
         Flags.ReservedStackInUse ? "TRUE" : "FALSE");
  printf("  Spare: %s\n", Flags.Spare ? "TRUE" : "FALSE");
  printf("  TimerSuspended: %s\n", Flags.TimerSuspended ? "TRUE" : "FALSE");
  printf("  SuspendedWaitMode: %s\n",
         Flags.SuspendedWaitMode ? "TRUE" : "FALSE");
  printf("  SuspendSchedulerApcWait: %s\n",
         Flags.SuspendSchedulerApcWait ? "TRUE" : "FALSE");
  printf("  CetUserShadowStack: %s\n",
         Flags.CetUserShadowStack ? "TRUE" : "FALSE");
  printf("  BypassProcessFreeze: %s\n",
         Flags.BypassProcessFreeze ? "TRUE" : "FALSE");
  printf("  CetKernelShadowStack: %s\n",
         Flags.CetKernelShadowStack ? "TRUE" : "FALSE");
  printf("  StateSaveAreaDecoupled: %s\n",
         Flags.StateSaveAreaDecoupled ? "TRUE" : "FALSE");
  printf("  Reserved: 0x%02x\n", Flags.Reserved);
}

void PrintMsrCet(MsrCetFlags_t Flags)
{
  printf("MSR CET flags (0x%llx):\n", *(uint64_t*)&Flags);
  printf("  SH_STK_EN: %s\n", Flags.SH_STK_EN ? "TRUE" : "FALSE");
  printf("  WR_SHSTK_EN: %s\n", Flags.WR_SHSTK_EN ? "TRUE" : "FALSE");
  printf("  ENDBR_EN: %s\n", Flags.ENDBR_EN ? "TRUE" : "FALSE");
  printf("  LEG_IW_EN: %s\n", Flags.LEG_IW_EN ? "TRUE" : "FALSE");
  printf("  NO_TRACK_EN: %s\n", Flags.NO_TRACK_EN ? "TRUE" : "FALSE");
  printf("  SUPPRESS_DIS: %s\n", Flags.SUPPRESS_DIS ? "TRUE" : "FALSE");
  printf("  Reserved: 0x%llx\n", Flags.Reserved);
  printf("  SUPPRESS: %s\n", Flags.SUPPRESS ? "TRUE" : "FALSE");
  printf("  TRACKER: %s\n", Flags.TRACKER ? "TRUE" : "FALSE");
  printf("  EB_LEG_BITMAP_BASE: 0x%llx\n", Flags.EB_LEG_BITMAP_BASE);
}

int32_t WriteCr4(uint8_t Value)
{
  int32_t ExitCode = EXIT_FAILURE;

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  if (!DeviceIoControl(
        gDevice,
        IOCTL_WRITE_CR4,
        &Value,
        sizeof(Value),
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t SkipNextFrame()
{
  int32_t ExitCode = EXIT_FAILURE;

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  if (!DeviceIoControl(
        gDevice,
        IOCTL_SKIP_NEXT_FRAME,
        NULL,
        0,
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t WriteMsr(uint32_t Register, uint64_t Value)
{
  int32_t ExitCode = EXIT_FAILURE;
  Msr_t   Msr      = { 0 };

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  Msr.Register = Register;
  Msr.Value    = Value;

  if (!DeviceIoControl(
        gDevice,
        IOCTL_WRITE_MSR,
        &Msr,
        sizeof(Msr_t),
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  printf("Register [0x%04x] <= 0x%016llx", Register, Value);

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t WriteShadowStack()
{
  int32_t                 ExitCode = EXIT_FAILURE;
  IoctlWriteShadowStack_t wss      = { 0 };

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  wss.Offset  = -0x200;
  wss.Address = 0x4141414141414141;

  if (!DeviceIoControl(
        gDevice,
        IOCTL_WRITE_CURRENT_SHADOW_STACK,
        &wss,
        sizeof(IoctlWriteShadowStack_t),
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}

int32_t WriteShadowStackPte()
{
  int32_t       ExitCode  = EXIT_FAILURE;
  PageEntry64_t PageEntry = { 0 };

  if (INVALID_HANDLE_VALUE == gDevice)
  {
    printf("[-] " __FUNCTION__ ": Device not opened\n");
    goto EXIT;
  }

  PageEntry.Write = 1;

  if (!DeviceIoControl(
        gDevice,
        IOCTL_WRITE_CURRENT_SHADOW_STACK_PTE,
        &PageEntry,
        sizeof(PageEntry64_t),
        NULL,
        0,
        NULL,
        NULL))
  {
    printf("[-] " __FUNCTION__ ": DeviceIoControl failed (0x%08x)\n",
           GetLastError());
    goto EXIT;
  }

  ExitCode = EXIT_SUCCESS;

EXIT:
  return ExitCode;
}
