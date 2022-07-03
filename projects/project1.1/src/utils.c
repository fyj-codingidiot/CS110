#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
/* str_to_machine defination */
unsigned int str_to_machine(char* str) {
    int res = 0, i = 0; /* initialize the machine code result */
    for (i = 0; i < 32; i++) { 
        /* tranverse the string and caculate the machine code */
        res = res * 2 + (int)str[i] - '0'; 
    }
    return res;
}
/* machine_to_str_32 defination */
void machine_to_str_16(unsigned int machine_code, char* str) {
    int i = 0;
    for (i = 0; i < 16; i++) {   
        /* tranverse the string and assign every char in the string */
        str[15-i] = (char)((machine_code & 1) + '0'); /* get the least significant bit of machine code */
        machine_code = machine_code >> 1; /* right shift one bit in machine code */
    }
}
/* machine_to_str_32 defination */
void machine_to_str_32(unsigned int machine_code, char* str) {
    int i = 0;
    for (i = 0; i < 32; i++) {   
        /* tranverse the string and assign every char in the string */
        str[31-i] = (char)((machine_code & 1) + '0'); /* get the least significant bit of machine code */
        machine_code = machine_code >> 1; /* right shift one bit in machine code */
    }
}
/* check_register defination */
bool check_register(unsigned int r_tag) {
    return (r_tag >= 8) && (r_tag <= 15);
}
/* get_bits defination */
unsigned int get_bits(unsigned int machine_code, int l, int r) {
    int len = l - r + 1; /* caculate the piece length */
    return (machine_code & (((1 << len) - 1) << r)) >> r; /* calculate result */
}
/* cal_i_imm defination */
int cal_i_imm(RISC_V* riscv) {
    int res = 0;
    if (get_bits(riscv->m_code[4], 11, 11)) {
        res = 0xFFFFF800;       /* negative case */
    }
    res |= get_bits(riscv->m_code[4], 11, 0); /* imm[11:0] */
    return res;
}
/* cal_s_imm defination */
int cal_s_imm(RISC_V* riscv) {
    int res = 0;
    if (get_bits(riscv->m_code[5], 6, 6)) {
        res = 0xFFFFF800;       /* negative case */
    }
    res |= get_bits(riscv->m_code[5], 6, 0) << 5;   /* imm[11:5] */
    res |= get_bits(riscv->m_code[1], 4, 0);    /* imm[4:0] */
    return res;
}
/* cal_u_imm defination */
int cal_u_imm(RISC_V* riscv) {
    int res = 0;
    if (get_bits(riscv->m_code[2], 19, 19)) {
        res = 0x80000000;       /* negative case */
    }
    res |= get_bits(riscv->m_code[2], 19, 0);   /* imm[31:12] */
    return res;
}
/* cal_sb_imm defination */
int cal_sb_imm(RISC_V* riscv) {
    int res = 0;
    if (get_bits(riscv->m_code[5], 6, 6)) {
        res = 0xFFFFF000;       /* negative case */
    }
    res |= get_bits(riscv->m_code[1], 4, 1);    /* imm[4:1|11] */
    res |= get_bits(riscv->m_code[5], 5, 0) << 4;   /* imm[12|10:5] */
    res |= get_bits(riscv->m_code[1], 0, 0) << 10;  /* imm[4:1|11] */
    res |= get_bits(riscv->m_code[5], 6, 6) << 11;  /* imm[12|10:5] */
    return res * 2;    /* left shift 1 bit */
}
/* cal_uj_imm defination */
int cal_uj_imm(RISC_V* riscv) {
    int res = 0;
    if (get_bits(riscv->m_code[2], 19, 19)) {
        res = 0xFFF00000;       /* negative case */
    }
    res |= get_bits(riscv->m_code[2], 18, 9);   /* imm[20|10:1|11|19:12] */
    res |= get_bits(riscv->m_code[2], 8, 8) << 10;   /* imm[20|10:1|11|19:12] */
    res |= get_bits(riscv->m_code[2], 7, 0) << 11;   /* imm[20|10:1|11|19:12] */
    res |= get_bits(riscv->m_code[2], 19, 19) << 19;   /* imm[20|10:1|11|19:12] */
    return res * 2;    /* left shift 1 bit */
}
/* riscv_delete defination */
void riscv_delete(RISC_V* riscv) {
    free((riscv)->m_code); /* free the m_code first */
    free(riscv); /* free riscv pointer */
}
/* rvc_delete defination */
void rvc_delete(RVC* rvc) {
    free((rvc)->m_code); /* free the m_code first */
    free(rvc); /* free riscv pointer */
}
/* machine_to_riscv defination */
RISC_V* machine_to_riscv(unsigned int machine_code) {
    unsigned int opcode, func3, func7; /* declare the opcode */
    RISC_V* riscv = (RISC_V*)malloc(sizeof(RISC_V)); /* allocate memory for RISC-V object */
    memset(riscv, 0, sizeof(RISC_V));
    /* get values of opcode func3 func7 */
    opcode = get_bits(machine_code, 6, 0);
    func3 = get_bits(machine_code, 14, 12);
    func7 = get_bits(machine_code, 31, 25);
    if (opcode == 0x033) {  /* 0b0110011 */
        riscv->format = R;      /* R case */
        switch (func3) {
            case 0x00:
                if (func7) {    /* sub: 0100000 add: 0000000*/
                    riscv->ins_type = sub;
                } else {
                    riscv->ins_type = add;
                }
                break;      /*Don't forget break*/
            case 0x04: /* 0b100 */
                riscv->ins_type = xor;      /* xor: 100 */
                break;
            case 0x06: /* 0b110 */
                riscv->ins_type = or;       /* or: 110 */
                break;
            case 0x07: /* 0b111 */
                riscv->ins_type = and;      /* and: 111 */
                break;
            default:
                riscv->ins_type = other_instr;  /* other cases */
        }
    }
    else if (opcode == 0x03B) {     /* Special R case xxxw 0b0111011 */
        riscv->format = R;
        riscv->ins_type = other_instr;
    }
    else if (opcode == 0x01B || opcode == 0x73 ) {     /* Special I case 0b0011011 || 0b01110011 */
        riscv->format = I;
        riscv->ins_type = other_instr;
    }
    else if (opcode == 0x03) {  /* 0b0000011 */
        riscv->format = I;      /* lw 0000011 */
        riscv->ins_type = lw;
    }
    else if (opcode == 0x013) { /* 0b0010011 */
        riscv->format = I;      /* I case */
        switch (func3) {
            case 0x00:     /* 0b000 */
                riscv->ins_type = addi;     /* addi: 000 */
                break;
            case 0x01:     /* 0b001 */
                riscv->ins_type = slli;     /* slli: 001 */
                break;
            case 0x05:     /* 0b101 */
                if (func7) {
                    riscv->ins_type = srai; /* srai: 0100000 */
                } else {
                    riscv->ins_type = srli; /* srli: 0000000 */
                }
                break;
            case 0x07:      /* 0b111 */
                riscv->ins_type = andi;     /* andi: 111 */
                break;  
            default:
                riscv->ins_type = other_instr;  /* other cases */
        }
    }
    else if (opcode == 0x067) {    /* 0b01100111 */
        riscv->format = I;      /* Special I case */
        riscv->ins_type = jalr; 
    }
    else if (opcode == 0x037) {    /* 0b00110111 */
        riscv->format = U;      /* Special U case */
        riscv->ins_type = lui;
    }
    else if (opcode == 0x017) {     /* 0b0010111 */
        riscv->format = U;      /* Other U case */
        riscv->ins_type = other_instr;
    }
    else if (opcode == 0x023) {     /* 0b0100011 */
        riscv->format = S;      /* S case */
        if (func3 == 0x02) {       /* 0b010 */
            riscv->ins_type = sw;       /* sw: 010 */
        } else {
            riscv->ins_type = other_instr;  /* other cases */
        }
    }
    else if (opcode == 0x063) {    /* 0b01100011 */
        riscv->format = SB;     /* SB case */
        switch (func3) {
            case 0x000:
                riscv->ins_type = beq;      /* beq: 000 */
                break;
            case 0x001:
                riscv->ins_type = bne;      /* bne: 001 */
                break;
            default:
                riscv->ins_type = other_instr;  /* other cases */
        }
    }
    else if (opcode == 0x06F) {      /* 0b01101111 */
        riscv->format = UJ;     /* UJ case only one instruction */
        riscv->ins_type = jal;
    }

    /* split the machine code to m_code array*/
    if (riscv->format == R || riscv->format == S || riscv->format == SB){
        /* First type */
        riscv->n_pieces = 6;    /* record the number of pieces */
        riscv->m_code = (unsigned int*)calloc(6, sizeof(int));  /* allocate memory for m_code array*/
        riscv->m_code[0] = opcode;      /* opcode */
        riscv->m_code[1] = get_bits(machine_code, 11, 7);   /* rd / imm[4:0] / imm[4:1|11] */
        riscv->m_code[2] = func3;   /* func3 */
        riscv->m_code[3] = get_bits(machine_code, 19, 15);  /* rs1 */
        riscv->m_code[4] = get_bits(machine_code, 24, 20);  /* rs2 */
        riscv->m_code[5] = func7;   /* func7 / imm[11:5] / imm[12|10:5] */
    }
    else if (riscv->format == I) {
        /* Second type */
        riscv->n_pieces = 5;    /* record the number of pieces */
        riscv->m_code = (unsigned int*)calloc(5, sizeof(int));  /* allocate memory for m_code array*/
        riscv->m_code[0] = opcode;      /* opcode */
        riscv->m_code[1] = get_bits(machine_code, 11, 7);   /* rd */
        riscv->m_code[2] = func3;   /* func3 */
        riscv->m_code[3] = get_bits(machine_code, 19, 15);  /* rs1 */
        riscv->m_code[4] = get_bits(machine_code, 31, 20);  /* imm[11:0] */
    }
    else {
        /* U/UJ Format third type*/
        riscv->n_pieces = 3;    /* record the number of pieces */
        riscv->m_code = (unsigned int*)calloc(3, sizeof(int));  /* allocate memory for m_code array*/
        riscv->m_code[0] = opcode;      /* opcode */
        riscv->m_code[1] = get_bits(machine_code, 11, 7);   /* rd */
        riscv->m_code[2] = get_bits(machine_code, 31, 12);  /* imm[31:12] / imm[20|10:1|11|19:12] */
    }
    return riscv; /* return the result */
}
/* rvc_to_machine defination */
unsigned int rvc_to_machine(RVC* rvc) {
    unsigned int machine_code = 0; 
    machine_code = rvc->m_code[0]; /* Set opcode */
    if (rvc->format == CR) {
        /* CR Format: 4 5 5 2 */
        machine_code |= (rvc->m_code[1] << 2); /* Set 6:2 */
        machine_code |= (rvc->m_code[2] << 7); /* Set 11:7 */
        machine_code |= (rvc->m_code[3] << 12); /* Set 15:12 */
    }
    else if (rvc->format == CI) {
        /* CI Format: 3 1 5 5 2 */
        machine_code |= (rvc->m_code[1] << 2); /* Set 6:2 */
        machine_code |= (rvc->m_code[2] << 7); /* Set 11:7 */
        machine_code |= (rvc->m_code[3] << 12); /* Set 12:12 */
        machine_code |= (rvc->m_code[4] << 13); /* Set 15:13 */
    }
    else if (rvc->format == CL) {
        /* CL Format: 3 3 3 2 3 2 */
        machine_code |= (rvc->m_code[1] << 2); /* Set 4:2 */
        machine_code |= (rvc->m_code[2] << 5); /* Set 6:5 */
        machine_code |= (rvc->m_code[3] << 7); /* Set 9:7 */
        machine_code |= (rvc->m_code[4] << 10); /* Set 12:10 */
        machine_code |= (rvc->m_code[5] << 13); /* Set 15:13 */
    }
    else if (rvc->format == CS) {
        /* CS Format two types */
        if (rvc->ins_type == c_sw) {
            /* type1: 3 3 3 2 3 2 */
            machine_code |= (rvc->m_code[1] << 2); /* Set 4:2 */
            machine_code |= (rvc->m_code[2] << 5); /* Set 6:5 */
            machine_code |= (rvc->m_code[3] << 7); /* Set 9:7 */
            machine_code |= (rvc->m_code[4] << 10); /* Set 12:10 */
            machine_code |= (rvc->m_code[5] << 13); /* Set 15:13 */
        } else {
            /* type2: 6 3 2 3 2 */
            machine_code |= (rvc->m_code[1] << 2); /* Set 4:2 */
            machine_code |= (rvc->m_code[2] << 5); /* Set 6:5 */
            machine_code |= (rvc->m_code[3] << 7); /* Set 9:7 */
            machine_code |= (rvc->m_code[4] << 10); /* Set 15:10 */
        }
    }
    else if (rvc->format == CB) {
        /* CS Format two types */
        if (rvc->ins_type == c_beqz || rvc->ins_type == c_bnez) {
            /* type1: 3 3 3 5 2 */
            machine_code |= (rvc->m_code[1] << 2); /* Set 6:2 */
            machine_code |= (rvc->m_code[2] << 7); /* Set 9:7 */
            machine_code |= (rvc->m_code[3] << 10); /* Set 12:10 */
            machine_code |= (rvc->m_code[4] << 13); /* Set 15:13 */
        } else {
            /* type2: 3 1 2 3 5 2 */
            machine_code |= (rvc->m_code[1] << 2); /* Set 6:2 */
            machine_code |= (rvc->m_code[2] << 7); /* Set 9:7 */
            machine_code |= (rvc->m_code[3] << 10); /* Set 11:10 */
            machine_code |= (rvc->m_code[4] << 12); /* Set 12:12 */
            machine_code |= (rvc->m_code[5] << 13); /* Set 15:13 */
        }
    }
    else if (rvc->format == CJ) {
        /* CJ Format: 3 11 2 */
        machine_code |= (rvc->m_code[1] << 2); /* Set 12:2 */
        machine_code |= (rvc->m_code[2] << 13); /* Set 15:13 */
    }
    return machine_code; /* return the result */
}