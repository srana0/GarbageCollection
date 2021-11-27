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


#define CAPACITY_OBJECT_POOL 50000 // Size of the Hash Table

// GLOBAL VARIABLE FOR ROOT SET
uint64_t * __ObjectPool_Pointer_STATIC=NULL;           // This will points to the root set, it will always points to the starting of the ObjectPool
uint64_t * __ObjectPool_Pointer_DYNAMIC=NULL;           // This will points to the root set, it will always points to the current free space of ObjectPool
uint64_t __TotalNumber_Of_Objects_In_ObjectPool=0;    // Counter to keep track of number of root set objects



//____________________________________________________________________________________
//Add Objects to ObjectPool
//_________________________________________________________________________________
void Add_Object_To_ObjectPool(uint64_t object_ID, uint64_t object_Address)
{
 long object_ID_LONG=(long)object_ID;
 long object_Address_LONG=(long)object_Address;

    if((object_ID_LONG!=-1) && (object_Address_LONG!=-1))                                     // ONLY ADD POSTION NUMBER, WE WILL ADD -1 WHEN WE DELETE
    {

    *__ObjectPool_Pointer_DYNAMIC=object_ID;                                          // Mark Bit         1 Byte
    __ObjectPool_Pointer_DYNAMIC=__ObjectPool_Pointer_DYNAMIC+1; 


    *__ObjectPool_Pointer_DYNAMIC=object_Address;                                      // Number of Slots  1 Byte
    __ObjectPool_Pointer_DYNAMIC=__ObjectPool_Pointer_DYNAMIC+1;  
    
    __TotalNumber_Of_Objects_In_ObjectPool++;  
    __TotalNumber_Of_Objects_In_ObjectPool++;
    } 

}


//____________________________________________________________________________________
//Search Objects In ObjectPool
//_________________________________________________________________________________
uint64_t Search_Object_In_ObjectPool(uint64_t object_ID)
{
 uint64_t i=0;
 
 uint64_t objID_In_ObjectPool=0;
 uint64_t objAddress_In_ObjectPool=0; 
 long objID_In_ObjectPool_LONG=0;

 uint64_t * ObjectPool_Pointer=__ObjectPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
   

    for(i=0;i<__TotalNumber_Of_Objects_In_ObjectPool;i=i+2)
    {    
     
      objID_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer+i));           // Check negative value
      objID_In_ObjectPool=*(ObjectPool_Pointer+i);
          
      if(objID_In_ObjectPool_LONG!=-1)                           
      {           
          if(object_ID==objID_In_ObjectPool )
          {
              objAddress_In_ObjectPool=*(ObjectPool_Pointer+i+1);       // Address saved after one object
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              return objAddress_In_ObjectPool;
 }


//____________________________________________________________________________________
//Delete Objects from ObjectPool
//_________________________________________________________________________________
void Delete_Object_From_ObjectPool(uint64_t object_ID)
{
    uint64_t i=0; 
    uint64_t objID_In_ObjectPool=0;
    uint64_t objAddress_In_ObjectPool=0; 
    long objID_In_ObjectPool_LONG=0;

    uint64_t * ObjectPool_Pointer=__ObjectPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION    

    for(i=0;i<__TotalNumber_Of_Objects_In_ObjectPool;i=i+2)
    {    
     
      objID_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer+i));           // Check negative value
      objID_In_ObjectPool=*(ObjectPool_Pointer+i);
          
      if(objID_In_ObjectPool_LONG!=-1)                           
      {           
          if(object_ID==objID_In_ObjectPool)
          {
              *(ObjectPool_Pointer+i)=-1;         // Assign -1 to ID
              *(ObjectPool_Pointer+i+1)=-1;       // Assign -1 to Address
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }

}

void PrintObjectPool()
{
    red();
    printf("\nPrintObjectPool");
   
     uint64_t * ObjectPool_Pointer=__ObjectPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION   

     uint64_t loopCounter;

    for(loopCounter=0;loopCounter<__TotalNumber_Of_Objects_In_ObjectPool;loopCounter=loopCounter+2)
    {

       printf("\nIndex [%d]  |\t\tID     : %ld\t\t|          Address: %ld (%p) \n",loopCounter,*(ObjectPool_Pointer+loopCounter),(ObjectPool_Pointer+loopCounter),(ObjectPool_Pointer+loopCounter));
       printf("\nIndex [%d]  |\t\tADDRESS: %ld\t\t|          Address: %ld (%p) \n",loopCounter+1,*(ObjectPool_Pointer+loopCounter+1),(ObjectPool_Pointer+loopCounter+1),(ObjectPool_Pointer+loopCounter+1));
    }
 reset();
}