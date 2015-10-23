///////////////////////////////////////////////////////////////////////////////
// test_load.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"
#include <iostream>

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

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	const int maxnodes = 1000;

	// create B-Tree header
	BT::HEADER header;
	BT::init(header, 41, sizeof(int), maxnodes);

	// calculate number of nodes required for header
	int reserved = (sizeof(header) + header.nodesize-1)/header.nodesize;

	// allocate memory
	size_t memsize = (header.free+reserved)*header.nodesize;
	char* mem = new char[memsize];

	// initialize memory to B-Tree image
	memcpy(mem, &header, sizeof(BT::HEADER));

	// cache the read/writes for performance
	BTCACHERW2 cache(4, mem, header.nodesize, sizeof(header), read, write);

	BT tree(&cache, getkey, compare, cache.cache_read, cache.cache_write); 
	//BT tree(mem, getkey, compare, read, write); 
	tree.clear();

	for (int numnodes=0; numnodes<=1000; ++numnodes)
	{
		std::cout << "================================" << std::endl;
		std::cout << "Testing " << numnodes << " nodes" << std::endl;
		std::cout.flush();

		BTBULK* bulk = tree.load_begin(numnodes);
		for (int i=0; i<numnodes; ++i)
		{
			int num = 10+i;
			tree.load_record(bulk, &num);
		}
		tree.load_end(bulk);

		std::cout << "Tree count returned " << tree.count() << std::endl;
		std::cout.flush();

		bool check = tree.check(0);
		std::cout << "Tree check returned " << (check?"ok":"***BAD***") << std::endl;
		std::cout.flush();

		cache.cache_read(&cache, 0, &header, sizeof(header));
		std::cout << "Order " << header.order << ", Levels " << tree.levels() << ", Nodes " << header.nodes << std::endl;
		std::cout.flush();

		//std::cout << "Tree contents:" << std::endl;
		//tree.traverse(display);
		//tree.display(display2);

		if (!check)
			throw "BAD TREE";

		cache.commit();
		cache.toss();

		int num=1;
		tree.add(&num);

		std::cout << "Tree count returned " << tree.count() << std::endl;
		std::cout.flush();

		check = tree.check(0);
		std::cout << "Tree check returned " << (check?"ok":"***BAD***") << std::endl;
		std::cout.flush();

		//std::cout << "Tree contents:" << std::endl;
		//tree.traverse(display);
		//tree.display(display2);

		cache.cache_read(&cache, 0, &header, sizeof(header));
		std::cout << "Order " << header.order << ", Levels " << tree.levels() << ", Nodes " << header.nodes << std::endl;
		std::cout.flush();

		if (!check)
			throw "BAD TREE";

		cache.commit();
		cache.toss();
	}

	delete [] mem;
	mem = 0;

	std::cout << "(Press a key and Enter)" << std::flush;
	char c;
	std::cin >> c;
}
