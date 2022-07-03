# Hash

```c
struct risc_v_or_rvc
{
    int Type; // 0 or 1 : risc-v/rvc
    int Format;
    int instr_type;
};
```
```c
/* define the instr type of two instruction set */
typedef enum INSTR_TYPE_ {
    c_add, c_mv, c_jr, c_jalr, /* RVC: CR Format */
    c_li, c_lui, c_addi, c_slli, /* RVC: CI Format */
    c_lw, c_j, c_jal, /* RVC: CL,CJ Format */
    c_sw, c_and, c_or, c_xor, c_sub, /* RVC: CS Format */
    c_beqz, c_bnez, c_srli, c_srai, c_andi, /* RVC: CB Format */
    other_instr /* Other instructions */
} INSTR_TYPE;
```

| Instruction name  | Hash code |
| :---------------- | --------- |
| other_instruction | 0         |
| c.add             | 1         |
| c.mv              | 2         |
| c.jr              | 3         |
| c.jalr            | 4         |
| c.li              | 5         |
| c.lui             | 6         |
| c.addi            | 7         |
| c.slli            | 8         |
| c.lw              | 9         |
| c.j               | 10        |
| c.jal             | 11        |
| c.sw              | 12        |
| c.and             | 13        |
| c.or              | 14        |
| c.xor             | 15        |
| c.sub             | 16        |
| c.beqz            | 17        |
| c.bnez            | 18        |
| c.srli            | 19        |
| c.srai            | 20        |
| c.andi            | 21        |


```c
typedef enum RISCV_FORMAT_ {R, I, U, S, SB, UJ} RISCV_FORMAT;
typedef enum RVC_FORMAT_ {CR, CI, CL, CS, CB, CJ} RVC_FORMAT;
```

| RISC-V Format | Hash Code |
| ------------- | --------- |
| Other_Format  | 0         |
| SB            | 1         |
| UJ            | 2         |


| RVC Format | Hash Code |
| ---------- | --------- |
| CR         | 0         |
| CI         | 1         |
| CL         | 2         |
| CS         | 3         |
| CB         | 4         |
| CJ         | 5         |
| CA         | 6         |

