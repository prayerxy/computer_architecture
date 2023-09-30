        # Basic branch test
	.text

main:
        addiu $v0, $zero, 0xa
l_0:    
        j l_1
l_1:
        bne $zero, $zero, l_3
l_2:
        beq $zero, $zero, l_4
        addiu $7, $zero, 0x347
        syscall
l_3:
	addiu $7, $zero, 0x1337
        # Should not reach here
l_4:
        addiu $7, $zero, 0xd00d
        syscall
        
         
        
User Text Segment [00400000]..[00440000]
[00400000] 2402000a  addiu $2, $0, 10         ; 5: addiu $v0, $zero, 0xa 
[00400004] 08100002  j 0x00400008 [l_1]       ; 7: j l_1 
[00400008] 14000004  bne $0, $0, 16 [l_3-0x00400008]; 9: bne $zero, $zero, l_3 
[0040000c] 10000004  beq $0, $0, 16 [l_4-0x0040000c]; 11: beq $zero, $zero, l_4 
[00400010] 24070347  addiu $7, $0, 839        ; 12: addiu $7, $zero, 0x347 
[00400014] 0000000c  syscall                  ; 13: syscall 
[00400018] 24071337  addiu $7, $0, 4919       ; 15: addiu $7, $zero, 0x1337 
[0040001c] 3407d00d  ori $7, $0, -12275       ; 18: addiu $7, $zero, 0xd00d 
[00400020] 0000000c  syscall                  ; 19: syscall 
