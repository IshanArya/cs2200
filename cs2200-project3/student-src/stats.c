#include "stats.h"
#include "paging.h"

/* The stats. See the definition in stats.h. */
stats_t stats;

/*  --------------------------------- PROBLEM 10
   -------------------------------------- Checkout PDF section 10 for this
   problem

    Calculate any remaining statistics to print out.

    You will need to include code to increment many of these stats in
    the functions you have written for other parts of the project.

    Use this function to calculate any remaining stats, such as the
    average access time (AAT).

    You may find the #defines in the stats.h file useful.
    -----------------------------------------------------------------------------------
*/
void compute_stats() {
    double totalDiskPageReadTime =
        (double)(DISK_PAGE_READ_TIME * stats.page_faults);
    double totalMemoryReadTime =
        (double)((stats.writes + stats.reads) * MEMORY_READ_TIME);
    double totalWriteTime = (double)(stats.writebacks * DISK_PAGE_WRITE_TIME);
    double totalTime =
        totalDiskPageReadTime + totalMemoryReadTime + totalWriteTime;
    stats.aat = totalTime / (double)stats.accesses;
}
