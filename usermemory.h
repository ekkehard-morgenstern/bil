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

void* allocUserMemory( size_t requestSize );


#endif
