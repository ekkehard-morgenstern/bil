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

#include "basedlists.h"

void initBasedList( char* base, basedlist_t* list ) {
    list->head.next = (char*)(&list->tail) - base;
    list->head.prev = -1;
    list->tail.next = -1;
    list->tail.prev = (char*)(&list->head) - base;
}

void initBasedNode( char* base, basednode_t* node ) {
    node->prev = -1;
    node->next = -1;
}

void enterBasedNode( char* base, basednode_t* prev, basednode_t* curr, basednode_t* next ) {
    if ( prev == 0 || curr == 0 || next == 0 || prev == curr || curr == next || prev == next ) {
        fprintf( stderr, "? enterBasedNode(): invalid nodes specified: prev=%p, curr=%p, next=%p\n", prev, curr, next );
        exit( EXIT_FAILURE );
    }
    prev->next = ((char*)curr) - base;
    curr->prev = ((char*)prev) - base;
    curr->next = ((char*)next) - base;
    next->prev = ((char*)curr) - base;
}

void removeBasedNode( char* base, basednode_t* node ) {
    if ( node == 0 || node->next == -1 || node->prev == -1 ) {
        fprintf( stderr, "? removeBasedNode(): invalid node specified: %p (null or not in list)\n", node );
        exit( EXIT_FAILURE );
    }
    ((basednode_t*)(base + node->prev))->next = node->next;
    ((basednode_t*)(base + node->next))->prev = node->prev;
    node->prev = -1;
    node->next = -1;
}

void addBasedNodeAtHead( char* base, basedlist_t* list, basednode_t* node ) {
    if ( list == 0 || node == 0 || node->next != -1 || node->prev != -1 ) {
        fprintf( stderr, "? enterBasedNode(): invalid parameters: list=%p, node=%p (if non-null, node may be in some list)\n", list, node );
        exit( EXIT_FAILURE );
    }
    enterBasedNode( base, &list->head, node, list->head.next );
}

void addBasedNodeAtTail( char* base, basedlist_t* list, basednode_t* node ) {
    if ( list == 0 || node == 0 || node->next != -1 || node->prev != -1 ) {
        fprintf( stderr, "? enterBasedNode(): invalid parameters: list=%p, node=%p (if non-null, node may be in some list)\n", list, node );
        exit( EXIT_FAILURE );
    }
    enterBasedNode( base, list->tail.prev, node, &list->tail );
}

bool listEmpty( char* base, basedlist_t* list ) {
    if ( list == 0 ) {
        fprintf( stderr, "? listEmpty(): invalid list specified: %p (null)\n", list );
        exit( EXIT_FAILURE );
    }
    if ( ((basednode_t*)(base + list->head.next)) == &list->tail ) return true;
    return false;
}

basednode_t* firstBasedNode( char* base, basedlist_t* list ) {
    if ( listEmpty( base, list ) ) return 0;
    return (basednode_t*)( base + list->head.next );
}

basednode_t* lastBasedNode( char* base, basedlist_t* list ) {
    if ( listEmpty( base, list ) ) return 0;
    return (basednode_t*)( base + list->tail.prev );
}

basednode_t* removeBasedNodeAtHead( char* base, basedlist_t* list ) {
    basednode_t* node = firstBasedNode( base, list );
    if ( node ) removeBasedNode( base, node );
    return node;
}

basednode_t* removeBasedNodeAtTail( char* base, basedlist_t* list ) {
    basednode_t* node = lastBasedNode( base, list );
    if ( node ) removeBasedNode( base, node );
    return node;
}

basednode_t* nextBasedNode( char* base, basednode_t* node ) {
    if ( node == 0 || node->next == -1 ) {
        fprintf( stderr, "? nextBasedNode(): invalid node specified: %p (null or tail)\n", node );
        exit( EXIT_FAILURE );
    }
    if ( ((basednode_t*)(base + node->next))->next != -1 ) return (basednode_t*)(base + node->next);  // next node not tail node
    return 0; // next node is tail node
}

basednode_t* prevBasedNode( char* base, basednode_t* node ) {
    if ( node == 0 || node->prev == -1 ) {
        fprintf( stderr, "? nextBasedNode(): invalid node specified: %p (null or head)\n", node );
        exit( EXIT_FAILURE );
    }
    if ( ((basednode_t*)(base + node->prev))->prev != -1 ) return (basednode_t*)(base + node->prev);  // prev node not head node
    return 0; // prev node is head node
}

