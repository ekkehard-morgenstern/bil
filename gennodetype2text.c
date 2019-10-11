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
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

int main( int argc, char** argv ) {
    FILE* fp = fopen( "parsingtable.h", "rt" );
    if ( fp == 0 ) return EXIT_FAILURE;
    char buf[512]; bool indecl = false;
    printf( "%s",
        "static const char* nodetype2text( int nodetype ) {\n"
        "    switch ( nodetype ) {\n"
    );
    while ( fgets( buf, sizeof(buf), fp ) ) {
        if ( strncmp( buf, "typedef enum _nodetype_t {", 26 ) == 0 ) {
            indecl = true;
            continue;
        }
        if ( strncmp( buf, "} nodetype_t;", 13 ) == 0 && indecl ) {
            indecl = false;
            continue;
        }
        if ( !indecl ) continue;
        char name[100];
        if ( sscanf( buf, "%*[ \t]%99[_A-Za-z0-9]", name ) >= 1 ) {
            printf( "        case %s: return \"%s\";\n", name, name );
        } else {
            fprintf( stderr, "? error: '%s'\n", buf );
            return EXIT_FAILURE;
        }
    }
    printf( "%s",
        "        default: break;\n"
        "    }\n"
        "    return \"???\";\n"
        "}\n\n"
    );
    fclose( fp );
    return EXIT_SUCCESS;
}