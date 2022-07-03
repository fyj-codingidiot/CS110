#ifndef UTILS_H
#define UTILS_H

/* define bool type */
typedef int bool;
#define true (1)
#define false (0)

/* define the formats of two instruction set */
typedef enum RISCV_FORMAT_ {R, I, U, S, SB, UJ} RISCV_FORMAT;
typedef enum RVC_FORMAT_ {CR, CI, CL, CS, CB, CJ} RVC_FORMAT;

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

/* define the struct of RISC-V */
typedef struct RISC_V_
{
    RISCV_FORMAT format;    /* Store the format of RISC-V instruction */
    INSTR_TYPE ins_type;    /* Store the type of RISC-V instruction */
    unsigned int* m_code;    /* Store the splited machine code pieces */
    int n_pieces;    /* Store the number of pieces */
} RISC_V;

/* define the struct of RVC */
typedef struct RVC_
{
    RVC_FORMAT format;      /* Store the format of RVC instruction */
    INSTR_TYPE ins_type;    /* Store the type of RVC instruction */
    unsigned int* m_code;    /* Store the splited machine code pieces */
    int n_pieces;    /* Store the number of pieces */
} RVC;

/* Translate string to binary machine_code */
unsigned int str_to_machine(char* str);

/* Translate 16bit machine_code to string */
void machine_to_str_16(unsigned int machine_code, char* str);

/* Translate 32bit machine_code to string */
void machine_to_str_32(unsigned int machine_code, char* str);

/* Get the [l:r] bits of 32-bit machine_code */
unsigned int get_bits(unsigned int machine_code, int l, int r);

/* Translate machine code to RISC-V object */
RISC_V* machine_to_riscv(unsigned int machine_code);

/* Translate RVC object to machine code */
unsigned int rvc_to_machine(RVC* rvc);

/* Delete a RISC-V object and free the memory allocated */
void riscv_delete(RISC_V* riscv);

/* Delete a RVC object and free the memory allocated */
void rvc_delete(RVC* rvc);

/* Compress RISC-V object to RVC object */
RVC* riscv_to_rvc(RISC_V* riscv, unsigned int line_idx, unsigned int* pre_sum);

/* Check whether the RISC-V object is compressible */
bool check_compressible(RISC_V* riscv);

/*  */
unsigned int adjust_offset_uncompressible(RISC_V* riscv, unsigned int line_idx, unsigned int* pre_sum);

/* Check whether the Register is between x8-x15 */
bool check_register(unsigned int r_tag);

/* Calculate the imm of itype instruction */
int cal_i_imm(RISC_V* riscv);

/* Calculate the imm of stype instruction */
int cal_s_imm(RISC_V* riscv);

/* Calculate the imm of sbtype instruction */
int cal_sb_imm(RISC_V* riscv);

/* Calculate the imm of utype instruction */
int cal_u_imm(RISC_V* riscv);

/* Calculate the imm of ujtype instruction */
int cal_uj_imm(RISC_V* riscv);
#endif