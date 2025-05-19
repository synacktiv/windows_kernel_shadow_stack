#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#include <stdint.h>

#ifndef _MAX_PATH_
#define _MAX_PATH_ (0x200)
#endif

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))

#define BASE_NAME "ShadowStack"

#define CLIENT_DEVICE_NAME "\\??\\" BASE_NAME
#define DRIVER_DEVICE_NAME L"\\Device\\" BASE_NAME
#define DOS_DEVICE_NAME    L"\\DosDevices\\" BASE_NAME

#define DEVICE_IOCTL_TYPE 4000

#define IOCTL_INC_RET_ADDR \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SKIP_NEXT_FRAME \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_CURRENT_SHADOW_STACK \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_CR4 \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MSR \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_CURRENT_SHADOW_STACK_PTE \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DIV_INTEGER \
  CTL_CODE(DEVICE_IOCTL_TYPE, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IA32_U_CET_REGISTER   (0x6a0)
#define IA32_S_CET_REGISTER   (0x6a2)
#define IA32_PL0_SSP_REGISTER (0x6a4)
#define IA32_PL1_SSP_REGISTER (0x6a5)
#define IA32_PL2_SSP_REGISTER (0x6a6)
#define IA32_PL3_SSP_REGISTER (0x6a7)

// ===========================================================================
// Structure definitions
// ===========================================================================

typedef struct _Cr4Flags
{
  uint64_t VME : 1;
  uint64_t PVI : 1;
  uint64_t TSD : 1;
  uint64_t DE : 1;
  uint64_t PSE : 1;
  uint64_t PAE : 1;
  uint64_t MCE : 1;
  uint64_t PGE : 1;
  uint64_t PCE : 1;
  uint64_t OSFXSR : 1;
  uint64_t OSXMMEXCPT : 1;
  uint64_t UMIP : 1;
  uint64_t LA57 : 1;
  uint64_t VMXE : 1;
  uint64_t SMXE : 1;
  uint64_t Reserved0 : 1;
  uint64_t FSGSBASE : 1;
  uint64_t PCIDE : 1;
  uint64_t OSXSAVE : 1;
  uint64_t KL : 1;
  uint64_t SMEP : 1;
  uint64_t SMAP : 1;
  uint64_t PKE : 1;
  uint64_t CET : 1;
  uint64_t PKS : 1;
  uint64_t UINTR : 1;
  uint64_t Reserved1 : 38;
} Cr4Flags_t;

typedef struct _MiscFlags
{
  uint32_t AutoBoostActive : 1;
  uint32_t ReadyTransition : 1;
  uint32_t WaitNext : 1;
  uint32_t SystemAffinityActive : 1;
  uint32_t Alertable : 1;
  uint32_t UserStackWalkActive : 1;
  uint32_t ApcInterruptRequest : 1;
  uint32_t QuantumEndMigrate : 1;
  uint32_t SecureThread : 1;
  uint32_t TimerActive : 1;
  uint32_t SystemThread : 1;
  uint32_t ProcessDetachActive : 1;
  uint32_t CalloutActive : 1;
  uint32_t ScbReadyQueue : 1;
  uint32_t ApcQueueable : 1;
  uint32_t ReservedStackInUse : 1;
  uint32_t Spare : 1;
  uint32_t TimerSuspended : 1;
  uint32_t SuspendedWaitMode : 1;
  uint32_t SuspendSchedulerApcWait : 1;
  uint32_t CetUserShadowStack : 1;
  uint32_t BypassProcessFreeze : 1;
  uint32_t CetKernelShadowStack : 1;
  uint32_t StateSaveAreaDecoupled : 1;
  uint32_t Reserved : 8;
} MiscFlags_t;

typedef struct _Msr
{
  uint32_t Register;
  uint64_t Value;
} Msr_t;

typedef struct _MsrCetFlags
{
  uint64_t SH_STK_EN : 1;
  uint64_t WR_SHSTK_EN : 1;
  uint64_t ENDBR_EN : 1;
  uint64_t LEG_IW_EN : 1;
  uint64_t NO_TRACK_EN : 1;
  uint64_t SUPPRESS_DIS : 1;
  uint64_t Reserved : 4;
  uint64_t SUPPRESS : 1;
  uint64_t TRACKER : 1;
  uint64_t EB_LEG_BITMAP_BASE : 52;
} MsrCetFlags_t;

typedef struct _PageEntry64
{
  uint64_t Valid : 1;
  uint64_t Write : 1;
  uint64_t Owner : 1;
  uint64_t WriteThrough : 1;
  uint64_t CacheDisabled : 1;
  uint64_t Accessed : 1;
  uint64_t Dirty : 1;
  uint64_t LargePage : 1;
  uint64_t Global : 1;
  uint64_t CopyOnWrite : 1;
  uint64_t PrototypeSoft : 1;
  uint64_t WriteSoft : 1;
  uint64_t Pfn : 36;
  uint64_t Reserved : 15;
  uint64_t NoExecute : 1;
} PageEntry64_t;

typedef struct _IoctlDivInteger
{
  int32_t Dividend;
  int32_t Divisor;
  int32_t Quotient;
} IoctlDivInteger_t;

typedef struct _IoctlOpenFileDesc
{
  uint32_t Length;
  char     Name[_MAX_PATH_];
} IoctlOpenFileDesc_t;

typedef struct _IoctlWriteShadowStack
{
  int32_t  Offset;
  uint64_t Address;
} IoctlWriteShadowStack_t;

#endif // __COMMON_UTILS_H__
