///////////////////////////////////////////////////////////////////////////////
// btcacherw.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

class BTCACHERW
{
public:
    BTCACHERW(
        int cache_size,
        void* context,
        size_t node_size,
        size_t header_size,
        bool (*read)(void* context, BT::BTIDX index, void* node, size_t size),
        bool (*write)(void* context, BT::BTIDX index, void* node, size_t size)
    );
    ~BTCACHERW();

    static bool cache_read(void* context, BT::BTIDX index, void* node, size_t size);
    static bool cache_write(void* context, BT::BTIDX index, void* node, size_t size);
    bool commit();
    void toss();

private:
    BTCACHE* cache;
    int alloc;
    int max;
    void* context;
    size_t node_size;
    size_t header_size;
    bool (*read)(void* context, BT::BTIDX index, void* node, size_t size);
    bool (*write)(void* context, BT::BTIDX index, void* node, size_t size);

    // disable copy constructor and assignment operator
    BTCACHERW(const BTCACHERW&); // not implemented
    BTCACHERW& operator =(const BTCACHERW&); // not implemented
};
