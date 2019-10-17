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

typedef struct _blockinfo_t {
    handle_t handle;
    objref_t block;
} blockinfo_t;

typedef struct _blocklist_t {
    blockinfo_t*    info;
    size_t          count;
} blocklist_t;

typedef struct _memregion_t {
    size_t offsetStart;
    size_t offsetEnd;           // points one byte past the end of the region
    size_t listIndexStart;
    size_t listIndexEnd;
} memregion_t;

typedef struct _memlist_t {
    memregion_t* regions;
    size_t       numRegions;
} memlist_t;

typedef struct _freeinfo_t {
    size_t      offset;
    size_t      size;
} freeinfo_t;

typedef struct _freelist_t {
    freeinfo_t* info;
    size_t      count;
} freelist_t;

usermemory_t theUserMemory;

static void badBlockOffset( objref_t block ) {
    fprintf( stderr, "? illegal user memory block: %#zx\n", (objref_t) block );
    exit( EXIT_FAILURE );
}

static inline char* validateUserMemory( objref_t block ) {
    if ( block < sizeof(memhdr_t) || block >= theUserMemory.memUsed ) badBlockOffset( block );
    return theUserMemory.memory + block;
}

static inline size_t getBlockSize( char* blk ) {
    return ((memhdr_t*)( blk - sizeof(memhdr_t) ))->size;
}

static inline void setBlockSize( char* blk, size_t size ) {
    ((memhdr_t*)( blk - sizeof(memhdr_t) ))->size = size;
}

static inline bool isUserMemoryLocked( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    return size & LOCKBIT ? true : false;
}

static inline bool isUserMemoryFreed( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    return size & FREEBIT ? true : false;
}

void initUserMemory( size_t initialSize ) {
    if ( initialSize < 65536U ) {
        fprintf( stderr, "? initial user memory size too small: %zu\n", initialSize );
        exit( EXIT_FAILURE );
    }
    if ( sizeof(size_t) != sizeof(ptrdiff_t) ) {
        fprintf( stderr, "? sizeof(size_t) != sizeof(ptrdiff_t) [%zu!=%zu]\n", sizeof(size_t), sizeof(ptrdiff_t) );
        exit( EXIT_FAILURE );
    }
    theUserMemory.memory = (char*) calloc( initialSize, sizeof(char) );
    if ( theUserMemory.memory == 0 ) {
        fprintf( stderr, "? failed to allocate user memory of %zu bytes: %m\n", initialSize );
        exit( EXIT_FAILURE );
    }
    theUserMemory.memSize   = initialSize;
    theUserMemory.memUsed   = 8U + sizeof(basedlist_t) * 2U;
    theUserMemory.allocList = 8;
    theUserMemory.freeList  = 8 + sizeof(basedlist_t);
    initBasedList( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.allocList ) );
    initBasedList( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList  ) );
}

/*

temporary solution!

*/

static bool canChangeUserMemory( void ) {
    // memory cannot be changed if there is a locked memory block inside it
    bool ok = true;
    for ( size_t i=0; i < theHandleSpace.usedObjRefs; ++i ) {
        objref_t ref = theHandleSpace.objrefs[i];
        if ( ref == 0 ) continue;   // skip freed blocks
        if ( isUserMemoryLocked( ref ) ) {
            fprintf( stderr, "? locked memory block at %zu\n", ref );
            ok = false;
        }
    }
    return ok;
}

static void oom( void ) {
    fprintf( stderr, "? out of memory\n" );
    exit( EXIT_FAILURE );
}

static int compare_blockinfo( const void* a, const void* b ) {
    const blockinfo_t* infoA = (const blockinfo_t*) a;
    const blockinfo_t* infoB = (const blockinfo_t*) b;
    if ( infoA->block < infoB->block ) return -1;
    if ( infoA->block > infoB->block ) return  1;
    return 0;
}

static blocklist_t getAllocatedBlocks( void ) {
    handle_t numBlocks = 0;
    for ( handle_t i=0; i < theHandleSpace.usedObjRefs; ++i ) {
        if ( theHandleSpace.objrefs[i] && !isUserMemoryLocked( theHandleSpace.objrefs[i] ) ) numBlocks++;
    }
    blocklist_t list;
    if ( numBlocks == 0 ) {
        list.info  = 0;
        list.count = 0;
        return list;
    }
    list.info = (blockinfo_t*) calloc( numBlocks, sizeof(blockinfo_t) );
    if ( list.info == 0 ) oom();
    numBlocks = 0;
    for ( handle_t i=0; i < theHandleSpace.usedObjRefs; ++i ) {
        if ( theHandleSpace.objrefs[i] && !isUserMemoryLocked( theHandleSpace.objrefs[i] ) ) {
            blockinfo_t info;
            info.block  = theHandleSpace.objrefs[i];
            info.handle = i;
            list.info[numBlocks++] = info;
        }
    }
    list.count = numBlocks;
    if ( list.count >= 2U ) {
        qsort( list.info, list.count, sizeof(blockinfo_t), compare_blockinfo );
    }
    return list;
}

static memlist_t getContiguousRegionsFromBlockList( const blocklist_t* list ) {
    memregion_t current; memlist_t regions;
    for ( size_t j=0; j <= 1U; ++j ) {
        current.offsetStart    = 0;
        current.offsetEnd      = 0;
        current.listIndexStart = 0;
        current.listIndexEnd   = 0;
        if ( j == 1U && regions.numRegions ) {
            regions.regions = (memregion_t*) calloc( regions.numRegions, sizeof(memregion_t) );
            if ( regions.regions == 0 ) oom();
        } else {
            regions.regions = 0;
        }
        regions.numRegions = 0;
        bool haveStart = false; 
        for ( size_t i=0; i < list->count; ++i ) {
           size_t newStart = list->info[i].block - sizeof(memhdr_t);
           size_t newEnd   = current.offsetStart + sizeof(memhdr_t) + sizeofUserMemory( list->info[i].block );
            if ( !haveStart ) {
                haveStart = true;
                current.offsetStart    = newStart;
                current.offsetEnd      = newEnd;
                current.listIndexStart = i;
                current.listIndexEnd   = i;
            } else if ( newStart == current.offsetEnd ) {  // contiguous
                current.offsetEnd      = newEnd;
                current.listIndexEnd   = i;
            } else { // a gap has occurred
                if ( j == 1U ) {
                    regions.regions[regions.numRegions++] = current;
                } else {
                    regions.numRegions++;
                }
                haveStart = false;
                --i;
            }
            if ( i == list->count-1U ) {
                if ( j == 1U ) {
                    regions.regions[regions.numRegions++] = current;
                } else {
                    regions.numRegions++;
                }
            } 
        } // i
    } // j
    return regions;
}

static memlist_t getContiguousRegionsFromFreeList( const freelist_t* list ) {
    memregion_t current; memlist_t regions;
    for ( size_t j=0; j <= 1U; ++j ) {
        current.offsetStart    = 0;
        current.offsetEnd      = 0;
        current.listIndexStart = 0;
        current.listIndexEnd   = 0;
        if ( j == 1U && regions.numRegions ) {
            regions.regions = (memregion_t*) calloc( regions.numRegions, sizeof(memregion_t) );
            if ( regions.regions == 0 ) oom();
        } else {
            regions.regions = 0;
        }
        regions.numRegions = 0;
        bool haveStart = false; 
        for ( size_t i=0; i < list->count; ++i ) {
           size_t newStart = list->info[i].offset;
           size_t newEnd   = current.offsetStart + sizeof(memhdr_t) + sizeofUserMemory( list->info[i].offset );
            if ( !haveStart ) {
                haveStart = true;
                current.offsetStart    = newStart;
                current.offsetEnd      = newEnd;
                current.listIndexStart = i;
                current.listIndexEnd   = i;
            } else if ( newStart == current.offsetEnd ) {  // contiguous
                current.offsetEnd      = newEnd;
                current.listIndexEnd   = i;
            } else { // a gap has occurred
                if ( j == 1U ) {
                    regions.regions[regions.numRegions++] = current;
                } else {
                    regions.numRegions++;
                }
                haveStart = false;
                --i;
            }
            if ( i == list->count-1U ) {
                if ( j == 1U ) {
                    regions.regions[regions.numRegions++] = current;
                } else {
                    regions.numRegions++;
                }
            } 
        } // i
    } // j
    return regions;
}

static int compare_freeinfo( const void* a, const void* b ) {
    const freeinfo_t* infoA = (const freeinfo_t*) a;
    const freeinfo_t* infoB = (const freeinfo_t*) b;
    if ( infoA->size < infoB->size ) return -1;
    if ( infoA->size > infoB->size ) return  1;
    return 0;
}

static freelist_t getFreeList( size_t minSize, size_t maxSize ) {
    freelist_t list;
    list.info = 0;
    for ( int i=0; i <= 1; ++i ) {
        list.count = 0;
        basednode_t* node = (basednode_t*) firstBasedNode( theUserMemory.memory, 
            (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ) );
        while ( node ) {
            memhdr_t* hdr = (memhdr_t*) node;
            size_t    siz = hdr->size & CHUNKMAX;
            if ( siz >= minSize && siz <= maxSize ) {
                if ( i == 1 ) {
                    freeinfo_t info;
                    info.offset = (size_t)( ((char*)hdr) - theUserMemory.memory );
                    info.size   = siz;
                    list.info[list.count] = info;
                }
                list.count++;
            }
            node = (basednode_t*) nextBasedNode( theUserMemory.memory, node );
        }
        if ( i == 0 && list.count ) {
            list.info = (freeinfo_t*) calloc( list.count, sizeof(freeinfo_t) );
            if ( list.info == 0 ) oom();
        }
    }
    if ( list.count >= 2 ) {
        qsort( list.info, list.count, sizeof(freeinfo_t), compare_freeinfo );
    }
    return list;
}

static bool findFreeBlock( size_t size, freeinfo_t* outInfo ) {
    freelist_t list = getFreeList( size, size + ( size / 3U ) );    // min = ideal size, max = ideal size + 33%
    if ( list.info == 0 || list.count == 0 ) return false;
    *outInfo = list.info[0];
    free( list.info );
    return true;
}

static void compactUserMemory( void ) {
    blocklist_t allocatedBlocks = getAllocatedBlocks();
    if ( allocatedBlocks.count == 0 ) { // compaction impossible
        return;
    }
    memlist_t allocatedRegions = getContiguousRegionsFromBlockList( &allocatedBlocks );
    if ( allocatedRegions.numRegions == 0 ) {
        free( allocatedBlocks.info );
        return;
    }
    freelist_t freeList = getFreeList( 0, CHUNKMAX );
    if ( freeList.count == 0 ) {
        free( allocatedRegions.regions );
        free( allocatedBlocks .info    );
        return;
    }
    memlist_t freeRegions = getContiguousRegionsFromFreeList( &freeList );
    if ( freeRegions.numRegions == 0 ) {
        free( freeList        .info    );
        free( allocatedRegions.regions );
        free( allocatedBlocks .info    );
        return;
    }

    // ...


    free( freeRegions     .regions );
    free( freeList        .info    );
    free( allocatedRegions.regions );
    free( allocatedBlocks .info    );
}

static void growUserMemory( void ) {
    // cannot be called when there are locked memory blocks inside user memory
    if ( !canChangeUserMemory() ) {
        fprintf( stderr, "? cannot grow user memory, there are locked memory blocks inside\n" );
        exit( EXIT_FAILURE );
    }
    size_t maxSize = SIZE_MAX / 2U;
    size_t newSize;
    if ( theUserMemory.memSize < maxSize ) {
        newSize = maxSize * 2U;
    } else {
        newSize = SIZE_MAX;
        if ( theUserMemory.memSize == newSize ) {
            fprintf( stderr, "? cannot grow user memory, it's too big\n" );
            exit( EXIT_FAILURE );
        }
    }
    char* newMem = calloc( newSize, sizeof(char) );
    if ( newMem == 0 ) {
        fprintf( stderr, "? failed to allocate user memory of %zu bytes: %m\n", newSize );
        exit( EXIT_FAILURE );
    }
    if ( theUserMemory.memUsed ) {
        memcpy( newMem, theUserMemory.memory, theUserMemory.memUsed );
    }
    free( theUserMemory.memory );
    theUserMemory.memory  = newMem;
    theUserMemory.memSize = newSize;
}

objref_t allocUserMemory( size_t requestSize ) {
    // align memory block size
    size_t alignedSize = ( sizeof(memhdr_t) + requestSize + (CHUNKALIGN-1U) ) & ~((size_t)(CHUNKALIGN-1U));
    // begin
    if ( alignedSize > CHUNKMAX ) {
        fprintf( stderr, "? requested block size too large: %zu (from: %zu) > %zu\n", alignedSize, requestSize, (size_t) CHUNKMAX );
        exit( EXIT_FAILURE );
    }
    size_t allocPos = theUserMemory.memUsed; size_t allocSize = alignedSize;
    bool recycled = false;
    if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
        // request does not meet remaining space: attempt to find suitable free block
        freeinfo_t info;
        bool found = findFreeBlock( allocSize, &info );
        if ( found ) {
            allocPos  = info.offset;
            allocSize = info.size;
            recycled  = true;
        } else {
            // attempt to garbage collect
            // compactUserMemory();        
            if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
                // didn't work: resize it
                // growUserMemory();
                if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
                    fprintf( stderr, "? internal error: not enough space for allocation of %zu bytes.\n", alignedSize );
                    exit( EXIT_FAILURE );
                } else {
                    allocPos = theUserMemory.memUsed;
                }
            } else {
                allocPos = theUserMemory.memUsed;
            }
        }
    } 
    char* blk = (char*) theUserMemory.memory + allocPos;
    if ( !recycled ) theUserMemory.memUsed += allocSize;
    memhdr_t* hdr = (memhdr_t*) blk;
    blk += sizeof(memhdr_t);
    initBasedNode( theUserMemory.memory, &hdr->node );
    hdr->size = allocSize;
    addBasedNodeAtTail( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.allocList ), 
        &hdr->node );
    return (objref_t)( blk - theUserMemory.memory );
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
    memhdr_t* hdr = (memhdr_t*)( blk - sizeof(memhdr_t) );
    removeBasedNode( theUserMemory.memory, &hdr->node );
    // check last node in free list if it would form a contiguous region with this node
    memhdr_t* last = (memhdr_t*) lastBasedNode( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ) );
    if ( last && ((char*)last) + last->size == (char*) hdr ) {
        // yes: merge into that node
        size_t newSize = ( last->size & CHUNKMAX ) + ( hdr->size & CHUNKMAX );
        if ( newSize <= CHUNKMAX ) {    // can safely merge
            last->size = newSize | FREEBIT;           
            // clear whole memory region that was added
            memset( hdr, 0, hdr->size & CHUNKMAX );
            return;
        }
    }
    // unmerged: simply clear and add to end of free list
    size = ( hdr->size & CHUNKMAX ) - sizeof(memhdr_t);
    if ( size ) memset( blk, 0, size );
    addBasedNodeAtTail( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ), &hdr->node );
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
    return (void*) blk;
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
    return ( size & CHUNKMAX ) - sizeof(memhdr_t);
}

