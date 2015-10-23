///////////////////////////////////////////////////////////////////////////////
// test4.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "btree.h"

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

void assert_trap()
{
}

#ifdef _DEBUG
#undef assert
#define assert(cond) if (!(cond)) { std::cerr << __FILE__ << ':' << __LINE__ << " assertion failed (" << #cond << ')' << std::endl; assert_trap(); exit(1); }
#define assertf(cond,str,arg) if (!(cond)) { fprintf(stderr,"%s:%d: assertion failed (%s)\n", __FILE__, __LINE__, #cond); fprintf(stderr, str, arg); assert_trap(); exit(1); }
#define assertf2(cond,str,arg1,arg2) if (!(cond)) { fprintf(stderr,"%s:%d: assertion failed (%s)\n", __FILE__, __LINE__, #cond); fprintf(stderr, str, arg1, arg2); assert_trap(); exit(1); }
#define assertfn(cond,fn) if (!(cond)) { fprintf(stderr,"%s:%d: assertion failed (%s)\n", __FILE__, __LINE__, #cond); fn; assert_trap(); exit(1); }
#else
#define assert(cond) ;
#define assertf(cond,str,arg) ;
#define assertf2(cond,str,arg1,arg2) ;
#define assertfn(cond,fn) ;
#endif

int debug_dump = 0;

void getkey(void* key, void* record)
{
    memcpy(key, record, sizeof(int));
}

int compare(void* key1, void* key2)
{
    return *(int*)key1 - *(int*)key2;
}

bool read(void* context, BT::BTIDX i, void* node, size_t size)
{
    memcpy(node, (char*)context+i*size, size);
	if (debug_dump)
	{
		if (i==0)
		{
			std::cout << "read header ";
			BT::dump_header(std::cout, (BT::HEADER*)context);
		}
		else
		{
			std::cout << "read ";
			BT::dump_node(std::cout, (BT::HEADER*)context, i, (char*)node);
		}
	}
    return true;
}

bool write(void* context, BT::BTIDX i, void* node, size_t size)
{
    memcpy((char*)context+i*size, node, size);
	if (debug_dump)
	{
		if (i==0)
		{
			std::cout << "wrote header ";
			BT::dump_header(std::cout, (BT::HEADER*)context);
		}
		else
		{
			std::cout << "wrote ";
			BT::dump_node(std::cout, (BT::HEADER*)context, i, (char*)node);
		}
	}
    return true;
}

void display(void* context, void* key, void* record)
{
	std::cout << *(int*)key << std::endl;
}

std::ostream& display2(std::ostream& str, void* key, void* record)
{
	return str << *(int*)key;
}

int main(int argc, char *argv[])
{
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	int count=10000;
	int *data=new int[count];
	int i, j;
	int repeat;
	BT::INTENTRIES order;
	int debug_i_start=9999;
	int debug_i_end=10000;
	int debug_count=1;
	int debug_slow=0;
	int debug_progress=0;
	int duplicates=0;
	char s[100];
	FILE *fp;

	if (getenv("COUNT") != NULL)
		count = atoi(getenv("COUNT"));

	if (getenv("DEBUG_COUNT") != NULL)
		debug_count=1;

	if (getenv("DEBUG_SLOW") != NULL)
		debug_slow=1;

	if (getenv("DEBUG_PROGRESS") != NULL)
		debug_progress=1;

	if (getenv("DUPLICATES") != NULL)
		duplicates=1;

	if (argc == 1)
		fp=fopen("random.data", "r");
	else
		fp=fopen(argv[1], "r");

	if (argc > 2)
		debug_i_start = atoi(argv[2]);

	if (argc > 3)
		debug_i_end = atoi(argv[3]);

	if (fp == NULL) {
		perror("btree");
		return 1;
	}

	// read the data
	for (i=0; i<count; ++i) {
		if (fgets(s, sizeof(s), fp) == NULL) {
			std::cerr << "btree: fgets returned NULL, exiting, i=" << i << std::endl;
			return 1;
		}
		data[i] = atoi(s);
	}

	fclose(fp);

	// test for various orders (must be odd, 3 or greater)
	for (order=3; order<=63; order+=2) 
	{
		// repeat each test n times
		for (repeat=0; repeat<1; ++repeat) 
		{

			// create B-Tree header
			BT::HEADER header;
			BT::init(header, order, sizeof(int), count);

			// calculate number of nodes required for header
			int reserved = (sizeof(header) + header.nodesize-1)/header.nodesize;

			// allocate memory
			size_t memsize = (header.free+reserved)*header.nodesize;
			char* mem = new char[memsize];

			// initialize memory to B-Tree image
			memcpy(mem, &header, sizeof(BT::HEADER));

			BT btree(mem, getkey, compare, read, write); 
			btree.clear();

			clock_t now;
			clock_t then=clock();
		
			assertf(btree.count() == 0, "%ld\n", btree.count());
			//tree->display(1);
		
			for (i=0; i<count; ++i) {
				//std::cerr << i << ' ';
				//assert(tree != NULL);

				if ( (i % 100 == 0 && debug_progress) ||
                     (i >= debug_i_start && i <= debug_i_end) )
				{
                    std::cout << std::endl << "BEFORE Adding " << data[i] << " (data["
						<< (int)i << "]):" << std::endl << std::flush;
					//if (i >= debug_i_start && i <= debug_i_end)
					//	btree.display(display2);
					debug_dump = 0;
					//btree.dump(std::cout);
				}

				//debug_dump = (i >= debug_i_start && i <= debug_i_end);
				BTITER iter = btree.add(&data[i]);

                if (debug_progress && (i >= debug_i_start && i <= debug_i_end) )
                {
                    std::cout << std::endl << "AFTER Adding " << data[i] << " (data["
						<< (int)i << "]):" << std::endl << std::flush;
                    //btree.display(&display2);
                    //std::cout << "tree->get_count()=" << btree.count() << std::endl;

					debug_dump = 0;
					//btree.dump(std::cout);

					bool check = btree.check(0);
					assert(check);
                }
	
				//assert(btree.check(0));

				//tree->print();
				//std::cout << std::endl << endl;
		
				//int levels = tree->levels();
				//for (j=1; j<=levels; ++j) {
				//	tree->display(j);
				//	std::cout << std::endl;
				//}
				//std::cout << std::endl;
		
				//tree->display2(0);
				//std::cout << "tree->get_count()=" tree->get_count() << std::endl;

				if (debug_count)
				{
					assertf2(btree.count() == i+1, "%ld != %d\n",
						btree.count(), i+1);
				}
			}
		
			//tree->display2(0);
			now = clock();
		
			std::cout << "Inserting " << count <<
				" items in " << int(order) << "-order B-Tree: ";
			//std::cout.setf(ios::showpoint);
			std::cout.precision(3);
			std::cout << (double)(now-then)/CLOCKS_PER_SEC << " seconds" << std::endl << std::flush;

			assertf2(btree.count() == count,
				"%ld != %d\n", btree.count(), count);

			then = clock();

			//btree.clear();
			for (i=count-1; i>=0; --i) 
			{
				//debug_dump = (i >= debug_i_start && i <= debug_i_end);

				char* record = new char[header.recordsize];
				BTITER ptr = btree.find(&data[i], record);
				delete [] record;
				assert(ptr != btree.end());
				//assertf2(ptr.pos >= 0, "pos=%d, i=%d\n", ptr.pos, i);
				//assertf2(pos < order, "pos=%d, i=%d\n", pos, i);

				if ( (i % 100 == 0 && debug_progress) || 
					(i >= debug_i_start && i <= debug_i_end) )
				{
					std::cout << "BEFORE DELETING " << data[i] << "(data[" << (int)i
						<< "])" << std::endl << std::flush;
					if (debug_dump)
					{
						std::cout << "header: ";
 						btree.dump_header(std::cout, (BT::HEADER*)mem);
					}
					//if (i >= debug_i_start && i <= debug_i_end)
					//	btree.display(&display2);
				}

				//assert(btree.check(0));

				btree.del(ptr);

				if ( (i % 100 == 0 && debug_progress) ||
					 (i >= debug_i_start && i <= debug_i_end) )
				{
					std::cout << std::endl << "AFTER DELETING " << data[i] << std::flush;
					//btree.display(&display2);
					//std::cout << std::endl;

					int save_debug_dump = debug_dump;
					debug_dump=0;
					bool check = btree.check(0);
					assert(check);
					debug_dump = save_debug_dump;
				}

				if (debug_count)
				{
					assertf(btree.count() == i, "i=%d\n", i);
				}

				if (!duplicates) {
					// shouldn't be able to find it again
					char* record = new char[header.recordsize];
					ptr = btree.find(&data[i], record);
					delete [] record;
					assert(ptr==btree.end());
				}

				if (debug_slow) {
					// verify that other entries are there
					char* record = new char[header.recordsize];
					for(j=0; j<i; ++j) {
						BTITER ptr = btree.find(&data[j], record);
						assert(ptr != btree.end());
						//assertfn(ptr.pos >= 0,
						//	std::cerr << "after deleting btree[" << i << "]="
						//		<< data[i] << ", btree[" << j << "]="
						//		<< data[j] << "missing" << std::endl );
					}
					delete [] record;
				}

			}

			now = clock();

			std::cout << ", Deleting: ";
			//std::cout.setf(ios::showpoint);
			std::cout.precision(3);
			std::cout << (double)(now-then)/CLOCKS_PER_SEC << " secs" << std::endl << std::flush;

			assertf(btree.count() == 0, "%ld\n", btree.count());

			delete mem;
		}
	}

	delete [] data;

	return 0;
}
