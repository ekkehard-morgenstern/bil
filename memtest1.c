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
#include "usermemory.h"

#define MAX_HANDLES     131072U
#define MAX_USERSPACE   1048576U

#define MAX_RANDOMBUF   1048576U

#define NUM_ITERATIONS  100000U

static FILE* randFp;

static unsigned randu( unsigned modVal ) {
    unsigned uval;
    if ( fread( &uval, sizeof(uval), 1U, randFp ) != 1U ) {
        fprintf( stderr, "I/O error from /dev/urandom\n" );
        exit( EXIT_FAILURE );
    }
    return uval % modVal;
}

static void fillrand( void* mem, size_t size ) {
    if ( fread( mem, size, 1U, randFp ) != 1U ) {
        fprintf( stderr, "I/O error from /dev/urandom\n" );
        exit( EXIT_FAILURE );
    }
}

static double gettime( void ) {
    struct timespec ts;
    if ( clock_gettime( CLOCK_MONOTONIC, &ts ) == -1 ) {
        fprintf( stderr, "clock_gettime(2) failed: %m\n" );
        exit( EXIT_FAILURE );
    }
    return ( (double) ts.tv_sec ) + ( ( (double) ts.tv_nsec ) / 1.0e9 );
}

int main( int argc, char** argv ) {

    initHandleSpace( MAX_HANDLES );
    initUserMemory( MAX_USERSPACE );

    randFp = fopen( "/dev/urandom", "rb" );
    if ( randFp == 0 ) {
        fprintf( stderr, "failed to open /dev/urandom: %m\n" );
        return EXIT_FAILURE;
    }
    setvbuf( randFp, 0, _IOFBF, MAX_RANDOMBUF );
    
    handle_t handles[NUM_ITERATIONS];
    size_t   numHandles = 0;

    double ti0 = gettime();

    for ( unsigned i=0; i < NUM_ITERATIONS; ++i ) {

        unsigned r = randu( 100U );

        if ( r >= 70U ) {           // in 30% of cases, allocate a new block
            if ( numHandles < NUM_ITERATIONS ) {
                size_t reqSize = 20U + randu( 9980U );
                handles[numHandles] = allocHandle( reqSize );
                void* mem = lockHandle( handles[numHandles] );
                fillrand( mem, reqSize );
                unlockHandle( handles[numHandles] );
                numHandles++;
            }
        } else if ( r >= 30U ) {    // in 40% of cases, do something with an existing block
            if ( numHandles > 0U ) {
                size_t handle = randu( numHandles );
                void*  mem    = lockHandle( handles[handle] );
                size_t siz    = handleSize( handles[handle] );
                fillrand( mem, siz );
                unlockHandle( handles[handle] );
            }
        } else {                    // in 30% of cases, free a block
            if ( numHandles > 0U ) {
                size_t handle = randu( numHandles );
                if ( handle == numHandles-1U ) {
                    freeHandle( handles[handle] );
                    --numHandles;
                } else {
                    freeHandle( handles[handle] );
                    memmove( &handles[handle], &handles[handle+1U],
                        sizeof(handle_t)*( numHandles - (handle+1U) ) );
                    --numHandles;
                }
            }
        }
    }

    double ti1 = gettime();
    double dur = ti1 - ti0;

    printf( "duration: %g sec(s)\n", dur );
    printf( "numHandles at end: %zu\n", numHandles );
    
    memusage_t usage = memoryUsage();
    printf( "total user space: %zu\nallocated user space: %zu\n"
        "free user space: %zu\ntotal handle space: %zu\n"
        "allocated handle space: %zu\nfree handle space: %zu\n"
        "total memory: %zu\nallocated memory: %zu\nfree memory: %zu\n"
        , usage.totalUserSpace, usage.allocatedUserSpace, usage.freeUserSpace,
        usage.totalHandleSpace, usage.allocatedHandleSpace, usage.freeHandleSpace,
        usage.totalMemory, usage.allocatedMemory, usage.freeMemory
    );

    return EXIT_SUCCESS;
}