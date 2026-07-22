;file3.as
;sample source code
.entry NEXT
.extern wNumber
STR: .asciz "aBcd"
MAIN: add $3,$5,$9
hello mcro GEN_MC
LOOP: ori $9,-5,$2
 la val1
 mcroend
 mcro GEN_MCR2
 jmp NEXT
 mcroend hi
NEXT: move $20,$4
LIST: .db 6,-9
 bgt $4,$2,END
 mcro bne
 la K
 sw $0,4,$10
 mcroend
 bne $31,$9, LOOP
 call val1
 mcro asciz
 jmp $4
 la wNumber
 mcroend
.extern val1
 .dh 27056
K: .dw 31,-12
END hlt
.entry K
