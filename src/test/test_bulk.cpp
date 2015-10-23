///////////////////////////////////////////////////////////////////////////////
// test_bulk.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "btree.h"

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

    char mem[65536];
    BT::init(*(BT::HEADER*)mem, 3, sizeof(int), 1000);
    BT btree(mem, getkey, compare, read, write);

    for (int i=1; i<=26; ++i)
        btree.add(&i);
    for (int i=1; i<=26; ++i)
        btree.add(&i);
    std::cout << "records = " << btree.count() << std::endl;
    std::cout << "nodes = " << ((BT::HEADER*)mem)->nodes << std::endl;
    std::cout << "reserved = " << (sizeof(BT::HEADER)+((BT::HEADER*)mem)->nodesize-1)/((BT::HEADER*)mem)->nodesize << std::endl;
    std::cout << "check returned " << (btree.check(0)?"true":"false") << std::endl;
    btree.display(display2);

    BTBULK* bulk = btree.load_begin(26);
    for (int i=1; i<=26; ++i)
        btree.load_record(bulk, &i);
    btree.load_end(bulk);
    std::cout << "records = " << btree.count() << std::endl;
    std::cout << "nodes = " << ((BT::HEADER*)mem)->nodes << std::endl;
    std::cout << "reserved = " << (sizeof(BT::HEADER)+((BT::HEADER*)mem)->nodesize-1)/((BT::HEADER*)mem)->nodesize << std::endl;
    std::cout << "check returned " << (btree.check(0)?"true":"false") << std::endl;
    btree.display(display2);

	std::cout << "(Press a key and Enter)" << std::flush;
	char c;
	std::cin >> c;
}
