///////////////////////////////////////////////////////////////////////////////
// btbulk.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

// BTBULK /////////////////////////////////////////////////////////////////////

class BTBULK
{
private:
    BTBULK(int order, BT::INTRECORDS count);
    ~BTBULK();

    BT::INTRECORDS count;
    int levels;
    BT::INTRECORDS* level_count;
    BT::BTIDX* level_idx;

    friend class BT;

    // disable copy constructor and assignment operator
    BTBULK(const BTBULK&); // not implemented
    BTBULK& operator =(const BTBULK&); // not implemented
};
