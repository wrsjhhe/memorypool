#pragma once

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
