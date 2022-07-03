#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compression.h"
#include "utils.h"

/* Your code here... */
bool check_compressible(RISC_V* riscv) {
    unsigned int rd, rs1, rs2;  /* declare the type of pieces */
    int imm;
    if (riscv->ins_type == other_instr) {
        return false;   /* other_instruction type must be uncompressible */
    }
    if (riscv->format == R) {   /* R case */
        rd = riscv->m_code[1];      /* rd [11:7] */
        rs1 = riscv->m_code[3];     /* rs1 [19:15] */
        rs2 = riscv->m_code[4];     /* rs2 [24:20] */
        if (riscv->ins_type != add) {
            if ((!check_register(rd)) || (!check_register(rs1)) || (!check_register(rs2))) {  /* Check the register name */
                return false;   
            }
            if (rd == rs1) {
                /* ins xxx rd', rd', rs2' */
                return true;
            }
            return false;   /* directly return false */
        }
        /* add case */
        if (rd == 0 || rs2 == 0) {
            /* CONSTRAINTS rd≠x0; rs2≠x0 */
            return false;
        }
        if (rd == rs1 || rs1 == 0) {
            /* add rd, rd, rs2 | add rd, x0, rs2 */
            return true;
        }
    }
    else if (riscv->format == I) {  /* I case */
        rd = riscv->m_code[1];      /* rd [11:7] */
        rs1 = riscv->m_code[3];     /* rs1 [19:15] */
        imm = cal_i_imm(riscv);     /* imm [31:20] */
        if (riscv->ins_type == jalr) { /* jalr case */
            if (rs1 == 0) {
                /* CONSTRAINTS rs1≠x0 */
                return false;
            }
            if ((rd == 0 || rd == 1) && imm == 0) {
                /* jalr x0, 0 (rs1) | jalr x1, 0 (rs1) */
                return true;
            }
        }
        else if (riscv->ins_type == addi) {     /* addi case */
            if (imm > 31 || imm < -32) {     /* infomation loss when compression imm[5:0] */
                return false;
            }
            if (rd != 0 && rs1 == 0 ) {
                /* addi rd, x0, imm && CONSTRAINTS rd≠x0 */
                return true;
            }
            if (rd == rs1 && rd != 0 && imm != 0) {
                /* addi rd, rd, nzimm && rd≠x0; immediate is nonzero */
                return true;
            }
        }
        else if (riscv->ins_type == slli) {     /* slli case */
            if (rd == rs1 && rd != 0 && get_bits(imm,5,5) == 0) {
                /* slli rd, rd, shamt && rd≠x0; shamt[5] must be zero */
                return true;
            }
        }
        else if (riscv->ins_type == lw) {       /* lw case */
            /* lw rd', offset(rs1') */
            if (imm % 4 != 0 || imm > 127 || imm < 0) {    /* infomation loss when compression imm[6:2] */
                return false;
            }
            if (check_register(rd) && check_register(rs1)) {    /* Check register name */
               return true;
            }
        }
        else if (riscv->ins_type == srli || riscv->ins_type == srai) {      /* srli/srai case*/
            if (rd == rs1 && get_bits(imm,5,5) == 0 && check_register(rd)) {
                /* srai/srli rd', rd', shamt && shamt[5] must be zero */
                return true;
            }
        }
        else if (riscv->ins_type == andi) {     /* andi case */
            printf("imm: %d\n", imm);
            if (imm > 31 || imm < -32) {     /* information loss when compression imm[5:0] */
                return false;
            }
            if (rd == rs1 && check_register(rd)) {
                /* andi rd', rd', imm */
                return true;
            }
        }
    }
    else if (riscv->format == U) {  /* U lui case */
        rd = riscv->m_code[1];      /* rd [11:7] */
        imm = cal_u_imm(riscv);     /* imm [31:12] */
        printf("imm: %d\n", imm);
        if (imm > 31|| imm < -32) {    /* information loss whn compression imm[17:12] */
            return false;
        }
        if (rd != 0 && rd != 2 && imm != 0) {
            /* rd≠{x0,x2}; immediate is nonzero */
            return true;
        }
        return false;
    }
    else if (riscv->format == S) {  /* S case */
        /* sw rs2, offset (rs1') */
        rs1 = riscv->m_code[3];     /* rs1 [19:15] */
        rs2 = riscv->m_code[4];     /* rs2 [24:20] */
        imm = cal_s_imm(riscv);     /* first imm [11:7] */
        if (imm % 4 != 0 || imm > 127 || imm < 0) {    /* infomation loss when compression imm[6:2] */
            return false;
        }
        if (check_register(rs1) && check_register(rs2)) {    /* Check register name */
            return true;
        }
    }
    else if (riscv->format == SB) { /* SB case */
        rs1 = riscv->m_code[3];     /* rs1 [19:15] */
        rs2 = riscv->m_code[4];     /* rs2 [24:20] */
        imm = cal_sb_imm(riscv);
        if (imm > 255 || imm < -256) {   /* infomation loss when compression imm[8:1] */
            return false;
        }
        if (check_register(rs1) && rs2 == 0) {
            /* beq/bne rs1', x0, offset	 */
            return true;
        }
    }
    else if (riscv->format == UJ) { /* UJ case */
        rd = riscv->m_code[1];      /* rd */
        imm = cal_uj_imm(riscv);
        if (imm > 2047 || imm < -2048) {  /* infomation loss when compression imm[11:1] */
            return false;
        }
        if (rd == 0 || rd == 1) {
            /* jal x0/x1, offset */
            return true;
        }
    }
    return false;   /* otherwise return false */
}
/*translate a  RISC_V object to  RVC object */
RVC* riscv_to_rvc(RISC_V* riscv, unsigned int line_idx, unsigned int* pre_sum){
    RVC* rvc = (RVC*)malloc(sizeof(RVC));
    INSTR_TYPE instr = riscv->ins_type; /*get the exact instruction*/
    memset(rvc, 0, sizeof(RVC));
    if(instr == add){ /*add can be compressed to two kind of RISC_V_C instruction*/
        rvc->n_pieces = 4; /* CR format can be splited to 4 pieces*/
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int)); /*malloc 4 int  */
        rvc->format = CR;  
        if(riscv->m_code[3]){ /*judge whether it is c.add*/
            rvc->ins_type = c_add;          
            rvc->m_code[0] = 2; /* opcode */
            rvc->m_code[1] = riscv->m_code[4];/* RS2 = rs2 */
            rvc->m_code[2] = riscv->m_code[1];/* RD/RS1 = rd */
            rvc->m_code[3] = 9;  /*func4 = 0b1001 */
        }
        else{ 
            rvc->ins_type = c_mv; /* the c.mv case */
            rvc->m_code[0] = 2; /*opcode*/
            rvc->m_code[1] = riscv->m_code[4];/* RS2 = rs2 */
            rvc->m_code[2] = riscv->m_code[1];/* RD/RS1 = rd */
            rvc->m_code[3] = 8; /* FUNCT4 = 0b1000*/
        }
    }
    else if(instr == jalr){ /* the jalr op can be compressed to 2 instrcution */
        rvc->format = CR; /* CR */
        rvc->n_pieces = 4; 
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        if(!riscv->m_code[1]){ /*rd == x0 c_jr case */
            rvc->ins_type = c_jr;
            rvc->m_code[0] = 2; /* opcode 0b10 */
            rvc->m_code[1] = 0; /* RS2 = 0 */
            rvc->m_code[2] = riscv->m_code[3]; /* RS1 = rs1 */
            rvc->m_code[3] = 8; /* FUNCT4 = 0b1000 */
        }
        else{
            rvc->ins_type = c_jalr; /* rd == x1 c_jalr case */
            rvc->m_code[0] = 2; /* opcode 0b10 */
            rvc->m_code[1] = 0; /* RS2 = 0 */
            rvc->m_code[2] = riscv->m_code[3]; /* RS1 = rs1 */
            rvc->m_code[3] = 9; /* FUNCT4 = 0b1001 */
        }
    }
    else if(instr==addi){   
        rvc->format = CI;   /* CI */
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        if(!riscv->m_code[3]){  /* rs1 = x0 c_li case */
            rvc->ins_type = c_li;
            rvc->m_code[0] = 1; /* opcode 0b01 */
            rvc->m_code[1] = get_bits(riscv->m_code[4],4,0); /* IMM = imm[4:0] */
            rvc->m_code[2] = riscv->m_code[1];      /* RD = rd*/
            rvc->m_code[3] = get_bits(riscv->m_code[4],5,5);    /* IMM = imm[5:5] */
            rvc->m_code[4] = 2;     /* FUNCT3 = 0b010 */
        }
        else{
            rvc->ins_type = c_addi; /* rd == rs1 addi case */
            rvc->m_code[0] = 1; /* opcode 0b01 */
            rvc->m_code[1] = get_bits(riscv->m_code[4],4,0); /* NZIMM = nzimm[4:0] */
            rvc->m_code[2] = riscv->m_code[1];  /* RD = rd */
            rvc->m_code[3] = get_bits(riscv->m_code[4],5,5);    /* NZIMM = nzimm[5:5] */
            rvc->m_code[4] = 0; /* FUNCT3 = 0b000 */
        }
    }
    else if(instr ==  lui){     
        rvc->format = CI;   /* c_lui case */
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->ins_type = c_lui;
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = get_bits(riscv->m_code[2],4,0);   /* NZIMM[16:12] = nzimm[16:12] actually [4:0] */
        rvc->m_code[2] = riscv->m_code[1];  /* RD = rd */
        rvc->m_code[3] = get_bits(riscv->m_code[2],5,5);     /* NZIMM[17] = nzimm[17] actually [5] */
        rvc->m_code[4] = 3; /* FUNCT3 = 0b011 */
    }
    else if(instr == slli){
        rvc->format = CI;   /* c_slli case */
        rvc->ins_type = c_slli;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 2; /* opcode 0b10 */
        rvc->m_code[1] = get_bits(riscv->m_code[4],4,0);    /* SHAMT[4:0] = IMM[4:0] */
        rvc->m_code[2] = riscv->m_code[1];  /* RD = rd */
        rvc->m_code[3] = get_bits(riscv->m_code[4],5,5);    /* SHAMT[5] = IMM[5] must be zero */
        rvc->m_code[4] = 0; /* FUNCT3 = 0b000 */
    }
    else if(instr == lw){
        rvc->format = CL;   /* c_lw case */
        rvc->ins_type = c_lw;
        rvc->n_pieces = 6;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 0; /* opcode 0b00 */
        rvc->m_code[1] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[2] = (get_bits(riscv->m_code[4],2,2)<<1) | (get_bits(riscv->m_code[4],6,6)); /* OFFSET[2|6] = imm[2|6] */
        rvc->m_code[3] = riscv->m_code[3]-8;    /* RS1' = rs1 - 8 */
        rvc->m_code[4] = get_bits(riscv->m_code[4],5,3);    /* OFFSET[5:3] = imm[5:3] */
        rvc->m_code[5] = 2;     /* FUNCT3 = 0b010 */
    }
    else if(instr == sw){
        rvc->format = CS;   /* c_sw case */
        rvc->ins_type = c_sw;
        rvc->n_pieces = 6;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 0; /* opcode 0b00 */
        rvc->m_code[1] = riscv->m_code[4]-8;    /* RS2' = rs2 - 8 */
        rvc->m_code[2] = (get_bits(riscv->m_code[1],2,2)<<1)|(get_bits(riscv->m_code[5],1,1)); /* OFFSET[2:6] = imm[2|6] acctually in two part */
        rvc->m_code[3] = riscv->m_code[3]-8;    /* RS1' = rs1 - 8 */
        rvc->m_code[4] = (get_bits(riscv->m_code[5],0,0)<<2)|(get_bits(riscv->m_code[1],4,3));    /* OFFSET[5:3] = imm[5:3] in two part*/
        rvc->m_code[5] = 6; /* FUNCT3 0b110 */
    }
    else if(instr == and){
        rvc->format = CS;   /* c_and case */
        rvc->ins_type = c_and;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = riscv->m_code[4]-8;    /* RS2' = rs2 - 8 */
        rvc->m_code[2] = 3; /* FUNCT2 = 0b11 */
        rvc->m_code[3] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[4] = 0x023;    /* FUNC6 = 0b10011 */
    }
    else if(instr == or){
        rvc->format = CS;   /* c_or case */
        rvc->ins_type = c_or;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode = 0b01*/
        rvc->m_code[1] = riscv->m_code[4]-8;    /* RS = rs2 - 8 */
        rvc->m_code[2] = 2; /* FUNCT2 = 0b10 */
        rvc->m_code[3] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[4] = 0x023;   /* FUNC6 = 0b10011 */
    }
    else if(instr == xor){
        rvc->format = CS;   /* c_xor case */
        rvc->ins_type = c_xor;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = riscv->m_code[4]-8;    /* RS2' = rs2 - 8 */
        rvc->m_code[2] = 1; /* FUNCT2 = 0b01 */
        rvc->m_code[3] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[4] = 0x023;   /* FUNC6 = 0b10011 */
    }
    else if(instr == sub){
        rvc->format = CS;   /* c_sub case */
        rvc->ins_type = c_sub;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = riscv->m_code[4]-8;    /* RS2' = rs2 - 8 */
        rvc->m_code[2] = 0; /* FUNCT2 = 0b00 */
        rvc->m_code[3] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[4] = 0x023;   /* FUNC6 = 0b10011 */
    }
    else if(instr == beq){
        int offset  = (int)cal_sb_imm(riscv);
        int jump_bytes = 0; /* calculate the offset */
        printf("line: %d  offset: %d\n", line_idx, offset);
        if(offset > 0){  /*if jump  down*/
            int jump_lines = offset / 4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset - (diff << 1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        else if(offset < 0){ /* if jump up */
            int jump_lines = offset / 4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset + (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        rvc->format = CB;   /* c_beqz case */
        rvc->ins_type = c_beqz;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01*/
        rvc->m_code[1] = get_bits(jump_bytes,5,5)|(get_bits(jump_bytes,2,1)<<1)|(get_bits(jump_bytes,7,6)<<3);
        /* OFFSET[7:6|2:1|5] = imm[7:6|2:1|5] actually in three parts */
        rvc->m_code[2] = riscv->m_code[3]-8;    /* RS1' = rs1 - 8 */
        rvc->m_code[3] = get_bits(jump_bytes,4,3)|(get_bits(jump_bytes,8,8)<<2);    /* OFFSET[8|4:3] = imm[8|4:3] */
        rvc->m_code[4] = 6; /* FUNCT3 = 0b110 */
    }
    else if(instr == bne){
        int offset  = (int)cal_sb_imm(riscv);
        int jump_bytes = 0; /*calculate the offset */
        printf("line: %d  offset: %d\n", line_idx, offset);
        if(offset>0){  /*if jump  down*/
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset - (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        else if(offset<0){ /* if jump up */
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset + (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        rvc->format = CB;   /* c_beqz case */
        rvc->ins_type = c_bnez;
        rvc->n_pieces = 5;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01*/
        rvc->m_code[1] = get_bits(jump_bytes,5,5)|(get_bits(jump_bytes,2,1)<<1)|(get_bits(jump_bytes,7,6)<<3);
        /* OFFSET[7:6|2:1|5] = imm[7:6|2:1|5] actually in three parts */
        rvc->m_code[2] = riscv->m_code[3]-8;    /* RS1' = rs1 - 8 */
        rvc->m_code[3] = get_bits(jump_bytes,4,3)|(get_bits(jump_bytes,8,8)<<2);    /* OFFSET[8|4:3] = imm[8|4:3] */
        rvc->m_code[4] = 7; /* FUNCT3 = 0b111 */
    }
    else if(instr == srli){
        rvc->format = CB;      /* c_srli case */
        rvc->ins_type = c_srli;
        rvc->n_pieces = 6;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = get_bits(riscv->m_code[4],4,0);    /* SHAMT[4:0] = imm[4:0] */
        rvc->m_code[2] = riscv->m_code[1]-8;    /* RD' = rd - 8*/
        rvc->m_code[3] = 0; /* FUNCT2 = 0b00*/
        rvc->m_code[4] = get_bits(riscv->m_code[4],5,5);    /* SHAMT[5] = imm[5] must be zero*/
        rvc->m_code[5] = 4; /* FUNCT3 = 0b100*/
    }
    else if(instr == srai){
        rvc->format = CB;      /* c_srai case */
        rvc->ins_type = c_srai;
        rvc->n_pieces = 6;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = get_bits(riscv->m_code[4],4,0);    /* SHAMT[4:0] = imm[4:0] */
        rvc->m_code[2] = riscv->m_code[1]-8;    /* RD' = rd - 8*/
        rvc->m_code[3] = 1; /* FUNCT2 = 0b01*/
        rvc->m_code[4] = get_bits(riscv->m_code[4],5,5);    /* SHAMT[5] = imm[5] must be zero*/
        rvc->m_code[5] = 4; /* FUNCT3 = 0b100*/
    }
    else if(instr == andi){
        rvc->format = CB;   /* c_addi case */
        rvc->ins_type = c_addi;
        rvc->n_pieces = 6;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        rvc->m_code[0] = 1; /* opcode 0b01 */
        rvc->m_code[1] = get_bits(riscv->m_code[4],4,0);    /* IMM[4:0] = imm[4:0] */
        rvc->m_code[2] = riscv->m_code[1]-8;    /* RD' = rd - 8 */
        rvc->m_code[3] = 2; /* FUNCT2 = 0b10 */
        rvc->m_code[4] = get_bits(riscv->m_code[4],5,5);    /* IMM[5] = imm[5] */
        rvc->m_code[5] = 4; /* FUNCT3 = 0b100 */
    }
    else if(instr == jal){
        int offset  = cal_uj_imm(riscv);
        int jump_bytes = 0; /*calculate the offset */
        printf("line: %d  offset: %d\n", line_idx, offset);
        if(offset>0){  /*if jump  down*/
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset-(diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        else if(offset<0){ /* if jump up */
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset + (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        rvc->format = UJ;   /* c_j&c_jal case  */
        rvc->n_pieces = 3;
        rvc->m_code = (unsigned int*)calloc(rvc->n_pieces, sizeof(int));
        /* riscv->m_code[2] >>= 1; */
        if(!riscv->m_code[1]){  /* c_j case rd = x0 */
            rvc->ins_type = c_j;
            rvc->m_code[0] = 1; /* opcode 0b01*/
            rvc->m_code[1] = (get_bits(jump_bytes,5,5))|(get_bits(jump_bytes,3,1)<<1)| /* OFFSET[3:1|5]*/
            (get_bits(jump_bytes,7,7)<<4)|(get_bits(jump_bytes,6,6)<<5)|    /* OFFSET[6|7] */
            (get_bits(jump_bytes,10,10)<<6)|(get_bits(jump_bytes,9,8)<<7)|    /* OFFSET[9:8|10] */
            (get_bits(jump_bytes,4,4)<<9)|(get_bits(jump_bytes,11,11)<<10);   /* OFFSET[11|4] */
            rvc->m_code[2] = 5; /* func3 0b101 */
        }
        else{
            rvc->ins_type = c_jal; /*c_jal case rd=x1 */
            rvc->m_code[0]=1; /* opcode 0b001*/
             rvc->m_code[1] = (get_bits(jump_bytes,5,5))|(get_bits(jump_bytes,3,1)<<1)| /* OFFSET[3:1|5]*/
            (get_bits(jump_bytes,7,7)<<4)|(get_bits(jump_bytes,6,6)<<5)|    /* OFFSET[6|7] */
            (get_bits(jump_bytes,10,10)<<6)|(get_bits(jump_bytes,9,8)<<7)|    /* OFFSET[9:8|10] */
            (get_bits(jump_bytes,4,4)<<9)|(get_bits(jump_bytes,11,11)<<10);   /* OFFSET[11|4] */
            rvc->m_code[2] = 1; /* func3 0b001*/
        }
    }
    return rvc;
}
/* defination of adjust_offset_uncompressible */
unsigned int adjust_offset_uncompressible(RISC_V* riscv, unsigned int line_idx, unsigned int* pre_sum) {
    unsigned int machine_code = 0;
    if (riscv->format == SB) {  /* SB case */
        int offset  = (int)cal_sb_imm(riscv);
        int jump_bytes = 0; /*calculate the offset */
        printf("line: %d  offset: %d\n", line_idx, offset);
        if(offset > 0){  /*if jump  down*/
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset - (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        else if(offset < 0){ /* if jump up */
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset + (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        machine_code |= riscv->m_code[0];   /* opcode */
        machine_code |= riscv->m_code[2] << 12;     /* funct3 */
        machine_code |= riscv->m_code[3] << 15;     /* rs1 */
        machine_code |= riscv->m_code[4] << 20;     /* rs2 */
        machine_code |= get_bits(jump_bytes, 11, 11) << 7;  /* imm[4:1|11] */
        machine_code |= get_bits(jump_bytes, 4, 1) << 8;    /* imm[4:1|11] */
        machine_code |= get_bits(jump_bytes, 10, 5) << 25; /* imm[12|10:5] */
        machine_code |= get_bits(jump_bytes, 12, 12) << 31; /* imm[12|10:5] */
    }
    else {  /* UJ case */
        int offset  = cal_uj_imm(riscv);
        int jump_bytes = 0; /*calculate the offset */
        printf("line: %d  offset: %d\n", line_idx, offset);
        if(offset > 0){  /*if jump  down*/
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset - (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        else if(offset < 0){ /* if jump up */
            int jump_lines = offset/4; /*to get how many lines it jumped */
            int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
            jump_bytes = offset + (diff<<1);
            printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
        }
        machine_code |= riscv->m_code[0];   /* opcode */
        machine_code |= riscv->m_code[1] << 7;   /* rd */
        machine_code |= get_bits(jump_bytes, 19, 12) << 12; /* imm[20|10:1|11|19:12] */
        machine_code |= get_bits(jump_bytes, 11, 11) << 20; /* imm[20|10:1|11|19:12] */
        machine_code |= get_bits(jump_bytes, 10, 1) << 21; /* imm[20|10:1|11|19:12] */
        machine_code |= get_bits(jump_bytes, 20, 20) << 31; /* imm[20|10:1|11|19:12] */
    }
    return machine_code;
}
