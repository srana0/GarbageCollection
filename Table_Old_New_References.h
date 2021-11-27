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


#define CAPACITY_ADDRESS_REFERENCE 50000 // Size of the Hash Table

// GLOBAL VARIABLE FOR ROOT SET
uint64_t * __Address_Translation_Pointer_STATIC=NULL;           // This will points to the ADDRESS set, it will always points to the starting of the AddressTranslationTable
uint64_t * __Address_Translation_Pointer_DYNAMIC=NULL;           // This will points to the ADDRESS set, it will always points to the current free space of AddressTranslationTable
uint64_t __TotalNumber_Of_References_In_Address_Traslation_Table=0;    // Counter to keep track of number of ADDRESS set objects



//____________________________________________________________________________________
//Add Objects to AddressTranslationTable
//_________________________________________________________________________________
void Add_References_To_AddressTranslationTable(uint64_t OldObject_Address, uint64_t NewObject_Address)
{
  long oldObj_Address_long=(long)OldObject_Address;
   long NewObject_Address_long=(long)NewObject_Address;

    if((oldObj_Address_long!=-1) && (NewObject_Address_long!=-1))                                    // ONLY ADD POSTION NUMBER, WE WILL ADD -1 WHEN WE DELETE
    {

    *__Address_Translation_Pointer_DYNAMIC=OldObject_Address;                                          // Mark Bit         1 Byte
    __Address_Translation_Pointer_DYNAMIC=__Address_Translation_Pointer_DYNAMIC+1; 


    *__Address_Translation_Pointer_DYNAMIC=NewObject_Address;                                      // Number of Slots  1 Byte
    __Address_Translation_Pointer_DYNAMIC=__Address_Translation_Pointer_DYNAMIC+1;  
    
    __TotalNumber_Of_References_In_Address_Traslation_Table++;
    __TotalNumber_Of_References_In_Address_Traslation_Table++;
    }
 

}


//____________________________________________________________________________________
//Search Objects In AddressTranslationTable
//_________________________________________________________________________________
uint64_t Search_Reference_In_AddressTranslationTable(uint64_t oldAddress)
{
 uint64_t i=0;
 
 uint64_t oldAddress_In_AddressTranslationTable=0;
 uint64_t newAddress_In_AddressTranslationTable=0; 

 long oldAddress_In_AddressTranslationTable_LONG=0;

 uint64_t * AddressTranslationTable_Pointer=__Address_Translation_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 

   for(i=0;i<__TotalNumber_Of_References_In_Address_Traslation_Table;i=i+2)
    {    
     
      oldAddress_In_AddressTranslationTable_LONG=(long)(*(AddressTranslationTable_Pointer+i));           // Check negative value
    
          
      if(oldAddress_In_AddressTranslationTable_LONG!=-1)                           
      {           

        oldAddress_In_AddressTranslationTable=*(AddressTranslationTable_Pointer+i);

          if(oldAddress==oldAddress_In_AddressTranslationTable)
          {
              newAddress_In_AddressTranslationTable=*(AddressTranslationTable_Pointer+i+1);       // Address saved after one object
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }
    return newAddress_In_AddressTranslationTable;
}


//____________________________________________________________________________________
//Delete Objects from AddressTranslationTable
//_________________________________________________________________________________
void Delete_References_From_AddressTranslationTable(uint64_t oldAddress)
{
    uint64_t i=0; 
    uint64_t OLD_Address_In_AddressTranslationTable=0;
    uint64_t NEW_Address_In_AddressTranslationTable=0; 
    long OLD_Address_In_AddressTranslationTable_LONG=0;

    uint64_t * AddressTranslationTable_Pointer=__Address_Translation_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION    

    for(i=0;i<__TotalNumber_Of_References_In_Address_Traslation_Table;i=i+2)
    {    
     
      OLD_Address_In_AddressTranslationTable_LONG=(long)(*(AddressTranslationTable_Pointer+i));           // Check negative value
     
          
      if(OLD_Address_In_AddressTranslationTable_LONG!=-1)                           
      {           
            OLD_Address_In_AddressTranslationTable=*(AddressTranslationTable_Pointer+i);

          if(oldAddress==OLD_Address_In_AddressTranslationTable)
          {
              *(AddressTranslationTable_Pointer+i)=-1;         // Assign -1 to ID
              *(AddressTranslationTable_Pointer+i+1)=-1;       // Assign -1 to Address
             
              break;
          }
        
      }
      else
      {
        
      }
      
    }

}

void Print_AddressTranslationTable()
{
    printf("\nPrint_AddressTranslationTable");
    yellow();
     uint64_t * AddressTranslationTable_Pointer=__Address_Translation_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION   

     uint64_t loopCounter;

    for(loopCounter=0;loopCounter<__TotalNumber_Of_References_In_Address_Traslation_Table;loopCounter=loopCounter+2)
    {

       printf("\nIndex [%d]  |\t\tOLD ADDRESS  : %ld\t\t|          Address: %ld (%p) \n",loopCounter,*(AddressTranslationTable_Pointer+loopCounter),(AddressTranslationTable_Pointer+loopCounter),(AddressTranslationTable_Pointer+loopCounter));
       printf("\nIndex [%d]  |\t\tNEW ADDRESS  : %ld\t\t|          Address: %ld (%p) \n",loopCounter+1,*(AddressTranslationTable_Pointer+loopCounter+1),(AddressTranslationTable_Pointer+loopCounter+1),(AddressTranslationTable_Pointer+loopCounter+1));
    }
reset();
}