#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h> 
#include <stdbool.h>
#include "GCHelper.h"
#include<arpa/inet.h>
#include <assert.h>

// // GLOBAL VARIABLE FOR ROOT SET
// uint64_t * __RootSet_Pointer_STATIC=NULL;           // This will points to the root set, it will always points to the starting of the RootSet
// uint64_t * __RootSet_Pointer_DYNAMIC=NULL;           // This will points to the root set, it will always points to the current free space of RootSet
// uint64_t * __TotalNumber_Of_Objects_In_RootSet=0;    // Counter to keep track of number of root set objects



// Declaration of Pointers 
uint8_t *heap_Start=NULL;                     // This will points to the starting of the Heap 
uint8_t *heap_End=NULL;                       // This will points to the ending of the Heap 
uint8_t *heap_Dynamic_Pointer=NULL;           // This is a dynamic pointer that will always points to the empty space of the heap where we can start allocating objects

uint32_t __size_of_heap=0;
// Heap Management System Pointer
// This Pointer will contain the starting location of the File  that will be passing from Command Line as trace.txt
// This pointer will contain the information regarding various commends to the heap
 uint8_t *heap_Manager=NULL;   

 uint8_t *heap_manager_Before_GC=NULL; // WHEN WE COLLECT THE GARBAGE THE INSTRUCTION FOR THAT COMMAND IS ALREADY READU, BUT CANNOT ALLOCATE
                                      // MEMORY DUE TO INSUFFICIENT MEMORY, I NEED TO GO BACK TO THAT LOCATION WHERE THE PREVIOUS INSTRUCTION WAS POINTED IN THE TRACE FILE AND RERUN THE COMMAND TO ALLOCAT THE OBJECT

// Size of the Trace File
uint32_t traceFileLength=0;

uint32_t objectID=0;                                         // Savint it in 4 Byte       Oj
uint16_t classID=0;                                          // Saving it in 2 Byte       Cj
uint32_t objectPayload_Size_in_Bytes=0;                       // Saving it in 4 Byte       Sx
uint8_t number_Of_Ref_Slot_In_Object=0;                       // Saving it in 1 Byte       Ny
uint32_t parent_Object_ID=0;                                  // Saving it in 4 Byte       Pj
uint32_t slotNumber_In_Reference_Field=0;                     // Saving it in 4 Byte     #k
uint32_t object_To_Delete_From_RootSet=0;                     // For Deletion Only
 
uint32_t  __Garbage_Collection_Run_Counter=0;                // This will keep track of how many times GC runs
uint32_t  __NumberOfUpdatedReferences=0;                     // This is the number of references that were updated as a result of the compacting process. 
                                                              // Note that this also includes updating references in the rootset and classpool.
uint32_t __Total_Live_Objects_GC=0;
uint32_t __Total_Dead_Objects_GC=0;
uint32_t __Total_Number_Of_Garbage_Collection=0;
uint32_t __Total_Number_Of_Pointers_Updated=0;
uint64_t __Total_Free_Space_In_Heap;                           // This will hold the total available free space in heap

uint8_t *__heapPointer_FREE_Space_START=NULL;                    //    __heapPointer_FREE_Space_START-->| 0  | 0|  1 | 
uint8_t *__heapPointer_FREE_Space_Dynamic_Index=NULL;            // | 0  | 0|  1 | <--heapPointer_FREE_SPACE_END

uint8_t *__LastAllocatedMemory_Pointer=NULL;                    // this will always point to the last allocated memory where GC starts

float __Percentage_Of_Total_Free_Heap=0;

uint64_t ProgramCounter=0;