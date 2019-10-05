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

#include <stdlib.h>
#include <mcheck.h>
#include <stdio.h>
#include "parser.h"


static char* readfile( const char* name ) {
    FILE* fp = fopen( name, "rt" );
    if ( fp == 0 ) {
        fprintf( stderr, "couldn't read file '%s': %m\n", name );
        return 0;
    }
    fseek( fp, 0, SEEK_END );
    long size = ftell( fp );
    rewind( fp );
    char* buf = (char*) xmalloc( size + 1 );
    fread( buf, size, 1, fp );
    fclose( fp );
    buf[size] = '\0';
    return buf;
}


int main( int argc, char** argv ) {

    if ( argc < 2 ) {
        fprintf( stderr, "usage: %s <file>\n", argv[0] );
        return EXIT_FAILURE;
    }

    mtrace();

    const char* filename = argv[1];

    char* file = readfile( filename );
    if ( file == 0 ) return EXIT_FAILURE;

    treenode_t* tree = parse( file );
    if ( tree == 0 ) {
        fprintf( stderr, "parsing failed!\n" );
        return EXIT_FAILURE;
    }

    dump_tree_node( tree, 0 );


    muntrace();

    return EXIT_SUCCESS;
}

