; valid1.as - arithmetic, load/store, a branch, and a data directive
MAIN: add $1, $2, $3
 sub $4, $1, $3
 addi $5, 10, $6
LOOP: sw $0, 4, $6
 lw $0, 4, $5
 bne $5, $6, LOOP
 hlt
NUM: .dw 100, -50
