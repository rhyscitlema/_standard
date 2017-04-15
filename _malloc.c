/*
    _malloc.c

    IMPORTANT: There must be NO memory corruption caused by the user.

    Big Problem:
    What should the user do if memory cannot be allocated? Especially
    for _realloc() where the originally allocated memory is even freed!

    Copyright (c) 2014, Rhyscitlema <info@rhyscitlema.com>.
*/

#include "_malloc.h"



#ifndef CUSTOM_MALLOC_H

#include <malloc.h>

void useFreeMemory (void* freeMemoryArray, long freeMemorySize) {}

void* _malloc (long size)
{ memory_alloc("Memory"); return malloc(size); }

void* _realloc (void* address, long size)
{ if(!address) memory_alloc("Memory"); return realloc(address, size); }

void _free (void* address)
{ if(address) memory_freed("Memory"); free(address); }

long getFragmentsCount ()
{ return 0; }

#else


#include <_stdio.h>


typedef struct _FreeMemory
{
    char* start;
    char* stop;
    struct _FreeMemory *next;
    struct _FreeMemory *prev;
} FreeMemory;

static FreeMemory *firstFreeMemory = NULL;


static void deleteNode (FreeMemory *freeMemory)
{
    if(freeMemory == NULL) return;
    if(freeMemory == firstFreeMemory) firstFreeMemory = freeMemory->next;
    if(freeMemory->prev != NULL) freeMemory->prev->next = freeMemory->next;
    if(freeMemory->next != NULL) freeMemory->next->prev = freeMemory->prev;
    freeMemory->start = freeMemory->stop = 0;
    freeMemory->prev  = freeMemory->next = 0;
}



static void printFreeMemory (const char* debugString)
{
    /*char str[1000];
    FreeMemory *freeMemory;
    sprintf0(str, "\n------------------------ %s\n", debugString);
    puts0(str);

    for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
    {
        sprintf0(str, "freeMemory = %p vs %p, start=%p stop=%p prev=%p next=%p\n",
        freeMemory, freeMemory->stop - sizeof(FreeMemory), freeMemory->start,
        freeMemory->stop, freeMemory->prev, freeMemory->next);
        puts0(str);
    }
    sprintf0(str, "fragmentsCount = %d\n", getFragmentsCount());
    puts0(str);*/
}



void useFreeMemory (void* freeMemoryArray, long freeMemorySize)
{
    //FreeMemory *freeMemory;
    if(!freeMemoryArray || !freeMemorySize) return;

    if(firstFreeMemory == NULL)
    {
        firstFreeMemory = (FreeMemory*) ((char*)freeMemoryArray + freeMemorySize - sizeof(FreeMemory));
        firstFreeMemory->start = (char*)freeMemoryArray;
        firstFreeMemory->stop  = (char*)freeMemoryArray + freeMemorySize;
        firstFreeMemory->next  = NULL;
        firstFreeMemory->prev  = NULL;
    }
    else // firstFreeMemory != NULL   TODO: to be implement later ...
    {
        //while(freeMemory->next != NULL)
        //    freeMemory = freeMemory->next;
    }
    printFreeMemory("useFreeMemory");
}



void* _malloc (long size)
{
    char debugString[100];
    FreeMemory *freeMemory;
    char* result;
    char* r;
    long x=0;

    if(size < sizeof(FreeMemory) - sizeof(long))
       size = sizeof(FreeMemory) - sizeof(long);

    for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
    {
        /* if( available memory >= required memory ) */
        if( freeMemory->stop >= (freeMemory->start + size + sizeof(long)) )
        { x =freeMemory->stop - (freeMemory->start + size + sizeof(long));
          break; }
    }
    if(freeMemory == NULL) { puts0("Error: Malloc Fail   "); return NULL; }

    // Reserve space where size of memory block will be stored.
    // Note: stored size is that of entire block including sizeof(long) bytes.
    result = freeMemory->start + sizeof(long);

    // If remaining memory can contain a structure
    if(x >= sizeof(FreeMemory))
    {
        sprintf0(debugString, "_MALLOC 1 result_start = %p  result_stop = %p",
            freeMemory->start, freeMemory->start + size + sizeof(long));

        // If the 'current' structure will not be overwritten,
        // Then re-assign the new size of the free memory block.
        *((long*)(freeMemory->start)) = size + sizeof(long);
        freeMemory->start             += size + sizeof(long);
    }
    else
    {
        sprintf0(debugString, "_MALLOC 2 result_start = %p  result_stop = %p",
            freeMemory->start, freeMemory->stop);

        // Else, completely remove (or un-free) the 'current' free memory block,
        // by storing the size of the allocated memory as being that of the entire freeMemory.
        *((long*)(freeMemory->start)) = freeMemory->stop - freeMemory->start;
        deleteNode (freeMemory);
    }

    // set all data to 0xFF, so that improper code will hopefully crash...!!!
    for(r = result; r < result+size; r++) *r = 0xFF;

    printFreeMemory(debugString);
    return result;
}



void _free (void* address)
{
    char debugString[100];
    char *startC;   // start of current freed memory block
    char *stopC;    // stop of current freed memory block
    char *stopP;    // stop of preceding free memory block
    char *startN;   // start of succeeding free memory block
    FreeMemory *freeMemory, *temp;

    if(address==NULL) return;

    // starting address = given address - memory reserved to store total size
    startC = (char*)address - sizeof(long);

    // stopping address = starting address + total size of memory block to be freed
    stopC  = startC + *((long*)startC);

    for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
    {
        if(stopC <= freeMemory->start)      // If allocated memory is before an already free memory
            break;
        else if(stopC <= freeMemory->stop)  // But if trying to free memory already freed
            return;                         // ...took me long to find this bug...!!!
    }
    if(freeMemory==NULL) return;

    if(freeMemory->prev != NULL)
        stopP  = freeMemory->prev->stop;    // stop of preceding free memory block
    else stopP = NULL;

    startN = freeMemory->start;             // start of succeeding free memory block

    if(stopP != startC && stopC != startN)  // If nothing to concatenate on neither left nor right
    {
        sprintf0(debugString, "_FREE 1 startC = %p  stopC = %p", startC, stopC);

        // Prepare to put structure at the end of freed memory block
        temp = (FreeMemory*)(stopC - sizeof(FreeMemory));

        temp->start = startC;
        temp->stop  = stopC;

        temp->next = freeMemory;
        temp->prev = freeMemory->prev;

        if(temp->next != NULL) temp->next->prev = temp;
        if(temp->prev != NULL) temp->prev->next = temp;
    }

    else if(stopP != startC && stopC == startN) // If can only concatenate with the right free block
    {
        sprintf0(debugString, "_FREE 2 startC = %p  stopC = %p", startC, stopC);

        freeMemory->start = startC;
    }

    else if(stopP == startC && stopC != startN) // If can only concatenate with the left free block
    {
        // this section will crash iff startC is ever NULL
        sprintf0(debugString, "_FREE 3 startC = %p  stopC = %p", startC, stopC);

        // move the structure from previous to new location
        temp = (FreeMemory*)(stopC - sizeof(FreeMemory));

        temp->start = freeMemory->prev->start;
        temp->stop  = stopC;

        temp->next = freeMemory->prev->next; // same as = freeMemory;
        temp->prev = freeMemory->prev->prev;

        if(temp->next != NULL) temp->next->prev = temp; // inform temp->next node of new location
        if(temp->prev != NULL) temp->prev->next = temp; // inform temp->prev node of new location
    }

    else //if(stopP == startC && stopC == startN) // If can concatenate with both left and right blocks
    {
        // this section will crash iff startC is ever NULL
        sprintf0(debugString, "_FREE 4 startC = %p  stopC = %p", startC, stopC);
        temp = freeMemory->prev;
        freeMemory->start = temp->start;
        freeMemory->prev  = temp->prev;
        if(freeMemory->prev != NULL) freeMemory->prev->next = freeMemory;
        deleteNode (temp);
    }

    while(freeMemory->prev != NULL)
        freeMemory = freeMemory->prev;
    firstFreeMemory = freeMemory;

    printFreeMemory(debugString);
}



void* _realloc (void* address, long size)
{
    int totalSize;

    if(address != NULL)
    {
        totalSize = *((long*)((char*)address - sizeof(long)));

        if(size == totalSize-sizeof(long))
            return address;

        _free (address);
    }
    return _malloc (size);
}



long getFragmentsCount ()
{
    long fragmentsCount = 0;
    FreeMemory *freeMemory;

    for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
        fragmentsCount++;

    return fragmentsCount;
}

#endif // end of #else of #ifndef CUSTOM_MALLOC_H



#include <string.h>
void* meminc (void* old_mem, long old_size, long new_size)
{
    void* new_mem;
    if(old_size >= new_size) return old_mem;
    new_mem = _malloc (new_size);
    memcpy (new_mem, old_mem, old_size);
    _free(old_mem);
    return new_mem;
}


/***********************************************************************************************************/

#ifndef NDEBUG

#include "_strfun.h"
#include <stdio.h> // TODO: removing this does not cause an error in Ubuntu-GCC, why?
#include <assert.h>

typedef struct _MEM {
    const char* str;
    int alloc, freed;
} MEM;

static MEM mem[20];

void memory_alloc (const char* str)
{
    int i;
    for(i=0; mem[i].str!=NULL; i++)
        if(0==strcmp(str, mem[i].str))
        { mem[i].alloc++; break; }
    if(!mem[i].str)
    {   mem[i].str = str;
        mem[i].alloc = 1;
    }
}

void memory_freed (const char* str)
{
    int i;
    for(i=0; mem[i].str!=NULL; i++)
        if(0==strcmp(str, mem[i].str))
        { mem[i].freed++; break; }
    if(!mem[i].str) printf("Software Error in memory_freed: '%s' not found.\n",str);
    assert(mem[i].str!=NULL);
}

void memory_print ()
{
    int i;
    for(i=0; mem[i].str!=NULL; i++)
        printf("%-15s:   alloc = %-7d   freed = %-7d   difference = %d\n",
            mem[i].str, mem[i].alloc, mem[i].freed, mem[i].alloc - mem[i].freed);
    printf("\n");
}

#endif


/***********************************************************************************************************/


/*  Finding the smallest power of two
    greater or equal to a given value

    starting from
      n    = 0010010100
     n-1   = 0010010011
    n&=n-1 = 0010010000
     n-1   = 0010001111
    n&=n-1 = 0010000000
     n-1   = 0001111111
    n&=n-1 = 0000000000

    So the result is r = the latest non-zero 'n'.
    If initial was not a power of 2 then do r<<1.
*/
static inline long pow2ceil (register long n)
{
    register long y=n;
    if(n<0) return 0;
    if(!(n&=n-1)) return y;
    for(y=n; n&=n-1; y=n);
    return y<<1;
}

static inline int ilog2 (register long n)
{
    register int y=0;
    for( ; n!=0; n>>=1) y++;
    return y;
}
