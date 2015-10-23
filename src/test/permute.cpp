///////////////////////////////////////////////////////////////////////////////
// permute.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "btree.h"

int order;
int limited;
int limit;

void assert_failed(const char* expr)
{
	std::cout << "assert " << expr << " failed" << std::endl;
	std::cout << "(Press a key and Enter)" << std::flush;
	char c;
	std::cin >> c;
	exit(1); 
}

#define assert(expr) { if (!(expr)) assert_failed(#expr); }
#define DEBUG_HERE std::cout << __LINE__ << std::endl;

void conv_units(double &value, char * &units)
{
    if (value >= 60) {
        value /= 60;
        if (value >= 60) {
            value /= 60;
            if (value >= 24) {
                value /= 24;
                units = (char*)"days";
            } else
                units = (char*)"hours";
        } else
            units = (char*)"minutes";
    } else
        units = (char*)"seconds";
}

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
    return true;
}

bool write(void* context, BT::BTIDX i, void* node, size_t size)
{
    memcpy((char*)context+i*size, node, size);
    return true;
}

void test_add(int count, int *nums)
{
    int i;

    //std::cout << count << ": ";
    //for (i=0; i<count; ++i)
    //    std::cout << nums[i] << ' ';
    //std::cout << std::endl;

    // create B-Tree header
    BT::HEADER header;
    BT::init(header, order, sizeof(int), 100);

    // calculate number of nodes required for header
    int reserved = (sizeof(header) + header.nodesize-1)/header.nodesize;

    // allocate memory
    size_t memsize = (header.free+reserved)*header.nodesize;
    char* mem = new char[memsize];

    // initialize memory to B-Tree image
    memcpy(mem, &header, sizeof(BT::HEADER));

    BTCACHERW2 cache(1, mem, header.nodesize, sizeof(header), read, write);
    BT btree(&cache, getkey, compare, BTCACHERW2::cache_read, BTCACHERW2::cache_write);
//    BT btree(mem, getkey, compare, read, write); 
    btree.clear();
    
    static int last_count=0;
    static int sub_count=0;
    static time_t last_time=time(0);

    if (count==last_count)
        ++sub_count;
    else 
    {
        if (last_count > 0) 
        {
            time_t now=time(0);
            double dur=(double)(now-last_time);
            double next_dur=dur*count;
            char *units, *next_units;

            last_time = now;
            std::cout << last_count << " #" << sub_count << " successful at "
				<< ctime(&now) << std::flush;
            conv_units(dur, units);
            conv_units(next_dur, next_units);
            std::cout << ' ' << dur << ' ' << units;
            if (limited && count > limit) 
            {
				delete [] mem;
				delete [] nums;
                std::cout << std::endl;
                exit(0);
            } 
            else 
            {
                std::cout << ", next result in " << next_dur << ' ' << next_units
					<< std::endl << std::flush;
            }
        }
        sub_count = 1;
    }

    for (i=0; i<count; ++i) 
    {
        long key;

        key=nums[i];
        btree.add(&key);
    }

    assert(btree.count() == count);

    for (i=0; i<count; ++i) 
    {
        BTITER found(btree.end());
        int key;

        key=nums[i];
        char* record = new char[header.recordsize];
        found = btree.find(&key, record);
        assert(found != btree.end());
        assert(key == *(int*)record);
        delete [] record;
    }

    last_count = count;
    
    cache.commit();
    cache.toss();

    delete [] mem;
    mem = 0;
}

void permute(int count, int *nums_taken, int remaining, int *nums_order)
{
    int i;

    if (count==remaining) {
        assert(nums_taken == 0);
        assert(nums_order == 0);
        nums_taken = new int[count];
        for (i=0; i<count; ++i)
            nums_taken[i] = 0;
        nums_order = new int[count];
    }

    for (i=0; i<count; ++i) {
        if (!nums_taken[i]) {
            nums_taken[i] = 1;
            nums_order[count-remaining] = i+1;
            if (remaining == 1)
                test_add(count, nums_order);
            else
                permute(count, nums_taken, remaining-1, nums_order);
            nums_taken[i] = 0;
        }
    }

    if (count==remaining) {
        delete [] nums_taken;
        delete [] nums_order;
    }
}

int main(int argc, char *argv[])
{
//    Serial console(USBTX, USBRX);
//    console.baud(921600); // mbed to pc usb serial supports: 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600
//    set_time(1256729737);
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    int count;
//    char *order_env;

//    if (argc > 1) {
//        limited = 1;
//        limit = atoi(argv[1]);
//    } else {
        limited = 1;
        limit = 50;
//    }

//    order_env = getenv("BTREEORDER");
//    if (order_env != (char *)0) {
//        order = atoi(order_env);
//        if (order < 3 || order%2 == 0)
//            order = 3;
//    } else
        order = 3;

    std::cout << "permutation testing, adding to " << order << "-order B-Tree" <<std::endl;

    for (count=1; 1; ++count)
        permute(count, 0, count, 0);
}
