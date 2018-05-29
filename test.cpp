#include "memorypool.h"
#include <iostream>
#include <time.h>
#include <vector>
int main()
{
	memorypool *m = new memorypool(sizeof(int), 4, sizeof(void*), 0);

	clock_t start_time = clock();

	for (int i = 0; i < 1000000; i++)
	{
		int* p0 = (int*)m->alloc();
		int* p1 = (int*)m->alloc();
		int* p2 = (int*)m->alloc();
		int* p3 = (int*)m->alloc();

		*p0 = 0;
		*p1 = 1;
		*p2 = 2;
		*p3 = 3;

		m->dealloc(p0);
		m->dealloc(p1);
		m->dealloc(p2);
		m->dealloc(p3);	
	}

	clock_t end_time = clock();
	std::cout << "memerypool用时:" << end_time - start_time << std::endl;


	start_time = clock();
	for (int i = 0; i < 1000000; i++)
	{
		int *p0 = new int;
		int *p1 = new int;
		int *p2 = new int;
		int *p3 = new int;

		*p0 = 0;
		*p1 = 1;
		*p2 = 2;
		*p3 = 3;

		delete p0, p1, p2, p3;
	}
	end_time = clock();
	std::cout << "newdelete用时:" << end_time - start_time << std::endl;

	std::vector<int> v(4);
	start_time = clock();
	for (int i = 0; i < 1000000; i++)
	{
		v.at(0) = 0;
		v.at(1) = 1;
		v.at(2) = 2;
		v.at(3) = 3;
	}
	end_time = clock();
	std::cout << "vector用时:" << end_time - start_time << std::endl;
	system("pause");
	return 0;
}
