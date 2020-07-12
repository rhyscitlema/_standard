/*
    _malloc.h
*/
#ifndef __MALLOC_H_
#define __MALLOC_H_


#define _malloc(size, type) _realloc(NULL, size, type)
#define _free(ptr, type) _realloc(ptr, 0, type)

/* Custom memory re-allocation:
   * size = memory size given in bytes.
   * if size==0 then memory is freed.
   * if old==NULL then a new malloc is done.
   * if type==NULL then it is set to "newtype".
   * if type!=NULL then it must be a string 'literal'.
*/
extern void* _realloc (void* old, long size, const char* type);


#ifdef USE_CUSTOM_MALLOC_H

/* make use of the given free memory block */
extern void useFreeMemory (long* freeMemoryArray, long length);

/* get number of fragments of free-memory.
 * this is a measure of efficiency of
 * the memory allocation algorithm. */
extern long getFragmentsCount ();

#endif


/* Upon using _malloc(), memory_alloc() is used to count
   the number of _malloc() calls made under given type.

   Upon using _free(), memory_free() is used to count
   the number of _free() calls made under given type.

   Use memory_print() to print the statistics.
   This helps keep track of memory allocations.
*/
#define memory_alloc(type) onMemoryOperation(type, 1)
#define memory_freed(type) onMemoryOperation(type, 0)
#define memory_print()     onMemoryOperation(NULL, 0)

#ifdef DEBUG
#include <stdbool.h>
extern void onMemoryOperation (const char* type, bool allocated);
#else
#define onMemoryOperation(type, alloc)
#endif


#endif
