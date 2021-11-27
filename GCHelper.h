//_____________________________________________________________________________________
// This file will have all the Method definiations
//____________________________________________________________________________________


//
void Validate_HeapAllocation(uint8_t* pointerToMemory);                                                             // Validates whether heap is allocated or not
//bool Validate_ObjectAllocation(uint8_t* memoryStart, int sizeOfObject,uint8_t * heap_End);
bool Validate_ObjectAllocation(uint8_t* memoryStart, uint32_t sizeOfObject,uint8_t * heap_End,uint32_t objectID);
uint64_t CreateClass_In_ClassPool(uint16_t classID,uint32_t sizeOfObject_in_Bytes);
void AllocateObject_In_Heap(uint8_t markBit,uint8_t number_of_Slots,uint32_t objectID,uint16_t classID,uint64_t classReference_In_ClassPool);
uint64_t GetSizeOfObject_From_ObjectPointer(uint8_t * pointer_To_Object);

// Below are the methods for the commands from the trace File
void MemoryAllocation();
void Add_Reference_Of_Object_To_RootSet();
void Store_Object_Reference_Field_Into_Object();
void Store_Write_Static_Object_Reference_Field_Into_Class();
void Read_Reference_Field_From_Object();
void Read_Reference_Field_From_Class();
void Delete_Object_Reference_From_RootSet();
void AdjustHeap_Manager_Pointer();
void SkipWhiteSpace_And_JumpToNextCommand();
void AdjustHeap_Manager_Pointer_WithNewLine();
int ReadTraceFile(char *argv[],int argPostion);

// Hash table Methods
// void Save_To_ClassPool(uint32_t key,uint64_t data);        // Save Class Information into Hash Table
// struct ClassPool *Search_ClassPool(uint32_t key);     // Search Class from the Hash Table
// struct ClassPool* Delete_ClassReference(struct ClassPool* classToDelete);
// void Display_ClassPool() ;
// int GetHashCode_ClassPool(int key);

// void Save_To_ObjectHashTable(uint32_t key,uint64_t data);         // Save Obj Information into Hash Table
// struct ObjectHashTable *Search_ObjectHashTable(uint32_t key);     // Search Obj from the Hash Table
// struct ObjectHashTable* Delete_ObjectReference(struct ObjectHashTable* objectToDelete);
// void Display_ObjectHashTable();
// int GetHashCode_ObjectHashTable(int key);


// void Save_To_RootSetHashTable(uint32_t key,uint64_t data);      // Save Obj Information into Hash Table
// struct RootSetHashTable *Search_RootSetHashTable(uint32_t key);  // Search Obj from the Hash Table
// struct RootSetHashTable* Delete_RootSetReference(struct RootSetHashTable* rootSetToDelete);
// void Display_RootSetHashTable();
// int GetHashCode_RootSetHashTable(int key);


// void Save_To_LiveObject_HashTable(uint32_t key,uint64_t data);
// struct LiveObject_HashTable *Search_LiveObject_HashTable(uint32_t key);
// struct LiveObject_HashTable* Delete_LiveObject(struct LiveObject_HashTable* liveObjectToDelete);
// void Display_LiveObject_HashTable();
// int GetHashCode_LiveObject_HashTable(int key);



// void Save_To_DeadObject_HashTable(uint32_t key,uint64_t data);
// struct DeadObject_HashTable *Search_DeadObject_HashTable(uint32_t key);
// struct DeadObject_HashTable* Delete_DeadObject(struct DeadObject_HashTable* deadObjectToDelete);
// void Display_DeadObject_HashTable();
// int GetHashCode_DeadObject_HashTable(int key);


// void Save_To_Reference_HashTable(uint64_t key,uint64_t data);
// struct Reference_HashTable *Search_Reference_HashTable(uint64_t key);
// struct Reference_HashTable* Delete_New_Reference(struct Reference_HashTable* objectToDelete);
// void Display_Reference_HashTable();
// int GetHashCode_Reference_HashTable(int key);

// void Save_To_FreePoolObject_HashTable(uint32_t key,uint64_t data);
// struct FreePoolObject_HashTable *Search_FreePoolObject_HashTable(uint32_t key);
// struct FreePoolObject_HashTable* Delete_FreeObjReference(struct FreePoolObject_HashTable* freeObjToDelete);
// void Display_FreePoolObject_HashTable();
// int GetHashCode_FreePoolObject_HashTable(int key);


// Interpreters for Trace File Commands
bool IsCommandInterpreted_MemoryAlloction();
bool IsCommandInterpreted_AddReferenceToRootSet();
bool IsCommandInterpreted_Write_Object_Reference_Object();
bool IsCommandInterpreted_Write_Static_Object_Into_Class();
bool IsCommandInterpreted_Read_Reference_From_Object();
bool IsCommandInterpreted_Read_Reference_From_Class();
bool IsCommandInterpreted_Delete_Object_Reference_From_Rootset();

// //void Record_Object_Details_In_Hash_Table(uint32_t objectID,uint64_t objectAddress);
// void Record_Free_Objects_In_Hash_Table(uint64_t objectAddress,uint64_t objectSize);
// void Record_DeadObject_Hash_Table(uint32_t object_ID,uint64_t address_of_Object);
// void Record_Old_New_References_Into_Reference_Table(uint64_t oldAddress,uint64_t newAddress);
// void Record_LiveObject_Hash_Table(uint32_t object_ID,uint64_t address_of_Object);

// PRINT FUNCTIONS
void PrintTraceFile(uint8_t *heap_Manager,uint32_t traceFileLength); // Print the Trace File
void red ();                                                 // Color Print
void yellow ();                                              // Color Print
void green ();                                               // Color Print
void blue () ;                                               // Color Print
void reset ();                                               // Color Print
void PrintOpcode(char commandCode,char *command_Name);       // Print Command Code
void Printheader(char *heading);                             // Print Header
void PrintObjectContent(uint64_t object_address);
void PrintClassContent(uint64_t class_start_Address);
void Display_ClassPool();
void PrintGarbageCollectionTrace(int garbageCollectionNumber, int numberOfLiveObjects, int numberOfDeadObjects,double percentOfTotalFeeHeap, int numberOfUpdatedReferences);

void CollectGarbage();
void MarkObjects();
void Scan_Object_And_Mark(uint64_t objAddress);
void Compact_Objects();
void Update_Pointers();
void Run_Compaction();
void Compact_MarkedObject(bool isObjectNeededToMove, uint64_t objectAddress);
void Add_MarkedObjects_To_TranslationTable_For_Compaction(uint64_t objAddress);

void Build_FreeObjects_List(uint32_t sweep_objectID,uint64_t sweep_ObjectAddress);
uint64_t GetSizeOfTheObjectPayload_From_Class_Address(uint64_t address_of_class);
uint64_t GetSizeOfObject_From_ObjectReference(uint64_t address_of_object);
uint8_t Get_No_Of_Ref_Slots_From_Object_Address(uint64_t address_of_object);
int Get_Total_Number_Of_Object_Created_In_Heap();
uint8_t * Get_Pointer_To_Free_Space();
uint32_t Get_Object_ID_From_Object_Address(uint64_t address_of_object);
uint32_t Get_Object_ID_From_Pointer(uint8_t * pointer_To_Object);
void Reset_Mark_Bit_To_Zero(uint8_t * pointer_to_object);
void Update_Pointers_For_Referred_Objects();
void Update_Pointers_For_Static_Variables();
void Update_Pointers_In_RootSet();

//int Get_Number_Of_Items_In_Hash_Table(int hashTableNumber);
//void Reset_Reference_table();
void Reset_Internal_Hash_Table();
void Update_Pointer_Of_Object_Hash_Table();


// CLASS POOL HASH TABLE
  // void Save_Into_ClassPool(uint64_t class_ID,uint64_t class_Address);
  // uint64_t Search_Address_In_ClassPool(uint64_t class_ID);
  // uint32_t Get_Number_Of_Classes_In_ClassPool();


// ROOTSET 
void Add_Object_To_RootSet(uint64_t objectID, uint64_t objectAddress);
uint64_t Search_Object_In_RootSet(uint64_t objectID);
void Delete_Object_From_RootSet(uint64_t objectID);
void PrintRootSet();

//ADDRES TRANSLATION TABLE
void Add_References_To_AddressTranslationTable(uint64_t OldObject_Address, uint64_t NewObject_Address);
uint64_t Search_Reference_In_AddressTranslationTable(uint64_t oldAddress);
void Delete_References_From_AddressTranslationTable(uint64_t oldAddress);
void Print_AddressTranslationTable();

// CLASS POOL
void Add_Class_To_ClassPool(uint64_t classID, uint64_t classAddress);
uint64_t Search_Class_In_ClassPool(uint64_t classID);
void Delete_Class_From_ClassPool(uint64_t classID);
void PrintClassPool();

// ALL OBJECTS IN HEAP OBJECT POOL

void Reset_Address_Translation_Table();
void PrintObjectPool();

void WriteToTextFile_Addresstranslation(uint64_t * pointer_to_contents,int lenght_of_contents);
void WriteToTextFile_DeadObjects(uint64_t * pointer_to_contents,int lenght_of_contents);
void WriteToTextFile_LiveObjects(uint64_t * pointer_to_contents,int lenght_of_contents);
void WriteToTextFile_AllObjects(uint64_t * pointer_to_contents,int lenght_of_contents);
void WriteToTextFile_RootSet(uint64_t * pointer_to_contents,int lenght_of_contents);
void WriteToTextFile_ClassPool(uint64_t * pointer_to_contents,int lenght_of_contents);