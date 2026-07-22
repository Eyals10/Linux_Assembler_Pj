; valid2.as - macros, entry/extern, and a string
.entry START
.extern helper

mcro PRINT_SETUP
 la MSG
 call helper
mcroend

START: add $1, $1, $2
 PRINT_SETUP
 jmp DONE
DONE: hlt
MSG: .asciz "hi"
