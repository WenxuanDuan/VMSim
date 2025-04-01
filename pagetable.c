#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "pagetable.h"
#include "swapfile.h"
#include "process.h"
#include "global.h"


/*******************************************************************************
 * GLOBALS (you will use this in the translate() function)
 */

/*
 * Current page table register, used by the processor to find the running
 * processes page table. The scheduler will update this when it dispatches
 * a new process, so you may assume it always points to the page table for
 * the /current/ process. A page table is an array of pte_t.
 */

pte_t* ptbr; /* Page Table Base Register on the CPU */



/*******************************************************************************
 * Get the page number from a virtual address.
 * @param addr The virtual address.
 * @return     The virtual page number (VPN) for this addres.
 */

vpn_t get_vpn(vaddr_t virt_addr) {
  /* calculate the number of bits of page size */
  int page_bits = 0;
  unsigned tmp = page_size;
  while (tmp > 1)
  {
    page_bits += 1;
    tmp = tmp / 2;
  }

  /* calculate the vpn_mask */
  int vpn_bits = 16 - page_bits;
  uint16_t vpn_mask = 0;
  for (int i = 1; i <= vpn_bits; i ++)
    vpn_mask = (vpn_mask << 1) + 1;
  vpn_mask = vpn_mask << page_bits;

  /* calculate the vpn */
  vpn_t vpn = (virt_addr & vpn_mask) >> page_bits;

	return vpn;
}

/*******************************************************************************
 * Get the offset for a particular address.
 * @param addr The virtual address.
 * @return     The offset into a page for the given virtual address.
 */

offset_t get_offset(vaddr_t virt_addr) {
  /* calculate the number of bits of page size */
  int page_bits = 0;
  unsigned tmp = page_size;
  while (tmp > 1)
  {
    page_bits += 1;
    tmp = tmp / 2;
  }

  /* calculate the offset_mask */ 
  int vpn_bits = 16 - page_bits;
  uint16_t offset_mask = 0 << vpn_bits;
  for (int i = 1; i <= page_bits; i ++)
    offset_mask = (offset_mask << 1) + 1;

  /* calculate the offset*/
  offset_t offset = virt_addr & offset_mask;

  return offset;
}


/*******************************************************************************
 * Create a physical address from a PFN and an offset.
 * @param The physical frame number (PFN).
 * @param The offset into the page.
 * @return The physical address.
 */

paddr_t create_paddr(pfn_t pfn, offset_t offset) {
  /* calculate the number of bits of page size */
  int page_bits = 0;
  unsigned tmp = page_size;
  while (tmp > 1)
  {
    page_bits += 1;
    tmp = tmp / 2;
  }
  
  /* calculate the physical address */
  paddr_t physAddr = (pfn << page_bits) | offset;

  return physAddr;
}


/*******************************************************************************
 * Looks up an address in the current page table and returns the translated
 * physical address. If the entry for the given page is not valid, traps to
 * the OS (simulated by calling the pagefault_handler function).
 *
 * @param virt_addr The virtual address to translate.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical address we are accessing.
 */

paddr_t translate(vaddr_t virt_addr, int write) {
  vpn_t vpn;
  offset_t offset;
  pfn_t pfn;
  paddr_t phys_addr;

  /* Sanity Check */
  assert(ptbr != NULL); /* do NOT modify */

  vpn = get_vpn(virt_addr);
  offset = get_offset(virt_addr);

  printf("Virtual Address %5.5hu (VPN: %5.5hu OFFSET: %5.5hu)\n",
         virt_addr, vpn, offset); /* do NOT modify this output statement */
         
  if (ptbr[vpn].valid == 0)
    pagefault_handler(vpn);
  else
    ptbr[vpn].used = 1;

  if (write == 1)
    ptbr[vpn].dirty = 1;

  /* Complete the translation */
  pfn = ptbr[vpn].pfn;
  phys_addr = create_paddr(pfn, offset);

  printf("Physical Address %5.5hu (PFN: %5.5hu OFFSET: %5.5hu)\n",
         phys_addr, pfn, offset); /* do NOT modify this output statement */

  return phys_addr;
}
