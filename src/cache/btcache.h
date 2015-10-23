///////////////////////////////////////////////////////////////////////////////
// btcache.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

class BTCACHE
{
public:
    BTCACHE(BT::BTIDX index, void* node, size_t size, bool modified);
    ~BTCACHE();
    bool add(BT::BTIDX index, void* node, size_t size, bool modified);
    bool update(BT::BTIDX index, void* node, size_t size);
    BTCACHE* find(BT::BTIDX index);
    void* get_node();
    BT::BTIDX get_index();
    BTCACHE* begin();
    BTCACHE* end();
    static void advance(BTCACHE*& p);
    static void rewind(BTCACHE*& p);
    static void del(BTCACHE*& p);
    void reorder(BTCACHE* p);
    int count();

    bool modified;

private:
    BTCACHE* prev;
    BTCACHE* next;
    BT::BTIDX idx;
    void* node;

    // disable copy constructor and assignment operator
    BTCACHE(const BTCACHE&); // not implemented
    BTCACHE& operator =(const BTCACHE&); // not implemented
};
