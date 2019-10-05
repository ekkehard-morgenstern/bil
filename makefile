############################################################################################
##    Basic Implementation Language (BIL)                                                 ##
##    Copyright (C) 2019  Ekkehard Morgenstern                                            ##
##                                                                                        ##
##    This program is free software: you can redistribute it and/or modify                ##
##    it under the terms of the GNU General Public License as published by                ##
##    the Free Software Foundation, either version 3 of the License, or                   ##
##    (at your option) any later version.                                                 ##
##                                                                                        ##
##    This program is distributed in the hope that it will be useful,                     ##
##    but WITHOUT ANY WARRANTY; without even the implied warranty of                      ##
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       ##
##    GNU General Public License for more details.                                        ##
##                                                                                        ##
##    You should have received a copy of the GNU General Public License                   ##
##    along with this program.  If not, see <https://www.gnu.org/licenses/>.              ##
##                                                                                        ##
##    Contact Info:                                                                       ##
##    E#Mail: ekkehard@ekkehardmorgenstern.de                                             ##
##    Mail: Ekkehard Morgenstern, Mozartstr. 1, 76744 Woerth am Rhein, Germany, Europe    ##
############################################################################################


# change this to the path of the EBNF compiler
# (project can be found at https://github.com/ekkehard-morgenstern/ebnfcomp )
EBNFCOMP=../ebnfcomp/ebnfcomp

CC=gcc


.SUFFIXES: .ebnf

ifdef DEBUG
CFLAGS=-g
else
CFLAGS=-O3
endif
CFLAGS+= -Wall

.c.o:
	$(CC) -c $(CFLAGS) $<


all: testparser
	echo ok >all

parsingtable.c:	bil.ebnf
	$(EBNFCOMP) <bil.ebnf >parsingtable.c

TESTPARSER_MODULES=testparser.o parser.o

testparser: $(TESTPARSER_MODULES)
	$(CC) $(CFLAGS) -o $@ $(TESTPARSER_MODULES)

parser.o: parser.c parsingtable.c nodetype2text.c parser.h

testparser.o: testparser.c parser.h

nodetype2text.c: parsingtable.c gennodetype2text
	./gennodetype2text >nodetype2text.c

gennodetype2text: gennodetype2text.o
	$(CC) $(CFLAGS) -o $@ $<

