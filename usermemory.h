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

#ifndef USERMEMORY_H
#define USERMEMORY_H    1

#include <stddef.h>

#define CHUNKMAX    (SIZE_MAX>>2U)
#define FREEBIT     (CHUNKMAX+1U)
#define LOCKBIT     (FREEBIT<<1U)

#define CHUNKALIGN  8U      // align chunks to 8-byte-boundaries

typedef struct _usermemory_t {
    char*       memory;
    size_t      memSize;
    size_t      memUsed;
} usermemory_t;

extern usermemory_t theUserMemory;

/*
    User memory is the main memory for the application. It is simply a region of bytes,
    which can be resized as necessary. The handle space makes sure that memory handles 
    are isolated from actual object references to memory. Thus, the application should
    never allocate user memory directly and only go through the handle space system.

    A chunk of user memory is preceded by a size field, which also contains bits for
    indicating if the memory has been freed or locked.
*/


void initUserMemory( size_t initialSize );
    /*
        This function initializes user memory to some initial size. This size
        cannot be smaller than a preset number of bytes, nor can it be larger than
        some preset maximum size (CHUNKMAX). The program will fail if the
        size specified is incorrect.
    */

void* allocUserMemory( size_t requestSize );
    /*
        This function allocates the memory for allocHandle(). The pointer returned
        will point to the first usable byte of the memory block, and the memory block
        is guaranteed to be at least requestSize bytes wide.

        When user memory is exhausted, a garbage collection is performed first, before
        an attempt is made to resize user memory. 

        Do not call this function directly, since it could inhibit garbage collection
        if handled the wrong way.

        If there is one lock on the memory, user memory cannot be resized. Thus, locks
        should be held only briefly to interact with a block of memory and never held
        for a continued period of time.

        If memory cannot be allocated for some reason, the program will terminate.
    */

void freeUserMemory( void* block );
    /*
        This function marks memory allocated by allocUserMemory() as free. The memory
        is not collected immediately, nor is it collected when it is locked.

        Calling this function on a locked memory block leads to program termination.

        If the block address specified is incorrect, the behavior may be undefined.
    */

void* lockUserMemory( void* block );
    /*
        This function marks a memory block as locked for reading/editing, and inhibits
        collection or moving of the block address for that time period.

        If the block is already locked, the program will terminate with an error message.

        If the block address specified is incorrect, the behavior may be undefined.
    */

void unlockUserMemory( void* block );
    /*
        This function clears the lock mark on a memory block.

        The program will terminate if the memory block wasn't locked.

        If the block address specified is incorrect, the behavior may be undefined.
    */

size_t sizeofUserMemory( void* block );
    /*
        Returns the actual size of the specified memory block, in bytes.

        If the block address specified is incorrect, the behavior may be undefined.
    */

#endif
