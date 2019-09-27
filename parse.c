/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
    char opbuf[7],stdbuf[6],funcbuf[7],immbuf[17],addbuf[27];
    mem_write_32(MEM_TEXT_START + index,strtol(buffer,NULL,2));
    instr.value=MEM_TEXT_START+index;
    strncpy(opbuf,buffer,6*sizeof(char));
    instr.opcode=(short)strtol(opbuf,NULL,2);
    switch(instr.opcode){
        //TYPE I
        case 0x9:		//(0x001001)ADDIU t s imm
        case 0xc:		//(0x001100)ANDI t s imm
        case 0xd:		//(0x001101)ORI t s imm
        case 0xf:		//(0x001111)LUI	t imm
        case 0xb:		//(0x001011)SLTIU t s imm
        case 0x23:		//(0x100011)LW t s imm
        case 0x2b:		//(0x101011)SW t s imm
        case 0x4:		//(0x000100)BEQ t s imm
        case 0x5:		//(0x000101)BNE t s imm
        strncpy(stdbuf,buffer+6,5*sizeof(char));//s
        instr.r_t.r_i.rs=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(stdbuf,buffer+11,5*sizeof(char));//t
        instr.r_t.r_i.rt=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(immbuf,buffer+16,16*sizeof(char));//imm
        instr.r_t.r_i.r_i.imm=(short)strtol(immbuf,NULL,2);
        break;

        //TYPE R
        case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
        strncpy(stdbuf,buffer+6,5*sizeof(char));//s
        instr.r_t.r_i.rs=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(stdbuf,buffer+11,5*sizeof(char));//t
        instr.r_t.r_i.rt=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(stdbuf,buffer+16,5*sizeof(char));//d
        instr.r_t.r_i.r_i.r.rd=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(stdbuf,buffer+21,5*sizeof(char));//shamt
        instr.r_t.r_i.r_i.r.shamt=(unsigned char)strtol(stdbuf,NULL,2);

        strncpy(funcbuf,buffer+26,6*sizeof(char));//funct
        instr.func_code=(short)strtol(funcbuf,NULL,2);
        break;

        //TYPE J
        case 0x2:		//(0x000010)J
        case 0x3:		//(0x000011)JAL
        strncpy(addbuf,buffer+6,26*sizeof(char));//addr
        instr.r_t.target= strtol(addbuf,NULL,2);
        break;

        default:
            printf("Not available instruction\n");
            assert(0);
    }
    return instr;
}

void parsing_data(const char *buffer, const int index)
{
    mem_write_32(MEM_DATA_START+index,strtol(buffer,NULL,2));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
