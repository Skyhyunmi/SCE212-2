/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
    if(CURRENT_STATE.PC<MEM_TEXT_START||CURRENT_STATE.PC>MEM_TEXT_SIZE+MEM_TEXT_START) return;
    instruction *cur = get_inst_info(CURRENT_STATE.PC);
    if(cur->value!=CURRENT_STATE.PC) {
        RUN_BIT=FALSE;
        return;
    }
    CURRENT_STATE.PC+=4;
    
	switch(OPCODE(cur)){
        //TYPE I
        case 0x9:		//(0x001001)ADDIU t s imm
        CURRENT_STATE.REGS[RT(cur)]=CURRENT_STATE.REGS[RS(cur)]+SIGN_EX(IMM(cur));
        break;
        
        case 0xc:		//(0x001100)ANDI t s imm
        CURRENT_STATE.REGS[RT(cur)]=CURRENT_STATE.REGS[RS(cur)]&IMM(cur);
        break;
        
        case 0xd:		//(0x001101)ORI t s imm
        CURRENT_STATE.REGS[RT(cur)]=CURRENT_STATE.REGS[RS(cur)]|IMM(cur);
        break;
        
        case 0xf:		//(0x001111)LUI	t imm
        CURRENT_STATE.REGS[RT(cur)]=IMM(cur)<<16; 
        break;
        
        case 0xb:		//(0x001011)SLTIU t s imm
        CURRENT_STATE.REGS[RT(cur)]=CURRENT_STATE.REGS[RS(cur)]<SIGN_EX(IMM(cur))?1:0;
        break;
        
        case 0x23:		//(0x100011)LW t s imm
        CURRENT_STATE.REGS[RT(cur)]=mem_read_32(CURRENT_STATE.REGS[RS(cur)]+SIGN_EX(IMM(cur)));
        break;
        
        case 0x2b:		//(0x101011)SW t s imm
        mem_write_32(CURRENT_STATE.REGS[RS(cur)]+SIGN_EX(IMM(cur)),CURRENT_STATE.REGS[RT(cur)]);
        break;
        
        case 0x4:		//(0x000100)BEQ t s imm
        if(CURRENT_STATE.REGS[RT(cur)] == CURRENT_STATE.REGS[RS(cur)])
           CURRENT_STATE.PC+=IMM(cur)<<2;
        break;
        
        case 0x5:		//(0x000101)BNE t s imm
        if(CURRENT_STATE.REGS[RT(cur)]!= CURRENT_STATE.REGS[RS(cur)])
            CURRENT_STATE.PC+=IMM(cur)<<2;
        break;

        //TYPE R
        case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
        switch(FUNC(cur)){
            case 0x21: //ADDU
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RS(cur)]+CURRENT_STATE.REGS[RT(cur)];
            break;

            case 0x24: //AND
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RS(cur)]&CURRENT_STATE.REGS[RT(cur)];
            break;

            case 0x27: //NOR
            CURRENT_STATE.REGS[RD(cur)]=~(CURRENT_STATE.REGS[RS(cur)]|CURRENT_STATE.REGS[RT(cur)]);
            break;

            case 0x25: //OR
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RS(cur)]|CURRENT_STATE.REGS[RT(cur)];
            break;
            
            case 0x2b: //SLTU
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RS(cur)]<CURRENT_STATE.REGS[RT(cur)]?1:0;
            break;
            
            case 0x00: //SLL
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RT(cur)]<<SHAMT(cur);
            break;
            
            case 0x02: //SRL
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RT(cur)]>>SHAMT(cur);
            break;
            
            case 0x23: //SUBU
            CURRENT_STATE.REGS[RD(cur)]=CURRENT_STATE.REGS[RS(cur)]-CURRENT_STATE.REGS[RT(cur)];
            break;

            case 0x08: //JR
            CURRENT_STATE.PC=CURRENT_STATE.REGS[RS(cur)];
            break;
        }
        break;

        //TYPE J
        case 0x2:		//(0x000010)J
        JUMP_INST((TARGET(cur)<<2));
        break;

        case 0x3:		//(0x000011)JAL
        CURRENT_STATE.REGS[31]=CURRENT_STATE.PC+4;
        JUMP_INST((TARGET(cur)<<2));
        break;

        default:
        break;
    }
}
