#ifndef __DRIVER_UTILS_H__
#define __DRIVER_UTILS_H__

#include <stdint.h>
#include <wdm.h>

#include "common/utils.h"

#define DbgPrintEnter() DbgPrint("Entering " __FUNCTION__ "\n")
#define DbgPrintLeave() DbgPrint("Leaving " __FUNCTION__ "\n")

// ===========================================================================
// Callback definitions
// ===========================================================================

typedef NTSTATUS (*ThreadCallback_t)(PKTHREAD Thread, void* Context);

// ===========================================================================
// Function declarations
// ===========================================================================

/**
 * \brief Creates a device with "\Device\ShadowStack" name and exposes it
 *        with "\DosDevices\ShadowStack" as symbolic link
 *
 * \param DriverObject Pointer to the driver object of the current driver
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS CreateExposedDevice(
  DRIVER_OBJECT* DriverObject);

/**
 * \brief Creates a device and expose it to the userland
 *
 * \param DriverObject Pointer to the driver object of the current driver
 * \param DeviceName   Pointer to the device name of the device to be created
 * \param SymlinkName  Pointer to the name that will be exposed to the userland
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS CreateExposedDeviceEx(
  DRIVER_OBJECT* DriverObject,
  wchar_t*       DeviceName,
  wchar_t*       SymlinkName);

/**
 * \brief Deletes the currently created device
 *
 * \param DriverObject Pointer to the driver object of the current driver
 */
void DeleteExposedDevice(
  DRIVER_OBJECT* DriverObject);

/**
 * \brief Deletes the device related to the given symbolic link
 *
 * \param DriverObject Pointer to the driver object of the current driver
 * \param SymlinkName  Pointer to the symbolic link name which has to be deleted
 */
void DeleteExposedDeviceEx(
  DRIVER_OBJECT* DriverObject,
  wchar_t*       SymlinkName);

/**
 * \brief Performs integer division
 *
 * \param Dividend Value to divide
 * \param Divisor  Value used to divide
 *
 * \return The quotient
 */
int32_t DivInteger(
  int32_t Dividend,
  int32_t Divisor);

/**
 * \brief Increments the return address to the caller by 1
 */
void IncRetAddr();

/**
 * \brief Retrieves the kernel shadow stack pointer of the current thread
 *
 * \return The kernel shadow stack pointer of the current thread
 */
void* GetCurrentThreadKernelShadowStack();

/**
 * \brief Retrieves the kernel shadow stack base pointer of the current thread
 *
 * \return The kernel shadow stack base pointer of the current thread
 */
void* GetCurrentThreadKernelShadowStackBase();

/**
 * \brief Retrieves the kernel shadow stack initial pointer of the current thread
 *
 * \return The kernel shadow stack initial pointer of the current thread
 */
void* GetCurrentThreadKernelShadowStackInitial();

/**
 * \brief Retrieves the kernel shadow stack limit of the current thread
 *
 * \return The kernel shadow stack limit of the current thread
 */
void* GetCurrentThreadKernelShadowStackLimit();

/**
 * \brief Retrieves the misc flags of the current thread
 *
 * \return Misc flags of the current thread
 */
MiscFlags_t GetCurrentThreadMiscFlags();

/**
 * \brief Looks for the kernel base address
 *
 * \return The kernel base address
 */
void* GetKernelBase();

/**
 * \brief Looks for the shadow PFN database
 *
 * \return The PFN database
 */
void* GetMmShadowPfnDatabase();

/**
 * \brief Looks for the module base address related to the given address
 *
 * \param Address Address in the module from which the base address has to be
 *                retrieved
 *
 * \return The module base address
 */
void* GetModuleBase(
  void* Address);

/**
 * \brief Looks for the virtual shadow PTE address of the given address
 *
 * \param Address Address of which to get the virtual PTE address
 *
 * \return The virtual PTE address
 */
void* GetShadowPteAddress(
  void* Address);

/**
 * \brief Retrieves the misc flags of the given thread
 *
 * \param Thread Pointer to the thread from which the misc flags are retrieved
 *
 * \return Misc flags of the given thread
 */
MiscFlags_t GetThreadMiscFlags(
  PKTHREAD Thread);

/**
 * \brief Retrieves the kernel shadow stack pointer of the given thread
 *
 * \param Thread Pointer to the thread from which retrieve the kernel shadow
 *               stack pointer
 *
 * \return The kernel shadow stack pointer of the given thread
 */
void* GetThreadKernelShadowStack(
  PKTHREAD Thread);

/**
 * \brief Retrieves the kernel shadow stack base pointer of the given thread
 *
 * \param Thread Pointer to the thread from which retrieve the kernel shadow
 *               stack base pointer
 *
 * \return The kernel shadow stack base pointer of the given thread
 */
void* GetThreadKernelShadowStackBase(
  PKTHREAD Thread);

/**
 * \brief Retrieves the kernel shadow stack initial pointer of the given thread
 *
 * \param Thread Pointer to the thread from which retrieve the kernel shadow
 *               stack initial pointer
 *
 * \return The kernel shadow stack initial pointer of the given thread
 */
void* GetThreadKernelShadowStackInitial(
  PKTHREAD Thread);

/**
 * \brief Retrieves the kernel shadow stack limit pointer of the given thread
 *
 * \param Thread Pointer to the thread from which retrieve the kernel shadow
 *               stack limit pointer
 *
 * \return The kernel shadow stack limit pointer of the given thread
 */
void* GetThreadKernelShadowStackLimit(
  PKTHREAD Thread);

/**
 * \brief Prints the CetUserShadowStack and the CetKernelShadowStack flags of
 *        the given thread
 *
 * \param Thread Pointer to the thread from which the flags have to be read
 */
void PrintCetKernelShadowStack(
  PKTHREAD Thread);

/**
 * \brief Callback used to call PrintCetKernelShadowStack (act as a wrapper)
 *
 * \param Thread  Pointer to the thread from which the flags have to be read
 * \param Context Unused parameter
 *
 * \return STATUS_SUCCESS always
 */
NTSTATUS PrintCetKernelShadowStackCallback(
  PKTHREAD Thread,
  void*    Context);

/**
 * \brief Prints the process name of the given thread
 *
 * \param Thread Pointer to the thread from which the process then process name
 *               are retrieved
 */
void PrintThreadProcessName(
  PKTHREAD Thread);

/**
 * \brief Callback used to call PrintThreadProcessName (act as a wrapper)
 *
 * \param Thread  Pointer to the thread from which the process then process name
 *                are retrieved
 * \param Context Unused parameter
 *
 * \return STATUS_SUCCESS always
 */
NTSTATUS PrintThreadProcessNameCallback(
  PKTHREAD Thread,
  void*    Context);

/**
 * \brief Sets the CetKernelShadowStack of the given thread with given value
 *
 * \param Thread Pointer to the thread to which the CetKernelShadowStack flag
 *               as to be set
 * \param Value  Value to set
 */
void SetThreadCetKernelShadowStack(
  PKTHREAD Thread,
  uint8_t  Value);

/**
 * \brief Callback used to call SetThreadCetKernelShadowStack (act as a wrapper)
 *
 * \param Thread  Pointer to the thread to which the CetKernelShadowStack flag
 *                as to be set
 * \param Context Represent the value used to set CetKernelShadowStack in
 *                SetThreadCetKernelShadowStack function
 */
NTSTATUS SetThreadCetKernelShadowStackCallback(
  PKTHREAD Thread,
  void*    Context);

/**
 * \brief Sets the kernel shadow stack pointer of the given thread
 *
 * \param Thread      Pointer to the thread to which set the kernel shadow stack
 *                    pointer
 * \param ShadowStack Pointer to the shadow stack to set
 */
void SetThreadKernelShadowStack(
  PKTHREAD Thread,
  void*    ShadowStack);

/**
 * \brief Sets the kernel shadow stack base pointer of the given thread
 *
 * \param Thread          Pointer to the thread to which set the kernel shadow
 *                        stack base pointer
 * \param ShadowStackBase Pointer to the shadow stack base to set
 */
void SetThreadKernelShadowStackBase(
  PKTHREAD Thread,
  void*    ShadowStackBase);

/**
 * \brief Sets the kernel shadow stack initial pointer of the given thread
 *
 * \param Thread             Pointer to the thread to which set the kernel
 *                           shadow stack initial pointer
 * \param ShadowStackInitial Pointer to the shadow stack initial to set
 */
void SetThreadKernelShadowStackInitial(
  PKTHREAD Thread,
  void*    ShadowStackInitial);

/**
 * \brief Sets the kernel shadow stack limit pointer of the given thread
 *
 * \param Thread           Pointer to the thread to which set the kernel shadow
 *                         stack limit pointer
 * \param ShadowStackLimit Pointer to the shadow stack limit to set
 */
void SetThreadKernelShadowStackLimit(
  PKTHREAD Thread,
  void*    ShadowStackLimit);

/**
 * \brief Skips the next stack frame
 */
void SkipNextFrame();

/**
 * \brief Walks through every thread of the current process
 *
 * \param Callback Pointer to the callback to call for each thread
 * \param Context  Pointer to the context to pass to each thread
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS ThreadWalker(
  ThreadCallback_t Callback,
  void*            Context);

#endif // __DRIVER_UTILS_H__
