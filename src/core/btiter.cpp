///////////////////////////////////////////////////////////////////////////////
// btiter.cpp
//
// B-Tree-block-fixed-C version 1.20 Copyright (c) 1995-2015 David R. Van Wagner
// See LICENSE.txt for licensing details
//
// btree@davevw.com
// http://www.davevw.com
///////////////////////////////////////////////////////////////////////////////

#include "btree.h"

// BTITER /////////////////////////////////////////////////////////////////////

BTITER::BTITER(const BT& tree, BT::BTIDX index, BT::INTENTRIES pos)
{
    this->tree = &tree;
    this->index = index;
    this->pos = pos;
    this->iter_version = tree.iter_version;
}

BTITER::BTITER(const BTITER& iter)
{
    tree = iter.tree;
    index = iter.index;
    pos = iter.pos;
    iter_version = iter.iter_version;
}

BTITER& BTITER::operator=(const BTITER& iter)
{
    tree = iter.tree;
    index = iter.index;
    pos = iter.pos;
    iter_version = iter.iter_version;
    return *this;
}

int BTITER::operator ==(const BTITER& iter)
{
    return tree == iter.tree
        && index == iter.index
        && pos == iter.pos;
}

int BTITER::operator !=(const BTITER& iter)
{
    return !(*this == iter);
}

BTITER& BTITER::operator ++()
{
    if (iter_version != tree->iter_version)
        *this = tree->end();
    else
    {
        BT::BTIDX idx = index;
        BT::INTENTRIES next_pos = pos;

        char* node = new char[tree->nodesize];
        if (!tree->read(tree->context, idx, node, tree->nodesize))
        {
            delete [] node;
            throw_exception("Failed to read from B-Tree");
        }
        BT::BTIDX right = *(BT::BTIDX*)(node + (next_pos + 2)*sizeof(BT::BTIDX));
        if (right == 0)
        {
            // THIS IS A LEAF, OR AT LEAST CAN'T DESCEND INTO RIGHT CHILD
            BT::INTENTRIES& entries = *(BT::INTENTRIES*)(node+tree->nodesize-sizeof(BT::INTENTRIES));
            if (next_pos < entries-1)
            {
                // NEXT IS NEXT ELEMENT IN LEAF
                ++next_pos;
            }
            else
            {
                BT::BTIDX parent = *(BT::BTIDX*)node;
                if (parent == 0)
                {
                    index = 0;
                    pos = 0;
                    delete [] node;
                    return *this; // FAILED, WAS LAST KEY
                }
                else
                {
                    // NEXT IS PARENT KEY WHOSE LEFT POINTER POINTS TO THIS
                    idx = parent;
                    next_pos = 0;
                    if (!tree->read(tree->context, idx, node, tree->nodesize))
                    {
                        delete [] node;
                        throw_exception("Failed to read from B-Tree");
                    }
                    entries = *(BT::INTENTRIES*)(node+tree->nodesize-sizeof(BT::INTENTRIES));
                    while (next_pos < entries)
                    {
                        BT::BTIDX child = *(BT::BTIDX*)(node + (next_pos+1)*sizeof(BT::BTIDX));
                        if (child == index)
                            break;
                        else
                            ++next_pos;
                    }
                    while (next_pos == entries)
                    {
                        // NO RIGHT POINTER, GO UP UNTIL FIND A RIGHT POINTER
                        parent = *(BT::BTIDX*)node;
                        if (parent == 0)
                        {
                            index = 0;
                            pos = 0;
                            delete [] node;
                            return *this; // FAILED, WAS LAST KEY
                        }
                        next_pos = 0;
                        if (!tree->read(tree->context, parent, node, tree->nodesize))
                        {
                            delete [] node;
                            throw_exception("Failed to read from B-Tree");
                        }
                        entries = *(BT::INTENTRIES*)(node+tree->nodesize-sizeof(BT::INTENTRIES));
                        while (next_pos < entries)
                        {
                            BT::BTIDX child = *(BT::BTIDX*)(node + (next_pos+1)*sizeof(BT::BTIDX));
                            if (child == idx)
                                break;
                            else
                                ++next_pos;
                        }
                        idx = parent;
                        if (!tree->read(tree->context, idx, node, tree->nodesize))
                        {
                            delete [] node;
                            throw_exception("Failed to read from B-Tree");
                        }
                        entries = *(BT::INTENTRIES*)(node+tree->nodesize-sizeof(BT::INTENTRIES));
                    }
                }
            }
            index = idx;
            pos = next_pos;
        }
        else
        {
            // FROM INTERIOR, NEXT IS LEFTMOST GRAND*-CHILD OF RIGHT POINTER
            index = right;
            do
            {
                if (!tree->read(tree->context, index, node, tree->nodesize))
                {
                    delete [] node;
                    throw_exception("Failed to read from B-Tree");
                }
                BT::BTIDX left = *(BT::BTIDX*)(node + sizeof(BT::BTIDX));
                if (left == 0)
                    break;
                else
                    index = left;
            } while (true);
            pos = 0;
        }
        delete [] node;
    }
    return *this;
}

BTITER BTITER::operator ++(int)
{
    if (iter_version == tree->iter_version)
    {
        BTITER iter = *this;
        ++(*this);
        return iter;
    }
    else
    {
        BTITER iter = tree->end();
        *this = iter;
        return iter;
    }
}
