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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GCHelper.h"


#define CAPACITY_ROOT_SET 1000 // Size of the Hash Table

// GLOBAL VARIABLE FOR ROOT SET
uint64_t * __RootSet_Pointer_STATIC=NULL;           // This will points to the root set, it will always points to the starting of the RootSet
uint64_t * __RootSet_Pointer_DYNAMIC=NULL;           // This will points to the root set, it will always points to the current free space of RootSet
uint64_t __TotalNumber_Of_Objects_In_RootSet=0;    // Counter to keep track of number of root set objects


//____________________________________________________________________________________
//Add Objects to RootSet
//_________________________________________________________________________________
void Add_Object_To_RootSet(uint64_t object_ID, uint64_t object_Address)
{
   long obj_ID_LONG=(long)object_ID;
   long obj_Address_LONG=(long) object_Address;

    if((obj_ID_LONG!=-1) && (obj_Address_LONG!=-1))                                     // ONLY ADD POSTION NUMBER, WE WILL ADD -1 WHEN WE DELETE
    {

    *__RootSet_Pointer_DYNAMIC=object_ID;                                          // Mark Bit         1 Byte
    __RootSet_Pointer_DYNAMIC=__RootSet_Pointer_DYNAMIC+1; 


    *__RootSet_Pointer_DYNAMIC=object_Address;                                      // Number of Slots  1 Byte
    __RootSet_Pointer_DYNAMIC=__RootSet_Pointer_DYNAMIC+1;  
    
    __TotalNumber_Of_Objects_In_RootSet++;
    __TotalNumber_Of_Objects_In_RootSet++;
    }
 

}


//____________________________________________________________________________________
//Search Objects In RootSet
//_________________________________________________________________________________
uint64_t Search_Object_In_RootSet(uint64_t object_ID)
{
 uint64_t i=0;
 
 uint64_t objID_In_RootSet=0;
 uint64_t objAddress_In_RootSet=0; 
 long objID_In_RootSet_LONG=0;
 long objAddress_In_RootSet_LONG=0;

 uint64_t * rootSet_Pointer=__RootSet_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 

   
    for(i=0;i<__TotalNumber_Of_Objects_In_RootSet;i=i+2)
    {    
     
      objID_In_RootSet_LONG=(long)(*(rootSet_Pointer+i));           // Check negative value
      objAddress_In_RootSet_LONG=(long)(*(rootSet_Pointer+i+1));
          
      if((objID_In_RootSet_LONG!=-1) && (objAddress_In_RootSet_LONG!=-1))                                
      {           
          
          objID_In_RootSet=*(rootSet_Pointer+i);
          if(object_ID==objID_In_RootSet)
          {
              objAddress_In_RootSet=*(rootSet_Pointer+i+1);       // Address saved after one object
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }
    return objAddress_In_RootSet;
}


//____________________________________________________________________________________
//Delete Objects from RootSet
//_________________________________________________________________________________
void Delete_Object_From_RootSet(uint64_t object_ID)
{
    uint64_t i=0; 
    uint64_t objID_In_RootSet=0;
    uint64_t objAddress_In_RootSet=0; 
    long objID_In_RootSet_LONG=0;
    long objAddress_In_RootSet_LONG=0;

    uint64_t * rootSet_Pointer=__RootSet_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION    

    for(i=0;i<__TotalNumber_Of_Objects_In_RootSet;i=i+2)
    {    
     
      objID_In_RootSet_LONG=(long)(*(rootSet_Pointer+i));           // Check negative value
     objAddress_In_RootSet_LONG=(long)(*(rootSet_Pointer+i+1)); 
          
      if((objID_In_RootSet_LONG!=-1) && (objAddress_In_RootSet_LONG!=-1))                       
      {           
         objID_In_RootSet=(*(rootSet_Pointer+i));

          if(object_ID==objID_In_RootSet)
          {
              *(rootSet_Pointer+i)=-1;         // Assign -1 to ID
              *(rootSet_Pointer+i+1)=-1;       // Assign -1 to Address
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }

}

void PrintRootSet()
{
    green();
    printf("\n PrintRootSet");
   
     uint64_t * rootSet_Pointer=__RootSet_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION   

     uint64_t loopCounter;

    for(loopCounter=0;loopCounter<__TotalNumber_Of_Objects_In_RootSet;loopCounter=loopCounter+2)
    {

       printf("\nIndex [%d]  |\t\tID     : %ld\t\t|          Address: %ld (%p) \n",loopCounter,*(rootSet_Pointer+loopCounter),(rootSet_Pointer+loopCounter),(rootSet_Pointer+loopCounter));
       printf("\nIndex [%d]  |\t\tADDRESS: %ld\t\t|          Address: %ld (%p) \n",loopCounter+1,*(rootSet_Pointer+loopCounter+1),(rootSet_Pointer+loopCounter+1),(rootSet_Pointer+loopCounter+1));
    }
     reset();

}