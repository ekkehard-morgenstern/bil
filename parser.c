#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// include auto-generated file from ebnfcomp
#include "parsingtable.c"

// NOTE to self: When encountering a TT_STRING node, check the length of the underlying character sequence
// (important for identifiers and keywords).
// Make sure EBNF is coded such that the longest sequence matches first. (comparison operators etc.)

typedef struct _treenode_t {
    nodetype_t              nodetype;
    char*                   text;
    struct _treenode_t**    branches;
    size_t                  branchAlloc;
    size_t                  numBranches;
} treenode_t;

static void* xmalloc( size_t size ) {
    size_t reqSize = size ? size : 1U;
    void* blk = malloc( reqSize );
    if ( blk == 0 ) {
        fprintf( stderr, "? out of memory\n" );
        exit( EXIT_FAILURE );
    }
    return blk;
}

static void xrealloc( void** pBlk, size_t newSize ) {
    size_t reqSize = newSize ? newSize : 0U;
    if ( *pBlk == 0 ) {
        *pBlk = xmalloc( reqSize );
    } else {
        void* newBlk = realloc( *pBlk, reqSize );
        if ( newBlk == 0 ) {
            fprintf( stderr, "? out of memory\n" );
            exit( EXIT_FAILURE );
        }
        *pBlk = newBlk;
    }
}

static char* xstrdup( const char* text ) {
    size_t len = strlen( text );
    char* blk = (char*) xmalloc( len + 1U );
    strcpy( blk, text );
    return blk;
}

static void dump_tree_node( treenode_t* node, int indent ) {
    if ( node == 0 ) return;
    if ( node->text == 0 ) {
        printf( "%-*.*s%s\n", indent, indent, "", token2text(node->nodetype) );
    } else {
        printf( "%-*.*s%s '%s'\n", indent, indent, "", token2text(node->nodetype), node->text );
    }
    for ( size_t i=0; i < node->numBranches; ++i ) {
        dump_tree_node( node->branches[i], indent+2 );
    }
}

static treenode_t* create_node( treenode_t nodetype, const char* text ) {
    treenode_t* node = (treenode_t*) xmalloc( sizeof(treenode_t) );
    node->nodetype     = nodetype;
    node->text         = text ? xstrdup(text) : 0;
    node->branches     = (struct _treenode_t**) xmalloc( sizeof(struct _treenode_t*) * 5U );
    node->branchAlloc  = 5U;
    node->numBranches  = 0U;
    return node;
}

static void delete_node( treenode_t* node ) {
    while ( node->numBranches > 0U ) {
        treenode_t* branch = node->branches[--node->numBranches];
        if ( branch ) delete_node( branch );
    }
    free( (void*)(node->branches) ); node->branches = 0;
    node->branchAlloc = 0U;
    if ( node->text ) { free( node->text ); node->text = 0; }
    free( node );
}

static void add_branch( treenode_t* node, treenode_t* branch ) {
    if ( node->numBranches >= node->branchAlloc ) {
        size_t newSize = node->branchAlloc * 2U;
        xrealloc( (void**)(&node->branches), sizeof(struct _treenode_t*) * newSize );
        node->branchAlloc = newSize;
    }
    node->branches[ node->numBranches++ ] = branch;
}

static parsingnode_t* find_parsing_node( nodetype_t nodetype ) {
    size_t numParsingNodes = sizeof(parsingTable) / sizeof(parsingnode_t);
    for ( size_t i=0; i < numParsingNodes; ++i ) {
        parsingnode_t* node = &parsingTable[i];
        if ( node->nodeType == nodetype ) return node;
    }
    return 0;
}

treenode_t* parse_node( const char** pPos, parsingnode_t* node ) {


}

treenode_t* parse( const char* text ) {
    parsingnode_t* root = find_parsing_node( NT_GLOBAL_SCOPE );
    if ( root == 0 ) return 0;
    const char* pos = text;
    parsingnode_t* result = parse_node( &pos, root );
    if ( result == 0 ) return 0;

    return result;
}