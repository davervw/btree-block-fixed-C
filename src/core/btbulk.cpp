///////////////////////////////////////////////////////////////////////////////
// btbulk.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"

// BTBULK /////////////////////////////////////////////////////////////////////

BTBULK::BTBULK(int order, BT::INTRECORDS count)
{
    // bulkloading algorithm is to fill up nodes on a level with one extra element each except for last node
    // take last element from all but last node and make parent nodes filling up again one extra element except for last node
    // repeat until reach root (exactly one node)

    // actual bulkloading will take this information into account
    // to know when to move up/down tree for right siblings vs. parents
    // bulkloading will be optimimal: nodes will be as full as possible except right side
    // note: bulkloading works best when read/write is cached so only writes full nodes to disk, doesn't reread recent nodes
    // cache must be at least number of nodes equal to the number of levels in tree
    // so can traverse top to bottom without cache misses on nodes already visited while bulkloading
    // when inserting, start with a new node to insert first element
    // next insert into current node if space, otherwise find next node
    // which is parent (create if necessary), if full then try left most descendent in right child until hit leaf node, then find right most ancestor (create if necessary), repeat

    // do once to count
    BT::INTRECORDS total_count = count;
    int levels = 0;
    do
    {
        BT::INTRECORDS level_count = 0;
        if (total_count > 0)
        {
            // construct leaf nodes where all but last has one extra element
            BT::INTRECORDS nodes = total_count / order + 1;

            // move extra elements above
            BT::INTRECORDS above_count = nodes-1; // easier to count elements above
            level_count = total_count - above_count; // remainder is on this level
            total_count -= level_count; // subtract from total before looping
        }
        ++levels;
    }
    while (total_count > 0);

    // allocate array for return
    BT::INTRECORDS* level_array = new BT::INTRECORDS[levels];

    // count again to store in array
    total_count = count;
    levels = 0;
    do
    {
        BT::INTRECORDS level_count = 0;
        if (total_count > 0)
        {
            BT::INTRECORDS nodes = total_count / order + 1;

            // move extra elements above
            BT::INTRECORDS above_count = nodes-1; // easier to count elements above
            level_count = total_count - above_count; // remainder is on this level
            total_count -= level_count; // subtract from total before looping
        }

        // store level count in bottom up order
        level_array[levels] = level_count;
        ++levels;
    }
    while (total_count > 0);

    // save state
    this->count = count;
    this->levels = levels;
    this->level_count = level_array;
    this->level_idx = new BT::BTIDX[levels+1];
    for (int i=0; i<levels; ++i)
        this->level_idx[i] = 0;
}

BTBULK::~BTBULK()
{
    count = -1;
    levels = 0;
    delete [] level_count;
    level_count = 0;
    delete [] level_idx;
    level_idx = 0;
}
