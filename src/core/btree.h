///////////////////////////////////////////////////////////////////////////////
// btree.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ostream>

class BTITER;
class BTBULK;

class BT
{
public:
    //typedef unsigned short BTIDX; // nodes in tree
    //typedef unsigned short INTRECORDS; // records in tree
    //typedef signed char INTENTRIES; // entries in a node
    typedef unsigned long BTIDX; // nodes in tree
    typedef unsigned long INTRECORDS; // records in tree
    typedef signed char INTENTRIES; // entries in a node

    struct HEADER
    {
        int intsize;
        int countsize;
        int idxsize;
        int ordersize;
        int order;
        int recordsize;
        int nodesize;
        INTRECORDS records;
        BTIDX root;
        BTIDX nodes;
        BTIDX free;
    };

    //struct NODE
    //{
    //     BTIDX parent;
    //     BTIDX children[order];
    //     unsigned char data[recordsize*(order-1)];
    //     INTENTRIES entries;
    //};

    BT(void* context,
        void (*getkey)(void* key, void* record),
        int (*comparekey)(void* key1, void* key2),
        bool (*read)(void* context, BTIDX index, void* node, size_t size),
        bool (*write)(void* context, BTIDX index, void* node, size_t size),
        bool active_sync = true);
    static void init(HEADER& header, int order, int recordsize, int maxnodes);
    void clear();
    BTITER add(void* record);
    bool del(void* key);
    bool del(const BTITER& iter);
    bool check(void (*progress)(int percent)) const;
    BTITER find(void* key, void* record) const;
    bool replace(void* key, void* record);
    INTRECORDS count() const;
    int sizeofnode() const;
    BTIDX numnodes() const;
    BTIDX freenodes() const;
    BTIDX offset() const;
    int levels() const;
    BTITER begin() const;
    BTITER end() const;
    void traverse(void (*op)(void* context, void* key, void* record)) const;
    void display(std::ostream& (*dispkey)(std::ostream& str, void* key, void* record));
    static void dump_node(std::ostream& str, const HEADER* header, BTIDX idx, char* node);
    static void dump_header(std::ostream& str, const HEADER* header);
    void dump(std::ostream& str);
    void sync();

    // bulk load does an optimized initialization of the tree
    // caller must call load_begin once, load_record() for each record in key order, followed by load_end to finish
    BTBULK* load_begin(BT::INTRECORDS count);
    void load_record(BTBULK* bulk, void* record);
    void load_end(BTBULK*& bulk);

private:
    void* context;
    int order;
    int recordsize;
    int nodesize;
    int records;
    BTIDX root;
    BTIDX nodes;
    BTIDX free;
    BTIDX freelist;

    bool active_sync;
    bool modified;
    int iter_version;
    BTIDX reserved_idx;

    void (*getkey)(void* key, void* record);
    int (*comparekey)(void* key1, void* key2);
    bool (*read)(void* context, BTIDX index, void* node, size_t size);
    bool (*write)(void* context, BTIDX index, void* node, size_t size);

    void sync_in();
    void sync_out(bool forced=false);
    BTITER add_internal(BTIDX idx, BTIDX left, BTIDX right, void* key, void* record, INTENTRIES split_pos);
    BTITER find_insert_position(BTIDX node, void* key);
    void display(int level, BTIDX index, std::ostream& (*dispkey)(std::ostream& str, void* key, void* record));
    void underflow(BTIDX idx, char* node);
    void remove_node(BTIDX idx);
    void remove_internal(const BTITER& iter);
    bool del_internal(const BTITER& iter);
    void purge_freelist();

    // disable copy constructor and assignment operator
    BT(const BT&); // not implemented
    BT& operator =(const BT&); // not implemented

    friend class BTITER;
};

// required
#include "btiter.h"
#include "btbulk.h"
#include "throwexception.h"

// optional
#include "btcache.h"
#include "btcacherw.h"
#include "btcacherw2.h"
