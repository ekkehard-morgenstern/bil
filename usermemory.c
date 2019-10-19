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

usermemory_t        theUserMemory;
size_t              numGCCycles = 0;
size_t              numScaleCycles = 0;
static pthread_t    compactorThread;
static bool         compactorRun  = false;
static bool         compactorQuit = false;
pthread_mutex_t     userMemLock   = PTHREAD_MUTEX_INITIALIZER;

static void badBlockOffset( objref_t block ) {
    fprintf( stderr, "? illegal user memory block: %#zx\n", (objref_t) block );
    _exit( EXIT_FAILURE );
}

static inline char* validateUserMemory( objref_t block ) {
    if ( block < sizeof(memhdr_t) || block >= theUserMemory.memUsed ) badBlockOffset( block );
    char* blk = theUserMemory.memory + block;
    memhdr_t* hdr = (memhdr_t*)( blk - sizeof(memhdr_t) );
    if ( hdr->magic != MEMHDR_MAGIC ) badBlockOffset( block );
    return blk;
}

static inline size_t getBlockSize( char* blk ) {
    memhdr_t* hdr = (memhdr_t*)( blk - sizeof(memhdr_t) );
    return hdr->size;
}

static inline void setBlockSize( char* blk, size_t size ) {
    memhdr_t* hdr = (memhdr_t*)( blk - sizeof(memhdr_t) );
    hdr->size = size;
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

static void compactUserMemory( void );

static double gettime( void ) {
    struct timespec ts;
    if ( clock_gettime( CLOCK_MONOTONIC, &ts ) == -1 ) {
        fprintf( stderr, "clock_gettime(2) failed: %m\n" );
        _exit( EXIT_FAILURE );
    }
    return ( (double) ts.tv_sec ) + ( ( (double) ts.tv_nsec ) / 1.0e9 );
}

#define CYCLETIME   0.1    // 1/10 sec

static void* compactorMain( void* arg ) {
    double cycleTime = CYCLETIME;
    while ( !compactorQuit ) {
        if ( compactorRun ) {
            double ti0 = gettime();
            pthread_mutex_lock( &userMemLock );
            compactUserMemory();
            pthread_mutex_unlock( &userMemLock );
            double ti1 = gettime();
            double dur = ti1 - ti0;
            if ( dur < cycleTime ) {
                double slp = cycleTime - dur;
                struct timespec ts, tsrem; double secs = trunc( slp );
                ts.tv_sec  = (long) secs;
                ts.tv_nsec = (long) ( ( slp - secs ) * 1e9 );
                for (;;) {
                    int rv = nanosleep( &ts, &tsrem );
                    if ( rv == -1 && errno == EINTR ) ts = tsrem; else break;
                }
                // if ( dur < cycleTime / 10.0 ) cycleTime /= 2.0;
            } else {
                cycleTime *= 2.0;
            }
        }
    }
    return 0;
}

static void compactorTerm( void ) {
    compactorQuit = true;
    void* retval = 0;
    pthread_join( compactorThread, &retval );
}

void initUserMemory( size_t initialSize ) {
    if ( initialSize < 65536U ) {
        fprintf( stderr, "? initial user memory size too small: %zu\n", initialSize );
        _exit( EXIT_FAILURE );
    }
    if ( sizeof(size_t) != sizeof(ptrdiff_t) ) {
        fprintf( stderr, "? sizeof(size_t) != sizeof(ptrdiff_t) [%zu!=%zu]\n", sizeof(size_t), sizeof(ptrdiff_t) );
        _exit( EXIT_FAILURE );
    }
    theUserMemory.memory = (char*) calloc( initialSize, sizeof(char) );
    if ( theUserMemory.memory == 0 ) {
        fprintf( stderr, "? failed to allocate user memory of %zu bytes: %m\n", initialSize );
        _exit( EXIT_FAILURE );
    }
    theUserMemory.memSize   = initialSize;
    theUserMemory.memUsed   = 8U + sizeof(basedlist_t) * 2U;
    theUserMemory.allocList = 8;
    theUserMemory.freeList  = 8 + sizeof(basedlist_t);
    initBasedList( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.allocList ) );
    initBasedList( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList  ) );
    int rv = pthread_create( &compactorThread, 0, compactorMain, 0 );
    if ( rv != 0 ) {
        errno = rv;
        fprintf( stderr, "? failed to create compactor thread: %m\n" );
        _exit( EXIT_FAILURE );
    }
    atexit( compactorTerm );
    compactorRun = true;
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
    _exit( EXIT_FAILURE );
}

static int compare_blockinfo( const void* a, const void* b ) {
    const blockinfo_t* infoA = (const blockinfo_t*) a;
    const blockinfo_t* infoB = (const blockinfo_t*) b;
    if ( infoA->block < infoB->block ) return  1;
    if ( infoA->block > infoB->block ) return -1;
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
           size_t memSize  = list->info[i].size;
           size_t newStart = list->info[i].offset;
           size_t newEnd   = newStart + memSize;
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

static int compare_freeinfo2( const void* a, const void* b ) {
    const freeinfo_t* infoA = (const freeinfo_t*) a;
    const freeinfo_t* infoB = (const freeinfo_t*) b;
    if ( infoA->offset < infoB->offset ) return -1;
    if ( infoA->offset > infoB->offset ) return  1;
    return 0;
}

static freelist_t getFreeList( size_t minSize, size_t maxSize, bool sortBySize ) {
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
    if ( list.count >= 2 && sortBySize ) {
        qsort( list.info, list.count, sizeof(freeinfo_t), compare_freeinfo );
    } else {
        qsort( list.info, list.count, sizeof(freeinfo_t), compare_freeinfo2 );
    }
    return list;
}

static bool findFreeBlock( size_t size, freeinfo_t* outInfo ) {
    basednode_t* node = (basednode_t*) firstBasedNode( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ) );
    while ( node ) {
        memhdr_t* hdr = (memhdr_t*) node;
        size_t    siz = hdr->size & CHUNKMAX;
        if ( siz >= size ) {
            freeinfo_t info;
            info.offset = (size_t)( ((char*)hdr) - theUserMemory.memory );
            info.size   = siz;
            *outInfo = info;
            return true;
        }
        node = (basednode_t*) nextBasedNode( theUserMemory.memory, node );
    }
    return false;
}

static bool mergeFreeRegions( freelist_t freeList, memlist_t freeRegions ) {
    bool change = false;
    for ( size_t i=0; i < freeRegions.numRegions; ++i ) {
        const memregion_t* rgn = &freeRegions.regions[i];
        if ( rgn->listIndexStart < rgn->listIndexEnd ) { // mergeable
            memhdr_t* hdr = (memhdr_t*)( theUserMemory.memory + rgn->offsetStart );
            size_t    siz = hdr->size & CHUNKMAX;
            // remove nodes from free list
            for ( size_t j=rgn->listIndexStart + 1U; j <= rgn->listIndexEnd; ++j ) {
                const freeinfo_t* info = &freeList.info[j];
                memhdr_t* blkhdr = (memhdr_t*)( theUserMemory.memory + info->offset );
                size_t    blksiz = blkhdr->size & CHUNKMAX;
                if ( siz + blksiz > CHUNKMAX ) { // merged block would be too large
                    // start new header
                    hdr = blkhdr;
                    siz = blksiz;
                } else {
                    // extend current header
                    siz += blksiz;
                    hdr->size = siz | FREEBIT;
                    removeBasedNode( theUserMemory.memory, &blkhdr->node );
                    // clear memory header
                    memset( theUserMemory.memory + info->offset, 0, sizeof(memhdr_t) );
                    change = true;
                }
            }
        }
    }
    return change;
}

static bool moveBlocks( blocklist_t allocatedBlocks, freelist_t freeList ) {
    bool change = false;
    for ( size_t i=0; i < allocatedBlocks.count; ++i ) {
        const blockinfo_t* info = &allocatedBlocks.info[i];
        memhdr_t* hdr = (memhdr_t*)( theUserMemory.memory + info->block - sizeof(memhdr_t) );
        size_t    siz = hdr->size & CHUNKMAX;
        size_t    cpy = siz;
        for ( size_t j=0; j < freeList.count; ++j ) {
            freeinfo_t* info2 = &freeList.info[j];
            // don't move blocks to higher addresses
            if ( info2->offset + sizeof(memhdr_t) > info->block ) break;
            if ( siz < info2->size ) {  // found a suitable free region
                memhdr_t* hdr2 = (memhdr_t*)( theUserMemory.memory + info2->offset );
                removeBasedNode( theUserMemory.memory, &hdr2->node );
                memset( hdr2, 0, sizeof(memhdr_t) );
                objref_t newPos = info2->offset;
                // see if the header can be moved forward
                size_t remain = info2->size - siz;
                if ( remain <= sizeof(memhdr_t) ) {     // no!
                    // allocate it all
                    siz = info2->size;
                    info2->offset = 0;
                    info2->size   = 0;
                } else {
                    // move block start address forward
                    info2->offset += siz;
                    info2->size   -= siz;
                    hdr2 = (memhdr_t*)( theUserMemory.memory + info2->offset );
                    initBasedNode( theUserMemory.memory, &hdr2->node );
                    hdr2->size  = info2->size | FREEBIT;
                    hdr2->magic = MEMHDR_MAGIC;
                    addBasedNodeAtTail( theUserMemory.memory, 
                        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ),
                        &hdr2->node );
                }
                // relocate node
                removeBasedNode( theUserMemory.memory, &hdr->node );
                memcpy( theUserMemory.memory + newPos, hdr, cpy );
                memhdr_t* old = hdr;
                memset( old, 0, cpy );
                initBasedNode( theUserMemory.memory, &old->node );
                old->size  = cpy | FREEBIT;
                old->magic = MEMHDR_MAGIC;
                addBasedNodeAtTail( theUserMemory.memory, 
                    (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ),
                    &old->node );
                hdr = (memhdr_t*)( theUserMemory.memory + newPos );
                addBasedNodeAtTail( theUserMemory.memory, 
                    (basedlist_t*)( theUserMemory.memory + theUserMemory.allocList ),
                    &hdr->node );
                theHandleSpace.objrefs[ info->handle ] = newPos + sizeof(memhdr_t);
                change = true;
                break;  // process next handle
            }
        }
    }
    return change;
}

static void compactUserMemory( void ) {
    blocklist_t allocatedBlocks;
    freelist_t  freeList;
    memlist_t   freeRegions;
    bool        change;

REDO_ALLOCATED_BLOCKS:

    allocatedBlocks = getAllocatedBlocks();
    if ( allocatedBlocks.count == 0 ) goto END1; // compaction impossible

REDO_FREELIST:

    freeList = getFreeList( 0, CHUNKMAX, false );
    if ( freeList.count == 0 ) goto END3;
    freeRegions = getContiguousRegionsFromFreeList( &freeList );
    if ( freeRegions.numRegions == 0 ) goto END4;

    // merge free regions
    change = mergeFreeRegions( freeList, freeRegions );

    if ( change ) {
        free( freeRegions.regions );
        free( freeList   .info    );
        goto REDO_FREELIST;
    }
 
    free( freeRegions.regions );

    // begin moving blocks from the end of the memory area to the available free areas
    change = moveBlocks( allocatedBlocks, freeList );

    if ( change ) {
        free( freeList       .info );
        free( allocatedBlocks.info );
        goto REDO_ALLOCATED_BLOCKS;
    }

    // see if the end of the free list could be used to reduce memUsed
    if ( freeList.count > 0U ) {
        const freeinfo_t* info = &freeList.info[freeList.count-1U];
        size_t offsetEnd = info->offset + info->size;
        if ( offsetEnd == theUserMemory.memUsed ) {
            theUserMemory.memUsed -= info->size;
            memhdr_t* hdr = (memhdr_t*)( theUserMemory.memory + info->offset );
            removeBasedNode( theUserMemory.memory, &hdr->node );
            memset( hdr, 0, info->size );
        }
    } 

END4:   free( freeList        .info    );
END3:   free( allocatedBlocks .info    );
END1:   numGCCycles++;
}

static void growUserMemory( void ) {
    // cannot be called when there are locked memory blocks inside user memory
    if ( !canChangeUserMemory() ) {
        fprintf( stderr, "? cannot grow user memory, there are locked memory blocks inside\n" );
        _exit( EXIT_FAILURE );
    }
    size_t maxSize = SIZE_MAX / 2U;
    size_t newSize;
    if ( theUserMemory.memSize < maxSize ) {
        newSize = theUserMemory.memSize * 2U;
    } else {
        newSize = SIZE_MAX;
        if ( theUserMemory.memSize == newSize ) {
            fprintf( stderr, "? cannot grow user memory, it's too big\n" );
            _exit( EXIT_FAILURE );
        }
    }
    char* newMem = calloc( newSize, sizeof(char) );
    if ( newMem == 0 ) {
        fprintf( stderr, "? failed to allocate user memory of %zu bytes: %m\n", newSize );
        _exit( EXIT_FAILURE );
    }
    if ( theUserMemory.memUsed ) {
        memcpy( newMem, theUserMemory.memory, theUserMemory.memUsed );
    }
    free( theUserMemory.memory );
    theUserMemory.memory  = newMem;
    theUserMemory.memSize = newSize;
    numScaleCycles++;
}

objref_t allocUserMemory( size_t requestSize ) {
    // align memory block size
    size_t alignedSize = ( sizeof(memhdr_t) + requestSize + (CHUNKALIGN-1U) ) & ~((size_t)(CHUNKALIGN-1U));
    // begin
    if ( alignedSize > CHUNKMAX ) {
        fprintf( stderr, "? requested block size too large: %zu (from: %zu) > %zu\n", alignedSize, requestSize, (size_t) CHUNKMAX );
        _exit( EXIT_FAILURE );
    }
    size_t allocPos = theUserMemory.memUsed; size_t allocSize = alignedSize;
    bool recycled = false;
    if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
        // request does not meet remaining space: attempt to find suitable free block
        freeinfo_t info;
        bool found = findFreeBlock( allocSize, &info );
        if ( found ) {
            // remove node from free list
            basednode_t* node = (basednode_t*)( theUserMemory.memory + info.offset );
            removeBasedNode( theUserMemory.memory, node );
            // now recycle it
            allocPos  = info.offset;
            allocSize = info.size;
            recycled  = true;
        } else {
            // compact now!
            compactUserMemory();
            if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
                // didn't work: resize it
                growUserMemory();
                if ( allocSize > theUserMemory.memSize - theUserMemory.memUsed ) {
                    fprintf( stderr, "? internal error: not enough space for allocation of %zu bytes.\n", alignedSize );
                    _exit( EXIT_FAILURE );
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
    hdr->size  = allocSize;
    hdr->magic = MEMHDR_MAGIC;
    addBasedNodeAtTail( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.allocList ), 
        &hdr->node );
    objref_t result = (objref_t)( blk - theUserMemory.memory );
    return result;
}

void freeUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx already freed\n", block );
        _exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %#zx still locked\n", block );
        _exit( EXIT_FAILURE );
    }
    size |= FREEBIT;
    setBlockSize( blk, size );
    size_t    chunkSize = size & CHUNKMAX;
    char*     blkStart  = blk - sizeof(memhdr_t);
    char*     blkEnd    = blkStart + chunkSize;
    memhdr_t* hdr       = (memhdr_t*) blkStart;
    removeBasedNode( theUserMemory.memory, &hdr->node );
    if ( blkEnd == theUserMemory.memory + theUserMemory.memUsed ) {
        // chunk was exactly at end of allocated area: in this case, don't add to free list
        theUserMemory.memUsed -= chunkSize;
        memset( blkStart, 0, chunkSize );
        return;
    }
    // check last node in free list if it would form a contiguous region with this node
    memhdr_t* last = (memhdr_t*) lastBasedNode( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ) );
    size_t lastSize = last ? last->size & CHUNKMAX : 0;
    if ( last && ((char*)last) + lastSize == (char*) hdr ) {
        // yes: merge into that node
        size_t newSize = lastSize + chunkSize;
        if ( newSize <= CHUNKMAX ) {    // can safely merge
            last->size = newSize | FREEBIT;           
            // clear whole memory region that was added
            memset( hdr, 0, chunkSize );
            return;
        }
    }
    // unmerged: simply clear and add to end of free list
    size = chunkSize - sizeof(memhdr_t);
    if ( size ) memset( blk, 0, size );
    addBasedNodeAtTail( theUserMemory.memory, 
        (basedlist_t*)( theUserMemory.memory + theUserMemory.freeList ), &hdr->node );
}

void* lockUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx not allocated\n", block );
        _exit( EXIT_FAILURE );
    }
    if ( size & LOCKBIT ) {
        fprintf( stderr, "? block %#zx already locked\n", block );
        _exit( EXIT_FAILURE );
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
        _exit( EXIT_FAILURE );
    }
    if ( ( size & LOCKBIT ) == 0 ) {
        fprintf( stderr, "? block %#zx not locked\n", block );
        _exit( EXIT_FAILURE );
    }
    size &= ~LOCKBIT;
    setBlockSize( blk, size );
}

size_t sizeofUserMemory( objref_t block ) {
    char*  blk  = validateUserMemory( block );
    size_t size = getBlockSize( blk );
    if ( size & FREEBIT ) {
        fprintf( stderr, "? block %#zx not allocated\n", block );
        _exit( EXIT_FAILURE );
    }
    return ( size & CHUNKMAX ) - sizeof(memhdr_t);
}

