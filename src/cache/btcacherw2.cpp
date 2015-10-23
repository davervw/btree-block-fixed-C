///////////////////////////////////////////////////////////////////////////////
// btcacherw2.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"

// BTCACHERW2 /////////////////////////////////////////////////////////////////

BTCACHERW2::BTCACHERW2(
        int cache_size,
        void* context,
        size_t node_size,
        size_t header_size,
        bool (*read)(void* context, BT::BTIDX index, void* node, size_t size),
        bool (*write)(void* context, BT::BTIDX index, void* node, size_t size)
    )
{
    nodes = new char[node_size*cache_size];
    memset(nodes, 0, node_size*cache_size);
    header = 0;
    header_modified = false;
    idx = new BT::BTIDX[cache_size];
    modified = new bool[cache_size];
    max = cache_size;
    for (int i=0; i<max; ++i)
    {
        idx[i] = 0;
        modified[i] = false;
    }
    this->context = context;
    this->node_size = node_size;
    this->header_size = header_size;
    this->read = read;
    this->write = write;
}

BTCACHERW2::~BTCACHERW2()
{
    commit();
    toss();

    delete [] nodes;
    nodes = 0;
    delete [] header;
    header = 0;
    header_modified = false;
    delete [] idx;
    idx = 0;
    delete [] modified;
    modified = 0;
    max = 0;
    context = 0;
    node_size = 0;
    header_size = 0;
    read = 0;
    write = 0;
}

int BTCACHERW2::find(BT::BTIDX index)
{
    if (index == 0)
        return -1; // invalid
    for (int i=0; i<max; ++i)
        if (idx[i] == index)
            return i; // found
    return -1; // not found
}

void BTCACHERW2::reorder(int i)
{
    if (i == 0)
        return; // nothing more to do

    // make copy of most recently used node info
    char* node = new char[node_size];
    memcpy(node, &nodes[i*node_size], node_size);
    BT::BTIDX node_idx = idx[i];
    bool node_modified = modified[i];

    // shift up one to fill in gap
    for (int j=i-1; j>=0; --j)
    {
        memcpy(&nodes[(j+1)*node_size], &nodes[j*node_size], node_size);
        idx[j+1] = idx[j];
        modified[j+1] = modified[j];
    }

    // store most recently used at index 0
    memcpy(nodes, node, node_size);
    idx[0] = node_idx;
    modified[0] = node_modified;

    // clean up storage
    delete [] node;
    node = 0;
}

bool BTCACHERW2::cache_read(void* context, BT::BTIDX index, void* node, size_t size)
{
    BTCACHERW2* that = (BTCACHERW2*)context;

    if (index == 0)
    {
        if (size != that->header_size)
            return false;
        if (that->header == 0)
        {
            that->header = new char[that->header_size];
            if (!that->read(that->context, 0, that->header, that->header_size))
                return false;
        }
        memcpy(node, that->header, that->header_size);
        return true;
    }
    else
    {
        if (size != that->node_size)
            return false;
        int i = that->find(index);
        if (i < 0)
        {
            // swap out last recently used
            i = that->max-1; 
            if (that->idx[i] != 0 && that->modified[i])
            {
                if (!that->write(that->context, that->idx[i], &that->nodes[i*that->node_size], that->node_size))
                    return false;
                that->idx[i] = 0;
                that->modified[i] = false;
            }
            if (!that->read(that->context, index, &that->nodes[i*that->node_size], that->node_size))
                return false;
            that->idx[i] = index;
            that->modified[i] = false;
        }
        memcpy(node, &that->nodes[i*that->node_size], that->node_size);
        that->reorder(i);
        return true;
    }
}

bool BTCACHERW2::cache_write(void* context, BT::BTIDX index, void* node, size_t size)
{
    BTCACHERW2* that = (BTCACHERW2*)context;

    if (index == 0)
    {
        if (size != that->header_size)
            return false;
        if (that->header == 0)
        {
            that->header = new char[that->header_size];
            memcpy(that->header, node, that->header_size);
            that->header_modified = true;
        }
        else
        {
            if (memcmp(that->header, node, that->header_size) != 0)
            {
                memcpy(that->header, node, that->header_size);
                that->header_modified = true;
            }
        }
        return true;
    }
    else
    {
        if (size != that->node_size)
            return false;
        int i = that->find(index);
        if (i < 0)
        {
            // swap out last recently used
            i = that->max-1; 
            if (that->idx[i] > 0 && that->modified[i])
            {
                if (!that->write(that->context, that->idx[i], &that->nodes[i*that->node_size], that->node_size))
                    return false;
                that->idx[i] = 0;
                that->modified[i] = false;
            }
            memcpy(&that->nodes[i*that->node_size], node, that->node_size);
            that->idx[i] = index;
            that->modified[i] = true;
        }
        else
        {
            if (memcmp(&that->nodes[i*that->node_size], node, that->node_size) != 0)
            {
                memcpy(&that->nodes[i*that->node_size], node, that->node_size);
                that->modified[i] = true;
            }
        }
        that->reorder(i);
        return true;
    }
}

bool BTCACHERW2::commit()
{
    bool success = true;
    if (header != 0 && header_modified)
    {
        if (write(context, 0, header, header_size))
            header_modified = false;
        else
            success = false;
    }
    for (int i=0; i<max; ++i)
    {
        if (idx[i] > 0 && modified[i])
        {
            if (write(context, idx[i], &nodes[i*node_size], node_size))
                modified[i] = false;
            else
                success = false;
        }
    }
    return success;
}

void BTCACHERW2::toss()
{
    delete [] header;
    header = 0;
    header_modified = false;
    for (int i=0; i<max; ++i)
    {
        idx[i] = 0;
        modified[i] = false;
    }
    memset(nodes, 0, max*node_size);
}

