#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "client/utils.h"

#include "common/utils.h"

void usage()
{
  printf("Options:\n");
  printf("  inc                   Increment the return address to the caller\n");
  printf("                        by 1\n");
  printf("  skip                  Patch RSP in order to point on the previous\n");
  printf("                        stack frame\n");
  printf("  cdisable              Disable CET in CR4\n");
  printf("  cenable               Enable CET in CR4\n");
  printf("  wrmsr REGISTER VALUE  Write the REGISTER with the given VALUE\n");
  printf("  wrshsk                Write into the shadow stack\n");
  printf("  wrshskpte             Rewrite the shadow stack PTE\n");
  printf("  div DIVIDEND DIVISOR  Perfom integer division in kernel space\n");
}

int32_t main(int32_t argc, char* argv[])
{
  if (2 > argc)
  {
    printf("Missing arguments\n\n");

    usage();

    goto EXIT;
  }

  if (EXIT_SUCCESS != OpenExposedDevice())
  {
    printf("[!] " __FUNCTION__ " failed\n");
    goto EXIT;
  }

  if (!strcmp("inc", argv[1]))
  {
    if (IncRetAddr())
    {
      printf("[!] " __FUNCTION__ ": IncRetAddr failed\n");
      goto EXIT;
    }
  }
  else if (!strcmp("skip", argv[1]))
  {
    if (SkipNextFrame())
    {
      printf("[!] " __FUNCTION__ ": SkipNextFrame failed\n");
      goto EXIT;
    }

    printf("Stack returned\n");
  }
  else if (!strcmp("cdisable", argv[1]))
  {
    if (WriteCr4(0))
    {
      printf("[!] " __FUNCTION__ ": WriteCr4 failed\n");
      goto EXIT;
    }

    printf("Kernel shadow stack disabled in CR4\n");
  }
  else if (!strcmp("cenable", argv[1]))
  {
    if (WriteCr4(1))
    {
      printf("[!] " __FUNCTION__ ": WriteCr4 failed\n");
      goto EXIT;
    }

    printf("Kernel shadow stack enabled in CR4\n");
  }
  else if (!strcmp("wrmsr", argv[1]))
  {
    if (4 > argc)
    {
      printf("[!] wrmsr: Missing arguments");
      goto EXIT;
    }

    if (WriteMsr(strtoul(argv[2], NULL, 0), strtoull(argv[3], NULL, 0)))
    {
      printf("[!] " __FUNCTION__ ": WriteMsr failed\n");
      goto EXIT;
    }
  }
  else if (!strcmp("wrshsk", argv[1]))
  {
    if (WriteShadowStack())
    {
      printf("[!] " __FUNCTION__ ": WriteShadowStack failed\n");
      goto EXIT;
    }
  }
  else if (!strcmp("wrshskpte", argv[1]))
  {
    if (WriteShadowStackPte())
    {
      printf("[!] " __FUNCTION__ ": WriteShadowStackPte failed\n");
      goto EXIT;
    }
  }
  else if (!strcmp("div", argv[1]))
  {
    if (4 > argc)
    {
      printf("[!] div: Missing arguments");
      goto EXIT;
    }

    if (DivInteger(strtol(argv[2], NULL, 0), strtol(argv[3], NULL, 0)))
    {
      printf("[!] " __FUNCTION__ ": DivInteger failed\n");
      goto EXIT;
    }
  }
  else
  {
    usage();
    goto EXIT;
  }

EXIT:
  CloseExposedDevice();

  return EXIT_SUCCESS;
}
