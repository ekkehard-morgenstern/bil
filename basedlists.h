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

#ifndef BASEDLISTS_H
#define BASEDLISTS_H 1

#ifndef TYPES_H
#include "types.h"
#endif

typedef struct _basednode_t {
    ptrdiff_t next;
    ptrdiff_t prev;
} basednode_t;

typedef struct _list_t {
    basednode_t  head;
    basednode_t  tail;
} basedlist_t;

void initBasedList( char* base, basedlist_t* list );
void initBasedNode( char* base, basednode_t* node );

void enterBasedNode( char* base, basednode_t* prev, basednode_t* curr, basednode_t* next );
void removeBasedNode( char* base, basednode_t* node );

void addBasedNodeAtHead( char* base, basedlist_t* list, basednode_t* node );
void addBasedNodeAtTail( char* base, basedlist_t* list, basednode_t* node );

bool basedListEmpty( char* base, basedlist_t* list ); 

basednode_t* firstBasedNode( char* base, basedlist_t* list );
basednode_t* lastBasedNode( char* base, basedlist_t* list );

basednode_t* removeBasedNodeAtHead( char* base, basedlist_t* list );
basednode_t* removeBasedNodeAtTail( char* base, basedlist_t* list );

basednode_t* nextBasedNode( char* base, basednode_t* node );
basednode_t* prevBasedNode( char* base, basednode_t* node );

#endif
