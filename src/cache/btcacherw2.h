///////////////////////////////////////////////////////////////////////////////
// btcacherw2.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

class BTCACHERW2
{
public:
    BTCACHERW2(
        int cache_size,
        void* context,
        size_t node_size,
        size_t header_size,
        bool (*read)(void* context, BT::BTIDX index, void* node, size_t size),
        bool (*write)(void* context, BT::BTIDX index, void* node, size_t size)
    );
    ~BTCACHERW2();

    static bool cache_read(void* context, BT::BTIDX index, void* node, size_t size);
    static bool cache_write(void* context, BT::BTIDX index, void* node, size_t size);
    bool commit();
    void toss();

private:
    char* nodes;
    char* header;
    bool header_modified;
    BT::BTIDX* idx;
    bool* modified;
    int max;
    void* context;
    size_t node_size;
    size_t header_size;
    bool (*read)(void* context, BT::BTIDX index, void* node, size_t size);
    bool (*write)(void* context, BT::BTIDX index, void* node, size_t size);

    int find(BT::BTIDX index);
    void reorder(int i);

    // disable copy constructor and assignment operator
    BTCACHERW2(const BTCACHERW2&); // not implemented
    BTCACHERW2& operator =(const BTCACHERW2&); // not implemented
};
