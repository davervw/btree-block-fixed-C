///////////////////////////////////////////////////////////////////////////////
// test1.cpp
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

int main(int argc, char* argv[])
{
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	// create B-Tree header
	BT::HEADER header;
	BT::init(header, 3, sizeof(int), 1000000);

	// calculate number of nodes required for header
	int reserved = (sizeof(header) + header.nodesize-1)/header.nodesize;

	// allocate memory
	size_t memsize = (header.free+reserved)*header.nodesize;
	char* mem = new char[memsize];

	// initialize memory to B-Tree image
	memcpy(mem, &header, sizeof(BT::HEADER));

	BT tree(mem, getkey, compare, read, write); 
	tree.clear();

	// adds evens 0..8 then odds 1..9, then deletes in same order
	for (int op=0; op<2; ++op)
	for (int pass=0; pass<2; ++pass)
	for (int i=pass; i<10; i+=2)
	{
		//int i = seq[index];
		if (op==0)
		{
			std::cout << "adding " << i << std::endl;
			tree.add(&i);
		}
		else
		{
			std::cout << "deleting " << i << std::endl;
			tree.del(&i);
		}

		std::cout << "result:" << std::endl;
		tree.traverse(display);
		tree.display(display2);
		tree.dump(std::cout);
	}

	tree.clear();
	delete [] mem;

	std::cout << "(Press a key and Enter)" << std::flush;
	char c;
	std::cin >> c;
}
