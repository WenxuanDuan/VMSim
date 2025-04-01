#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "swapfile.h"
#include "process.h"
#include "global.h"

pfn_t get_free_frame(void);


/*******************************************************************************
 * Page fault handler. When the CPU encounters an invalid address mapping in a
 * process' page table, it invokes the CPU via this handler. The OS then
 * allocates a physical frame for the requested page (either by using a free
 * frame or evicting one), changes the process' page table to reflect the
 * mapping and then restarts the interrupted process.
 *
 * @param vpn The virtual page number requested.
 */

void pagefault_handler(vpn_t request_vpn) {
  pfn_t frame_pfn;
  vpn_t victim_vpn;
  pcb_t *victim_pcb;

  /* Sanity Check */
  assert(ptbr != NULL); /* do NOT modify */

  /* Add 1 to the Page Fault count */
  count_pagefaults++; /* do NOT modify */
  
  frame_pfn = get_free_frame();

  assert(frame_pfn < CPU_NUM_FRAMES); /* make sure the frame no. is valid */

  /* Find the victim pcb and check whether it is a free frame*/
  victim_pcb = frametable[frame_pfn].pcb;
  if (victim_pcb)
    victim_vpn = frametable[frame_pfn].vpn;
  else
    victim_vpn = 0;

  printf("****PAGE FAULT has occurred at VPN %u. Evicting (PFN %u VPN %u) as victim.\n",
         request_vpn, frame_pfn, victim_vpn); /* do NOT modify this output statement */

  /* If there is a victim frame, then evict it */
  if (victim_pcb) {
    printf("**** evicting victim page VPN %hu from frame PFN %hu\n", victim_vpn, frame_pfn);

    /* Mark victim page as invalid */
    victim_pcb->pagetable->valid = 0;

    /* If victim page is dirty, save it to the disk */
    if (victim_pcb->pagetable->dirty)
    {
      save_page_to_disk (victim_pcb->pagetable->pfn, victim_vpn, victim_pcb->pid);
      victim_pcb->pagetable->dirty = 0;
    }

  } else {
    printf("**** using free frame PFN %hu\n", frame_pfn);
  }

  /* Retreive the page from disk */
  load_page_from_disk(frame_pfn, request_vpn, current->pid);

  /* Update the faulting process page table; set the pfn, valid bit, dirty bit, used bit */
  ptbr[request_vpn].pfn = frame_pfn;
  ptbr[request_vpn].valid = 1;
  ptbr[request_vpn].used = 0;

  /* Update the frame table */
  frametable[frame_pfn].vpn = request_vpn;
  if (victim_pcb) {
    frametable[frame_pfn].pcb->pid = current->pid;
    frametable[frame_pfn].pcb->pagetable = ptbr + request_vpn;
  }
  else
  {
    pcb_t *current_pcb = new pcb_t();
    current_pcb->pid = current->pid;
    current_pcb->pagetable = ptbr + request_vpn;
    frametable[frame_pfn].pcb = current_pcb;
  }

}

/*******************************************************************************
 * Finds a free physical frame. If none are available, uses a clock sweep
 * algorithm to find a used frame for eviction.
 *
 * @return The physical frame number of a free (or evictable) frame.
 */

pfn_t get_free_frame(void) {
   static pfn_t next_frame = 0;
   pfn_t victim_frame, i;
   pte_t* pt;

  // For debugging
  // printf("frame\tpid\tvpn\tpfn\tvalid\tdirty\tused\n");
  // for (i = 0; i < CPU_NUM_FRAMES; i++)
  // {
  //   if (frametable[i].pcb != NULL)
  //   {
  //     printf("%u\t%u\t%u\t%u\t%u\t%u\t%u\n", i, frametable[i].pcb->pid, frametable[i].vpn, frametable[i].pcb->pagetable->pfn, frametable[i].pcb->pagetable->valid, frametable[i].pcb->pagetable->dirty, frametable[i].pcb->pagetable->used);
  //   }
  // }
    
   /* See if there are any free frames */
   /* do NOT modify this for loop */
   for (i = 0; i < CPU_NUM_FRAMES; i++)
      if (frametable[i].pcb == NULL) {
         return i;
      }

   /* Implement the Clock Sweep Algorithm */
   assert(frametable[next_frame].pcb != NULL); /* should be NO free frames */
   while (1) {
      pt = frametable[next_frame].pcb->pagetable;
      if (pt->used == 0)   
         break;
      else {
        pt->used = 0;
        next_frame = (next_frame + 1) % CPU_NUM_FRAMES;
      }
   }
   
   /* AT this point, next_frame now points to the chosen victim (which has used bit == 0) */
   victim_frame = next_frame;                      /* assign the chosen victim to frame (temp variable) */
   next_frame = (next_frame + 1) % CPU_NUM_FRAMES; /* increment next_frame to point to next one to look at */
   /* victim_frame now points to the chose victim, and
    * next_frame points to where to resume for the next eviction */

   return victim_frame;
}
