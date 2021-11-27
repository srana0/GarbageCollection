#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h> 
#include<stdbool.h>
#include "GCHelper.h"
#include<arpa/inet.h>
#include<assert.h>



// Gn LOy DOz HF%x URq
// Where:
// n is the garbage collection # - starting at 1 and incrementing with each GC that 
// occurs.
// y is the number of live objects after the garbage collection is complete.
// z is the number of dead objects that was collected from the GC process. Note: In 
// order to know the number of dead objects you need to track the number of objects 
// allocated and subtract the number of ones that remain alive!
// x is the percentage of the total heap that is free for new objects to be allocated.
// q is the number of references that were updated as a result of the compacting process. 
// Note that this also includes updating references in the rootset and classpool

void PrintGarbageCollectionTrace(int garbageCollectionNumber, int numberOfLiveObjects, int numberOfDeadObjects,double percentOfTotalFeeHeap, int numberOfUpdatedReferences)
{
  printf("G%d\tLO%d\tDO%d\tHF%%%lf\tUR%d\n",garbageCollectionNumber,numberOfLiveObjects,numberOfDeadObjects,percentOfTotalFeeHeap,numberOfUpdatedReferences);
}




//_______________________________________________________________________________________________________________________________
// This function will print the trace file
//_______________________________________________________________________________________________________________________________
void PrintTraceFile(uint8_t *heap_Manager,uint32_t traceFileLength)
{
reset();
  int loopCounter=0;
  uint8_t* filePointer=heap_Manager;
 
  // Printing the File 
  for(loopCounter=0;loopCounter<traceFileLength;loopCounter++)
  { 
  //printf("%hhu\t",*filePointer); 
    printf("%c",*filePointer); 
  filePointer++;
  }
 // printf("\n"); 
}


//____________________________________________________________________________________________________________________________
// Print the Object Content
//____________________________________________________________________________________________________________________________
void PrintObjectContent(uint64_t object_address)
{
     int loopCounter;
     uint8_t *object_start_address=NULL;
     object_start_address=object_start_address+object_address;
     uint8_t number_Of_Slots_for_Ref=Get_No_Of_Ref_Slots_From_Object_Address(object_address);
    printf("\n\n\n\n");
    printf("\nMark Bit                     |\t\t%d\t\t| Address: %ld (%p)\n",*(object_start_address),object_start_address,object_start_address);
    object_start_address=object_start_address+1;
    printf("\nNumber of Slots for Reference|\t\t%d\t\t| Address: %ld (%p)\n",*(object_start_address),object_start_address,object_start_address);
    object_start_address=object_start_address+1; 
    uint32_t *temp_ObjectID=(uint32_t *)object_start_address;
    printf("\nObject ID                    |\t\t%d\t\t| Address: %ld (%p)\n",*(temp_ObjectID),temp_ObjectID,temp_ObjectID);
    object_start_address=object_start_address+4;
    uint16_t * temp_ClassID=(uint16_t *)object_start_address;  
    printf("\nClass ID                     |\t\t%d\t\t| Address: %ld (%p) \n",*(temp_ClassID),temp_ClassID,temp_ClassID);
    object_start_address=object_start_address+2;
    uint64_t * class_Address=(uint64_t *)object_start_address;
    printf("\nClass Address                |\t\t%ld\t\t| Address: %ld (%p) \n",*(class_Address),class_Address,class_Address);
    object_start_address=object_start_address+8;
    uint64_t * heapPointer_Type_Long=(uint64_t *) object_start_address; 
    for(loopCounter=0;loopCounter<number_Of_Slots_for_Ref;loopCounter++)
    {

       printf("\nReference To Object [%d]  |\t\t%ld\t\t| Address: %ld (%p) \n",loopCounter,*(heapPointer_Type_Long+loopCounter),(heapPointer_Type_Long+loopCounter),(heapPointer_Type_Long+loopCounter));
    }

}


//____________________________________________________________________________________________________________________________
// Print the CLASS Content
//____________________________________________________________________________________________________________________________
void PrintClassContent(uint64_t class_start_Address)
{
     int loopCounter;
     uint8_t *class_start_address_Pointer=NULL;
     class_start_address_Pointer=class_start_address_Pointer+class_start_Address;

     // Change the pointer to hold 64 Bytes
     uint64_t * class_Pointer_64Bit=( uint64_t *) class_start_address_Pointer;

    printf("\n\n\n");
    printf("\nClass ID                    |\t\t%d\t\t| Address: %ld (%p)\n",*(class_Pointer_64Bit),class_Pointer_64Bit,class_Pointer_64Bit);
    class_Pointer_64Bit=class_Pointer_64Bit+1;
    printf("\nSize of Object              |\t\t%d\t\t| Address: %ld (%p)\n",*(class_Pointer_64Bit),class_Pointer_64Bit,class_Pointer_64Bit);
    class_Pointer_64Bit=class_Pointer_64Bit+1;
    for(loopCounter=0;loopCounter<16;loopCounter++)
    {
       printf("\nReference To Object [%d]|\t\t%ld\t\t| Address: %ld (%p) \n",loopCounter,*(class_Pointer_64Bit+loopCounter),(class_Pointer_64Bit+loopCounter),(class_Pointer_64Bit+loopCounter));
    }

}



//____________________________________________________________________________________________________________________________
// Colors to  Print in the console
//____________________________________________________________________________________________________________________________
void red () { printf("\033[1;31m");}
void yellow (){ printf("\033[1;33m");}
void green () {printf("\033[0;32m");}
void blue () {printf("\033[0;34m");}
void reset () {printf("\033[0m");}


//______________________________________________________________________________________________________________________________
//This will print the header
//______________________________________________________________________________________________________________________________
void Printheader(char *heading)
{
    red();
    printf("\n____________________________________________________________________________________________________________________________\n");
    green();
    printf("%s\n", heading);
    reset();
    yellow();
   printf("____________________________________________________________________________________________________________________________\n");
   reset();
}


//____________________________________________________________________________________________________________________________
// THIS METHOD WILL PRINT THE OPCODE
//____________________________________________________________________________________________________________________________
void PrintOpcode(char commandCode,char *command_Name)
{
    yellow();
    printf("\n[%ld ]COMMAND %c", ProgramCounter,commandCode);
    reset();
    green();
    printf(" %s", command_Name);
    reset(); 
}

