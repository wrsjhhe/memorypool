#pragma once
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Memorypool                                                                //
//                                                                           //
// A structure for memory allocation.                                        //
//                                                                           //
// firstblock is the first block of items. nowblock is the block from which  //
//   items are currently being allocated. nextitem points to the next slab   //
//   of free memory for an item. deaditemstack is the head of a linked list  //
//   (stack) of deallocated items that can be recycled.  unallocateditems is //
//   the number of items that remain to be allocated from nowblock.          //
//                                                                           //
// Traversal is the process of walking through the entire list of items, and //
//   is separate from allocation.  Note that a traversal will visit items on //
//   the "deaditemstack" stack as well as live items.  pathblock points to   //
//   the block currently being traversed.  pathitem points to the next item  //
//   to be traversed.  pathitemsleft is the number of items that remain to   //
//   be traversed in pathblock.                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
class memorypool {

public:

	void **firstblock, **nowblock;
	void *nextitem;
	void *deaditemstack;
	void **pathblock;
	void *pathitem;
	int  alignbytes;
	int  itembytes, itemwords;
	int  itemsperblock;
	long items, maxitems;
	int  unallocateditems;
	int  pathitemsleft;

	memorypool();
	memorypool(int, int, int, int);
	~memorypool();

	void poolinit(int, int, int, int);
	void restart();
	void *alloc();
	void dealloc(void*);
	void traversalinit();
	void *traverse();
};
