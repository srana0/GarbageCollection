# Garbage Collection
<a name="br1"></a> 
# Problem Statement
Your code is expected to be compiled and executed on a linux machine with a gcc compiler. The standard configuration in our labs. Please ensure this is the case before you submit as the TA needs to be able to run your code and cannot do so if it is not in this environment.

For this assignment we are going to step away from our interpreter and focus more on garbage collection by implementing the mark-sweep-compact garbage collection algorithm.
The heap effectively can be viewed as a black box where user applications make requests. Requests are transactions that allocate objects, set fields of objects/classes, and get fields of objects/classes, as well as add/remove objects from the execution stack (rootset). 
To abstract the garbage collection for implementation we will focus on the following summarized requests:

| MM Operations | Command  |  Arguments| 
|---|---|---|
|  Allocation |  a  |  Oj Sx Cj Ny |
|Add a reference of an object to the rootset|+ Oj|
|Store/write an object reference field into an object|w|Pj #k Ol|
|Store/write a static object reference field into a class object|c|Cj #k Ol|
|Read a reference field from an object|r | Oj #k|
|Read a reference field from a class |r |Cj #k|
|Delete an object reference from the rootset of a thread|-|Oj|



You can assume that references are 64-bits in size and references in an object are indexed from 0 and start at the beginning of the object instance or class. You can also assume that the size of an object is large enough to hold any reference slot that is accessed. For example, if you access reference slot 10 this means that the object is at least 88 bytes in
size. Each object is allocated with a header of size 16 bytes. The header is in addition to 2 the size of the object – i.e. an object of size Sx actually consumes x + 16 bytes on the heap. The header has to contain a pointer to the class in the classpool and any other
information of your choosing. You can assume that the maximum number of static reference fields in a class is 16 – indexed from 0 to 15 and each requiring 8 bytes. The commands above can be received by a heap management system and considered as
transactions on the heap. For this assignment, you will receive the commands via an input text file that is specified on the command line. Also, specified on the command line is the size of the heap (in bytes). 

For example:
./A3 trace.txt 1024
would be to run your garbage collector on the file trace.txt with a heap size of 1KB of memory. A garbage collection will occur any time that an allocation cannot occur as the heap is full AND at the end of the trace file before halting execution. In the event that an
allocation cannot occur as the amount of memory needed to hold live objects exceeds the size of the heap, then execution halts. 
Note that the rootset and the classpool are not part of the heap. You can choose how to design and maintain the rootset and classpool to your benefit. When you perform a garbage collection you do not need to delete any classes from the class pool that are not instantiated.

Each time you perform a mark-sweep-compact garbage collection, you are asked to print
the following:
Gn LOy DOz HF%x URq
Where:
n is the garbage collection # - starting at 1 and incrementing with each GC that
occurs.
y is the number of live objects after the garbage collection is complete.
z is the number of dead objects that was collected from the GC process. Note: In order to know the number of dead objects you need to track the number of objects allocated and subtract the number of ones that remain alive!
x is the percentage of the total heap that is free for new objects to be allocated. q is the number of references that were updated as a result of the compacting process. Note that this also includes updating references in the rootset and classpool.
