///////////////////////////////////////////////////////////////////////////////
// btcacherw.cpp 
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"

// BTCACHERW //////////////////////////////////////////////////////////////////

BTCACHERW::BTCACHERW(
    int cache_size,
    void* context,
    size_t node_size,
    size_t header_size,
    bool (*read)(void* context, BT::BTIDX index, void* node, size_t size),
    bool (*write)(void* context, BT::BTIDX index, void* node, size_t size)
)
{
    max = cache_size;
    alloc = 0;
    cache = 0;
    this->context = context;
    this->node_size = node_size;
    this->header_size = header_size;
    this->read = read;
    this->write = write;
}

BTCACHERW::~BTCACHERW()
{
    commit();
    toss();
}

bool BTCACHERW::cache_read(void* context, BT::BTIDX index, void* node, size_t size)
{
    BTCACHERW* that = (BTCACHERW*)context;
    BTCACHE* p = 0;
    if (that->cache != 0)
        p = that->cache->find(index);
    if (p != 0)
    {
        memcpy(node, p->get_node(), size);
        return true;
    }
    else
    {
        bool success = that->read(that->context, index, node, size);
        if (success)
        {
            if (that->cache == 0)
            {
                that->cache = new BTCACHE(index, node, size, false);
                that->alloc = 1;
            }
            else
            {
                that->cache->add(index, node, size, false);
                if (++that->alloc > that->max)
                {
                    // remove least recently used
                    p = that->cache->end();
                    if (p->modified)
                    {
                        // flush modifications to storage first
                        BT::BTIDX idx = p->get_index();
                        if (!that->write(that->context, idx, p->get_node(), idx==0?that->header_size:that->node_size))
                            success = false;
                    }

                    // fixup if deleting primary entry
                    if (p == that->cache)
                        BTCACHE::rewind(that->cache);

                    BTCACHE::del(p);
                    delete p;
                    p = 0;
                    --that->alloc;
                }
            }
        }
        return success;
    }
}

bool BTCACHERW::cache_write(void* context, BT::BTIDX index, void* node, size_t size)
{
    bool success=true;
    BTCACHERW* that = (BTCACHERW*)context;
    BTCACHE* p = 0;
    if (that->cache != 0)
        p = that->cache->find(index);
    if (p != 0)
    {
        void* dest_node = p->get_node();
        if (memcmp(dest_node, node, size) != 0)
        {
            memcpy(dest_node, node, size);
            p->modified = true;
        }
    }
    else
    {
        if (that->cache == 0)
        {
            that->cache = new BTCACHE(index, node, size, true);
            that->alloc = 1;
        }
        else
        {
            that->cache->add(index, node, size, true);
            if (++that->alloc > that->max)
            {
                // remove last recently used
                p = that->cache->end();
                if (p->modified)
                {
                    // flush modifications to storage first
                    BT::BTIDX idx = p->get_index();
                    if (!that->write(that->context, idx, p->get_node(), idx==0?that->header_size:that->node_size))
                        success = false;
                }

                // fixup if deleting primary entry
                if (p == that->cache)
                    BTCACHE::rewind(that->cache);

                BTCACHE::del(p);
                delete p;
                p = 0;
                --that->alloc;
            }
        }
    }
    return success;
}

bool BTCACHERW::commit()
{
    bool success = true;
    if (cache != 0)
    {
        BTCACHE* p = cache->begin();
        while (p != 0)
        {
            if (p->modified)
            {
                BT::BTIDX idx = p->get_index();
                void* node = p->get_node();
                if (!write(context, idx, node, idx==0?header_size:node_size))
                    success = false;
                p->modified = false;
            }
            BTCACHE::advance(p);
        }
    }
    return success;
}

void BTCACHERW::toss()
{
    if (cache != 0)
    {
        delete cache;
        cache = 0;
    }
    alloc = 0;
}

