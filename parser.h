/*  Basic Implementation Language (BIL)
    Copyright (C) 2019  Ekkehard Morgenstern

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    NOTE: Programs created with BIL do not fall under this license.

    CONTACT INFO:
        E-Mail: ekkehard@ekkehardmorgenstern.de
        Mail: Ekkehard Morgenstern, Mozartstr. 1, D-76744 Woerth am Rhein, Germany, Europe */

#ifndef PARSER_H
#define PARSER_H    1

#include <stddef.h>

#ifndef PARSINGTABLE_H
#include "parsingtable.h"
#endif

// node type aliases
#define NT_IDENTIFIER   NT_TERMINAL_19




typedef struct _treenode_t {
    int                     nodetype;
    char*                   text;
    struct _treenode_t**    branches;
    size_t                  branchAlloc;
    size_t                  numBranches;
} treenode_t;

void* xmalloc( size_t size );
void xrealloc( void** pBlk, size_t newSize );
char* xstrdup( const char* text );

void dump_tree_node( treenode_t* node, int indent );
void delete_node( treenode_t* node );
void fatal( const char* pos, const char* fmt, ... );
treenode_t* parse( const char* text );



#endif
