# bil

Basic Implementation Language (BIL)

This is a Basic Implementation Language (called BIL), that should have a small footprint.

The project is still in a very early stage, so don't expect it to be usable yet. I will add stuff as time progresses.

It's based on elements from programming languages like C, Pascal, BASIC, BCPL and PHP.

The syntax analyzer should already be working correctly, but I haven't tested it with many input files yet.

I'm not sure yet whether it will create C or C++ output or even assembly language (originally, I wanted it to create halfways portable output).

Building BIL makes use of my EBNF compiler, which can be found on my Github page. If necessary, change the path in the makefile to the correct executable.

To test the syntax analyzer, all you have to do currently is to type "./testparser test1.bil" or other input file.

There might still be bugs in the analyzer, so if you notice any, please notify me. I also trust that my EBNF compiler generates the parsing table without error. If that isn't the case, please notify me.
