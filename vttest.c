#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Question 2: Virtual-to-Physical Address Translation Test ===\n\n");

  char *ptr = sbrk(4096);
  ptr[0] = 'X'; // Force allocation and page table mapping

  uint64 va1 = (uint64)ptr;
  uint64 pa1 = vtop(va1);

  if (pa1 != (uint64)-1) {
    // Cast to (void*) to use %p safely without compiler warnings
    printf("[PASS] Allocated Page Start VA: %p -> PA: %p\n", (void*)va1, (void*)pa1);
  }

  uint64 va2 = va1 + 0x123;
  uint64 pa2 = vtop(va2);

  if (pa2 != (uint64)-1) {
    printf("[PASS] Address with Offset VA: %p -> PA: %p\n", (void*)va2, (void*)pa2);
  }

  uint64 va_invalid = 0x7FFFFFFF0000;
  printf("\nTesting Invalid / Unmapped Address: %p\n", (void*)va_invalid);
  uint64 pa_invalid = vtop(va_invalid);

  if (pa_invalid == (uint64)-1) {
    printf("[PASS] Correctly identified unmapped virtual address.\n");
  }

  exit(0);
}
