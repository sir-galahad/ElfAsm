# ElfAsm
Assembler for the 1802 processor

I am intentionally going into this project somewhat blind as far as how to build an assembler and how other 1802 assemblers work. It's very likely my approach here will be "wrong" and incompatible with other 1802 assemblers. In particular I don't know what syntax other 1802 assemblers use so I'm going my own way.

## Notes on syntax:
Numbers can be written as either decimal numbers or hexidecimal numbers prefixed with 0x or 0X.

Registers the documentation I've seen for the 1802's 16 registers lists them as R0, R1, R2... unfortunately that doesn't tell me if the higher registers are R10-R15 or RA-RF for ElfAsm those registers will be specified as RA-RF (case insensitive).

Labels can be specified by a name starting with a letter after the first character, and ending with a colon ':' letters, digits, and the underscore character are valid for labels. When the assember runs the label will become a symbol for a 16 bit address, when used as an argument the label name can be used alone as an argument for long branchs, or prefixed by ^ or _ to use just the high or low byte respectively.

## Current state
At the time of writing only 1802 hardware opcodes and the creation of flat binaries are supported. Next changes will be to add ORG, DB, and RESB directives. Also I've approached parsing with a "fast and loose" method that should probably be made much more rigorous

## You can't trust me
I reserve the right to make changes that break backwards compatibility at any time
