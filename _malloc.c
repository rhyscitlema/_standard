/*
	_malloc.c
*/

#include "_malloc.h"



#ifndef USE_CUSTOM_MALLOC_H

#include <malloc.h>

void* _realloc (void* old, long size, const char* type)
{
	void* out = NULL;
	if(size<0) size=0;
	if(!size) free(old);
	else out = realloc(old, size);

	if(!type) type = "newtype";
	if(out || !size){
		if(!old &&  size) memory_alloc(type);
		if( old && !size) memory_freed(type);
	}
	return out;
}

#else

/*
	IMPORTANT: There must be NO memory corruption caused by the user.

	Big Problem:
	What should the user do if memory cannot be allocated? Especially
	for _realloc() where the originally allocated memory is even freed!

	Copyright (c) 2014, Rhyscitlema <info@rhyscitlema.com>.
*/

#include "_stddef.h"
#define puts1(...)
#define sprintf1(...)

typedef struct _FreeMemory
{
	size_t* start;
	size_t* stop;
	struct _FreeMemory *next;
	struct _FreeMemory *prev;
} FreeMemory;

#define FMSIZE (sizeof(FreeMemory)/sizeof(size_t))

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
	sprintf1(str, "\n------------------------ %s\n", debugString);
	puts1(str);

	for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
	{
		sprintf1(str, "freeMemory = %p vs %p, start=%p stop=%p prev=%p next=%p\n",
		freeMemory, freeMemory->stop - FMSIZE, freeMemory->start,
		freeMemory->stop, freeMemory->prev, freeMemory->next);
		puts1(str);
	}
	sprintf1(str, "fragmentsCount = %d\n", getFragmentsCount());
	puts1(str);*/
}



void useFreeMemory (long* freeMemoryArray, long length)
{
	if(!freeMemoryArray || !length) return;
	length = length*sizeof(long)/sizeof(size_t);
	size_t* freeMemoryArr = (size_t*)freeMemoryArray;

	if(firstFreeMemory==NULL)
	{
		firstFreeMemory = (FreeMemory*) (freeMemoryArr + length - FMSIZE);
		firstFreeMemory->start = freeMemoryArr;
		firstFreeMemory->stop  = freeMemoryArr + length;
		firstFreeMemory->next  = NULL;
		firstFreeMemory->prev  = NULL;
	}
	else // firstFreeMemory != NULL   TODO: to be implemented later ...
	{
		//while(freeMemory->next != NULL)
		//    freeMemory = freeMemory->next;
	}
	printFreeMemory("useFreeMemory");
}



static void* do_malloc (size_t size)
{
	char debugString[100];
	FreeMemory *freeMemory;
	size_t *r, *result;
	size_t x=0;

	if(size < FMSIZE-1)
	   size = FMSIZE-1;

	for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
	{
		/* if( available memory >= required memory ) */
		if( freeMemory->stop >= (freeMemory->start + size + 1) )
		{ x =freeMemory->stop - (freeMemory->start + size + 1); break; }
	}
	if(freeMemory == NULL) { puts1("Error: Malloc Fail   "); return NULL; }

	// Reserve space where size of memory block will be stored.
	// Note: stored size is that of entire block, so do a +1.
	result = freeMemory->start + 1;

	// If remaining memory can contain a structure
	if(x >= FMSIZE)
	{
		sprintf1(debugString, "_MALLOC 1 result_start = %p  result_stop = %p",
			freeMemory->start, freeMemory->start + size + 1);

		// If the 'current' structure will not be overwritten,
		// Then re-assign the new size of the free memory block.
		*freeMemory->start = size+1;
		freeMemory->start += size+1;
	}
	else
	{
		sprintf1(debugString, "_MALLOC 2 result_start = %p  result_stop = %p",
			freeMemory->start, freeMemory->stop);

		// Else, completely remove (or un-free) the 'current' free memory block,
		// by storing the size of the allocated memory as being that of the entire freeMemory.
		*freeMemory->start = freeMemory->stop - freeMemory->start;
		deleteNode(freeMemory);
	}

	// set all data to 0xFF, so that improper code will hopefully crash...!!!
	for(r = result; r < result+size; r++) *r = ~(size_t)0;

	printFreeMemory(debugString);
	return result;
}



static void do_free (void* address)
{
	char debugString[100];
	size_t *startC;   // start of current freed memory block
	size_t *stopC;    // stop of current freed memory block
	size_t *stopP;    // stop of preceding free memory block
	size_t *startN;   // start of succeeding free memory block
	FreeMemory *freeMemory, *temp;

	if(address==NULL) return;

	// starting address = given address - memory reserved to store total size
	startC = (size_t*)address - 1;

	// stopping address = starting address + total size of memory block to be freed
	stopC  = startC + *startC;

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
		sprintf1(debugString, "_FREE 1 startC = %p  stopC = %p", startC, stopC);

		// Prepare to put structure at the end of freed memory block
		temp = (FreeMemory*)(stopC - FMSIZE);

		temp->start = startC;
		temp->stop  = stopC;

		temp->next = freeMemory;
		temp->prev = freeMemory->prev;

		if(temp->next != NULL) temp->next->prev = temp;
		if(temp->prev != NULL) temp->prev->next = temp;
	}

	else if(stopP != startC && stopC == startN) // If can only concatenate with the right free block
	{
		sprintf1(debugString, "_FREE 2 startC = %p  stopC = %p", startC, stopC);

		freeMemory->start = startC;
	}

	else if(stopP == startC && stopC != startN) // If can only concatenate with the left free block
	{
		// this section will crash iff startC is ever NULL
		sprintf1(debugString, "_FREE 3 startC = %p  stopC = %p", startC, stopC);

		// move the structure from previous to new location
		temp = (FreeMemory*)(stopC - FMSIZE);

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
		sprintf1(debugString, "_FREE 4 startC = %p  stopC = %p", startC, stopC);
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



void* _realloc (void* old, long size, const char* type)
{
	size = ((size+sizeof(size_t)-1)/sizeof(size_t));
	void* out = NULL;
	if(size<0) size=0;
	if(!size) do_free(old);
	else if(!old)
		out = do_malloc(size);
	else{
		size_t totalSize = *((size_t*)old-1);
		if(size == totalSize-1)
			out = old;
		else{
			do_free(old);
			out = do_malloc(size);
			size_t* n = (size_t*)out;
			size_t* o = (size_t*)old;
			while(size--) *n++ = *o++;
		}
	}
	if(!type) type = "newtype";
	if(out || !size){
		if(!old &&  size) memory_alloc(type);
		if( old && !size) memory_freed(type);
	}
	return out;
}



long getFragmentsCount ()
{
	long fragmentsCount = 0;
	FreeMemory *freeMemory;

	for(freeMemory = firstFreeMemory; freeMemory != NULL; freeMemory = freeMemory->next)
		fragmentsCount++;

	return fragmentsCount;
}

#endif // end of #else of #ifndef USE_CUSTOM_MALLOC_H



/***********************************************************************************************************/

#ifdef DEBUG

#include <stdio.h>
#include <assert.h>

typedef struct _MEM {
	const char* type;
	int alloc, freed;
} MEM;

static MEM mem[100];

static _Bool equal (const char* a, const char* b)
{
	_Bool r=false;
	if(a && b)
	{
		for( ; *a==*b; a++,b++)
		{ if(!*a) { r=true; break; } }
	}
	return r;
}

void onMemoryOperation (const char* type, bool allocated)
{
	int i;
	if(!type) // print statistics
	{
		for(i=0; mem[i].type!=NULL; i++)
			printf("%-15s:   alloc = %-7d   freed = %-7d   difference = %d\n",
				mem[i].type, mem[i].alloc, mem[i].freed, mem[i].alloc - mem[i].freed);
		printf("\n");
	}
	else if(!allocated) // record memory freed
	{
		for(i=0; mem[i].type!=NULL; i++)
			if(equal(type, mem[i].type))
			{ mem[i].freed++; break; }
		if(mem[i].type==NULL)
			printf("Software Error in memory_freed: '%s' not found.\n", type);
		assert(mem[i].type!=NULL);
	}
	else // record memory allocated
	{
		for(i=0; mem[i].type!=NULL; i++)
			if(equal(type, mem[i].type))
			{ mem[i].alloc++; break; }
		if(!mem[i].type){
			mem[i].type = type;
			mem[i].alloc = 1;
		}
	}
}

#endif // end of #ifdef DEBUG


/***********************************************************************************************************/


/*  Finding the smallest power of two
*   greater or equal to a given value
*
*   starting from
*     n    = 0010010100
*    n-1   = 0010010011
*   n&=n-1 = 0010010000
*    n-1   = 0010001111
*   n&=n-1 = 0010000000
*    n-1   = 0001111111
*   n&=n-1 = 0000000000
*
*   So the result is r = the latest non-zero 'n'.
*   If initial was not a power of 2 then do r<<1.

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
*/

