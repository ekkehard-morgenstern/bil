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
        printf( "%-*.*s%d\n", indent, indent, "", (int) node->nodetype );
    } else {
        printf( "%-*.*s%d '%s'\n", indent, indent, "", (int) node->nodetype, node->text );
    }
    for ( size_t i=0; i < node->numBranches; ++i ) {
        dump_tree_node( node->branches[i], indent+2 );
    }
}

static treenode_t* create_node( nodetype_t nodetype, const char* text ) {
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

static const parsingnode_t* find_parsing_node( nodetype_t nodetype ) {
    size_t numParsingNodes = sizeof(parsingTable) / sizeof(parsingnode_t);
    for ( size_t i=0; i < numParsingNodes; ++i ) {
        const parsingnode_t* node = &parsingTable[i];
        if ( node->nodeType == nodetype ) return node;
    }
    return 0;
}

static void fatal( const char* fmt, ... ) {
    va_list ap;
    va_start( ap, fmt );
    fprintf( stderr, "fatal: " );
    vfprintf( stderr, fmt, ap );
    fprintf( stderr, "\n" );
    va_end( ap );
    exit( EXIT_FAILURE );
}

static treenode_t* parse_node( const parsingnode_t* node, const char** pTextPos ) {
    treenode_t* temp; treenode_t* result = 0; size_t i;
    switch ( node->nodeClass ) {
        case NC_TERMINAL:
            switch ( node->termType ) {
                case TT_STRING:
                    break;
                case TT_REGEX:
                    break;
                default:
                    fatal( "bad parsing node (type A)" );
            }
            break;
        case NC_PRODUCTION:
            if ( node->numBranches != 1U ) fatal( "bad parsing node (type C)" );
            return parse_node( &parsingTable[branches[node->branches]], pTextPos );           
        case NC_MANDATORY:
            result = create_node( node->nodeType, 0 );
            for ( i=0; i < node->numBranches; ++i ) {
                temp = parse_node( &parsingTable[branches[node->branches+i]], pTextPos );
                if ( temp == 0 ) { delete_node( result ); return 0; }
                add_branch( result, temp );
            }
            break;
        case NC_ALTERNATIVE:
            for ( i=0; i < node->numBranches; ++i ) {
                temp = parse_node( &parsingTable[branches[node->branches+i]], pTextPos );
                if ( temp ) return temp;
            }
            return 0;          
        case NC_OPTIONAL:
            if ( node->numBranches != 1U ) fatal( "bad parsing node (type D)" );
            return parse_node( &parsingTable[branches[node->branches]], pTextPos );
        case NC_OPTIONAL_REPETITIVE:
            if ( node->numBranches != 1U ) fatal( "bad parsing node (type E)" );
            result = create_node( node->nodeType, 0 );
            for (;;) {
                temp = parse_node( &parsingTable[branches[node->branches]], pTextPos );
                if ( temp == 0 ) break;
                add_branch( result, temp );
            }
            break;
        default:
            fatal( "bad parsing node (type B)" );
    }
    if ( result ) {
        if ( result->numBranches == 0U ) { delete_node( result ); return 0; }
        if ( result->numBranches == 1U ) {
            temp = result->branches[0]; result->branches[0] = 0;
            delete_node( result ); result = temp;
        }
    }
    return result;
}

treenode_t* parse( const char* text ) {
    const parsingnode_t* node  = find_parsing_node( NT_GLOBAL_SCOPE );
    if ( node == 0 ) return 0;
    const char* textPos = text;
    return parse_node( node, &textPos );
}
