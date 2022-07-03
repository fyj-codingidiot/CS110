typedef struct risc_v_or_rvc
{
    int Type; // 0 or 1 : risc-v/rvc
    int Format;
    int instr_type;
}Code;
/* define the instr type of two instruction set */
typedef enum INSTR_TYPE_ {
    add, and, or, xor, sub, /* RISCV: R Format */
    jalr, addi, slli, lw, srli, srai, andi, /* RISCV: I Format */
    lui, sw, beq, bne, jal, /* RISCV: U,S,SB,UJ Format */
    c_add, c_mv, c_jr, c_jalr, /* RVC: CR Format */
    c_li, c_lui, c_addi, c_slli, /* RVC: CI Format */
    c_lw, c_j, c_jal, /* RVC: CL,CJ Format */
    c_sw, c_and, c_or, c_xor, c_sub, /* RVC: CS Format */
    c_beqz, c_bnez, c_srli, c_srai, c_andi, /* RVC: CB Format */
    other_instr /* Other instructions */
} INSTR_TYPE;
typedef enum RISCV_FORMAT_ {R, I, U, S, SB, UJ} RISCV_FORMAT;
typedef enum RVC_FORMAT_ {CR, CI, CL, CS, CB, CJ} RVC_FORMAT;

int cal_sb_imm(int machine_code) {
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
int cal_uj_imm(int machine_code) {
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
int rvc_extend(Code element, int rvc_code,int line_idx, int* pre_sum) {
    int machine_code;
    if (element.Format == 0) {
        if (riscv->format == SB) {  /* SB case */
            int offset  = (int)cal_sb_imm(riscv);
            int jump_bytes = 0; /*calculate the offset */
            if(offset > 0){  /*if jump  down*/
                int jump_lines = 0; /*to get how many lines it jumped */
                while (offset != jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx]) * 2 ) {
                    jump_lines ++;
                }
                jump_bytes = jump_lines * 4;
            }
            else if(offset < 0){ /* if jump up */
                int jump_lines = 0; /*to get how many lines it jumped */
                while (offset != jump_lines * 4 - (pre_sum[line_idx - 1] - pre_sum[line_idx - jump_lines]) * 2 ) {
                    jump_lines ++;
                }
                jump_bytes = jump_lines * 4;
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

    }
    else {  /* rvc extention */
        if (element.instr_type == c_add) {
            int func7 = 0;
            int func3 = 0;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 11, 7);
            int rs1 = rd;
            int rs2 = get_bits(rvc_code, 6, 2);
            machine_code += func7;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_mv) {
            int func7 = 0;
            int func3 = 0;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 11, 7);
            int rs1 = 0;
            int rs2 = get_bits(rvc_code, 6, 2);
            machine_code += func7;
            machine_code += rs2 << 20;
            machine_code += rs1;
            machine_code += func3;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_jr) {
            int func3 = 0;
            int opcode = 0b1100111;
            int imm = 0;
            int rs1 = get_bits(rvc_code, 11, 7);
            int rd = 0;
            machine_code += imm;
            machine_code += rs1 << 15;
            machine_code += func3;
            machine_code += rd;
            machine_code += opcode;
        }
        if (element.instr_type == c_jalr) {
            int func3 = 0;
            int opcode = 0b1100111;
            int imm = 0;
            int rs1 = get_bits(rvc_code, 11, 7);
            int rd = 1;
            machine_code += imm;
            machine_code += rs1 << 15;
            machine_code += func3;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_li) {
            int func3 = 0;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rs1 = 0;
            int rd = get_bits(rvc_code, 11, 7);
            machine_code += imm << 20;
            machine_code += rs1;
            machine_code += func3;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_lui) {
            int opcode = 0b0110111;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 11, 7);
            machine_code += imm << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_addi) {
            int func3 = 0;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 11, 7);
            int rs1 = rd;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_slli) {
            int func3 = 1;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 11, 7);
            int rs1 = rd;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_lw) {
            int func3 = 0b010;
            int opcode = 0b0000011;
            int imm = (get_bits(rvc_code,5,5) << 6) + (get_bits(rvc_code,12,10) << 3) + (get_bits(rvc_code, 6,6) << 2);
            int rd = get_bits(rvc_code, 4, 2) + 8;
            int rs1 = get_bits(rvc_code, 9, 7) + 8;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_sw) {
            int func3 = 0b010;
            int opcode = 0b0000011;
            int imm = (get_bits(rvc_code,5,5) << 6) + (get_bits(rvc_code,12,10) << 3) + (get_bits(rvc_code, 6,6) << 2);
            int rs2 = get_bits(rvc_code, 4, 2) + 8;
            int rs1 = get_bits(rvc_code, 9, 7) + 8;
            machine_code += get_bits(imm, 11, 5) << 25;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += get(imm, 4, 0) << 6;
            machine_code += opcode;
        }
        if (element.instr_type == c_and) {
            int func7 = 0;
            int func3 = 0b111;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            int rs2 = get_bits(rvc_code, 4, 2) + 8;
            machine_code += func7 << 25;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_or) {
            int func7 = 0;
            int func3 = 0b110;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            int rs2 = get_bits(rvc_code, 4, 2) + 8;
            machine_code += func7 << 25;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_xor) {
            int func7 = 0;
            int func3 = 0b100;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            int rs2 = get_bits(rvc_code, 4, 2) + 8;
            machine_code += func7 << 25;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_sub) {
            int func7 = 0b0100000;
            int func3 = 0b111;
            int opcode = 0b0110011;
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            int rs2 = get_bits(rvc_code, 4, 2) + 8;
            machine_code += func7 << 25;
            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_beqz) {
            int func3 = 0b000;
            int opcode = 0b1100011;
            int rs2 = 0;
            int rs1 = get_bits(rvc_code, 9, 7) + 8;
            int imm = cal_CB_imm(rvc_code);

            int jump_bytes = 0; /*calculate the offset */
            if(imm > 0){  /*if jump  down*/
                int jump_lines = imm/4; /*to get how many lines it jumped */
                int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
                jump_bytes = imm - (diff<<1);
                printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
            }
            else if(imm < 0){ /* if jump up */
                int jump_lines = imm/4; /*to get how many lines it jumped */
                int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
                jump_bytes = imm + (diff<<1);
                printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
            }

            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += opcode;
            machine_code += get_bits(jump_bytes,12,12) << 31;
            machine_code += get_bits(jump_bytes,10,5) << 25;
            machine_code += get_bits(jump_bytes,4,1) << 8;
            machine_code += get_bits(jump_bytes, 11, 11) << 7;
        }
        if (element.instr_type == c_bnez) {
            int func3 = 0b001;
            int opcode = 0b1100011;
            int rs2 = 0;
            int rs1 = get_bits(rvc_code, 9, 7) + 8;
            int imm = cal_CB_imm(rvc_code);

            int jump_bytes = 0; /*calculate the offset */
            if(imm > 0){  /*if jump  down*/
                int jump_lines = imm/4; /*to get how many lines it jumped */
                int diff = pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]; /*how many lines can be compressed between instructions */
                jump_bytes = imm - (diff<<1);
                printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
            }
            else if(imm < 0){ /* if jump up */
                int jump_lines = imm/4; /*to get how many lines it jumped */
                int diff = pre_sum[line_idx - 1] - pre_sum[line_idx + jump_lines - 1]; /*how many lines can be compressed between instructions */
                jump_bytes = imm + (diff<<1);
                printf("jump_lines: %d  jump_bytes: %d\n", jump_lines, jump_bytes);
            }

            machine_code += rs2 << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += opcode;
            machine_code += get_bits(jump_bytes,12,12) << 31;
            machine_code += get_bits(jump_bytes,10,5) << 25;
            machine_code += get_bits(jump_bytes,4,1) << 8;
            machine_code += get_bits(jump_bytes, 11, 11) << 7;
        }
        if (element.instr_type == c_srli) {
            int func3 = 0b101;
            int func7 = 0b000;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_srai) {
            int func3 = 0b101;
            int func7 = 0b0100000;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            machine_code += func7 << 25;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_andi) {
            int func3 = 0b111;
            int opcode = 0b0010011;
            int imm = ((get_bits(rvc_code,12,12) << 5) + get_bits(rvc_code,6,1));
            int rd = get_bits(rvc_code, 9, 7) + 8;
            int rs1 = rd;
            machine_code += imm << 20;
            machine_code += rs1 << 15;
            machine_code += func3 << 12;
            machine_code += rd << 7;
            machine_code += opcode;
        }
        if (element.instr_type == c_j) {
            int opcode = 0b1101111;
            int rd = 0;
            int offset = cal_cj_imm(rvc_code);
            int jump_bytes = 0; /*calculate the offset */
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
            machine_code += opcode;
            machine_code += rd << 7;
            machine_code += get_bits(jump_bytes,19,12) << 12;
            machine_code += get_bits(jump_bytes, 11,11) << 20;
            machine_code += get_bits(jump_bytes, 10, 1) << 21;
            machine_code += get_bits(jump_bytes, 20, 20) << 31;
        }
    }
    return machine_code;
}