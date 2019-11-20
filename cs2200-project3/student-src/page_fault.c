#include "pagesim.h"
#include "paging.h"
#include "stats.h"
#include "swapops.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*  --------------------------------- PROBLEM 6
   -------------------------------------- Checkout PDF section 7 for this
   problem

    Page fault handler.

    When the CPU encounters an invalid address mapping in a page table,
    it invokes the OS via this handler.

    Your job is to put a mapping in place so that the translation can
    succeed. You can use free_frame() to make an available frame.
    Update the page table with the new frame, and don't forget
    to fill in the frame table.

    When you map a frame, you should update its LRU timestamp.

    Lastly, you must fill your newly-mapped page with data. If the page
    has never mapped before, just zero the memory out. Otherwise, the
    data will have been swapped to the disk when the page was
    evicted. Call swap_read() to pull the data back in.

    HINTS:
         - You will need to use the global variable current_process when
           setting the frame table entry.

    ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    /* First, split the faulting address and locate the page table entry.
       Remember to keep a pointer to the entry so you can modify it later. */

    vpn_t vpn = vaddr_vpn(address);
    pte_t *ptEntry = (pte_t *)(mem + (PTBR * PAGE_SIZE)) + vpn;

    /* It's a page fault, so the entry obviously won't be valid. Grab
       a frame to use by calling free_frame(). */
    pfn_t newFrame = free_frame();
    /* Update the page table entry. Make sure you set any relevant values. */
    ptEntry->valid = TRUE;
    ptEntry->pfn = newFrame;
    ptEntry->dirty = FALSE;
    /* Update the frame table. Make sure you set any relevant values. */
    frame_table[newFrame].mapped = TRUE;
    frame_table[newFrame].process = current_process;
    frame_table[newFrame].vpn = vpn;

    /* Update the timestamp of the appropriate frame table entry with the
     * provided get_current_timestamp function. */
    frame_table[newFrame].timestamp = get_current_timestamp();
    /* Initialize the page's memory. On a page fault, it is not enough
     * just to allocate a new frame. We must load in the old data from
     * disk into the frame. If there was no old data on disk, then
     * we need to clear out the memory (why?).
     *
     * 1) Get a pointer to the new frame in memory.
     * 2) If the page has swap set, then we need to load in data from memory
     *    using swap_read().
     * 3) Else, just zero the page's memory. If the page is later written
     *    back, swap_write() will automatically allocate a swap entry.
     */

    // u_int8_t newFrameInMem = mem[newFrame << OFFSET_LEN];
    u_int8_t *newFrameInMem = mem + (newFrame * PAGE_SIZE);
    if (ptEntry->swap) {
        swap_read(ptEntry, newFrameInMem);
    } else {
        memset(newFrameInMem, 0, PAGE_SIZE);
    }
}

#pragma GCC diagnostic pop
