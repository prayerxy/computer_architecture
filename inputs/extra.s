	.text
main:   
        addiu $v0, $zero, 0xa
        addiu $3,$zero,0xFFFF #-1
        #补充slti指令
        slti $4,$3,0xFFFE
        addiu $5,$zero,4
        #补充sltiu指令
        sltiu $6,$5,10
        addiu $7,$zero,0x1234
        #补充sllv
        lui $8,0x8376
        addu $7,$7,$8
        sllv $9,$7,$5
        #补充srlv
        srlv $10,$7,$5
        #补充srav 符号扩展右移
        srav $11,$7,$5
        #补充sub
        lui $13,0x7FFF
        sub $14,$13,$7
        subu $15,$13,$7
        slt $16,$7,$13
        sltu $17,$7,$13
        syscall