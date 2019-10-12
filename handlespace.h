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

#ifndef HANDLESPACE_H
#define HANDLESPACE_H   1

#include <stddef.h>

typedef void* objref_t;

    /*
        An object reference is a pointer to a region of memory in an allocation space.
        The memory block being pointed to is preceded by the length of the block (size_t).
        This length field also contains a bit which indicates whether the block is still in use
        and another bit to indicate whether the block is currently being locked for access.
        The reference itself is set to 0 when it is no longer in use, making it available for reuse.
    */

typedef struct _handlespace_t {
    objref_t*   objrefs;
    unsigned    numObjRefs;
    unsigned    usedObjRefs;
} handlespace_t;

extern handlespace_t theHandleSpace;

typedef unsigned handle_t;

    /* 
        A handle space is a region of object references. The handle itself is an index into
        the object reference table. Thus, the handle space can be resized without existing handles
        to be invalidated. There is only one handle space per process. Changes to the handle
        space must be atomic, in a way that the running application does not notice it.

        For now, memory management is being done in a synchronous manner.
    */

void initHandleSpace( unsigned initialMaxHandles );
    /*
        This function intitializes global handle space to a specified size. This size cannot be
        smaller than a preset minimum and cannot exceed UINT_MAX. 

        The program will terminate if an incorrect size is specified.
    */

handle_t allocHandle( size_t requestSize );
    /*
        This function allocates a memory handle and user memory of the requested size.
        If either operation fails, the program will be terminated.

        For now, the algorithm is such that a free handle slot is sought within the current
        handle space. If none is found, the handle space is resized. If resizing fails, the
        program terminates. After a handle has been allocated, user memory is requested from
        the global user memory pool (using allocUserMemory()).
      
        The memory block allocated will be in allocated, unlocked state.
    */

void freeHandle( handle_t handle );
    /*
        This function releases the handle slot occupied by the specified handle after
        marking the memory associated as free (using freeUserMemory()). If the memory is
        currently locked, the program will terminate with an error message.
    */

size_t handleSize( handle_t handle );
    /*
        This function returns the actual size of the memory block allocated with
        allocHandle(). If the handle is marked as unused, the program will terminate.
    */

void* lockHandle( handle_t handle );
    /*
        This function calls lockUserMemory() on the object reference held by the handle.
        If the handle is marked as unused, or if the memory is already locked, the program
        will terminate.
    */

void unlockHandle( handle_t handle );
    /*
        This function calls unlockUserMemory() on the object reference held by the handle.
        If the handle is marked as unused, or if the memory isn't currently locked, the
        program terminates.
    */

typedef struct _memusage_t {
    size_t totalUserSpace;
    size_t allocatedUserSpace;
    size_t freeUserSpace;
    size_t totalHandleSpace;
    size_t allocatedHandleSpace;
    size_t freeHandleSpace;
    size_t totalMemory;
    size_t allocatedMemory;
    size_t freeMemory;
} memusage_t;

memusage_t memoryUsage( void );
    /*
        This function sums up all memory occupied by the handle space and the user memory
        in bytes.
    */

#endif