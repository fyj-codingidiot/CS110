##### Exercise1

- Given the way the address was broken down, how big are the pages in this model?

  The offset is 5 bits and the simulator is word addressable,so one page is 32 words.

- How many TLB Hits and Misses did we have for the randomly generated set of ten addresses? What about for Page Hits and Page Faults?

  TLB hits: 2

  TLB miss: 8

  page hit: 0

  page miss: 8

- Did you have any Page Hits? Explain why. Can you think of a set of ten addresses that would result in a Page Hit?

  No.This is because the TLB and physical memory have the same number of entry(frames).So,when we first check the TLB and it hits,  then we will not check the page table and if it doesn't hit, then the page is sure to miss. So,by the analysis above, there can not be any set of ten addresses that have page hit. 

- Explain the process by which we turn a virtual address into a physical address for the very first access (emphasizing on TLB Misses and Page Faults).

  1.Split the virtual address into vpn and offset. 

  2.Use the vpn to find the page.

  3.Check the TLB and it must miss

  4.Check page table and it also must miss

  5.Load the data from disk to physical memory,the first available frame must be 0.

  6.Write TLB and Page table

##### Exercise 2

- Demonstrate that your ten memory accesses results in ten TLB Misses and ten Page Faults. Explain why such behavior occurs.

  `00 20 40 60 80 A0 C0 E0 00 20 `

  This is because every address is new to the TLB(000,001,010,011,100,101,110,111,000,001),so it will always cause a TLB miss,and as we explained in ex1, if TLB misses,the the page table is sure to miss,so it will always cause page fault.

##### Exercise 3

- Explain the single parameter change that would result in ten TLB misses, but **fewer** than ten page faults.

  Enlarge the number of physical memory frames and the physical memory.

