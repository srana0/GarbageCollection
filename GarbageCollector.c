#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h> 
#include <stdbool.h>
#include<arpa/inet.h>
#include <assert.h>
#include<stdbool.h>
#include<string.h>


#include "GlobalVariableHelper.h"
#include "GCHelper.h"
#include "PrintHelper.h"
#include "ValidationHelper.h"


#include "Table_ClassPool.h"
#include "Table_RootSet.h"
#include "Table_Old_New_References.h"
#include "Table_Objects_In_Heap.h"
#include "Table_LiveObjects.h"
#include "Table_DeadObjects.h"


#define OBJECT_HEADER_SIZE 16                            // Object Header Size is of 16 Byte
#define MAX_NUMBER_OF_STATIC_VARIABLES 16                // Number of Static Variables 
#define CLASS_HEADER_SIZE 16                              //Size of Class Header 16 Bytes

//____________________________________________________________________________________________________________________________________________
//THIS WAS SUBMITED AS ASSIGNEMENT : 3 ( Garbage Collection)
//____________________________________________________________________________________________________________________________________________
//.Desciption:   The is Mark Sweep Compact Garbage Collector
//.Author:       Subhabrata Rana
//.Compile:      gcc GarbageCollector.c -o GarbageCollector
//.Run Program: ./GarbageCollector trace.trace 1024


//________________________________________________________________________________________________________________________________________________________________________
// MAIN Garbage Collector START
//________________________________________________________________________________________________________________________________________________________________________
//HashTable_CLASS_POOL* ht_classPool;
uint64_t Global_live_counter=0;
int main(int argc, char* argv[])
{    

    //____________________________________________________________________________________________________________
   //NAME: CLASS POOL
   //PURPOSE: Marking Start from this list. It can contain valid or invalid refernces to the objects based on the command
   //FORMAT: CLASS ID | CLASS ADDRESS
   //NOTE: Class Pool will NEVER BE DELETED. A class can have multiple Objects.
   //NOTE: It contains static references to the Objects. Updating Class Pool means update the Reference Slots of Static Variables
  //** NOTE: Update will count towards's -->Pointer Updation Count
  //____________________________________________________________________________________________________________
     
   __ClassPool_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_CLASS_POOL); 
   __ClassPool_Pointer_DYNAMIC=__ClassPool_Pointer_STATIC;

   //____________________________________________________________________________________________________________
   //NAME: ROOT SET
   //PURPOSE: Marking Start from this list. It can contain valid or invalid refernces to the objects based on the command
   //FORMAT: OBJECT ID | OBJECT ADDRESS
   //NOTE: List MUST be UPDATED after each GC
  //** NOTE: Update will count towards's -->Pointer Updation Count
   //____________________________________________________________________________________________________________

   __RootSet_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_ROOT_SET); 
   __RootSet_Pointer_DYNAMIC=__RootSet_Pointer_STATIC;
   
  //____________________________________________________________________________________________________________
   //NAME: REFERENCE HASH TABLE AFTER COMPACTION 
   //PURPOSE: Save Old and New Object References
   //FORMAT: OLD ADDRES | NEW ADDRESS
   //NOTE: List MUST be UPDATED after each GC
   //** NOTE: Update will count towards's-->Pointer Updation Count
   //____________________________________________________________________________________________________________
 
   __Address_Translation_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_ADDRESS_REFERENCE); 
   __Address_Translation_Pointer_DYNAMIC=__Address_Translation_Pointer_STATIC;
  //____________________________________________________________________________________________________________
   //NAME: OBJECT LIST 
   //PURPOSE: This will have all the objects created in heap, irrespective of status of LIVE or DEAD
   //FORMAT: OLD ADDRES | NEW ADDRESS
   //NOTE: List MUST be UPDATED after each GC. 
   //NOTE: Dead Object's references will be DELETED after each GC 
   //NOTE: Live Object's reference will be UPDATED after each GC 
   //** NOTE: Update will count towards's-->Pointer Updation Count
   //____________________________________________________________________________________________________________  
 
   __ObjectPool_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_OBJECT_POOL); 
   __ObjectPool_Pointer_DYNAMIC=__ObjectPool_Pointer_STATIC;
  //_____________________________________________________________________________

   //____________________________________________________________________________________________________________
   //NAME: LIVE OBJECT HASH TABLE    (***OPTIONAL TABLE FOR INTERNAL TRACKING PURPOSE)
   //PURPOSE: Save Live Objects
   //FORMAT: Object ID | Object Address
   //NOTE: List should be DELETED after each GC
  //** NOTE: Update will NOT count towards's-->Pointer Updation Count ** INTERNAL USER ONLY
   //____________________________________________________________________________________________________________
  
    __LiveObject_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_LIVE_OBJECTS); 
    __LiveObject_Pointer_DYNAMIC=__LiveObject_Pointer_STATIC;

   //____________________________________________________________________________________________________________
   //NAME: DEAD OBJECT HASH TABLE (***OPTIONAL TABLE FOR INTERNAL TRACKING PURPOSE)
   //PURPOSE: Save DEAD Objects
   //FORMAT: Object ID | Object Address
   //NOTE: List should be DELETED after each GC
   //** NOTE: Update will NOT count towards's-->Pointer Updation Count ** INTERNAL USER ONLY
   //____________________________________________________________________________________________________________
  
   __DeadObject_Pointer_STATIC=(uint64_t *)malloc(CAPACITY_DEAD_OBJECTS); 
   __DeadObject_Pointer_DYNAMIC=__DeadObject_Pointer_STATIC;


     // Get the Trace File and return the content of the file pointed by the pointer
    traceFileLength=ReadTraceFile(argv,1);  

     // Get the Size of the Heap
    int heapSize=atoi(argv[2]);

    __size_of_heap=heapSize;                                // GLOBAL 
     if(heapSize!=0)
     {
       
       heap_Start=(uint8_t*)malloc(heapSize);               // Allocate memory for the Heap. It will return the pointer to the starting of the Heap
       Validate_HeapAllocation(heap_Start);                 // Validate the Memory Allocation
       __LastAllocatedMemory_Pointer=heap_Start;            // Assigning the Heap Start to another Pointer, Heap_Start will be static. It will always points to the starting of the heap, whereas, dynamic pointer will increment and decrement                            
      
       heap_End=heap_Start+heapSize;                         // Heap End

      int loopCounter=0;
      char commandCode; 
      char *commandName; 
      


    for(loopCounter=0;loopCounter<traceFileLength;loopCounter++)
      {  
        // Decode the Instuction for the Heap Operation. Need to read line by line for the instruction and decode the instruction
         commandCode=*heap_Manager;
          switch (commandCode)
          {
            case 'a':                         //  Allocation a Oj Sx Cj Ny               
                    MemoryAllocation(); 
                    break;    
                           

             case '+':                         //Add a reference of an object to the rootset  
                     Add_Reference_Of_Object_To_RootSet();
                     break;


             case 'w':                           //Store/write an object reference field into an object                                                                    
                      Store_Object_Reference_Field_Into_Object();
                      break;


             case 'c':                            //Store/write a static object reference field into a class object 
                       Store_Write_Static_Object_Reference_Field_Into_Class();
                       break;           

             case 'r':    
                   SkipWhiteSpace_And_JumpToNextCommand();             
                   if(*(heap_Manager)=='O')         // Read a reference field from an object r Oj #k   
                   {                                                  
                                
                     Read_Reference_Field_From_Object();
                  }                 
                 if(*(heap_Manager)=='C')          // Read a reference field from a class  r Cj #k                  
                   {                                                           
                                                                       
                     Read_Reference_Field_From_Class();
                   }   
                   break;

             case '-':                             //Delete an object reference from the rootset of a thread    
                    Delete_Object_Reference_From_RootSet();                             
                    break;

             default:
              if(commandCode=='\n')                 // If New Line then read Next Char
              {
                 heap_Manager++;
              }  
            break;
          }     
       
        }
         printf("\n");
     }
     else
     { 
       exit(1);
  
     } 
}

//____________________________________________________________________________________________________________________________
// START: ALL COMMAND INSTRUCTION 
//____________________________________________________________________________________________________________________________

//COMMAND 1: Allocation 
// FORMAT: a Oj Sx Cj Ny 
void MemoryAllocation()
{
    bool isAllCommandExtracted=IsCommandInterpreted_MemoryAlloction();
    if(isAllCommandExtracted)
    { 
      // ALGORITHM:
      // First Check if we can allocate the memory or not
      // TOTAL MEMORY= Header (16 BYTES) + Size of the Payload

      uint32_t totalObjectSize=OBJECT_HEADER_SIZE+objectPayload_Size_in_Bytes;
      bool isObjEligibleForAllocation=Validate_ObjectAllocation(__LastAllocatedMemory_Pointer,totalObjectSize,heap_End,objectID);
     
      if(isObjEligibleForAllocation)                // Memory available for allocation
      {
        
        // We need to first create a class in the class Pool. And then assign the pointer of the class to the object 
        uint64_t classReference_In_ClassPool=CreateClass_In_ClassPool(classID,objectPayload_Size_in_Bytes);
    
        // Once the class is created in the class pool. allocate the object in heap
        // Irrespective of whether a new class is created or it was already created in the class pool, we need to allocate object in the heap 
        // For the First time, the Mark bit will be zero for newly created object.
        AllocateObject_In_Heap(0,number_Of_Ref_Slot_In_Object,objectID,classID,classReference_In_ClassPool);
   
      } 
      else                                       
      {
        // Before Collecting the Garbage need to know where the Last place for allocated memory in the heap      
         CollectGarbage();
       __LastAllocatedMemory_Pointer=__heapPointer_FREE_Space_START; // RESET DYNAMIC HEAP POINTER AS IT WILL POINT TO THE FREE SPACE POINTED BY THE __heapPointer_FREE_Space_START
                                                                     // new objects will be allocated after compacted objects till the heap end
            
            
            heap_Manager=heap_manager_Before_GC-1;                    // |1|1|1|1|0|0|0| --<--__LastAllocatedMemory_Pointer && <--__heapPointer_FREE_Space_START
      
          
      }      
   
      heap_Manager++;
    }
    else
    {       
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    }        
}

// COMMAND 2: Add a reference of an object to the rootset 
// FORMAT: + Oj 
void Add_Reference_Of_Object_To_RootSet()
{

  
    bool isAllCommandExtracted= IsCommandInterpreted_AddReferenceToRootSet();
   
    uint64_t rootset_ObjectAddress=0;
    // IMPLEMENT START
    if(isAllCommandExtracted)
    {
      // First Get which Object needed to be added into the RootSet
      // Get the Address of the Object's Address from ObjectHash Table
      rootset_ObjectAddress=Search_Object_In_ObjectPool((uint64_t) objectID);  
      if(rootset_ObjectAddress != 0) 
      {      
        Add_Object_To_RootSet((uint64_t) objectID, (uint64_t) rootset_ObjectAddress);       
        
      }
      else
      {
        /* code */
      }     

      heap_Manager++;
    }
    else
    {
       //printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    }  
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           }

//COMMAND 3: Store/write an object reference field into an object
//FORMAT : w Pj #k Ol 
void Store_Object_Reference_Field_Into_Object()
{

    bool isAllCommandExtracted= IsCommandInterpreted_Write_Object_Reference_Object();
    // IMPLEMENT START
    if(isAllCommandExtracted)
    {
     // Check which is the parent ID number
     // Get the Address of the Object to access the Kth Slot of the Parent Object
     // Assign a value in that Slot that can refer to the Child Object
     // searchObject_ObjectHashTable = Search_ObjectHashTable(parent_Object_ID);

      uint64_t starting_Address_ParentObject=Search_Object_In_ObjectPool((uint64_t) parent_Object_ID);  
      if(starting_Address_ParentObject != 0) 
      {      
       
      // CREATE A POINTER TO THAT ADDRESS
       uint8_t *pointer_to_ParentObject=NULL;
       pointer_to_ParentObject=pointer_to_ParentObject+starting_Address_ParentObject;
       uint8_t *pointer_to_ParentObject_Static=pointer_to_ParentObject; // Saving this addess for future use
       // Get the Number of Slots
       uint8_t number_of_Slots=*(pointer_to_ParentObject+1);
       // Increment the Pointer till the starting of the Object's Payload
      // From Stating Address, we can easily get the Stating of the Object payload
      // Each Object has a Header Size of 16 Byte
      // So the address of the Payload will be rootset_ObjectAddress+16
      pointer_to_ParentObject=pointer_to_ParentObject+OBJECT_HEADER_SIZE;
      // Convert it to points to 64 Bit references
       uint64_t *pointer_to_Payload_Of_ParentObject_64Bit=(uint64_t *)pointer_to_ParentObject;
     
         // Get the Address of the Child Object
          uint64_t address_ChildObject=0;       
          address_ChildObject=Search_Object_In_ObjectPool((uint64_t) objectID);
         if(address_ChildObject != 0) 
          {
         
            // Access the kth Location Slot of the Parent to save the Child Object's Address          
            //access the kth slot
            if(number_of_Slots>slotNumber_In_Reference_Field)
            {
               *(pointer_to_Payload_Of_ParentObject_64Bit+slotNumber_In_Reference_Field)=address_ChildObject;
            }
           else
           {
             //printf("\n tyring to write into a invalid slot");
           }
            
          }
      }       
      else
      {
        /* code */
      }     
     
      heap_Manager++;
    }
    else
    {
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    }      
}

//COMMAND 4: Store/write a static object reference field into a class object
//FORMAT: c Cj #k Ol 
void Store_Write_Static_Object_Reference_Field_Into_Class()
{
   bool isAllCommandExtracted= IsCommandInterpreted_Write_Static_Object_Into_Class();   

    //   IMPLEMENT START
    if(isAllCommandExtracted)
    {  
     // We can get the address of the Class Starting Address
     uint64_t starting_Address_Class=Search_Class_In_ClassPool((uint64_t)classID);
      if(starting_Address_Class != 0) 
      {   
      // CREATE A POINTER TO THAT ADDRESS
       uint8_t *pointer_to_Class=NULL;
       pointer_to_Class=pointer_to_Class+starting_Address_Class;   // Pointer now points to the actual starting address of the class
       uint8_t *pointer_to_Class_Static=pointer_to_Class;          // Saving for Printing Later
     
      // Since class Header is 16 Byte, the slots starts after 16 bytes where payload starts
       pointer_to_Class=pointer_to_Class+OBJECT_HEADER_SIZE; // Points to the First Element in the Payload

      // Convert it to points to 64 Bit references
       uint64_t *pointer_to_Payload_Of_Class_64Bit=(uint64_t *)pointer_to_Class;

       // We need to get the address reference of the Object  

          uint64_t starting_Address_Object=0;       
          starting_Address_Object=Search_Object_In_ObjectPool((uint64_t) objectID);    
        

         if(starting_Address_Object != 0)
         {
         
           // Now that we are pointing to the payload, we can access the slot information     
          // CHECK SLOT NUMBER MUST BE LESS THAN SIZE OF SLOTS
          if(slotNumber_In_Reference_Field<16)
          {
            *(pointer_to_Payload_Of_Class_64Bit+slotNumber_In_Reference_Field)=(uint64_t)starting_Address_Object; 
          }
          else
          {
           // printf("\n trying to write to a invalid slot");
          }        
        
        }      

      }

  
      heap_Manager++;
    }
    else
    {
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    }   
}

//COMMAND 5: Read a reference field from an object
//FORMAT: r Oj #k
void Read_Reference_Field_From_Object()
{
    uint64_t starting_Address_Object=0;   
    bool isAllCommandExtracted= IsCommandInterpreted_Read_Reference_From_Object();

    if(isAllCommandExtracted)
    { 
      // Get the Object Address
      //searchObject_ObjectHashTable = Search_ObjectHashTable(objectID);

    starting_Address_Object=Search_Object_In_ObjectPool(objectID);

      if(starting_Address_Object != 0) 
      {    
      // Go to the Slot and Read

      // CREATE A POINTER TO THAT ADDRESS
       uint8_t *pointer_to_Object=NULL;
       pointer_to_Object=pointer_to_Object+starting_Address_Object;
       uint8_t *pointer_to_ParentObject_Static=pointer_to_Object; // Saving this addess for future use
       // Get the Number of Slots
       uint8_t number_of_Slots=*(pointer_to_Object+1);
       // Increment the Pointer till the starting of the Object's Payload
      // From Stating Address, we can easily get the Stating of the Object payload
      // Each Object has a Header Size of 16 Byte
      // So the address of the Payload will be rootset_ObjectAddress+16
       pointer_to_Object=pointer_to_Object+OBJECT_HEADER_SIZE;
      // Convert it to points to 64 Bit references
        uint64_t *pointer_to_Payload_Of_Object_64Bit=(uint64_t *)pointer_to_Object;
        // Saving value at variable

        // CHECK THE NUMBER OF SLOTS . IT MUST BE LESS GREATER THAN SLOT NUMBER
        // EXAMPLE OBJECT 79 TRIES TO READ THE SLOT NUMBER OF OBJECT THAT DOES NOT EXISTS
        if(number_of_Slots>slotNumber_In_Reference_Field)
        {
          uint64_t addressAtSlot_In_Object=*(pointer_to_Payload_Of_Object_64Bit+slotNumber_In_Reference_Field);
        }
        else
        {
          //printf("\n trying to read invalid slot");
        }   
        
      }

      heap_Manager++;
    }
    else
    {
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    }  
}

//COMMAND 6: Read a reference field from a class
//FORMAT: r Cj #k
void Read_Reference_Field_From_Class()
{
    bool isAllCommandExtracted= IsCommandInterpreted_Read_Reference_From_Class();
    // IMPLEMENT START
    if(isAllCommandExtracted)
    {
     // We can get the address of the Class Starting Address
      uint64_t starting_Address_Class=Search_Class_In_ClassPool((uint64_t)classID);
 
      if(starting_Address_Class != 0) 
      {
       // CREATE A POINTER TO THAT ADDRESS
       uint8_t *pointer_to_Class=NULL;
       pointer_to_Class=pointer_to_Class+starting_Address_Class;   // Pointer now points to the actual starting address of the class
       uint8_t *pointer_to_Class_Static=pointer_to_Class;          // Saving for Printing Later
       
     
      // Since class Header is 16 Byte, the slots starts after 16 bytes where payload starts
       pointer_to_Class=pointer_to_Class+OBJECT_HEADER_SIZE; // Points to the First Element in the Payload
      //   Convert it to points to 64 Bit references
       uint64_t *pointer_to_Payload_Of_Class_64Bit=(uint64_t *)pointer_to_Class;
            // SLOT NUMBER MUST NOT CROSS NUMNER OF SLOTS    
            if(slotNumber_In_Reference_Field<16)
            {
           uint64_t addressAtSlot_In_Class=*(pointer_to_Payload_Of_Class_64Bit+slotNumber_In_Reference_Field);
            } 
            else
            {
             // printf("\n trying to read invalid slot");
            }
            
    
      }
      heap_Manager++;
    }
    else
    {
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    } 
}

//COMMAND 7: Delete an object reference from the rootset of a thread 
//FORMAT : - Oj 
void Delete_Object_Reference_From_RootSet()
{
//printf("\n before deletion RootSet\n");
//PrintRootSet();
   bool isAllCommandExtracted= IsCommandInterpreted_Delete_Object_Reference_From_Rootset();
    // IMPLEMENT START
    if(isAllCommandExtracted)
    {
  
     uint64_t objAddress_In_RootSet=Search_Object_In_RootSet((uint64_t)object_To_Delete_From_RootSet);

      if(objAddress_In_RootSet != 0) 
      {
  
        Delete_Object_From_RootSet((uint64_t)object_To_Delete_From_RootSet);

      }
      //Get the Object from RootSet and Delete
      heap_Manager++;
    }
    else
    {
       printf("\nAll Commands could not be read\n");
       exit(EXIT_FAILURE);
    } 
}

//____________________________________________________________________________________________________________________________
// END: ALL COMMAND INSTRUCTION 
//____________________________________________________________________________________________________________________________


//______________________________________________________________________________________________________
//This function will create a class in the class pool
// ** THIS IS NOT IN THE HEAP
// ** THIS WILL NEVER BE DELETED
// ** ONE CLASS MAY HAVE MULTIPLE OBJECTS
// ** SAME CLASS WILL NOT BE CRETED MULTIPLE TIMES
// ** CLASS STRUCTURE
// ** 1ST PLACE--> CLASS ID: 8 BYTE
// ** 2ND PLACE--> OBJECT PAYLOAD SIZE: 8 BYTE
// ** 3RD TO 19 PLACE--> TOTAL 16 PLACES FOR STATIC REFERENCES (16*8 BYTE)
//______________________________________________________________________________________________________
uint64_t CreateClass_In_ClassPool(uint16_t classID,uint32_t objectPayload_Size_in_Bytes)
{
  // BEFORE CREATING ANY CLASS IN THE CLASS POOL, WE NEED TO MAKE SURE THAT THE CLASS IS NOT ALREADY CREATED IN THE CLASS POOL

  // We can get the address of the Class Starting Address
      uint64_t starting_Address_Class=Search_Class_In_ClassPool((uint64_t)classID);  
      if(starting_Address_Class != 0) 
      {
        return starting_Address_Class;
      }
      else
      {
        uint32_t totalSizeOfClass=CLASS_HEADER_SIZE+(MAX_NUMBER_OF_STATIC_VARIABLES*8);                   
       //Allocate Memory according to the Size of the Class
      uint64_t * class_Starting_Address=(uint64_t *)malloc(totalSizeOfClass);    
     
      uint64_t *class_Starting_Address_Static=class_Starting_Address; // Assigning this pointer as it will not change, we can use it later 
     
     // CLASS POOL 
     Add_Class_To_ClassPool((uint64_t)classID,(uint64_t)class_Starting_Address);
     
    
      // Save Information of Classes in the Class header
      // Later we can use if we need to allocate the static variable in different slots
      (*class_Starting_Address)=(uint64_t)classID;                             //  SAVE THE CLASS ID 
      (*(class_Starting_Address+1))=(uint64_t)objectPayload_Size_in_Bytes;     //  SAVE THE VALUE OF THE SIZE OF THE OBJECT ( PAYLOAD ONLY)
      class_Starting_Address=class_Starting_Address+2;
       int loopCounter; 
     // Need to assign null /0 to these reference variables, otherwise they are holding garbage value
      for(loopCounter=0;loopCounter<MAX_NUMBER_OF_STATIC_VARIABLES;loopCounter++)
      {
          *(class_Starting_Address+loopCounter)=0;           
      } 
   
      //WriteToTextFile_ClassPool(__ClassPool_Pointer_STATIC,__TotalNumber_Of_Objects_In_ClassPool); //TEST  
     return (uint64_t)class_Starting_Address_Static;
      }
}



//______________________________________________________________________________________________________________________
//This function will allocate the objects in the Heap
//---------------------------------------------------------------------------------------------------------------
  // OBJECT HEADER
  //---------------------------------------------------------------------------------------------------------------
  // Mark Bit         1 Byte
  // Number of Slots  1 Byte
  // Object ID        4 Byte
  // Class ID         2 Byte
  // Pointer to Class 8 Byte  
//_____________________________________________________________________________________________________________________

void AllocateObject_In_Heap(uint8_t markBit,uint8_t number_of_Slots,uint32_t objectID,uint16_t classID,uint64_t classReference_In_ClassPool)
{

  // ALGORITHM
  // STEP 1: CONVERT THE POINTER TO ITS APPROPRIATE TYPE
  // STEP 2: SAVE THE VALUE
  // STEP 3: INCREMENT ACCORDINGLY

  // Save Object ID and Object Starting Address in a translation Table
  uint64_t Object_Starting_Address=(uint64_t)__LastAllocatedMemory_Pointer;

  // HASH TABLE : Create a Hash table of all objectes created in the heap
   Add_Object_To_ObjectPool((uint64_t) objectID, (uint64_t)Object_Starting_Address);

  uint8_t *temp_heap_Pointer=__LastAllocatedMemory_Pointer; 

  *__LastAllocatedMemory_Pointer=markBit;                                               // Mark Bit         1 Byte
  __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+1; 


  *__LastAllocatedMemory_Pointer=number_of_Slots;                                        // Number of Slots  1 Byte
  __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+1;  


  uint32_t * temp___LastAllocatedMemory_Pointer_32Bit=(uint32_t *)__LastAllocatedMemory_Pointer;
  *(temp___LastAllocatedMemory_Pointer_32Bit)=objectID;                                  // Object ID        4 Byte
  __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+4;


  uint16_t * temp___LastAllocatedMemory_Pointer_16Bit=(uint16_t *)__LastAllocatedMemory_Pointer;  
  *(temp___LastAllocatedMemory_Pointer_16Bit)=classID;                                     // Class ID         2 Byte
  __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+2;


   uint64_t * temp___LastAllocatedMemory_Pointer_64Bit=(uint64_t *)__LastAllocatedMemory_Pointer;
   *(temp___LastAllocatedMemory_Pointer_64Bit)=(uint64_t)classReference_In_ClassPool;    // Pointer to Class 8 Byte 
  __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+8;
  
   // Now the __LastAllocatedMemory_Pointer points to the values for the reference slots
   // Reset all the Referece values to Zero so that it does not have any garbage value
   // later we will assign the reference values to these slots

  int loopCounter; 
  uint64_t * heapPointer_Type_64Bit=(uint64_t *) __LastAllocatedMemory_Pointer;  // Need to change the pointer type to points to Long (64 bit Reference Types)
  for(loopCounter=0;loopCounter<number_of_Slots;loopCounter++)
  {
      *(heapPointer_Type_64Bit+loopCounter)=0; 
  } 
 // Increment the Dynamic Loop Counter to point to the next free space for object allocation
 __LastAllocatedMemory_Pointer=__LastAllocatedMemory_Pointer+objectPayload_Size_in_Bytes;

}



//___________________________________________________________________________________________________________________________________
//This is a central function to validate whether Mark, Sweep and Compaction is completed as part of the garbage collection process
//__________________________________________________________________________________________________________________________________
void CollectGarbage()
{
__Total_Live_Objects_GC=0;
__Total_Dead_Objects_GC=0;
__Total_Number_Of_Pointers_Updated=0;

     __Garbage_Collection_Run_Counter++; 
                                  

      MarkObjects();                  // Mark Objects iteratively                   

                                      

      Compact_Objects();              // Once all the objects are marked, need to sweep and Compact the entire heap 

                                       

       Update_Pointers();             // Once Compaction is done, need to update Pointers
                                       
      // CALCULATION OF GC
      uint64_t total_memory_Freed=(heap_End-__heapPointer_FREE_Space_START);      
       __Percentage_Of_Total_Free_Heap=(((double)total_memory_Freed)*100.00)/(double)__size_of_heap;

      PrintGarbageCollectionTrace(__Garbage_Collection_Run_Counter,__Total_Live_Objects_GC,__Total_Dead_Objects_GC,__Percentage_Of_Total_Free_Heap,__Total_Number_Of_Pointers_Updated);
     
     if(__Total_Dead_Objects_GC==0)
     {     
      
       exit(1);
     }
     else
     {
  
       // printf("\n GC COLLECTED");
     
     }    
}




//___________________________________________________________________________________________________________________
//This function will mark all the live objects in the heap
// SCAN ALL ITEMS IN THE ROOT SET
//___________________________________________________________________________________________________________________
void MarkObjects()
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
    

      if((objID_In_RootSet_LONG!=-1)  && (objAddress_In_RootSet_LONG!=-1))                         
      {           
         
        objID_In_RootSet=*(rootSet_Pointer+i); 
        objAddress_In_RootSet=*(rootSet_Pointer+i+1);  

        Scan_Object_And_Mark(objAddress_In_RootSet);
      }
      else
      {
        // Nothing
      }
      
    }
}

//___________________________________________________________________________________________________________________
//This function helps to mark objects iteratively
//STEP 1:  Go to the Address and check the mark Bit First
//STEP 2:A If it is marked, then dont procced with the checking all references
//STEP 2:B If it is NOT marked, then mark it and then check all the references  
//___________________________________________________________________________________________________________________
void Scan_Object_And_Mark(uint64_t objAddress)
{
  //printf("\n objAddress %ld (%p)",objAddress,objAddress);
   // PrintObjectContent(objAddress);  // testing



    uint8_t * pointer_to_Object_For_Marking=NULL;
    pointer_to_Object_For_Marking=pointer_to_Object_For_Marking + objAddress;

    // only requires if we need to print the object for debugging purpose
    uint8_t * pointer_to_Object_For_Marking_Static=pointer_to_Object_For_Marking;  

 
    if(*pointer_to_Object_For_Marking==0)                                             // Unmarked? Mark it
    {
        *pointer_to_Object_For_Marking=1;                                            // MARK THE LIVE OBJECT WITH 1        
      
        Global_live_counter++;

        uint32_t objID=Get_Object_ID_From_Object_Address(objAddress);
   
       // optional: Saving purpose
         Add_LiveObject_To_LiveObjectPool((uint64_t) objID, (uint64_t) objAddress);
     

        // GET THE NUMBER OF SLOTS FOR LOOPING THOUGH THE OBJECT
        uint8_t number_of_ref_slots=Get_No_Of_Ref_Slots_From_Object_Address(objAddress);

        // INCREMENT THE POINTER TO APPROPIRATE POSITION SO THAT IT CAN REFER THE REFERENCE SLOTS
         pointer_to_Object_For_Marking=pointer_to_Object_For_Marking+OBJECT_HEADER_SIZE;
   
         int loopCounter; 
         uint64_t * pointer_To_Ref_Slot=(uint64_t *) pointer_to_Object_For_Marking;  // Need to change the pointer type to points to Long (64 bit Reference Types)
      
        for(loopCounter=0;loopCounter<number_of_ref_slots;loopCounter++)
        {
          uint64_t address_of_referred_object =*(pointer_To_Ref_Slot+loopCounter);          
          if(address_of_referred_object!=0)                                         // Scan only if there is a reference
          {
            Scan_Object_And_Mark(address_of_referred_object);
          
          }         
            
        } 
    }
    else                                                                            
    { 
  
      //ALREADY MARKED? // Do Nothing, the object and its all neighbours are already marked previously
    }

}

//_____________________________________________________________________________________________________
//This method will perform compaction on the objects
//___________________________________________________________________________________________________
void Compact_Objects()
{
   __heapPointer_FREE_Space_START=heap_Start;
   __heapPointer_FREE_Space_Dynamic_Index=heap_Start;

  // Step 1: Need to have two pointers. One will points to the starting of the free space and 2nd will points to the Object's End
  // For the first time, both will point to the same address i.e startring of the heap

   uint64_t address_of_Object=0;
   uint8_t markBit=0;    
   uint32_t object_ID=0;                  
   uint64_t total_size_of_Object=0;     

  // Sweep the Heap
  while(__heapPointer_FREE_Space_Dynamic_Index<__LastAllocatedMemory_Pointer)
  {

      // SCAN THE FIRST OBJECT

      // Address of the Object will be always pointed by the ____heapPointer_FREE_Space_Dynamic_Index
      // And the starting of the empty space will be pointed by the __heapPointer_FREE_Space_START
     address_of_Object=(uint64_t )__heapPointer_FREE_Space_Dynamic_Index;  // 64 BIT PHYSICAL ADDRESS

      // CHECK THE MARK BIT
     markBit=*(__heapPointer_FREE_Space_Dynamic_Index);  
    
     object_ID=Get_Object_ID_From_Pointer(__heapPointer_FREE_Space_Dynamic_Index);   // Optional: For saving purpose

     total_size_of_Object=GetSizeOfObject_From_ObjectPointer(__heapPointer_FREE_Space_Dynamic_Index);

    // if total Size of the object is zero, HALT
    if(total_size_of_Object!=0)
    {
      if(markBit==0)
      {
            __Total_Dead_Objects_GC++;                                                     // add to the global variable list for Total Dead Object
                        
               Add_DeadObject_To_DeadObjectPool((uint64_t) object_ID, (uint64_t) address_of_Object);  // optional: Saving purpose

              __heapPointer_FREE_Space_Dynamic_Index=__heapPointer_FREE_Space_Dynamic_Index+total_size_of_Object; // Increment the Pointer to the Next Object
      }
      else if(markBit==1)
      {
        __Total_Live_Objects_GC++;                                                         // add to the global variable list for Total LIVE Object
    
      
      // If OLD AND NEW REFERENCES ARE SAME, WE SHOULD NOT ADD  TO THE HASH TABLE
      // RECORD THE OLD AND NEW REFERENCES
       
      if(((uint64_t)__heapPointer_FREE_Space_Dynamic_Index)!=(uint64_t)__heapPointer_FREE_Space_START)
      {
          // NEW ADDESS WILL BE WHERE I NEED TO MOVE THE OBJECT         
         Add_References_To_AddressTranslationTable((uint64_t)__heapPointer_FREE_Space_Dynamic_Index, (uint64_t)__heapPointer_FREE_Space_START);

          Reset_Mark_Bit_To_Zero(__heapPointer_FREE_Space_Dynamic_Index);            
           // ACTUALLY MOVE THE OBJECT
           memmove(__heapPointer_FREE_Space_START, __heapPointer_FREE_Space_Dynamic_Index, total_size_of_Object);
           
           // RESET THE MARK BIT SO THAT NEXT GC CAN PICK IT UP, SINCE THE OBJECT IS ALREADY MOVE THE THE PLACE POINTED BY THE FREE SPACE POINTER, NEED TO PASS THE FREE SPACE POINTER TO RESET THE BIT
   
          // AFTER MOVEMENT UPDATE THE REFERENCE TABLE
           __heapPointer_FREE_Space_START=__heapPointer_FREE_Space_START+total_size_of_Object;
           __heapPointer_FREE_Space_Dynamic_Index=__heapPointer_FREE_Space_Dynamic_Index+total_size_of_Object;
       
      }
      else
      {
        // SINCE THE OBJECT IS NOT MOVED, WE NEED TO PASS THE DYNAMIC POINER
           Reset_Mark_Bit_To_Zero(__heapPointer_FREE_Space_Dynamic_Index);            
   

          // AFTER MOVEMENT UPDATE THE REFERENCE TABLE
           __heapPointer_FREE_Space_START=__heapPointer_FREE_Space_START+total_size_of_Object;
           __heapPointer_FREE_Space_Dynamic_Index=__heapPointer_FREE_Space_Dynamic_Index+total_size_of_Object;
          
         }
              
   
      }
   
      
    }
    else
    {
      printf("\n No Object to Traverse");
    }  
      
  }
 
  //new add: ONCE THE COMPACTION IS OVER, RESET THE DYNAMIC POINTER TO THE FREE SPACE START

  __heapPointer_FREE_Space_Dynamic_Index=__heapPointer_FREE_Space_START;
}


//______________________________________________________________________________________________________
//This method will update the pointers for the object
//_____________________________________________________________________________________________________
void Update_Pointers()
{
 //  We need to start from the beginning of the heap
 //  Access all the pointers in the slots, access even 
 //  Pointers needed to be updated in Object Ref Slots
 //  Class Slots
 //  Root Set Hash Table

// DO NOT CHANGE THE SEQUENCE
                                     

   Update_Pointers_For_Referred_Objects();      // OBJECT'S SLOT

   Update_Pointers_For_Static_Variables();      // CLASS POOL

   Update_Pointers_In_RootSet();                  // ROOT SET   

   Update_Pointer_Of_Object_Hash_Table();         //ALL OBJECTS
 

    // BELOW ARE MY INTERNAL TABLE, I USE IT FOR DEBUG 
     Reset_Address_Translation_Table();          

    Reset_Internal_Hash_Table();
       
         
}



void Update_Pointer_Of_Object_Hash_Table()
{
  // For Object Hash Table,I need to first reset/delete the dead object
  // I need to update pointers reference of live objects



   uint64_t i_DEAD=0;  
   long objectID_In_ObjectPool_LONG;
   long objectAddress_In_ObjectPool_LONG;
 
   uint64_t objectID_In_ObjectPool=0;
   uint64_t objectAddress_In_ObjectPool=0;   

   uint64_t * ObjectPool_Pointer=__ObjectPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
   
   uint64_t address_of_deadObject=0;

    for(i_DEAD=0;i_DEAD<__TotalNumber_Of_Objects_In_ObjectPool;i_DEAD=i_DEAD+2)
    {    
     
      objectID_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer+i_DEAD));           // Check negative value
      objectAddress_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer+i_DEAD+1));
          
      if((objectID_In_ObjectPool_LONG!=-1) && (objectAddress_In_ObjectPool_LONG!=-1))                       
      {           
      objectID_In_ObjectPool=(*(ObjectPool_Pointer+i_DEAD));           
      objectAddress_In_ObjectPool=(*(ObjectPool_Pointer+i_DEAD+1));
      // Check if the address in the Dead Pool is found in the address translation table

        address_of_deadObject=Search_DeadObject_In_DeadObjectPool((uint64_t)objectID_In_ObjectPool);
        if(address_of_deadObject!=0)
        {
            // Dead Object Found. Delete the Object in Object Pool 
           (*(ObjectPool_Pointer+i_DEAD))=-1;           
           (*(ObjectPool_Pointer+i_DEAD+1))=-1;

           //__Total_Number_Of_Pointers_Updated++;
        }
      }
      else
      {
        
      }
      
    }

  // SINCE ALL DEAD OBJECTS ARE TAKEN CARE OF, ALL OBJECTS ARE LIVE APART FROM THE DEAD OBJECTS
  // NEED TO UPDATE THE POINTERS



   uint64_t i_LIVE=0;  
   long liveObjectID_In_ObjectPool_LONG;
   long liveObjectAddress_In_ObjectPool_LONG;
 
   uint64_t liveObjectID_In_ObjectPool=0;
   uint64_t liveObjectAddress_In_ObjectPool=0;   

   uint64_t * ObjectPool_Pointer_live=__ObjectPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
   
   uint64_t oldAddress_of_liveObject=0;
   uint64_t newAddress_of_liveObject=0;

    for(i_LIVE=0;i_LIVE<__TotalNumber_Of_Objects_In_ObjectPool;i_LIVE=i_LIVE+2)
    {    
     
      liveObjectID_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer_live+i_LIVE));           // Check negative value
      liveObjectAddress_In_ObjectPool_LONG=(long)(*(ObjectPool_Pointer_live+i_LIVE+1));
          
      if((liveObjectID_In_ObjectPool_LONG!=-1) && (liveObjectAddress_In_ObjectPool_LONG!=-1))                       
      {           
      liveObjectID_In_ObjectPool=(*(ObjectPool_Pointer_live+i_LIVE));           
      objectAddress_In_ObjectPool=(*(ObjectPool_Pointer_live+i_LIVE+1));
     
      // Check if the address in the Dead Pool is found in the address translation table

        newAddress_of_liveObject=Search_Reference_In_AddressTranslationTable((uint64_t)objectAddress_In_ObjectPool);
        if(newAddress_of_liveObject!=0)
        {
            // Object references changed for the live object, need to update the table
              
           (*(ObjectPool_Pointer+i_LIVE+1))=newAddress_of_liveObject;

           //__Total_Number_Of_Pointers_Updated++;
        }
      }
      else
      {
        
      }
      
    }
}


//_________________________________________________________________________________________________________________
//This method will update the pointers to my internal tables that I am using for tracking and debugging purpose
// These are optional, so changing pointer will not be considered here
//________________________________________________________________________________________________________________

void Reset_Internal_Hash_Table()
{

  uint64_t * ObjectPool_Pointer_live=__LiveObject_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
  uint64_t i=0;

    for(i=0;i<__TotalNumber_Of_LiveObjects;i=i+2)
    {         
      *(ObjectPool_Pointer_live+i)=-1;           
      *(ObjectPool_Pointer_live+i+1)=-1;     
    }

   uint64_t * ObjectPool_Pointer_dead=__DeadObject_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
  uint64_t j=0;

    for(j=0;j<__TotalNumber_Of_DeadObjects;j=j+2)
    {         
      *(ObjectPool_Pointer_dead+j)=-1;           
      *(ObjectPool_Pointer_dead+j+1)=-1;     
    }
        
}



//__________________________________________________________________________________________________________________
//This method will reset the Reference Table
//_______________________________________________________________________________________________________________
void Reset_Address_Translation_Table()
{
uint64_t i=0;

 uint64_t * AddressTranslationTable_Pointer=__Address_Translation_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 

   for(i=0;i<__TotalNumber_Of_References_In_Address_Traslation_Table;i=i+2)
    {    
     
    *(AddressTranslationTable_Pointer+i)=-1;        
    *(AddressTranslationTable_Pointer+i+1)=-1; 
      
    }
}

//_______________________________________________________________________________________________________________________
//RESET THE MARK BIT OF THE OBJECT
//_______________________________________________________________________________________________________________________
void Reset_Mark_Bit_To_Zero(uint8_t * pointer_to_object)
{
  if(*(pointer_to_object)==1)
  {
  *(pointer_to_object)=0;
  }
}


//_______________________________________________________________________________________________________________________
//Update Pointers in Root Set
//_______________________________________________________________________________________________________________________
void Update_Pointers_In_RootSet()   // DONE
{


 uint64_t i=0; 
 uint64_t objID_In_RootSet=0;
 uint64_t objAddress_In_RootSet=0; 

 long objID_In_RootSet_LONG=0;
 long objAddress_In_RootSet_LONG=0;
 uint64_t new_Obj_Address=0;

 
 uint64_t * rootSet_Pointer=__RootSet_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
   
    for(i=0;i<__TotalNumber_Of_Objects_In_RootSet;i=i+2)
    {    
     
      objID_In_RootSet_LONG=(long)(*(rootSet_Pointer+i));           // Check negative value
      objAddress_In_RootSet_LONG=(long)(*(rootSet_Pointer+i+1));           // Check negative value

      objID_In_RootSet=*(rootSet_Pointer+i);
      objAddress_In_RootSet=*(rootSet_Pointer+i+1);

      if((objID_In_RootSet_LONG!=-1) && (objAddress_In_RootSet_LONG!=-1))                          
      {    
        // Check First Addres, if it it present in the Compaction_Reference_HashTable, then update with the new address       
         new_Obj_Address=Search_Reference_In_AddressTranslationTable((uint64_t) objAddress_In_RootSet);
            if(new_Obj_Address!=0)
            {
              *(rootSet_Pointer+i+1)=new_Obj_Address;   // Update the ROOT SET ADDRESS WITH NEW ADDRESS
               __Total_Number_Of_Pointers_Updated++; 
            }              
             
      }
      else
      {
        
      }
      
    }
}

//_______________________________________________________________________________________________________________________
//Update Pointers in Root Ser
//_______________________________________________________________________________________________________________________
void Update_Pointers_For_Static_Variables() // DONE
{
      //LOOP THROUGH THE CLASS POOL
      // FOR EACH OBJECT GET THE ADDRESS, GO THE TO REF SLOT AND CHECK IF THE ADDRESS IN THAT SLOTS CHANGED OR NOT


 int COUNTER=0; 

//0000
 uint64_t i=0; 
 uint64_t j=0;
 uint64_t clasID_In_ClassPool=0;
 uint64_t classAddress_In_ClassPool=0; 

 uint64_t OLD_Static_VariableAddress_ClassPool=0; 
 uint64_t NEW_Static_VariableAddress_ClassPool=0; 

 long classID_In_ClassPool_LONG=0;
 long classAddress_In_ClassPool_LONG=0; 
 uint64_t * ClassPool_Pointer=__ClassPool_Pointer_STATIC;   //ALWAYS START FROM THE STARTRING OF THE MEMORY LOCATION 
  

    for(i=0;i<__TotalNumber_Of_Objects_In_ClassPool;i=i+2)
    {    
     
      classID_In_ClassPool_LONG=(long)(*(ClassPool_Pointer+i));                  // Check negative value
      classAddress_In_ClassPool_LONG=(long)(*(ClassPool_Pointer+i+1));           // Check negative value

               
      if((classID_In_ClassPool_LONG!=-1)  && (classAddress_In_ClassPool_LONG!=-1))                        
      {      
          clasID_In_ClassPool=*(ClassPool_Pointer+i);
          classAddress_In_ClassPool=*(ClassPool_Pointer+i+1);  

         // Once we get the address of the Class, need to get the metadata saved in the class
           uint8_t *pointer_to_Class=NULL;
           pointer_to_Class=pointer_to_Class+classAddress_In_ClassPool;

         //  PrintClassContent((uint64_t)pointer_to_Class);

           // Change the Pointer to 64 Bit
           uint64_t * pointer_to_Class_64Bit=(uint64_t *) pointer_to_Class;

            // First 8 byte for class ID
            // Second 8 Byte for Size of Object Payload.
            // Increment the Pointer to till we get the slots for Static Variables
            pointer_to_Class_64Bit=pointer_to_Class_64Bit+2;  // Location where slots exists for Static Variables
            for(j=0;j<MAX_NUMBER_OF_STATIC_VARIABLES;j++)
            {

                  if(*(pointer_to_Class_64Bit+j)!=0)
                  {
                    // Get the Static Variable Address
                    OLD_Static_VariableAddress_ClassPool= *(pointer_to_Class_64Bit+j);
                    // Find if that address exists in the New Old Object hash Table
                    // if exists, we need to update this reference too.                    
                 
                     NEW_Static_VariableAddress_ClassPool=Search_Reference_In_AddressTranslationTable((uint64_t) OLD_Static_VariableAddress_ClassPool);

                      if(NEW_Static_VariableAddress_ClassPool!=0)
                      {    
                          *(pointer_to_Class_64Bit+i)=NEW_Static_VariableAddress_ClassPool;
                          __Total_Number_Of_Pointers_Updated++;                      
                      }   
                  }

            }
        
      }
      else
      {
        
      }
      
    }

}

//_______________________________________________________________________________________________________________________
//Update Pointers in REFERRED OBJECT
//_______________________________________________________________________________________________________________________
void Update_Pointers_For_Referred_Objects()
{
   uint8_t *sweeper_Dynamic=heap_Start;  // SWEEPER POINTER WILL START SWEEPING FROM THE START OF THE HEAP TO UPDATE THE REFERENCES
                                        // This will be dynamic in nature i.e. it will be incremented when objects are encounter
   uint8_t *sweeper_Static=NULL;
   int COUNTER=0;
   uint64_t address_of_Object=0;
   uint8_t number_of_ref_slots=0;
   uint64_t old_object_reference=0;
   uint64_t new_object_reference=0;
  
   uint64_t total_object_size=0;

//Print_AddressTranslationTable();

   while(sweeper_Dynamic<__heapPointer_FREE_Space_START)
   {
     
       sweeper_Static=sweeper_Dynamic;   
     // STATIC WILL ALWAYS POINTS TO THE START OF THE OBJECT. THIS WAY IT WILL BE HELPFUL WHEN I INCREMENT THE POINTER TO SIZE OF THE OBJECT
     // Step 1: Get the Number of Reference Slots
     // Step 2: Go to the address of the starting of the slots 

      address_of_Object=(uint64_t )sweeper_Dynamic;  // 64 BIT PHYSICAL ADDRESS

      number_of_ref_slots=Get_No_Of_Ref_Slots_From_Object_Address(address_of_Object);
  
      // Get the Total Size of the Object, 
      total_object_size=GetSizeOfObject_From_ObjectPointer(sweeper_Static);
      // Go the slot
   
        sweeper_Dynamic=sweeper_Dynamic+OBJECT_HEADER_SIZE;

        uint64_t * sweeper_64Bit_Pointer=(uint64_t * )sweeper_Dynamic;

        for(COUNTER=0;COUNTER<number_of_ref_slots;COUNTER++)
        {
          if(*(sweeper_64Bit_Pointer+COUNTER)!=0)            // SKIP THE EMPY REFERENCE SLOTS
          {
             old_object_reference=*(sweeper_64Bit_Pointer+COUNTER);
             // Check if this address exists in the Reference Table        

              new_object_reference=Search_Reference_In_AddressTranslationTable((uint64_t)old_object_reference);

              if(new_object_reference!=0)
              {                
                            
               
                *(sweeper_64Bit_Pointer+COUNTER)=new_object_reference;
                __Total_Number_Of_Pointers_Updated++;   
                            
             }  
          }

        }   

        // Update the Pointer accordingly so that it can refer to the next object.
      sweeper_Dynamic=sweeper_Static+total_object_size;
    
   }

}



//______________________________________________________________________________________________________________
//Get Object ID from Pointer to the Object
//______________________________________________________________________________________________________________
uint32_t Get_Object_ID_From_Pointer(uint8_t * pointer_To_Object)
{
 pointer_To_Object=pointer_To_Object+2;
 uint32_t * object_ID_Pointer=(uint32_t *) pointer_To_Object;
 uint32_t object_ID=*(object_ID_Pointer);


  return object_ID;
}


//__________________________________________________________________________________________________________________________
//This function will provide the Size of the Object from the 
//__________________________________________________________________________________________________________________________
uint64_t GetSizeOfTheObjectPayload_From_Class_Address(uint64_t address_of_class)
{

// Create a Pointer
uint8_t * poiner_to_Class=NULL;
poiner_to_Class=poiner_to_Class+address_of_class;

uint64_t *poiner_to_Class_64Bit=(uint64_t *)poiner_to_Class;
poiner_to_Class_64Bit=poiner_to_Class_64Bit+1;
//Size of the object
uint64_t size_of_object_payload=(*(poiner_to_Class_64Bit));

return size_of_object_payload;

}

//__________________________________________________________________________________________________________________
//This function takes address of a object and return the entire size including payload
//__________________________________________________________________________________________________________________
uint64_t GetSizeOfObject_From_ObjectReference(uint64_t address_of_object)
{

// We know at 8th place the class address situated
uint8_t *pointer_to_Object=NULL;
pointer_to_Object=pointer_to_Object+address_of_object;

uint8_t *pointer_to_Object_Static=pointer_to_Object;   // This is a static pointer, always points to the starting of the object

pointer_to_Object=pointer_to_Object+8;
// Change the Pointer to point 64 bit address
uint64_t * pointer_to_Object_64Bit=(uint64_t * )pointer_to_Object;

// Get the Class Address
uint64_t classAddress=*(pointer_to_Object_64Bit);

// Once we get the address of the Class, need to get the metadata saved in the class
uint8_t *pointer_to_Class=NULL;
pointer_to_Class=pointer_to_Class+classAddress;

// Change the Pointer to 64 Bit
uint64_t * pointer_to_Class_64Bit=(uint64_t *) pointer_to_Class;
// Get the Size of the Object
uint64_t size_of_Object_Payload=*(pointer_to_Class_64Bit+1); 
 //  TOTAL SIZE OF FREE SPACE HEADER + PAYLOAD
uint64_t total_size_of_Object=size_of_Object_Payload+OBJECT_HEADER_SIZE;

return total_size_of_Object;

}



//__________________________________________________________________________________________________________________
//This function takes address of a object and return the entire size including payload
//__________________________________________________________________________________________________________________
uint64_t GetSizeOfObject_From_ObjectPointer(uint8_t * pointer_To_Object)
{

// We know at 8th place the class address situated
pointer_To_Object=pointer_To_Object+8;
// Change the Pointer to point 64 bit address
uint64_t * pointer_to_Object_64Bit=(uint64_t * ) pointer_To_Object;

// Get the Class Address
uint64_t classAddress=*(pointer_to_Object_64Bit);

if(classAddress!=0)
{
// Once we get the address of the Class, need to get the metadata saved in the class
uint8_t *pointer_to_Class=NULL;
pointer_to_Class=pointer_to_Class+classAddress;

// Change the Pointer to 64 Bit
uint64_t * pointer_to_Class_64Bit=(uint64_t *) pointer_to_Class;
// Get the Size of the Object
uint64_t size_of_Object_Payload=*(pointer_to_Class_64Bit+1); 
 //  TOTAL SIZE OF FREE SPACE HEADER + PAYLOAD
uint64_t total_size_of_Object=size_of_Object_Payload+OBJECT_HEADER_SIZE;

return total_size_of_Object;
}
else
{
 return 0;
}

}



//________________________________________________________________________________________________________________
//GET THE OBJECT ID FROM OBJECT ADDRESS
//_______________________________________________________________________________________________________________
uint32_t Get_Object_ID_From_Object_Address(uint64_t address_of_object)
{
    // We know at 8th place the class address situated
    uint8_t *pointer_to_Object=NULL;
    pointer_to_Object=pointer_to_Object+address_of_object;

    uint8_t *pointer_to_Object_Static=pointer_to_Object;   // This is a static pointer, always points to the starting of the object

    pointer_to_Object=pointer_to_Object+2;
    // Change the Pointer to point 64 bit address
    uint32_t * pointer_to_Object_32Bit=(uint32_t * )pointer_to_Object;

    //Get the Object ID
    uint32_t obj_ID=*(pointer_to_Object_32Bit); 

    return obj_ID;

}



//_____________________________________________________________________________________________________________________
//Get the Slot Number from the address of the Object
//____________________________________________________________________________________________________________________
uint8_t Get_No_Of_Ref_Slots_From_Object_Address(uint64_t address_of_object)
{

// We know at 8th place the class address situated
uint8_t *pointer_to_Object=NULL;
uint8_t number_of_ref_slots=*(pointer_to_Object+address_of_object+1);

return number_of_ref_slots;
}


//****************************************************************************************************************************************
// START: COMMAND INTERPRETATION
//_______________________________________________________________________________________________________________________________________

#pragma region INTERPRET COMMAND

//___________________________________________________________________________________
//COMMAND 1: MEMEROY ALLOCTION FOR OBJECT: This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_MemoryAlloction()
{
heap_manager_Before_GC=heap_Manager;
    bool isAllCommandExtracted=false;
    SkipWhiteSpace_And_JumpToNextCommand();

     if(*(heap_Manager)=='O')
      {
        
        objectID=atoi(++heap_Manager);              // Received Object ID               
        AdjustHeap_Manager_Pointer();        
                    
        SkipWhiteSpace_And_JumpToNextCommand();
        if(*(heap_Manager)=='S')
        {          
          objectPayload_Size_in_Bytes=atoi(++heap_Manager);   // Received Size of Object in Bytes
          AdjustHeap_Manager_Pointer();   

          SkipWhiteSpace_And_JumpToNextCommand();
          if(*(heap_Manager)=='C')
          {
              // Received Class ID
              classID=atoi(++heap_Manager);           // Index[9]
                AdjustHeap_Manager_Pointer();

                SkipWhiteSpace_And_JumpToNextCommand();

              if(*(heap_Manager)=='N')
              {
                number_Of_Ref_Slot_In_Object=atoi(++heap_Manager);  // Index[12]
                isAllCommandExtracted=true;
                AdjustHeap_Manager_Pointer_WithNewLine();             
              }
              else
                {
                  isAllCommandExtracted=false;
                  printf("\n No of reference Field in Object could not be traced.");
                }                           
          }
          else
          {
            isAllCommandExtracted=false;
            printf("\n Class ID Could not be traced.");
          }
          
        }
        else
        {
          isAllCommandExtracted=false;
          printf("\n Size of the object Could not be traced.");
        }
        
      }
      else
      {
        isAllCommandExtracted=false;
        printf("\n Object ID could not be traced.");
      }

      return isAllCommandExtracted;
}

//___________________________________________________________________________________
//COMMAND 2: ADD REFERENCE TO ROOTSET OBJECT: This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_AddReferenceToRootSet()
{
 bool isAllCommandExtracted=false;
  SkipWhiteSpace_And_JumpToNextCommand();
   if(*(heap_Manager)=='O')
    {
      // Received Object ID
      objectID=atoi(++heap_Manager);                 
       AdjustHeap_Manager_Pointer_WithNewLine();
       isAllCommandExtracted=true;
    }
    else
    {
      isAllCommandExtracted=false;
      printf("\n Object ID could not be traced.");
    }
  return isAllCommandExtracted;

}

//___________________________________________________________________________________
//COMMAND 3: Write_Object_Reference_Object: This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_Write_Object_Reference_Object()
{
  bool isAllCommandExtracted=false;
  SkipWhiteSpace_And_JumpToNextCommand();
   if(*(heap_Manager)=='P')
    {
      // Received Object ID
       parent_Object_ID=atoi(++heap_Manager);           
        AdjustHeap_Manager_Pointer();
    
      SkipWhiteSpace_And_JumpToNextCommand();
      if(*(heap_Manager)=='#')
      {
          slotNumber_In_Reference_Field=atoi(++heap_Manager); 
          AdjustHeap_Manager_Pointer();

         SkipWhiteSpace_And_JumpToNextCommand();
         if(*(heap_Manager)=='O')
         {             
             objectID=atoi(++heap_Manager);               // Received Object ID
             AdjustHeap_Manager_Pointer_WithNewLine();
             isAllCommandExtracted=true;
         }
         else
         {
           isAllCommandExtracted=false;
            printf("\n Object ID could not be traced.");
         }
      }
      else
      {
        isAllCommandExtracted=false;
       printf("\n Slot Number of a Reference Field could not be traced.");
      }
    }
    else
    {
      isAllCommandExtracted=false;
      printf("\n Parent Object ID could not be traced.");
    }

  return isAllCommandExtracted;

}

//___________________________________________________________________________________
//COMMAND 4: write a static object reference field into a class object: This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_Write_Static_Object_Into_Class()
{
  bool isAllCommandExtracted=false;
  SkipWhiteSpace_And_JumpToNextCommand();
   if(*(heap_Manager)=='C')
    {
      // Received Object ID
       classID=atoi(++heap_Manager);                   //Index[3]
       AdjustHeap_Manager_Pointer();

      SkipWhiteSpace_And_JumpToNextCommand();
      if(*(heap_Manager)=='#')
      {
          slotNumber_In_Reference_Field=atoi(++heap_Manager);
          AdjustHeap_Manager_Pointer();
      
         SkipWhiteSpace_And_JumpToNextCommand();
         if(*(heap_Manager)=='O')
         {            
             objectID=atoi(++heap_Manager);                 // Received Object ID
              AdjustHeap_Manager_Pointer_WithNewLine();
              isAllCommandExtracted=true;
         }
         else
         {
           isAllCommandExtracted=false;
            printf("\n Object ID could not be traced.");
         }
      }
      else
      {
        isAllCommandExtracted=false;
       printf("\n Slot Number of a Reference Field could not be traced.");
      }
    }
    else
    {
      isAllCommandExtracted=false;
      printf("\n Class ID could not be traced.");
    }

  return isAllCommandExtracted;

}

//___________________________________________________________________________________
//COMMAND 5: Read a reference field from an object : This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_Read_Reference_From_Object()
{
      bool isAllCommandExtracted=false;
          
        // Received Object ID
        objectID=atoi(++heap_Manager);                               
        AdjustHeap_Manager_Pointer();

        SkipWhiteSpace_And_JumpToNextCommand();
        if(*(heap_Manager)=='#')
        {
         slotNumber_In_Reference_Field=atoi(++heap_Manager); 
         AdjustHeap_Manager_Pointer_WithNewLine();
         isAllCommandExtracted=true;
        }
        else
        {
          isAllCommandExtracted=false;
          printf("\n Slot Number Could Not be traced.");
        }   

     return isAllCommandExtracted;
}

//___________________________________________________________________________________
//COMMAND 6: Read a reference field from a class : This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_Read_Reference_From_Class()
{
      bool isAllCommandExtracted=false;         
         
      // Received Class ID
      classID=atoi(++heap_Manager);                               //Index[3]
      AdjustHeap_Manager_Pointer();

      SkipWhiteSpace_And_JumpToNextCommand();
      if(*(heap_Manager)=='#')
      {
        slotNumber_In_Reference_Field=atoi(++heap_Manager); //Index[6]
        AdjustHeap_Manager_Pointer_WithNewLine();
          isAllCommandExtracted=true;
      }
      else
      {
          isAllCommandExtracted=false;
        printf("\n Slot Number Could Not be traced.");
      } 
     return isAllCommandExtracted;
}

//___________________________________________________________________________________
//COMMAND 7: Delete an object reference from the rootset of a thread : This Function Extracts The Trace File Command
//__________________________________________________________________________________
bool IsCommandInterpreted_Delete_Object_Reference_From_Rootset()
{
      bool isAllCommandExtracted=false;         
    
    SkipWhiteSpace_And_JumpToNextCommand();
   if(*(heap_Manager)=='O')
    {      
       object_To_Delete_From_RootSet=atoi(++heap_Manager);                 // Received Object ID  
     
       AdjustHeap_Manager_Pointer_WithNewLine();
       isAllCommandExtracted=true;  
    }
    else
    {
      isAllCommandExtracted=false;  
      printf("\n Object ID could not be traced.");
    }
     return isAllCommandExtracted;
}

//______________________________________________________________________________________________________
// This function will adjust the heap_manager pointer when there is a space after each command
//______________________________________________________________________________________________________
void AdjustHeap_Manager_Pointer()
{  
     char *buffer=heap_Manager;
     int numberOfChars=0;
      while((*buffer!=' '))// If there are more then one char as Object ID, need to read all the bytes
      {
        numberOfChars++;
        buffer++;
      }
    heap_Manager=heap_Manager+numberOfChars-1;
}

//______________________________________________________________________________________________________
// This function will adjust the heap_manager pointer when there is a new line
//______________________________________________________________________________________________________
void AdjustHeap_Manager_Pointer_WithNewLine()
{    
     char *buffer=heap_Manager;
     int numberOfChars=0;
      while((*buffer!='\n'))// If there are more then one char as Object ID, need to read all the bytes
      {
        numberOfChars++;
        buffer++;
      }
    heap_Manager=heap_Manager+numberOfChars-1;
}


//____________________________________________________________________________________________________________________
//This is a helper function to read the input file and jump to the particular postion
//_____________________________________________________________________________________________________________________
void SkipWhiteSpace_And_JumpToNextCommand()
{
   heap_Manager++;                                      
   heap_Manager++;  
}

#pragma endregion INTERPRET COMMAND
//****************************************************************************************************************************************
// END: COMMAND INTERPRETATION
//_______________________________________________________________________________________________________________________________________


//____________________________________________________________________________________________________________________________
// This function will load the File Containing Byte Code from the argument
// Files must the in the same location where our C output file exists
//____________________________________________________________________________________________________________________________
int ReadTraceFile(char *argv[],int argPostion)
{

FILE *filePointer;
uint8_t*instuctionStart;
uint8_t*instructionEnd;
uint8_t*program_Pointer_VM;
int fileLength; 
uint8_t*pointerBinaryFile; // This is a pointer to the binay file that will be loaded 
int i; 
// Open the Text File 
filePointer=fopen(argv[argPostion],"r"); 
// Validate if File can be opened or not
if (!filePointer)
{
perror("fopen");
exit(EXIT_FAILURE);
}
fseek(filePointer,0,SEEK_END);
fileLength=ftell(filePointer); 
rewind(filePointer);
heap_Manager=(signed char *)malloc((fileLength+1)*sizeof(char)); // will return the address to the space 

for(i=0;i<fileLength;i++)
{ 
   fread(heap_Manager+i,1,1,filePointer);
} 
*(heap_Manager+fileLength)='\n';

fclose(filePointer); 
fileLength=fileLength+1;

return fileLength;
}



//____________________________________________________________________________________________________________________________
// This function will load the File Containing Byte Code from the argument
// Files must the in the same location where our C output file exists
//____________________________________________________________________________________________________________________________
void WriteToTextFile_RootSet(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_RootSet.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{

    fprintf(filePointer, "%ld\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}


void WriteToTextFile_AllObjects(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_AllObjectsInHeap.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{
    fprintf(filePointer, "%d\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}

void WriteToTextFile_LiveObjects(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_LiveObjects.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{
    fprintf(filePointer, "%d\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}

void WriteToTextFile_DeadObjects(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_DeadObjects.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{
    fprintf(filePointer, "%d\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}

void WriteToTextFile_Addresstranslation(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_AddressTraslation.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{
    fprintf(filePointer, "%d\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}

void WriteToTextFile_ClassPool(uint64_t * pointer_to_contents,int lenght_of_contents)
{

FILE *filePointer;
int fileLength; 

int i; 
// Open the Text File 
filePointer=fopen("Output_ClassPool.txt","w"); 
// Validate if File can be opened or not

if(filePointer == NULL)
    exit(-1);
for(i=0;i<lenght_of_contents;i++)
{
    fprintf(filePointer, "%d\n",*(pointer_to_contents+i));
}

fclose(filePointer);
}
