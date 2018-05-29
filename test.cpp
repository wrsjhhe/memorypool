#include "memerypool.h"
#include <iostream>
#include <time.h>

int main()
{
	memorypool *m = new memorypool(sizeof(int), 4, sizeof(void*), 0);

	clock_t start_time = clock();

	for (int i = 0; i < 1000000; i++)
	{
		int* p1 = (int*)m->alloc();
		int* p2 = (int*)m->alloc();
		int* p3 = (int*)m->alloc();
		int* p4 = (int*)m->alloc();

		m->dealloc(p1);
		m->dealloc(p2);
		m->dealloc(p3);
		m->dealloc(p4);
	}

	clock_t end_time = clock();
	std::cout << "memerypool用时:" << end_time - start_time << std::endl;

	start_time = clock();
	for (int i = 0; i < 1000000; i++)
	{
		int *p1 = new int;
		int *p2 = new int;
		int *p3 = new int;
		int *p4 = new int;

		delete p1, p2, p3, p4;
	}
	end_time = clock();
	std::cout << "newdelete用时:" << end_time - start_time << std::endl;

	system("pause");
	return 0;
}
