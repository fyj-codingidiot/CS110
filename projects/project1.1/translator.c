/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you 
    should define and implement your own functions in translator.c, compression.c, 
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"

/*check if file can be correctly opened */
static int open_files(FILE** input, FILE** output, const char* input_name, const char* output_name){ 
    *input = fopen(input_name, "r");
    if (!*input){ /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output){ /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE** input, FILE** output){
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit() {
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}


/*Run the translator 
*/
const char* Format1_name[] = {"R", "I", "U", "S", "SB", "UJ"};
const char* Format2_name[] = {"CR", "CI", "CL", "CS", "CB", "CJ"};
const char* INSTR_TYPE_name[] = {
    "add", "and", "or", "xor", "sub", /* RISCV: R Format */
    "jalr", "addi", "slli", "lw", "srli", "srai", "andi", /* RISCV: I Format */
    "lui", "sw", "beq", "bne", "jal", /* RISCV: U,S,SB,UJ Format */
    "c_add", "c_mv", "c_jr", "c_jalr", /* RVC: CR Format */
    "c_li", "c_lui", "c_addi", "c_slli", /* RVC: CI Format */
    "c_lw", "c_j", "c_jal", /* RVC: CL,CJ Format */
    "c_sw", "c_and", "c_or", "c_xor", "c_sub", /* RVC: CS Format */
    "c_beqz", "c_bnez", "c_srli", "c_srai", "c_andi", /* RVC: CB Format */
    "other_instr" /* Other instructions */
};
void binary_print(unsigned int n) 
{ 
	if(n) /* Judge */
		binary_print(n/2); 
	else /* Return */
		return; 
	printf("%d",n%2); /* last bit */
}
/* translate defination */
int translate(const char*in, const char*out){
    FILE *input, *output;
    int err = 0;
    char* str_line;     /* char* to store the content of one line */
    unsigned int* all_code;     /* unsigned int* to store the content of all file */
    unsigned int code_line;      /* the machine code of one line */
    unsigned int cnt_line = 0;      /* record the number of lines */
    unsigned int* pre_sum;      /* prefix sum array to record the number of compressible instructions */
    size_t i;   /* loop iterator */
    RISC_V* riscv;
    RVC* rvc;
    /* int i; */
    if (in){    /* correct input file name */
        if(open_files(&input, &output, in, out) != 0)
            exit(1);
        pre_sum = (unsigned int*)calloc(MAX_LINE_NUM, sizeof(unsigned int)); /* allocate memory for pre_sum */
        all_code = (unsigned int*)calloc(MAX_LINE_NUM, sizeof(unsigned int)); /* allocate memory for all_code */
        str_line = (char*)malloc(LINE_MAX_LENGTH * sizeof(char)); /* allocate memory for string */
        while (!feof(input)) {
            memset(str_line, 0, LINE_MAX_LENGTH * sizeof(char));    /* initialize the str_line */
            fgets(str_line, LINE_MAX_LENGTH, input);    /* read one line */
            if (str_line[0] != '0' && str_line[0] != '1') {   /* check whether the str is machine code */
                break;
            }
            str_line[32] = '\0';    /* overwrite '\n' */
            code_line = str_to_machine(str_line);   /* caculate the machine code */
            riscv = machine_to_riscv(code_line);    /* translate machine code to RISC-V object*/
            cnt_line ++;        /* update cnt_line */
            all_code[cnt_line] = code_line;       /* save the machine code */
            pre_sum[cnt_line] = pre_sum[cnt_line - 1];      /* pre_sum move */
            if (check_compressible(riscv)) {
                pre_sum[cnt_line] ++;   /* if compressible, add 1 */
            }
            riscv_delete(riscv);   /* delete RISC-V object */
            printf("%s| pre_sum: %d \n", str_line, pre_sum[cnt_line]);
        }
        /* write correct result to the output file */
        for (i = 1; i <= cnt_line; i++) {   /* tranverse the str array */
            memset(str_line, 0, LINE_MAX_LENGTH * sizeof(char)); /* initialize str_line */
            code_line = all_code[i];        /* get the ith line machine code (index start from 1)*/
            riscv = machine_to_riscv(code_line);    /* translate machine code to RISC-V object*/
            if (pre_sum[i] == pre_sum[i-1]) {       /* current line is not compressible */
                if (riscv->format == SB  || riscv->format == UJ) {   /* May need offset adjustment*/
                    code_line = adjust_offset_uncompressible(riscv, i, pre_sum);    /* adjust offset */
                }
                machine_to_str_32(code_line, str_line); /* return to string */
                fprintf(output, "%s\n", str_line);      /* print to file */
                riscv_delete(riscv);   /* delete RISC-V object */
                continue;
            }
            rvc = riscv_to_rvc(riscv, i, pre_sum);      /* Tranverse RISV-V to RVC */
            code_line = rvc_to_machine(rvc);        /* calculate machine code */
            memset(str_line, 0, LINE_MAX_LENGTH * sizeof(char));        /* reset the str_line */
            machine_to_str_16(code_line, str_line);        /* get the final answer */
            fprintf(output, "%s\n", str_line);          /* print to file */
            riscv_delete(riscv);        /* delete RISC-V object */
            rvc_delete(rvc);            /* delete RVC object */
        }
        free(str_line);     /* delete str_line */
        free(pre_sum);      /* delete pre_sum */
        free(all_code);      /* delete all line */
        close_files(&input, &output);    /* close file stream */
    }
    return err;
}

/* main func */
int main(int argc, char **argv){
    char* input_fname, *output_fname;
    int err;
    
    if (argc != 3) /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}
