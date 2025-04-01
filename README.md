Name: Wenxuan Duan  
NUID: 002839590

# Virtual Memory Simulator

## Introduction
The Virtual Memory Simulator(vm-sim) is a program for simulating virtual memory system. It could translate virtual address to physical address. If there are tlb missing or page fault during accessing, this simulator will also help us to handle it. 

## Getting Started
These instructions will help you set up and execute this program.

### Compiling
To compile this program, follow these steps:  
1. Open a terminal window.
2. Connect to login.khoury.northeastern.edu using your own account.   
3. Navigate to the directory containing all the simulator code files.
4. Using the following command to compile the program:  
`g++ -g -Wall -pedantic -std=c++11 *.c -o vm-sim`  
This will generate an executable file named **'vm-sim'**.  

## Usage  
The vm-sim supports various options to customize your execution. It takes the name of a memory trace file as an argument. There are also several optional command line options that you can play around with, such as changing the memory size and the page size. The defaults are memory size = 16 and page size = 2.

### Command Line Options
Usage: ./vm-sim [-p <page_size>] [-m <memory_size>] [-j <num_jobs>] [-h] <file>
- '-p <page_size>': Set size of VM pages in power of two
- '-m <memory_size>': Set size of physical memory in powers of two
- '-j <num_jobs>': Set maximum number of jobs
- '-h': Display this message.

### Input File Format  
The input file should be well formatted:
- The first several lines contains @, process number and name, which indicates that the process id and its corresponding process name. For example, "@ 0 init" means that process id 0 is assigned to init process. 
- After that, every line has four columns (separated by blanks) as "access-action process-id virtual-address data". For example, "s 0 1 4" means that process whose id is 0 STORED 4 as data in virtual address 00001.
```
An example for input file:

basic:
@ 0 init
@ 1 mozilla
s 0 1 4
s 0 2 8
s 0 3 9
s 0 4 10
l 0 2 8
s 1 1 6
s 1 2 9
s 1 3 42
s 1 4 11
l 1 2 9
l 1 1 6
l 0 3 9
l 0 2 8
```

### Command Examples  
To show all command options, using the following command:  
`./vm-sim -h`

To run the simulator with the file named by everything, using the following command:  
`./vm-sim everything`

To run the simularor with page size 4 and the file named by basic, using the following command:  
`./vm-sim -p 2 basic`

### Results  
A successful execution will display the memory access statistics like below:

```
./vm-sim -p 2 ../workloads/basic
page_size = 4 bytes
mem_size = 16 bytes
max_jobs = 8 jobs
filename = ../workloads/basic

Forking new process init.  Assigning pid: 0.

Forking new process mozilla.  Assigning pid: 1.

===================================================
Switching to process: init. Corresponding pid: 0.
===================================================

Virtual Address 00001 (VPN: 00000 OFFSET: 00001)
****PAGE FAULT has occurred at VPN 0. Evicting (PFN 0 VPN 0) as victim.
**** using free frame PFN 0
Physical Address 00001 (PFN: 00000 OFFSET: 00001)
STORED 004 as data in virtual address 00001.
Complete

Virtual Address 00002 (VPN: 00000 OFFSET: 00002)
Physical Address 00002 (PFN: 00000 OFFSET: 00002)
STORED 008 as data in virtual address 00002.
Complete

Virtual Address 00003 (VPN: 00000 OFFSET: 00003)
Physical Address 00003 (PFN: 00000 OFFSET: 00003)
STORED 009 as data in virtual address 00003.
Complete

Virtual Address 00004 (VPN: 00001 OFFSET: 00000)
****PAGE FAULT has occurred at VPN 1. Evicting (PFN 1 VPN 0) as victim.
**** using free frame PFN 1
Physical Address 00004 (PFN: 00001 OFFSET: 00000)
STORED 010 as data in virtual address 00004.
Complete

Virtual Address 00002 (VPN: 00000 OFFSET: 00002)
Physical Address 00002 (PFN: 00000 OFFSET: 00002)
LOADED 008 as data from virtual address 00002.
Complete

===================================================
Switching to process: mozilla. Corresponding pid: 1.
===================================================

Virtual Address 00001 (VPN: 00000 OFFSET: 00001)
****PAGE FAULT has occurred at VPN 0. Evicting (PFN 2 VPN 0) as victim.
**** using free frame PFN 2
Physical Address 00009 (PFN: 00002 OFFSET: 00001)
STORED 006 as data in virtual address 00001.
Complete

Virtual Address 00002 (VPN: 00000 OFFSET: 00002)
Physical Address 00010 (PFN: 00002 OFFSET: 00002)
STORED 009 as data in virtual address 00002.
Complete

Virtual Address 00003 (VPN: 00000 OFFSET: 00003)
Physical Address 00011 (PFN: 00002 OFFSET: 00003)
STORED 042 as data in virtual address 00003.
Complete

Virtual Address 00004 (VPN: 00001 OFFSET: 00000)
****PAGE FAULT has occurred at VPN 1. Evicting (PFN 3 VPN 0) as victim.
**** using free frame PFN 3
Physical Address 00012 (PFN: 00003 OFFSET: 00000)
STORED 011 as data in virtual address 00004.
Complete

Virtual Address 00002 (VPN: 00000 OFFSET: 00002)
Physical Address 00010 (PFN: 00002 OFFSET: 00002)
LOADED 009 as data from virtual address 00002.
Complete

Virtual Address 00001 (VPN: 00000 OFFSET: 00001)
Physical Address 00009 (PFN: 00002 OFFSET: 00001)
LOADED 006 as data from virtual address 00001.
Complete

===================================================
Switching to process: init. Corresponding pid: 0.
===================================================

Virtual Address 00003 (VPN: 00000 OFFSET: 00003)
Physical Address 00003 (PFN: 00000 OFFSET: 00003)
LOADED 009 as data from virtual address 00003.
Complete

Virtual Address 00002 (VPN: 00000 OFFSET: 00002)
Physical Address 00002 (PFN: 00000 OFFSET: 00002)
LOADED 008 as data from virtual address 00002.
Complete


-----------------------
Statistics:
-----------------------
Reads:                    5
Writes:                   8
Page Faults:              4
Page Fault Rate:   0.307692
```
