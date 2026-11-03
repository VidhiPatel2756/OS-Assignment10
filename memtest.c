#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("--- Initial Memory Stats ---\n");
  memstats();

  printf("Allocating 10 pages (40960 bytes) via sbrk()...\n");
  sbrk(10 * 4096);

  printf("--- Stats After Allocation ---\n");
  memstats();

  printf("Deallocating 10 pages via sbrk(-40960)...\n");
  sbrk(-10 * 4096);

  printf("--- Stats After Deallocation ---\n");
  memstats();

  exit(0);
}
