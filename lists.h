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

#ifndef LISTS_H
#define LISTS_H 1

#ifndef TYPES_H
#include "types.h"
#endif

typedef struct _node_t {
    struct _node_t* next;
    struct _node_t* prev;
} node_t;

typedef struct _list_t {
    node_t  head;
    node_t  tail;
} list_t;

void initList( list_t* list );
void initNode( node_t* node );

void enterNode( node_t* prev, node_t* curr, node_t* next );
void removeNode( node_t* node );

void addNodeAtHead( list_t* list, node_t* node );
void addNodeAtTail( list_t* list, node_t* node );

bool listEmpty( list_t* list ); 

node_t* firstNode( list_t* list );
node_t* lastNode( list_t* list );

node_t* removeNodeAtHead( list_t* list );
node_t* removeNodeAtTail( list_t* list );

node_t* nextNode( node_t* node );
node_t* prevNode( node_t* node );

#endif
