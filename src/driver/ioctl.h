#ifndef __DRIVER_IOCTL_H__
#define __DRIVER_IOCTL_H__

#include <stdint.h>
#include <wdm.h>

#include "common/utils.h"

extern int32_t DoRun;

// ===========================================================================
// Function declarations
// ===========================================================================

/**
 * \brief Permforms an integer division
 *
 * \param Buffer Pointer to the buffer which contains the values for the
 *               division the register address and the
 * \param Length Length of the input buffer
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS IoctlDivInteger(
  void*    Buffer,
  uint32_t Length);

/**
 * \brief Increments the return address to the caller by 1
 *
 * \return STATUS_SUCCESS
 */
NTSTATUS IoctlIncRetAddr();

/**
 * \brief Sets the CET field in the current CR4 register
 *        process
 *
 * \param Buffer Pointer to the buffer which contains the CET value to set
 * \param Length Length of the input buffer
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS IoctlWriteCr4(
  void*    Buffer,
  uint32_t Length);

/**
 * \brief Patches its own return address in order to return on the next return
 *        address in the stack
 *
 * \return STATUS_SUCCESS always
 */
NTSTATUS IoctlSkipNextFrame();

/**
 * \brief Writes the given MSR register with the given value
 *
 * \param Buffer Pointer to the buffer which contains the register address and
 *               the value to be written
 * \param Length Length of the input buffer
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS IoctlWriteMsr(
  void*    Buffer,
  uint32_t Length);

/**
 * \brief Writes the given value into the shadow stack at the given offset
 *
 * \param Buffer Pointer to the buffer which contains the offset and value to
 *               write
 * \param Length Length of the input buffer
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS IoctlWriteShadowStack(
  void*    Buffer,
  uint32_t Length);

/**
 * \brief Rewrites the shadow stack PTE with the given PTE write flag
 *
 * \param Buffer Pointer to the buffer which contains the PTE to use
 * \param Length Length of the input buffer
 *
 * \return STATUS_SUCCESS on success, otherwise error code
 */
NTSTATUS IoctlWriteShadowStackPte(
  void*    Buffer,
  uint32_t Length);

#endif // __DRIVER_IOCTL_H__
