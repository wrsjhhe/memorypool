#include "memerypool.h"
#include <cstdio>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// memorypool()   The constructors of memorypool.                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

memorypool::memorypool()
{
	firstblock = nowblock = (void **)NULL;
	nextitem = (void *)NULL;
	deaditemstack = (void *)NULL;
	pathblock = (void **)NULL;
	pathitem = (void *)NULL;
	alignbytes = 0;
	itembytes = itemwords = 0;
	itemsperblock = 0;
	items = maxitems = 0l;
	unallocateditems = 0;
	pathitemsleft = 0;
}

memorypool::memorypool(int bytecount, int itemcount, int wsize,
	int alignment)
{
	poolinit(bytecount, itemcount, wsize, alignment);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// ~memorypool()   Free to the operating system all memory taken by a pool.  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

memorypool::~memorypool()
{
	while (firstblock != (void **)NULL) {
		nowblock = (void **) *(firstblock);
		free(firstblock);
		firstblock = nowblock;
	}
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// poolinit()    Initialize a pool of memory for allocation of items.        //
//                                                                           //
// A `pool' is created whose records have size at least `bytecount'.  Items  //
// will be allocated in `itemcount'-item blocks.  Each item is assumed to be //
// a collection of words, and either pointers or floating-point values are   //
// assumed to be the "primary" word type.  (The "primary" word type is used  //
// to determine alignment of items.)  If `alignment' isn't zero, all items   //
// will be `alignment'-byte aligned in memory.  `alignment' must be either a //
// multiple or a factor of the primary word size;  powers of two are safe.   //
// `alignment' is normally used to create a few unused bits at the bottom of //
// each item's pointer, in which information may be stored.                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void memorypool::poolinit(int bytecount, int itemcount, int wordsize,
	int alignment)
{
	// Find the proper alignment, which must be at least as large as:
	//   - The parameter `alignment'.
	//   - The primary word type, to avoid unaligned accesses.
	//   - sizeof(void *), so the stack of dead items can be maintained
	//       without unaligned accesses.
	if (alignment > wordsize) {
		alignbytes = alignment;
	}
	else {
		alignbytes = wordsize;
	}
	if ((int) sizeof(void *) > alignbytes) {
		alignbytes = (int) sizeof(void *);
	}
	itemwords = ((bytecount + alignbytes - 1) / alignbytes)
		* (alignbytes / wordsize);
	itembytes = itemwords * wordsize;
	itemsperblock = itemcount;

	// Allocate a block of items.  Space for `itemsperblock' items and one
	//   pointer (to point to the next block) are allocated, as well as space
	//   to ensure alignment of the items. 
	firstblock = (void **)malloc(itemsperblock * itembytes + sizeof(void *)
		+ alignbytes);
	if (firstblock == (void **)NULL) {
		
	}
	// Set the next block pointer to NULL.
	*(firstblock) = (void *)NULL;
	restart();
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// restart()   Deallocate all items in this pool.                            //
//                                                                           //
// The pool is returned to its starting state, except that no memory is      //
// freed to the operating system.  Rather, the previously allocated blocks   //
// are ready to be reused.                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void memorypool::restart()
{
	uintptr_t alignptr;

	items = 0;
	maxitems = 0;

	// Set the currently active block.
	nowblock = firstblock;
	// Find the first item in the pool.  Increment by the size of (void *).
	alignptr = (uintptr_t)(nowblock + 1);
	// Align the item on an `alignbytes'-byte boundary.
	nextitem = (void *)
		(alignptr + (uintptr_t)alignbytes -
		(alignptr % (uintptr_t)alignbytes));
	// There are lots of unallocated items left in this block.
	unallocateditems = itemsperblock;
	// The stack of deallocated items is empty.
	deaditemstack = (void *)NULL;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// alloc()   Allocate space for an item.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void* memorypool::alloc()
{
	void *newitem;
	void **newblock;
	uintptr_t alignptr;

	// First check the linked list of dead items.  If the list is not 
	//   empty, allocate an item from the list rather than a fresh one.
	if (deaditemstack != (void *)NULL) {
		newitem = deaditemstack;                     // Take first item in list.
		deaditemstack = *(void **)deaditemstack;
	}
	else {
		// Check if there are any free items left in the current block.
		if (unallocateditems == 0) {
			// Check if another block must be allocated.
			if (*nowblock == (void *)NULL) {
				// Allocate a new block of items, pointed to by the previous block.
				newblock = (void **)malloc(itemsperblock * itembytes + sizeof(void *)
					+ alignbytes);
				if (newblock == (void **)NULL) {
					
				}
				*nowblock = (void *)newblock;
				// The next block pointer is NULL.
				*newblock = (void *)NULL;
			}
			// Move to the new block.
			nowblock = (void **)*nowblock;
			// Find the first item in the block.
			//   Increment by the size of (void *).
			alignptr = (uintptr_t)(nowblock + 1);
			// Align the item on an `alignbytes'-byte boundary.
			nextitem = (void *)
				(alignptr + (uintptr_t)alignbytes -
				(alignptr % (uintptr_t)alignbytes));
			// There are lots of unallocated items left in this block.
			unallocateditems = itemsperblock;
		}
		// Allocate a new item.
		newitem = nextitem;
		// Advance `nextitem' pointer to next free item in block.
		nextitem = (void *)((uintptr_t)nextitem + itembytes);
		unallocateditems--;
		maxitems++;
	}
	items++;
	return newitem;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// dealloc()   Deallocate space for an item.                                 //
//                                                                           //
// The deallocated space is stored in a queue for later reuse.               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void memorypool::dealloc(void *dyingitem)
{
	// Push freshly killed item onto stack.
	*((void **)dyingitem) = deaditemstack;
	deaditemstack = dyingitem;
	items--;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// traversalinit()   Prepare to traverse the entire list of items.           //
//                                                                           //
// This routine is used in conjunction with traverse().                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void memorypool::traversalinit()
{
	uintptr_t alignptr;

	// Begin the traversal in the first block.
	pathblock = firstblock;
	// Find the first item in the block.  Increment by the size of (void *).
	alignptr = (uintptr_t)(pathblock + 1);
	// Align with item on an `alignbytes'-byte boundary.
	pathitem = (void *)
		(alignptr + (uintptr_t)alignbytes -
		(alignptr % (uintptr_t)alignbytes));
	// Set the number of items left in the current block.
	pathitemsleft = itemsperblock;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// traverse()   Find the next item in the list.                              //
//                                                                           //
// This routine is used in conjunction with traversalinit().  Be forewarned  //
// that this routine successively returns all items in the list, including   //
// deallocated ones on the deaditemqueue. It's up to you to figure out which //
// ones are actually dead.  It can usually be done more space-efficiently by //
// a routine that knows something about the structure of the item.           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void* memorypool::traverse()
{
	void *newitem;
	uintptr_t alignptr;

	// Stop upon exhausting the list of items.
	if (pathitem == nextitem) {
		return (void *)NULL;
	}
	// Check whether any untraversed items remain in the current block.
	if (pathitemsleft == 0) {
		// Find the next block.
		pathblock = (void **)*pathblock;
		// Find the first item in the block.  Increment by the size of (void *).
		alignptr = (uintptr_t)(pathblock + 1);
		// Align with item on an `alignbytes'-byte boundary.
		pathitem = (void *)
			(alignptr + (uintptr_t)alignbytes -
			(alignptr % (uintptr_t)alignbytes));
		// Set the number of items left in the current block.
		pathitemsleft = itemsperblock;
	}
	newitem = pathitem;
	// Find the next item in the block.
	pathitem = (void *)((uintptr_t)pathitem + itembytes);
	pathitemsleft--;
	return newitem;
}
