        # mult instructions
        # your basic functionality.
        # No overflow exceptions should occur
	.text
main:   
        addiu $v0, $zero, 0xa
        addiu  $3, $zero, 100
        addiu  $4, $zero, 3
        #无符号除法
        divu $3,$4
        mfhi $11
        mflo $12
        addiu  $3,$zero,0xFFFE  #-2
        addiu  $4,$zero,0xFFFF  #-1
        #有符号除法
        div $3,$4
        mfhi $13
        mflo $14
        lui  $5,0x3FFF
        addiu $6,$zero,9
        #无符号乘法
        multu $5,$6
        mfhi $15
        mflo $16
        addiu $7,$zero,0xFFFB  #-5
        addiu $8,$zero,0xFFFF
        #有符号乘法
        mult $7,$8
        mfhi $17
        mflo $18
        mthi $5
        mtlo $6
        syscall