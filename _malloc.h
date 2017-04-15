/*
    _malloc.h
*/
#ifndef __MALLOC_H_
#define __MALLOC_H_


/* make use of the given free memory block */
extern void useFreeMemory (void* freeMemoryArray, long freeMemorySize);

/* memory allocation, of size given in number of bytes */
extern void* _malloc (long size);

/* memory re-allocation, to size given in number of bytes */
extern void* _realloc (void* address, long size);

/* free or release allocated memory */
extern void _free (void* address);

/* memory increase, to a larger chunk while preserving data */
extern void* meminc (void* old_mem, long old_size, long new_size);

/* get number of fragments of free-memory
 * this is a measure of efficiency of
 * the memory allocation algorithm */
extern long getFragmentsCount ();


/* upon using _malloc(), call memory_alloc()
   this will count the number of _malloc calls made under given name

   upon using _free()  , call memory_freed()
   this will count the number of _free   calls made under given name

   memory_print will then print the statistics (the counts).
   This helps to keep track of memory allocations.
*/

#ifndef NDEBUG
extern void memory_alloc (const char* str);
extern void memory_freed (const char* str);
extern void memory_print ();
#else
#define memory_alloc(str)
#define memory_freed(str)
#define memory_print(str)
#endif

#endif

