;file file2.as
;sample source code
.entry NEXT
.extern wNumber
STR: .asciz "aBcd"
MAIN: add $3,$5,$9
LOOP: ori $9,-5,$2
mcro GEN_MC
 la val1
 jmp NEXT
 mcroend
NEXT: move $20,$4
LIST: .db 6,-9
GEN_MC
GEN_MC
mcro GEN_MC2
 sw $0,4,$10
 jmp NEXT
 mcroend
 bgt $4,$2,END
 la K
 GEN_MC2
 sw $0,4,$10
 bne $31,$9, LOOP
 call val1
 jmp $4
 la wNumber
.extern val
 .dh 27056
K: .dw 31,-12
END hlt
.entry K
