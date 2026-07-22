;file4.as
;sample source code
.entry NAXT
.extern wNumber
STR: .asciz "aBcd
MAIN: add $3,$5,,$9
LOOP: ori $9,-5,$2,
 la val1,$3
 jump NIXT
NEXT: move $20
LIST: .db 6,-9,,4
LIST2: .db 6.5,-9,4
 bgt ,$4,$2,END
 lala K
 sw $0,4,$10,$5
 bne $31,$9, LOOOP
 call val
 jmp $4
 la $4
.extern val1
 .dt 27056
K: .dw 31,-12
END hlt
.entry K
