///////////////////////////////////////////////////////////////////////////////
// throwexception.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "throwexception.h"

void throw_exception(const char* message)
{
#if 0
    std::cerr << message << std::endl;
    exit(1);
#else
    throw message;
#endif
}
