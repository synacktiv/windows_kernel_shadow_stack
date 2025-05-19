#ifndef __CLIENT_UTILS_H__
#define __CLIENT_UTILS_H__

#include <stdint.h>

#include "common/utils.h"

// ===========================================================================
// Function declarations
// ===========================================================================

/**
 * \brief Closes the current opened device
 */
void CloseExposedDevice();

/**
 * \brief Permforms an integer division in kernel space
 *
 * \param Dividend Value to divide
 * \param Divisor  Value used to divide
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t DivInteger(
  int32_t Dividend,
  int32_t Divisor);

/**
 * \brief Increments the kernel return address to the caller in order to
 *        trigger a mismatch with the shadow stack
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t IncRetAddr();

/**
 * \brief Opens the device related to "\??\ShadowStack"
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t OpenExposedDevice();

/**
 * \brief Opens the device related to the given name
 *
 * \param DeviceName Pointer to the device name of the device to be opened
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t OpenExposedDeviceEx(
  char* DeviceName);

/**
 * \brief Prints the fields of the given CR4 flags
 *
 * \param Flags CR4 flags to print
 */
void PrintCr4Flags(
  Cr4Flags_t Flags);

/**
 * \brief Prints the fields of the given misc flags
 *
 * \param Flags Misc flags to print
 */
void PrintMiscFlags(
  MiscFlags_t Flags);

/**
 * \brief Prints the fields of the given MSR CET flags
 *
 * \param Flags MSR CET flags to print
 */
void PrintMsrCet(
  MsrCetFlags_t Flags);

/**
 * \brief Sets the CET flag of the current CR4 register
 *
 * \param Value CET value to set
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t WriteCr4(
  uint8_t Value);

/**
 * \brief Skips the next stack frames in order to trigger a valid mismatch
 *        between the stack and the shadow stack
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t SkipNextFrame();

/**
 * \brief Writes the given value to the given register address
 *
 * \param Register Register address where to write
 * \param Value    Value to set
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t WriteMsr(
  uint32_t Register,
  uint64_t Value);

/**
 * \brief Writes data into the current shadow stack
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t WriteShadowStack();

/**
 * \brief Sets the write flag of the shadow stack PTE
 *
 * \return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int32_t WriteShadowStackPte();

#endif // __CLIENT_UTILS_H__
