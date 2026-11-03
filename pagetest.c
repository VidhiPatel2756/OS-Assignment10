#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Question 3: Page Table Info & Page Protection Test ===\n\n");

  // Allocate a page
  char *ptr = sbrk(4096);
  ptr[0] = 'A'; // Write to force allocation

  printf("1. Initial Page Status:\n");
  pageinfo((uint64)ptr);

  printf("\n2. Setting Page to READ-ONLY...\n");
  setreadonly((uint64)ptr);

  printf("\n3. Updated Page Status:\n");
  pageinfo((uint64)ptr);

  printf("\n4. Attempting to write to read-only page...\n");
  printf("(Expect Store Page Fault / process kill by kernel)\n\n");

  // This write attempt triggers hardware trap (scause 15: Store Page Fault)
  ptr[0] = 'B';

  printf("ERROR: Successfully wrote to read-only page!\n");
  exit(0);
}
