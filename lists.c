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

#include "lists.h"

void initList( list_t* list ) {
    list->head.next = &list->tail;
    list->head.prev = 0;
    list->tail.next = 0;
    list->tail.prev = &list->head;
}

void initNode( node_t* node ) {
    node->prev = 0;
    node->next = 0;
}

void enterNode( node_t* prev, node_t* curr, node_t* next ) {
    if ( prev == 0 || curr == 0 || next == 0 || prev == curr || curr == next || prev == next ) {
        fprintf( stderr, "? enterNode(): invalid nodes specified: prev=%p, curr=%p, next=%p\n", prev, curr, next );
        exit( EXIT_FAILURE );
    }
    prev->next = curr;
    curr->prev = prev;
    curr->next = next;
    next->prev = curr;
}

void removeNode( node_t* node ) {
    if ( node == 0 || node->next == 0 || node->prev == 0 ) {
        fprintf( stderr, "? removeNode(): invalid node specified: %p (null or not in list)\n", node );
        exit( EXIT_FAILURE );
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev       = 0;
    node->next       = 0;
}

void addNodeAtHead( list_t* list, node_t* node ) {
    if ( list == 0 || node == 0 || node->next || node->prev ) {
        fprintf( stderr, "? enterNode(): invalid parameters: list=%p, node=%p (if non-null, node may be in some list)\n", list, node );
        exit( EXIT_FAILURE );
    }
    enterNode( &list->head, node, list->head.next );
}

void addNodeAtTail( list_t* list, node_t* node ) {
    if ( list == 0 || node == 0 || node->next || node->prev ) {
        fprintf( stderr, "? enterNode(): invalid parameters: list=%p, node=%p (if non-null, node may be in some list)\n", list, node );
        exit( EXIT_FAILURE );
    }
    enterNode( list->tail.prev, node, &list->tail );
}

bool listEmpty( list_t* list ) {
    if ( list == 0 ) {
        fprintf( stderr, "? listEmpty(): invalid list specified: %p (null)\n", list );
        exit( EXIT_FAILURE );
    }
    if ( list->head.next == &list->tail ) return true;
    return false;
}

node_t* firstNode( list_t* list ) {
    if ( listEmpty( list ) ) return 0;
    return list->head.next;
}

node_t* lastNode( list_t* list ) {
    if ( listEmpty( list ) ) return 0;
    return list->tail.prev;
}

node_t* removeNodeAtHead( list_t* list ) {
    node_t* node = firstNode( list );
    if ( node ) removeNode( node );
    return node;
}

node_t* removeNodeAtTail( list_t* list ) {
    node_t* node = lastNode( list );
    if ( node ) removeNode( node );
    return node;
}

node_t* nextNode( node_t* node ) {
    if ( node == 0 || node->next == 0 ) {
        fprintf( stderr, "? nextNode(): invalid node specified: %p (null or tail)\n", node );
        exit( EXIT_FAILURE );
    }
    if ( node->next->next ) return node->next;  // next node not tail node
    return 0; // next node is tail node
}

node_t* prevNode( node_t* node ) {
    if ( node == 0 || node->prev == 0 ) {
        fprintf( stderr, "? nextNode(): invalid node specified: %p (null or head)\n", node );
        exit( EXIT_FAILURE );
    }
    if ( node->prev->prev ) return node->prev;  // prev node not head node
    return 0; // prev node is head node
}

