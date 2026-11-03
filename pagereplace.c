#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Reference string from Question 4(b)
int ref_str[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
int ref_len = 20;

// Helper function to print frame contents
void print_frames(int frames[], int num_frames) {
  printf("[ ");
  for(int i = 0; i < num_frames; i++) {
    if(frames[i] == -1)
      printf("- ");
    else
      printf("%d ", frames[i]);
  }
  printf("]");
}

// ------------------- FIFO ALGORITHM -------------------
int run_fifo(int num_frames, int verbose) {
  int frames[10];
  int faults = 0, hits = 0;
  int fifo_idx = 0; // Pointer to the oldest page frame

  for(int i = 0; i < num_frames; i++) frames[i] = -1;

  if(verbose) printf("\n--- FIFO Algorithm (%d Frames) ---\n", num_frames);

  for(int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int hit = 0;

    // Check if page is already present in frames
    for(int j = 0; j < num_frames; j++) {
      if(frames[j] == page) {
        hit = 1;
        hits++;
        break;
      }
    }

    // Page Fault: Replace the oldest allocated frame in circular order
    if(!hit) {
      faults++;
      frames[fifo_idx] = page;
      fifo_idx = (fifo_idx + 1) % num_frames;
    }

    if(verbose) {
      printf("Ref %d: ", page);
      print_frames(frames, num_frames);
      printf(hit ? " -> HIT\n" : " -> FAULT\n");
    }
  }

  if(verbose) printf("Total Page Faults: %d | Total Page Hits: %d\n", faults, hits);
  return faults;
}

// ------------------- LRU ALGORITHM -------------------
int run_lru(int num_frames, int verbose) {
  int frames[10];
  int last_used[10]; // Tracks last accessed timestamp for each frame index
  int faults = 0, hits = 0;

  for(int i = 0; i < num_frames; i++) {
    frames[i] = -1;
    last_used[i] = -1;
  }

  if(verbose) printf("\n--- LRU Algorithm (%d Frames) ---\n", num_frames);

  for(int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int hit = 0;

    // Check if page is already present in frames
    for(int j = 0; j < num_frames; j++) {
      if(frames[j] == page) {
        hit = 1;
        hits++;
        last_used[j] = i; // Update access timestamp
        break;
      }
    }

    // Page Fault: Replace the frame that has not been used for the longest time
    if(!hit) {
      faults++;
      int lru_idx = 0;
      int min_time = 9999;

      for(int j = 0; j < num_frames; j++) {
        if(frames[j] == -1) { // Empty frame available
          lru_idx = j;
          break;
        }
        if(last_used[j] < min_time) {
          min_time = last_used[j];
          lru_idx = j;
        }
      }

      frames[lru_idx] = page;
      last_used[lru_idx] = i;
    }

    if(verbose) {
      printf("Ref %d: ", page);
      print_frames(frames, num_frames);
      printf(hit ? " -> HIT\n" : " -> FAULT\n");
    }
  }

  if(verbose) printf("Total Page Faults: %d | Total Page Hits: %d\n", faults, hits);
  return faults;
}

// ------------------- OPTIMAL ALGORITHM -------------------
int run_optimal(int num_frames, int verbose) {
  int frames[10];
  int faults = 0, hits = 0;

  for(int i = 0; i < num_frames; i++) frames[i] = -1;

  if(verbose) printf("\n--- OPTIMAL Algorithm (%d Frames) ---\n", num_frames);

  for(int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int hit = 0;

    // Check if page is already present in frames
    for(int j = 0; j < num_frames; j++) {
      if(frames[j] == page) {
        hit = 1;
        hits++;
        break;
      }
    }

    // Page Fault: Replace page that will not be used for the longest period in the future
    if(!hit) {
      faults++;
      int replace_idx = -1;
      int farthest = i;

      for(int j = 0; j < num_frames; j++) {
        if(frames[j] == -1) { // Empty frame available
          replace_idx = j;
          break;
        }

        // Look ahead in the reference string
        int k;
        for(k = i + 1; k < ref_len; k++) {
          if(frames[j] == ref_str[k]) break;
        }

        if(k > farthest) {
          farthest = k;
          replace_idx = j;
        }
      }

      if(replace_idx == -1) replace_idx = 0;
      frames[replace_idx] = page;
    }

    if(verbose) {
      printf("Ref %d: ", page);
      print_frames(frames, num_frames);
      printf(hit ? " -> HIT\n" : " -> FAULT\n");
    }
  }

  if(verbose) printf("Total Page Faults: %d | Total Page Hits: %d\n", faults, hits);
  return faults;
}

// Helper function to print a table row for task (c)
void print_row(char *algo, int frames, int faults) {
  int hits = ref_len - faults;
  int hit_ratio = (hits * 100) / ref_len;
  int fault_ratio = (faults * 100) / ref_len;

  printf("| %s\t | %d     | %d          | %d        | %d%%       | %d%%        |\n",
         algo, frames, faults, hits, hit_ratio, fault_ratio);
}

int
main(int argc, char *argv[])
{
  printf("=== Question 4: Page Replacement Algorithms Simulation ===\n");

  printf("\n=== Tasks (a & b): Detailed 3-Frame Step-by-Step Trace ===\n");
  run_fifo(3, 1);
  run_lru(3, 1);
  run_optimal(3, 1);

  printf("\n=== Task (c): Summary Comparison Table (3, 4, and 5 Frames) ===\n");
  printf("+---------+--------+--------------+------------+-----------+------------+\n");
  printf("| Algo\t | Frames | Page Faults | Page Hits | Hit Ratio | Fault Ratio|\n");
  printf("+---------+--------+--------------+------------+-----------+------------+\n");

  print_row("FIFO", 3, run_fifo(3, 0));
  print_row("FIFO", 4, run_fifo(4, 0));
  print_row("FIFO", 5, run_fifo(5, 0));
  printf("+---------+--------+--------------+------------+-----------+------------+\n");

  print_row("LRU", 3, run_lru(3, 0));
  print_row("LRU", 4, run_lru(4, 0));
  print_row("LRU", 5, run_lru(5, 0));
  printf("+---------+--------+--------------+------------+-----------+------------+\n");

  print_row("Optimal", 3, run_optimal(3, 0));
  print_row("Optimal", 4, run_optimal(4, 0));
  print_row("Optimal", 5, run_optimal(5, 0));
  printf("+---------+--------+--------------+------------+-----------+------------+\n");

  printf("\n--- Observations & Explanation ---\n");
  printf("1. Optimal algorithm yields the lowest number of page faults (9 faults with 3 frames)\n");
  printf("   because it uses future reference string knowledge to replace pages needed farthest in the future.\n");
  printf("2. LRU outperforms FIFO for 3 frames (12 vs 15 faults) by exploiting temporal locality.\n");
  printf("3. As frame size increases (3 -> 4 -> 5), page fault count drops across all algorithms.\n");

  exit(0);
}
