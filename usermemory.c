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

void* allocUserMemory( size_t requestSize ) {

}

static char* validateUserMemory( void* block ) {
    char* blk = (char*) block;
    if ( blk == 0 || blk < ((char*)sizeof(size_t)) || blk - sizeof(size_t) < theUserMemory.memory || 
        blk >= theUserMemory.memory + theUserMemory.memUsed ) {
        fprintf( stderr, "? illegal user memory address: %p\n", block );
        exit( EXIT_FAILURE );
    }
    return blk;
}

void freeUserMemory( void* block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = *( (size_t*)( blk - sizeof(size_t) ) );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %p already freed\n", block );
        exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %p still locked\n", block );
        exit( EXIT_FAILURE );
    }
    size |= FREEBIT;
    *( (size_t*)( blk - sizeof(size_t) ) ) = size;
}

void* lockUserMemory( void* block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = *( (size_t*)( blk - sizeof(size_t) ) );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %p not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %p already locked\n", block );
        exit( EXIT_FAILURE );
    }
    size |= LOCKBIT;
    *( (size_t*)( blk - sizeof(size_t) ) ) = size;
    return block;
}

void unlockUserMemory( void* block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = *( (size_t*)( blk - sizeof(size_t) ) );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %p not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    if ( ( size & LOCKBIT ) == 0 ) {
        fprintf( stderr, "? block %p not locked\n", block );
        exit( EXIT_FAILURE );
    }
    size &= ~LOCKBIT;
    *( (size_t*)( blk - sizeof(size_t) ) ) = size;
}

size_t sizeofUserMemory( void* block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = *( (size_t*)( blk - sizeof(size_t) ) );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %p not allocated\n", block );
        exit( EXIT_FAILURE );
    }
    return size & CHUNKMAX;
}
