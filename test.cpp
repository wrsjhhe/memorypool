#include "memorypool.h"
#include <iostream>
#include <time.h>
#include <vector>

//效率测验
void bench()
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

	delete m;
}

class People
{
public:
	char name[20];
	int age;

	People(char* n, int a) :age(a)
	{
		strcpy_s(name, n);
	}

	~People()
	{
		std::cout << "destructor" << std::endl;
	}
};


//功能示例
void test()
{
	//堆上分配内存大小32 * 4 + sizeof(void*) + 8 = 144
	memorypool *m = new memorypool(30, 4, sizeof(void*), 0);

	//分配内存
	People* p0 = (People*)m->alloc();
	People* p1 = (People*)m->alloc();
	People* p2 = (People*)m->alloc();
	People* p3 = (People*)m->alloc();

	//添加数据
	strcpy_s(p0->name, "zhangsan");
	p0->age = 20;

	strcpy_s(p1->name, "lisi");
	p1->age = 19;

	strcpy_s(p2->name, "wangwu");
	p0->age = 21;

	strcpy_s(p3->name, "zhaoliu");
	p1->age = 18;

	//遍历
	m->traversalinit();
	People* px = (People*)m->traverse();
	px = (People*)m->traverse();
	px = (People*)m->traverse();
	m->traversalinit();
	px = (People*)m->traverse();


	//释放内存
	m->dealloc(p0);
	m->dealloc(p1);
	m->dealloc(p2);
	m->dealloc(p3);

	delete m;
}

int main()
{
	test();
	bench();

	system("pause");
	return 0;
}

