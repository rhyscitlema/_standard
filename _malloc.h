/*
    _malloc.h
*/
#ifndef __MALLOC_H_
#define __MALLOC_H_


/* memory allocation, of size given in number of bytes */
extern void* _malloc (long size);

/* memory re-allocation, to size given in number of bytes */
extern void* _realloc (void* address, long size);

/* free or release allocated memory */
extern void _free (void* address);

/* memory increase, to a larger chunk while preserving data */
extern void* meminc (void* old_mem, long old_size, long new_size);


#ifdef USE_CUSTOM_MALLOC_H

/* make use of the given free memory block */
extern void useFreeMemory (void* freeMemoryArray, long freeMemorySize);

/* get number of fragments of free-memory
 * this is a measure of efficiency of
 * the memory allocation algorithm */
extern long getFragmentsCount ();

#endif


/* Upon using _malloc(), call memory_alloc("a string literal")
   to count the number of _malloc calls made under given name.

   Upon using _free()  , call memory_freed("a string literal")
   to count the number of _free calls made under given name.

   memory_print() will then print the statistics (or counts).
   This helps to keep track of memory allocations.
*/

#ifdef DEBUG
extern void memory_alloc (const char* name);
extern void memory_freed (const char* name);
extern void memory_print ();
#else
#define memory_alloc(str)
#define memory_freed(str)
#define memory_print(str)
#endif

#endif

