///////////////////////////////////////////////////////////////////////////////
// btiter.h
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

class BTITER
{
public:
    BTITER(const BT& tree, BT::BTIDX index, BT::INTENTRIES pos);
    BTITER(const BTITER& iter); // copy constructor
    int operator ==(const BTITER& iter);
    int operator !=(const BTITER& iter);
    BTITER& operator ++(); // prefix ++
    BTITER operator ++(int); // postfix ++
    BTITER& operator =(const BTITER& iter); // assignment operator

private:
    const BT* tree;
    BT::BTIDX index;
    BT::INTENTRIES pos;
    int iter_version;

    friend class BT;
};

