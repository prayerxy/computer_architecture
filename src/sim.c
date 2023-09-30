#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    // 定义所需要的变量
    uint32_t instruction_val; //存储获取到的指令
    uint32_t opcode; //是我们指令的op字段z
    uint32_t jump_target;//跳转指令获取的值，设置为32位
    //注意这里要设置成16位，便于符号扩展成32位
    uint16_t immediate,offset;//立即数字段 偏移 branch指令 load等用到。之后经常会符号扩展
    int32_t e_immediate;//符号扩展后
    uint32_t branch_target;//branch指令会用到
    uint32_t rs,rt,rd,sa,funct;//funct是R型系列指令的标识符
    uint32_t base,virt_addr,flag_inst;  //base,virt_addr在lb中
    //flag_inst在rgimm系列指令中作为标识符
    int32_t tmp;//中间数 有符号
    uint32_t R_jump=0; //R型指令中jump标识符
    //32-bit 2's complement 要用int 是有符号数
    int64_t mult;
    //无符号乘法用到
    uint64_t mult_u;

    instruction_val=mem_read_32(CURRENT_STATE.PC); //获取当前指令，在内存中取出
	 
	opcode=(instruction_val&0xFC000000)>>26;  //instruction_val的高6位
    //开始获取指令进行相关处理
    switch(opcode){
        case OP_J:
            jump_target=(instruction_val&0x03FFFFFF)<<2;
            NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|jump_target;
            break;
        
        //与j的区别是要把一个地址放进去
        case OP_JAL:
            jump_target=(instruction_val&0x03FFFFFF)<<2;
            NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|jump_target;
            NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;//没有delay slot直接加4
            break;

        //这里要把target符号扩展，采取右移的方式
        case OP_BEQ:
            offset=instruction_val&0x0000FFFF;
            branch_target=(int32_t)(offset<<16)>>14;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            //先算出对应的索引，在regs里面取
            if(CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt]){
                NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
            }
            else{
                NEXT_STATE.PC=NEXT_STATE.PC+4;//下一条指令
            }
            break;

        //与上面的beq类似，只是现在是rs!=rt
        case OP_BNE:
            offset=instruction_val&0x0000FFFF;
            branch_target=(int32_t)(offset<<16)>>14;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            if(CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt]){
                NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
            }
            else{
                NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            break;

        //branch on less than or equal to zero
        case OP_BLEZ:
            offset=instruction_val&0x0000FFFF;
            branch_target=(int32_t)(offset<<16)>>14;
            rs=(instruction_val&0x03E00000)>>21;
            if((CURRENT_STATE.REGS[rs]&0x80000000)||(CURRENT_STATE.REGS[rs]==0)){
                NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
            }
            else{
                NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            break;

        case OP_BGTZ:
            offset=instruction_val&0x0000FFFF;
            branch_target=(int32_t)(offset<<16)>>14;
            rs=(instruction_val&0x03E00000)>>21;
            if((CURRENT_STATE.REGS[rs]&0x80000000)==0&&(CURRENT_STATE.REGS[rs]!=0)){   //这里出过错 (CURRENT_STATE.REGS[rs]&0x80000000)==0一定加括号
                NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
            }
            else{
                NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            break;
        
        //这里的立即数同样要符号扩展
        case OP_ADDI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            tmp=(int32_t)CURRENT_STATE.REGS[rs]+((int32_t)(immediate<<16)>>16);
            //overflow没有发生
            //负数加正数肯定不会溢出
            if((CURRENT_STATE.REGS[rs]&0x80000000)^(((int32_t)(immediate<<16)>>16)&0x80000000)){
                NEXT_STATE.REGS[rt]=tmp;
            }
            //符号一致，判断相加后最高位与一个最高位是否相同
            else if(!((CURRENT_STATE.REGS[rs]&0x80000000)^(((int32_t)(immediate<<16)>>16)&0x80000000))&&!((tmp&0x80000000)^(CURRENT_STATE.REGS[rs]&0x80000000))){
                NEXT_STATE.REGS[rt]=tmp;
            }
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        //不会关注溢出
        case OP_ADDIU:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+((int32_t)(immediate<<16)>>16);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        //操作数被看成有符号数
        case OP_SLTI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            e_immediate=(int32_t)(immediate<<16)>>16; //符号扩展
            if((CURRENT_STATE.REGS[rs]&0x80000000)&&!(e_immediate&0x80000000)){
                NEXT_STATE.REGS[rt]=1;
            }
            else if(!(CURRENT_STATE.REGS[rs]&0x80000000)&&(e_immediate&0x80000000)){
                NEXT_STATE.REGS[rt]=0;
            }
            else{
                //这里相减要用int32 tmp也是int而不是uint，在后面比大小用到
                tmp=(int32_t)CURRENT_STATE.REGS[rs]-e_immediate;
                if(tmp<0){
                    NEXT_STATE.REGS[rt]=1;
                }
                else{
                    NEXT_STATE.REGS[rt]=0;
                }
            }
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        //操作数为无符号数
        case OP_SLTIU:
            immediate=instruction_val&0x0000FFFF;//符号扩展
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            e_immediate=(int32_t)(immediate<<16)>>16;
            if(CURRENT_STATE.REGS[rs]<(uint32_t)e_immediate){
                NEXT_STATE.REGS[rt]=1;
            }
            else{
                NEXT_STATE.REGS[rt]=0;
            }
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        case OP_ANDI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]&(immediate&0x0000FFFF);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        case OP_ORI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]|(immediate&0x0000FFFF);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        case OP_XORI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]^(immediate&0x0000FFFF);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;

        case OP_LUI:
            immediate=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            NEXT_STATE.REGS[rt]=immediate<<16;//注意这里imm是16位，移位之后是32位 无符号扩展
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //load byte
        case OP_LB:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);
            NEXT_STATE.REGS[rt]=(int32_t)(((int8_t)mem_read_32(virt_addr))<<24)>>24;//符号扩展
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //load halfword
        case OP_LH:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);
            NEXT_STATE.REGS[rt]=(int32_t)((int16_t)mem_read_32(virt_addr)<<16)>>16;//符号扩展
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //load word
        case OP_LW:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);
            NEXT_STATE.REGS[rt]=mem_read_32(virt_addr); //32位是一个字
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        case OP_LBU:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);//这里offset移位由16位符号扩展为变为32
            //无符号扩展
            NEXT_STATE.REGS[rt]=((mem_read_32(virt_addr)&0x000000FF));//前24位是0 只要后8位
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
		    break;
        case OP_LHU:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);//这里offset移位由16位符号扩展为变为32
            NEXT_STATE.REGS[rt]=mem_read_32(virt_addr)&0x0000FFFF;//无符号扩展 前16位是0
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //store byte
        case OP_SB:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);//这里offset移位由16位符号扩展为变为32
            //使用mem_write函数写
            mem_write_32(virt_addr,(uint8_t)CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //store halfword
        case OP_SH:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);//这里offset移位由16位符号扩展为变为32
            mem_write_32(virt_addr,(uint16_t)CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //store word
        case OP_SW:
            offset=instruction_val&0x0000FFFF;
            base=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            virt_addr=CURRENT_STATE.REGS[base]+((int32_t)(offset<<16)>>16);//这里offset移位由16位符号扩展为变为32
            mem_write_32(virt_addr,(uint32_t)CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
            break;
        //bltz bgez bltzal bgezal
        case OP_REGIMM:
            offset=instruction_val&0x0000FFFF;
            rs=(instruction_val&0x03E00000)>>21;
            flag_inst=(instruction_val&0x001F0000)>>16;
            branch_target=(int32_t)(offset<<16)>>14;//高14位符号扩展+16位offset+2位0
            switch (flag_inst)
            {   // branch on less than zero
                case rt_BLTZ:
                    if(CURRENT_STATE.REGS[rs]&0x80000000){
                        NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
                    }
                    else{
                        NEXT_STATE.PC=CURRENT_STATE.PC+4;
                    }
                    break;
                // branch on greater than/equal to zero 
                case rt_BGEZ:
                    if((CURRENT_STATE.REGS[rs]&0x80000000)==0x00000000){
                        NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
                    }
                    else{
                        NEXT_STATE.PC=CURRENT_STATE.PC+4;
                    }
                    break;
                //...and link
                case rt_BLTZAL:
                    if(CURRENT_STATE.REGS[rs]&0x80000000){
                        NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
                    }
                    else{
                        NEXT_STATE.PC=CURRENT_STATE.PC+4;
                    }
                    //这里模拟没有delay slot所以+8改成+4
                    NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
                    break;
                //..and link
                case rt_BGEZAL:
                    if((CURRENT_STATE.REGS[rs]&0x80000000)==0x00000000){
                        NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
                    }
                    else{
                        NEXT_STATE.PC=CURRENT_STATE.PC+4;
                    }
                    NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
                    break;

                default:
                    printf("Illegal instruction!");
                    break;
            }
            break;
        //R型指令系列 OP字段相同
        case OP_SPECIAL:
            rs=(instruction_val&0x03E00000)>>21;
            rt=(instruction_val&0x001F0000)>>16;
            rd=(instruction_val&0x0000F800)>>11;
            sa=(instruction_val&0x000007C0)>>6;
            funct=(instruction_val&0x0000003F);//获取funct字段
            switch (funct)
            {
                case funct_SLL:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<sa;
                    break;
                case funct_SRL:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>sa;//前面是sa个0
                    break;

                case funct_SRA:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>sa;
                    break;
                case funct_SLLV:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<(CURRENT_STATE.REGS[rs]&0x0000001F);//rs的低5位
                    break;
                case funct_SRLV:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>(CURRENT_STATE.REGS[rs]&0x0000001F);
                    break;
                case funct_SRAV:
                    NEXT_STATE.REGS[rd]=(int32_t)CURRENT_STATE.REGS[rt]>>(CURRENT_STATE.REGS[rs]&0x0000001F);//符号扩展
                    break;
                case funct_JR:
                    NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
                    R_jump=1;
                    break;
                case funct_JALR:
                    //同样由于没有delay，加4，不是加8
                    NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.PC+4;
                    R_jump=1;
                    break;
                case funct_ADD:
                    tmp=(int32_t)CURRENT_STATE.REGS[rs]+(int32_t)CURRENT_STATE.REGS[rt];//tmp是int有符号
                    //overflow没有发生
                    //符号不同肯定不会溢出
                    if((CURRENT_STATE.REGS[rs]&0x80000000)^(CURRENT_STATE.REGS[rt]&0x80000000)){
                        NEXT_STATE.REGS[rd]=tmp;
                    }
                    //符号一致，判断相加后最高位与一个最高位是否相同
                    else if(!((CURRENT_STATE.REGS[rs]&0x80000000)^(CURRENT_STATE.REGS[rt]&0x80000000))&&!((tmp&0x80000000)^(CURRENT_STATE.REGS[rs]&0x80000000))){
                        NEXT_STATE.REGS[rd]=tmp;
                    }
                    break;
                case funct_ADDU:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+CURRENT_STATE.REGS[rt];
                    break;
                case funct_SUB:
                    //同样要考虑溢出的问题
                    tmp=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
                    //符号相同相减不会溢出
                    if(!((CURRENT_STATE.REGS[rs]&0x80000000)^(CURRENT_STATE.REGS[rt]&0x80000000))){
                        NEXT_STATE.REGS[rd]=tmp;
                    }
                    //符号不同，判断相减完后rs与tmp的符号，相同说明没有溢出
                    else if(((CURRENT_STATE.REGS[rs]&0x80000000)^(CURRENT_STATE.REGS[rt]&0x80000000))&&!((tmp&0x80000000)^(CURRENT_STATE.REGS[rs]&0x80000000))){
                        NEXT_STATE.REGS[rd]=tmp;
                    }
                    break;
                case funct_SUBU:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
                    break;
                case funct_AND:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]&CURRENT_STATE.REGS[rt];
                    break;
                case funct_OR:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt];
                    break;
                case funct_XOR:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]^CURRENT_STATE.REGS[rt];
			        break;
                //not or运算
                case funct_NOR:
                    NEXT_STATE.REGS[rd]=~(CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt]);
                    break;
                case funct_SLT:
                    tmp=(int32_t)((int32_t)CURRENT_STATE.REGS[rs]-(int32_t)CURRENT_STATE.REGS[rt]);
                    if(tmp<0){
                        NEXT_STATE.REGS[rd]=1;
                    }
                    else{
                        NEXT_STATE.REGS[rd]=0;
                    }
                    break;
                case funct_SLTU:
                    if((uint32_t)(CURRENT_STATE.REGS[rs])<(uint32_t)(CURRENT_STATE.REGS[rt])){
                        NEXT_STATE.REGS[rd]=0x00000001;
                    }
                    else{
                        NEXT_STATE.REGS[rd]=0;
                    }
                    break;
                       
                case funct_MULT:
                    //先转int32再转int64
                    uint64_t temp = (int64_t)(int32_t)CURRENT_STATE.REGS[rs] * (int64_t)(int32_t)CURRENT_STATE.REGS[rt];
                    NEXT_STATE.LO = (uint32_t)temp;
                    NEXT_STATE.HI = (uint32_t)(temp>>32);
                    break;
                case funct_MFHI:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.HI;
                    break;
                //move from lo
                case funct_MFLO:
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.LO;
                //move to hi
                case funct_MTHI:
                    NEXT_STATE.HI=CURRENT_STATE.REGS[rs];
                    break;
                case funct_MTLO:
                    NEXT_STATE.LO=CURRENT_STATE.REGS[rs];
                    break;
                //在mult上，把操作数当成无符号数
                case funct_MULTU:
                    mult_u=(uint64_t)CURRENT_STATE.REGS[rs]*(uint64_t)CURRENT_STATE.REGS[rt];
                    NEXT_STATE.HI=(uint32_t)(mult_u>>32);
                    NEXT_STATE.LO=(uint32_t)(mult_u&0x00000000FFFFFFFF);
                    break;
                case funct_DIV:
                    NEXT_STATE.HI=((int32_t)CURRENT_STATE.REGS[rs]%(int32_t)CURRENT_STATE.REGS[rt]);
			        NEXT_STATE.LO=((int32_t)CURRENT_STATE.REGS[rs]/(int32_t)CURRENT_STATE.REGS[rt]);
                    break;
                //无符号数 默认是无符号
                case funct_DIVU:
                    NEXT_STATE.HI=(CURRENT_STATE.REGS[rs]%CURRENT_STATE.REGS[rt]);
			        NEXT_STATE.LO=(CURRENT_STATE.REGS[rs]/CURRENT_STATE.REGS[rt]);
                    break;
                case funct_SYSCALL:
                    if(CURRENT_STATE.REGS[2]==0x0A){
                        RUN_BIT=FALSE;
                    }
                    R_jump=1;//不再继续下一条指令
                    break;
            
                default:
                    break;
            }
            if(R_jump==0){
                NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            break;
        
        default:
            printf("Illegal instruction!");
            break;


        
        

    }
}
