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
#include "GCHelper.h"
//_______________________________________________________________________________________________________________________________
// This function will check whether there are any free space left in the heap for allocation
// Othereise there will be no allocation
// This answer will decide the garbage collection
//________________________________________________________________________________________________________________________________
bool Validate_ObjectAllocation(uint8_t* lastAllocatedMemory, uint32_t TotalSizeOfObject,uint8_t * heap_End,uint32_t objectID)
{
     uint8_t *temp_mem_start=lastAllocatedMemory+TotalSizeOfObject;
     uint64_t memoryRequired=(uint64_t)temp_mem_start;     
     uint64_t memoryAvailable=(uint64_t)heap_End;
   

     if(memoryRequired>memoryAvailable)
     {
       uint64_t totalMemoryAllocated=(uint64_t)lastAllocatedMemory-(uint64_t)heap_Start;
       uint64_t totalFreeSpaceRemained=(uint64_t)heap_End- (uint64_t)lastAllocatedMemory;
     //  yellow();
      // printf("\nCannot allocate memory at  : %ld (%p) for Object ID: %d (size: %ld Bytes)as it excedes the size of heap : %ld\n",lastAllocatedMemory,lastAllocatedMemory,objectID,TotalSizeOfObject,__size_of_heap); 
      // red();
      // printf("Total Memory Allocated %ld Bytes  .Total Free Memory for allocation %ld\n",totalMemoryAllocated,totalFreeSpaceRemained);      
     //  reset();      
       return false;
     }
     else
     {
       blue();
      // printf("\nMemory allocation successful at location : %ld (%p)\n",memoryStart,memoryStart);           
       reset();  
       return true;
     }
}

//___________________________________________________________________________________________________________________________
// This will validate the memory allocation
//___________________________________________________________________________________________________________________________
void Validate_HeapAllocation(uint8_t* pointerToMemory)
{

     if(pointerToMemory==NULL)
     {
       yellow();
       printf("\nMemory allocation failed at location : %ld (%p)\n",pointerToMemory,pointerToMemory);
       exit(EXIT_FAILURE);
       reset();
     }
     else
     {
         // green();
        // printf("\nMemory allocation successful at location : %ld (%p)\n",pointerToMemory,pointerToMemory);           
         //reset();  
     }    
}
