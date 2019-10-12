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

#include "handlespace.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

handlespace_t theHandleSpace;

void initHandleSpace( unsigned initialMaxHandles ) {
    if ( initialMaxHandles < 1024U ) {
        fprintf( stderr, "? bad maximum number of handles: %u\n", initialMaxHandles );
        exit( EXIT_FAILURE );
    }
    theHandleSpace.objrefs = (objref_t*) calloc( initialMaxHandles, sizeof(objref_t*) );
    if ( theHandleSpace.objrefs == 0 ) {
        fprintf( stderr, "? failed to allocate handle space of size %u: %m\n", initialMaxHandles );
        exit( EXIT_FAILURE );
    }
    theHandleSpace.numObjRefs  = initialMaxHandles;
    theHandleSpace.usedObjRefs = 0;
}

handle_t allocHandle( size_t requestSize ) {
    // attempt to allocate handle quickly
    handle_t hnd = UINT_MAX;
    if ( theHandleSpace.usedObjRefs < theHandleSpace.numObjRefs ) {
        hnd = theHandleSpace.usedObjRefs++;
    } else {
        // attempt to find unallocated handle first before resizing memory
        for ( unsigned i=0; i < theHandleSpace.usedObjRefs; ++i ) {
            if ( theHandleSpace.objrefs[i] == 0 ) { hnd = i; break; }
        }
        if ( hnd == UINT_MAX ) {    // no unassigned handles: have to resize
            if ( theHandleSpace.numObjRefs == UINT_MAX ) {  // no way to resize
                fprintf( stderr, "? maximum number of handles exceeded\n" );
                exit( EXIT_FAILURE );
            }
            unsigned newSize;
            if ( theHandleSpace.numObjRefs <= UINT_MAX/2U ) {
                newSize = theHandleSpace.numObjRefs * 2U;
            } else {
                newSize = UINT_MAX;
            }
            objref_t* newChunk = (objref_t*) calloc( newSize, sizeof(objref_t*) );
            if ( newChunk == 0 ) {
                fprintf( stderr, "? failed to allocate handle space of size %u: %m\n", newSize );
                exit( EXIT_FAILURE );
            }
            // copy over object references from old chunk
            memcpy( newChunk, theHandleSpace.objrefs, sizeof(objref_t*) * theHandleSpace.usedObjRefs );
            // free old chunk and assign new chunk
            free( theHandleSpace.objrefs ); 
            theHandleSpace.objrefs    = newChunk;
            theHandleSpace.numObjRefs = newSize;
            // now allocate next handle
            hnd = theHandleSpace.usedObjRefs++;
        }
    }
    // the handle is allocated; now get a chunk of user memory

}

void freeHandle( handle_t handle ) {

}

size_t handleSize( handle_t handle ) {

}

void* lockHandle( handle_t handle ) {

}

void unlockHandle( handle_t handle ) {

}

size_t memoryUsage( void ) {

}
