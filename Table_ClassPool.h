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


#define CAPACITY_CLASS_POOL 50000 // Size of the Hash Table

// GLOBAL VARIABLE FOR ROOT SET
uint64_t * __ClassPool_Pointer_STATIC=NULL;           // This will points to the root set, it will always points to the starting of the ClassPool
uint64_t * __ClassPool_Pointer_DYNAMIC=NULL;           // This will points to the root set, it will always points to the current free space of ClassPool
uint64_t __TotalNumber_Of_Objects_In_ClassPool=0;    // Counter to keep track of number of root set objects



//____________________________________________________________________________________
//Add Objects to ClassPool
//_________________________________________________________________________________
void Add_Class_To_ClassPool(uint64_t class_ID, uint64_t class_Address)
{
   long lass_ID_LONG=(long)class_ID;
   long class_Address_LONG=(long)class_Address;

    if((lass_ID_LONG!=-1) && ((long)class_Address_LONG!=-1))                                     // ONLY ADD POSTION NUMBER, WE WILL ADD -1 WHEN WE DELETE
    {

    *__ClassPool_Pointer_DYNAMIC=class_ID;                                          // Mark Bit         1 Byte
    __ClassPool_Pointer_DYNAMIC=__ClassPool_Pointer_DYNAMIC+1; 


    *__ClassPool_Pointer_DYNAMIC=class_Address;                                      // Number of Slots  1 Byte
    __ClassPool_Pointer_DYNAMIC=__ClassPool_Pointer_DYNAMIC+1;  
    
    __TotalNumber_Of_Objects_In_ClassPool++;
    __TotalNumber_Of_Objects_In_ClassPool++;
    } 

}


//____________________________________________________________________________________
//Search Objects In ClassPool
//_________________________________________________________________________________
uint64_t Search_Class_In_ClassPool(uint64_t classID)
{
 uint64_t i=0;
 
 uint64_t clasID_In_ClassPool=0;
 uint64_t classAddress_In_ClassPool=0; 
 long classID_In_ClassPool_LONG=0;

 uint64_t * ClassPool_Pointer=__ClassPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
  

    for(i=0;i<__TotalNumber_Of_Objects_In_ClassPool;i=i+2)
    {    
     
      classID_In_ClassPool_LONG=(long)(*(ClassPool_Pointer+i));           // Check negative value
      clasID_In_ClassPool=*(ClassPool_Pointer+i);
          
      if(classID_In_ClassPool_LONG!=-1)                           
      {           
          if(classID==clasID_In_ClassPool)
          {
              classAddress_In_ClassPool=*(ClassPool_Pointer+i+1);       // Address saved after one object
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }
    return classAddress_In_ClassPool;
}


//____________________________________________________________________________________
//Delete Objects from ClassPool
//_________________________________________________________________________________
void Delete_Class_From_ClassPool(uint64_t classID)
{
    uint64_t i=0; 
    uint64_t classID_In_ClassPool=0;
    uint64_t classAddress_In_ClassPool=0; 
    long classID_In_ClassPool_LONG=0;

    uint64_t * ClassPool_Pointer=__ClassPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION    

    for(i=0;i<__TotalNumber_Of_Objects_In_ClassPool;i=i+2)
    {    
     
      classID_In_ClassPool_LONG=(long)(*(ClassPool_Pointer+i));           // Check negative value
      classID_In_ClassPool=*(ClassPool_Pointer+i);
          
      if(classID_In_ClassPool_LONG!=-1)                           
      {           
          if(classID==classID_In_ClassPool)
          {
              *(ClassPool_Pointer+i)=-1;         // Assign -1 to ID
              *(ClassPool_Pointer+i+1)=-1;       // Assign -1 to Address
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }

}

void PrintClassPool()
{
    red();
    printf("\n Class Pool");
     uint64_t * ClassPool_Pointer=__ClassPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION   

     uint64_t loopCounter;

    for(loopCounter=0;loopCounter<__TotalNumber_Of_Objects_In_ClassPool;loopCounter=loopCounter+2)
    {

       printf("\nIndex [%d]  |\t\tID     : %ld\t\t|          Address: %ld (%p) \n",loopCounter,*(ClassPool_Pointer+loopCounter),(ClassPool_Pointer+loopCounter),(ClassPool_Pointer+loopCounter));
       printf("\nIndex [%d]  |\t\tADDRESS: %ld\t\t|          Address: %ld (%p) \n",loopCounter+1,*(ClassPool_Pointer+loopCounter+1),(ClassPool_Pointer+loopCounter+1),(ClassPool_Pointer+loopCounter+1));
    }
reset();
}