#include "basedlists.h"

static char mem[1024];

static void dump( size_t count ) {
    for ( size_t i=0; i < count; ++i ) {
        unsigned char b = mem[i];
        printf( " %02X", b );
    }
    printf( "\n" );
}

int main( int argc, char** argv ) {

    memset( mem, 0, sizeof(mem) );
    char* base = mem;
    basedlist_t* list = (basedlist_t*) base;

    initBasedList( base, list );
    dump( sizeof(basedlist_t) );

    basednode_t* node = (basednode_t*)( base + sizeof(basedlist_t) );
    initBasedNode( base, node );
    dump( sizeof(basedlist_t) + sizeof(basednode_t) );

    addBasedNodeAtTail( base, list, node );
    dump( sizeof(basedlist_t) + sizeof(basednode_t) );

    basednode_t* node2 = firstBasedNode( base, list );
    basednode_t* node3 = lastBasedNode( base, list );
    if ( node2 != node || node3 != node ) {
        fprintf( stderr, "node=%p, node2=%p, node3=%p\n", node, node2, node3 );
        return EXIT_FAILURE;
    }

    removeBasedNode( base, node );
    dump( sizeof(basedlist_t) + sizeof(basednode_t) );

    return EXIT_SUCCESS;
}