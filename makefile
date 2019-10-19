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
CFLAGS+= -Wall -pthread

.c.o:
	$(CC) -c $(CFLAGS) $<


all: testparser memtest1 testbasedlist
	echo ok >all

parsingtable.c:	bil.ebnf separateparsingtable
	$(EBNFCOMP) <bil.ebnf >parsingtable.c
	./separateparsingtable

TESTPARSER_MODULES=testparser.o parser.o
MEMTEST1_MODULES=memtest1.o handlespace.o usermemory.o basedlists.o
MEMTEST1_INCLUDES=types.h basedlists.h handlespace.h usermemory.h
TESTBASEDLIST_MODULES=testbasedlist.o basedlists.o
TESTBASEDLIST_INCLUDES=basedlists.h types.h
LIBS=-lrt -lm

testparser: $(TESTPARSER_MODULES)
	$(CC) $(CFLAGS) -o $@ $(TESTPARSER_MODULES)

parser.o: parser.c parsingtable.c nodetype2text.c parser.h

testparser.o: testparser.c parser.h

nodetype2text.c: parsingtable.c gennodetype2text
	./gennodetype2text >nodetype2text.c

gennodetype2text: gennodetype2text.o
	$(CC) $(CFLAGS) -o $@ $<

separateparsingtable: separateparsingtable.o
	$(CC) $(CFLAGS) -o $@ $<

memtest1: $(MEMTEST1_MODULES)
	$(CC) $(CFLAGS) -o $@ $(MEMTEST1_MODULES) $(LIBS)

memtest1.o: memtest1.c $(MEMTEST1_INCLUDES)

handlespace.o: handlespace.c $(MEMTEST1_INCLUDES)

usermemory.o: usermemory.c $(MEMTEST1_INCLUDES)

basedlists.o: basedlists.c $(MEMTEST1_INCLUDES)

testbasedlist: $(TESTBASEDLIST_MODULES)
	$(CC) $(CFLAGS) -o $@ $(TESTBASEDLIST_MODULES)

testbasedlist.o: testbasedlist.c $(TESTBASEDLIST_INCLUDES)


