///////////////////////////////////////////////////////////////////////////////
// btcache.cpp 
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"

// BTCACHE ////////////////////////////////////////////////////////////////////

BTCACHE::BTCACHE(BT::BTIDX idx, void* node, size_t size, bool modified)
{
    prev = 0;
    next = 0;
    this->idx = idx;
    this->node = new char[size];
    this->modified = modified;
    memcpy(this->node, node, size);
}

BTCACHE::~BTCACHE()
{
    BTCACHE* p_next = next;
    BTCACHE* p_prev = prev;

    delete [] node;
    node = 0;
    next = 0;
    prev = 0;

    if (p_next != 0)
    {
        p_next->prev = 0;
        delete p_next; // recursive
    }

    if (p_prev != 0)
    {
        p_prev->next = 0;
        delete p_prev; // recursive
    }
}

bool BTCACHE::add(BT::BTIDX idx, void* node, size_t size, bool modified)
{
    if (find(idx) != 0)
        return false;
    BTCACHE* p = new BTCACHE(idx, node, size, modified);

    // add to end because quick
    BTCACHE* last = end();
    if (last != 0)
        last->next = p;
    p->prev = last;

    // move to beginning as just recently used
    reorder(p);

    return true;
}

bool BTCACHE::update(BT::BTIDX idx, void* node, size_t size)
{
    BTCACHE* p = find(idx);
    if (p == 0)
        return false;

    reorder(p); // just recently used

    if (memcmp(p->node, node, size) != 0)
    {
        memcpy(p->node, node, size);
        modified = true;
    }
    return true;
}

BTCACHE* BTCACHE::find(BT::BTIDX idx)
{
    // find
    BTCACHE* p = begin();
    while (p != 0)
    {
        if (p->idx == idx)
            break;
        p = p->next;
    }

    // put node at beginning of list, because recently used
    if (p != 0)
        reorder(p);

    return p;
}

void* BTCACHE::get_node()
{
    return node;
}

BT::BTIDX BTCACHE::get_index()
{
    return idx;
}

BTCACHE* BTCACHE::begin()
{
    BTCACHE* p = this;
    while (p->prev != 0)
        p = p->prev;
    return p;
}

BTCACHE* BTCACHE::end()
{
    BTCACHE* p = this;
    while (p->next != 0)
        p = p->next;
    return p;
}

void BTCACHE::advance(BTCACHE*& p)
{
    p = p->next;
}

void BTCACHE::rewind(BTCACHE*& p)
{
    p = p->prev;
}

void BTCACHE::del(BTCACHE*& p)
{
    BTCACHE* next = p->next;
    if (p->prev != 0)
        p->prev->next = next;
    if (next != 0)
        next->prev = p->prev;
    delete [] p->node;
    p->node = 0;
    p->prev = 0;
    p->next = 0;
}

int BTCACHE::count()
{
    int i = 0;
    BTCACHE* p = begin();
    while (p != 0)
    {
        ++i;
        p = p->next;
    }
    return i;
}

void BTCACHE::reorder(BTCACHE* p)
{
    // make entry just recently used

    // nothing to do if is only entry in list
    if (p->prev == 0 && p->next == 0)
        return;

    BTCACHE* first = begin();
    if (p != first) // only if not already at beginning of list
    {
        // remove from current location
        if (p->prev != 0)
            p->prev->next = p->next;
        if (p->next != 0)
            p->next->prev = p->prev;
        p->next = 0;
        p->prev = 0;

        // just recently used, move to beginning of list
        if (first != 0 && first->idx == 0)
        {
            // header found, so make second entry in list, just recently used
            p->next = first->next;
            p->prev = first;
            first->next = p;
            if (p->next != 0)
                p->next->prev = p;
        }
        else
        {
            // new beginning of list, just recently used
            p->next = first;
            p->prev = 0;
            if (first != 0)
                first->prev = p;
        }
    }
}
