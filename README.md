# Proof of Concepts

This project aims to highlight certain aspects of the shadow stack mitigation implemented by Microsoft in Windows 11.
The code is specifically focused on Windows 11 24H2.
**It may not work on other version of Windows 11, due to the kernel offset management**.

The project is divided into two parts: a client and a driver.
Communication between them is handled through the IOCTL mechanism.
The client requests the driver to perform actions at the kernel level.

## Features

- Increment one of the kernel return addresses of the current thread to misalign it and trigger a BSOD.
- Skip one of the kernel stack frames of the current thread to highlight the resilience of the shadow stack.
- Enable/disable CET in the current `CR4` register, leading to a BSOD
- Write to MSR registers, which typically leads to a BSOD
- Write to the shadow stack, which results in a BSOD
- Write the shadow stack PTE, triggering a BSOD
- Perform divisions to highlight `try/except` and shadow stack behaviors during a division by zero

## Build

Use `build.bat`.

### Prerequisites

As a prerequisite, the following kits have to be for the same Windows build:

- Microsoft Windows SDK (Software Development Kit)
- Microsoft WDK (Windows Driver Kit)

Note:
The POC have been tested with the version `10.0.26100.0` of the kits.

## Target Machine Configuration

To activate the kernel shadow stack mitigation, the following values have to be added or set in the `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceGuard\` key:

| Name                                              | Type      | Data
| ---                                               | ---       | ---
| EnableVirtualizationBasedSecurity                 | REG_DWORD | 1
| Scenarios\HypervisorEnforcedCodeIntegrity\Enabled | REG_DWORD | 1
| Scenarios\KernelShadowStacks\Enabled              | REG_DWORD | 1

Also, in order to load a test-signed driver, the following command have to be run in an administrator console:

```console
bcdedit /set testsigning on
```

Then, a reboot is required.

## Usage

### Driver Loading

To load the driver, the following commands have to be entered in an administrator console:

```console
sc create shadow type= kernel binPath= ABSOLUTE_PATH_TO_THE_DRIVER
sc start shadow
```

### Client

The client has the following options:

| Option               | Description
| ---                  | ---
| inc                  | Increment the return address to the caller by 1
| skip                 | Patch RSP in order to point on the previous stack frame
| cdisable             | Disable CET in CR4
| cenable              | Enable CET in CR4
| wrmsr REGISTER VALUE | Write the REGISTER with the given VALUE
| wrshsk               | Write into the shadow stack
| wrshskpte            | Rewrite the shadow stack PTE
| div DIVIDEND DIVISOR | Perfom integer division in kernel space


## Credits

SergiusTheBest for its [FindWDK](https://github.com/SergiusTheBest/FindWDK).
