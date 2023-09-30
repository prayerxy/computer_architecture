        # Basic LW/SW test
	.text
main:
        #;;  Set a base address
        lui    $3, 0x1000

        addiu  $5, $zero, 255
        add    $6, $5, $5
        add    $7, $6, $6
        addiu  $8, $7, 30000
        
        #;; Place a test pattern in memory
        sw     $5, 0($3)
        sw     $6, 4($3)
        sw     $7, 8($3)
        sw     $8, 12($3)

        lw     $9,  0($3)
        lw     $10, 4($3)
        lw     $11, 8($3)
        lw     $12, 12($3)

        addiu  $3, $3, 4
        sw     $5, 0($3)
        sw     $6, 4($3)
        sw     $7, 8($3)
        sw     $8, 12($3)

        lw     $13,  -4($3)
        lw     $14,  0($3)
        lw     $15,  4($3)
        lw     $16,  8($3)
               
        #;; Calculate a "checksum" for easy comparison
        add    $17, $zero, $9
        add    $17, $17, $10
        add    $17, $17, $11
        add    $17, $17, $12
        add    $17, $17, $13
        add    $17, $17, $14
        add    $17, $17, $15
        add    $17, $17, $16
        
        #;;  Quit out 
        addiu $v0, $zero, 0xa
        syscall
        

[00400000] 3c031000  lui $3, 4096             ; 5: lui $3, 0x1000 
[00400004] 240500ff  addiu $5, $0, 255        ; 7: addiu $5, $zero, 255 
[00400008] 00a53020  add $6, $5, $5           ; 8: add $6, $5, $5 
[0040000c] 00c63820  add $7, $6, $6           ; 9: add $7, $6, $6 
[00400010] 24e87530  addiu $8, $7, 30000      ; 10: addiu $8, $7, 30000 
[00400014] ac650000  sw $5, 0($3)             ; 13: sw $5, 0($3) 
[00400018] ac660004  sw $6, 4($3)             ; 14: sw $6, 4($3) 
[0040001c] ac670008  sw $7, 8($3)             ; 15: sw $7, 8($3) 
[00400020] ac68000c  sw $8, 12($3)            ; 16: sw $8, 12($3) 
[00400024] 8c690000  lw $9, 0($3)             ; 18: lw $9, 0($3) 
[00400028] 8c6a0004  lw $10, 4($3)            ; 19: lw $10, 4($3) 
[0040002c] 8c6b0008  lw $11, 8($3)            ; 20: lw $11, 8($3) 
[00400030] 8c6c000c  lw $12, 12($3)           ; 21: lw $12, 12($3) 
[00400034] 24630004  addiu $3, $3, 4          ; 23: addiu $3, $3, 4 
[00400038] ac650000  sw $5, 0($3)             ; 24: sw $5, 0($3) 
[0040003c] ac660004  sw $6, 4($3)             ; 25: sw $6, 4($3) 
[00400040] ac670008  sw $7, 8($3)             ; 26: sw $7, 8($3) 
[00400044] ac68000c  sw $8, 12($3)            ; 27: sw $8, 12($3) 
[00400048] 8c6dfffc  lw $13, -4($3)           ; 29: lw $13, -4($3) 
[0040004c] 8c6e0000  lw $14, 0($3)            ; 30: lw $14, 0($3) 
[00400050] 8c6f0004  lw $15, 4($3)            ; 31: lw $15, 4($3) 
[00400054] 8c700008  lw $16, 8($3)            ; 32: lw $16, 8($3) 
[00400058] 00098820  add $17, $0, $9          ; 35: add $17, $zero, $9 
[0040005c] 022a8820  add $17, $17, $10        ; 36: add $17, $17, $10 
[00400060] 022b8820  add $17, $17, $11        ; 37: add $17, $17, $11 
[00400064] 022c8820  add $17, $17, $12        ; 38: add $17, $17, $12 
[00400068] 022d8820  add $17, $17, $13        ; 39: add $17, $17, $13 
[0040006c] 022e8820  add $17, $17, $14        ; 40: add $17, $17, $14 
[00400070] 022f8820  add $17, $17, $15        ; 41: add $17, $17, $1 
        