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

#include "usermemory.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

usermemory_t theUserMemory;

void initUserMemory( size_t initialSize ) {

}

static void collectUserMemory( void ) {

}

static void resizeUserMemory( void ) {

}

objref_t allocUserMemory( size_t requestSize ) {
    // make sure size field does not interfere with memory alignment of user area
    size_t sizeField   = sizeof(size_t) < CHUNKALIGN ? CHUNKALIGN : sizeof(size_t);
    // align memory block size
    size_t alignedSize = ( sizeField + requestSize + (CHUNKALIGN-1U) ) & ~((size_t)(CHUNKALIGN-1U));
    // begin
    if ( alignedSize > CHUNKMAX ) {
        fprintf( stderr, "? requested block size too large: %zu (from: %zu) > %zu\n", alignedSize, requestSize, (size_t) CHUNKMAX );
        exit( EXIT_FAILURE );
    }
    if ( alignedSize > theUserMemory.memSize - theUserMemory.memUsed ) {
        // request does not meet remaining space: attempt to garbage collect first
        collectUserMemory();        
        if ( alignedSize > theUserMemory.memSize - theUserMemory.memUsed ) {
            // didn't work: resize it
            resizeUserMemory();
            if ( alignedSize > theUserMemory.memSize - theUserMemory.memUsed ) {
                fprintf( stderr, "? internal error: not enough space for allocation of %zu bytes.\n", alignedSize );
                exit( EXIT_FAILURE );
            }
        }
    }
    char* blk = (char*) theUserMemory.memory + theUserMemory.memUsed;
    theUserMemory.memUsed += alignedSize;
    blk += sizeField;
    *( (size_t*)( blk - sizeof(size_t) ) ) = alignedSize;
    return (objref_t)( blk - theUserMemory.memory );
}

static void badBlockOffset( objref_t block ) {
    fprintf( stderr, "? illegal user memory block: %#zx\n", (objref_t) block );
    exit( EXIT_FAILURE );
}

static inline char* validateUserMemory( objref_t block ) {
    if ( block >= theUserMemory.memUsed ) badBlockOffset( block );
    return theUserMemory.memory + block;
}

static inline size_t getBlockSize( char* blk ) {
    return *( (size_t*)( blk - sizeof(size_t) ) );
}

static inline void setBlockSize( char* blk, size_t size ) {
    *( (size_t*)( blk - sizeof(size_t) ) ) = size;
}

void freeUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx already freed\n", block );
        exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %#zx still locked\n", block );
        exit( EXIT_FAILURE );
    }
    size |= FREEBIT;
    setBlockSize( blk, size );
}

void* lockUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %#zx already locked\n", block );
        exit( EXIT_FAILURE );
    }
    size |= LOCKBIT;
    setBlockSize( blk, size );
    return block;
}

void unlockUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    if ( ( size & LOCKBIT ) == 0 ) {
        fprintf( stderr, "? block %#zx not locked\n", block );
        exit( EXIT_FAILURE );
    }
    size &= ~LOCKBIT;
    setBlockSize( blk, size );
}

size_t sizeofUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    return size & CHUNKMAX;
}
