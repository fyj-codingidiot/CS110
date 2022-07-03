#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.
main:
    # save the address of rvc code array
    la s8, rvc_codes

    # allocate memory needed
    # allocate memory for result code (s9)
    lw t0, lines_of_rvc_codes

    mv a1, t0       # set nitems
    li a2, 4        # set size of each item
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 2        # set a6 2 for ecall (calloc)
    ecall           # calloc
    mv s9, a0       # store the return value to s9

    # allocate memory for struct element (s10)
    li a2, 12        # set size of each item
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 2        # set a6 2 for ecall (calloc)
    ecall           # calloc
    mv s10, a0       # store the return value to s10

    # allocate memory for prefix array (s11)
    addi a1, a1, 1  # allocate 1 more for 0
    li a2, 4        # set size of each item
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 2        # set a6 2 for ecall (calloc)
    ecall           # calloc
    mv s11, a0       # store the return value to s11
    addi s11, s11, 4 # the address of first element

    # address of rvc_codes in s8, address of code in s9, 
    # address of struct in s10, address of prefix array in s11
    # store all instructions as 32-bit struct format && original code, and calculate the struct member
    jal ra, infor_extract
    # jal ra, print_all
    # rvc machine code in a2, struct element address in a3, line_idx in a4, return risc-v machine code in a1, do extention and update offset for single line
    li t1, 0
    rvc_loop:
        beq t1, t0, rvc_done
        slli t2, t1, 2  # address offset
        add t3, s9, t2  # calculate the address of rvc/riscv code
        lw a2, 0(t3)    # load to a2 to pass to rvc_extend
        li t3, 12
        mul t2, t1, t3  # address offset of struct array
        add t3, s10, t2  # calculate the address of struct element
        mv a3, t3       # move to a3 to pass to rvc_extend
        mv a4, t1       # line_idx in a4 to pass to rvc_extend
        jal ra, rvc_extend  # call rvc_extend

        # mv a2, a1
        # jal ra, print_riscv

        # addi a1, x0, '\n'   # print a newline
        # addi a0, x0, 11
        # ecall

        slli t2, t1, 2  # address offset
        add t2, s9, t2  # calculate the address of result
        sw a1, 0(t2)
        addi t1, t1, 1  # increase 1
        j rvc_loop
    rvc_done:

    jal print_all
    # free all memory allocated
    # free result code (s9)
    mv a1, s9       # mv address to a1 for ecall
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 4        # set a6 4 for ecall (free)
    ecall

    # free struct element array (s10)
    mv a1, s10      # mv address to a1 for ecall
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 4        # set a6 4 for ecall (free)
    ecall

    # free prefix array (s11)
    addi s11, s11, -4 # address of 0
    mv a1, s11      # mv address to a1 for ecall
    li a0, 0x3CC    # set a0 0x3CC for ecall
    li a6, 4        # set a6 4 for ecall (free)
    ecall

    # Exit with error code 0
    add	   a1, x0, x0
    addi   a0, x0, 17
    ecall

print_all:
	# Save the ra in the stack since it will be overwritten
	addi sp, sp, -16
	sw ra, 0(sp)
    # Save other registers
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t3, 12(sp)

    lw t0, lines_of_rvc_codes    # load the number of lines to t0
    slli t0, t0, 2  # multiply 4 
    # li t3, 12
    # mul t0, t0, t3
    li t1, 0        # set iterator 0
    pa_loop:
        beq t1, t0, pa_done # loop end conditon
        add t3, s9, t1      # calculate the address of risc-v code
        lw a2, 0(t3)        # load it to a2
        # add t3, s10, t1
        # lw a2, 8(t3)
        jal ra, print_riscv # print a single line of risc-v code
        addi a1, x0, '\n'   # print a newline
        addi a0, x0, 11
        ecall
        addi t1, t1, 4      # increase the iterator
        j pa_loop
    pa_done:
	# Get back saved return address from stack and go to it
	lw ra, 0(sp)
    # recover other registers
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t3, 12(sp)
	addi sp, sp, 16 # recover sp pointer
	jr ra

# risc-v machine code in a2, output it
print_riscv:
    # Save the registers in the stack since it will be overwritten
	addi sp, sp, -24
	sw t2, 0(sp)
    # Save other registers
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t3, 12(sp)   # t3
    sw t4, 16(sp)
    sw t5, 20(sp)

    li t0, 32   # length of risc-v code
    li t1, 0    # set iterator 0
    li t2, 0x80000000   # the and 'tool'
    prv_loop:
        beq t1, t0, prv_done    # loop end condition
        and t3, a2, t2          # to get the single bit
        li t4, 31
        sub t5, t4, t1
        srl t3, t3, t5
        addi a1, t3, '0'        # add '0' to get ASCII value
        li a0, 11
        ecall                   # print the single bit
        srli t2, t2, 1          # update and tool
        addi t1, t1, 1          # increase iterator
        j prv_loop
    prv_done:   # done
    lw t2, 0(sp)
    # recover other registers
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t3, 12(sp)   # t3
    lw t4, 16(sp)
    lw t5, 20(sp)
	addi sp, sp, 24 # recover sp pointer
    jr ra   # return to ra

# address of rvc_codes in s8, address of code in s9, 
# address of struct in s10, address of prefix array in s11
# edit the value in memory
infor_extract:
    # Save the registers in the stack since it will be overwritten
    addi sp sp -40  # adjust sp
    sw  t4  36(sp)  # t4
    sw  t3  32(sp)
    sw  t2  28(sp)
    sw  t1  24(sp)  # t1
    sw  t0  20(sp)
    sw  ra  16(sp)
    sw  s7  12(sp)  # s7
    sw  s6  8(sp)
    sw  s5  4(sp)
    sw  s4  0(sp)   # s4

    lw  s7  lines_of_rvc_codes
    mv s5  zero # the index of the structure
    mv t0 zero # pos = 0
    mv t1 zero #temp = 0
    mv t2 zero # i = 0
    infor_loop:
        bge t2  s7  exit_infor_loop # if i>=rvc_code_line,then finishes the loop
        slli s6  t0 1  #pos*2
        add  s6  s8  s6 #get the address of rvc_code[pos]
        lhu  s6    0(s6) # load the unsigned short to s6,ie first = rvc_code[pos]
        andi  t4  s6  3 # first & 3   
        li t3 3        #0b11
        case_riscv:
            bne  t4  t3  case_rvc # case of riscv
            mv   t3 zero  # t3 = 0
            addi t3 t0 1  #pos+1
            slli t3 t3 1 #(pos+1)*2
            add  t3 s8 t3 #get the address of rvc_code[pos+1]
            lhu  t3  0(t3) #second = rvc_code[pos+1]
            slli t3  t3  16 #second<<16
            or   t3  t3  s6 #(second<<16)|first
            mv  t4  t2  
            slli t4  t4  2 # i*4
            add  t4  s9  t4 #get the address of code[i]
            sw   t3  0(t4)  #code[i] = (second<<16)|first
            add  s4  s5  s10 #the address of structure[i]
            sw   zero  0(s4) #struct[i].type = 0
            mv  t4  t2  
            slli t4  t4  2 # i*4
            add  t4  s11  t4 #the address of prefix[i]
            sw   t1  0(t4) #prefix[i] = temp
            addi t0 t0 2 #pos+=2
            mv  a2  t3 #a2 = mcode
            li  a3  6 # l = 6
            li  a4  0  #r=0
            jal ra get_bits #cal get_bits
            case_SB:
                li t4 0x063
                bne a1 t4 case_UJ 
                li t4 1  #format =1
                sw t4 4(s4) 
                sw zero 8(s4)#instr = 0
                addi t2 t2 1
                addi s5 s5 12
                j infor_loop    # jump
            case_UJ:
                li t4 0x06F
                bne a1 t4 case_other_riscv
                li t4 2
                sw t4 4(s4)#format =2
                sw zero 8(s4)#instr = 0
                addi t2 t2 1
                addi s5 s5 12
                j infor_loop    # jump
            case_other_riscv:
                sw zero 4(s4)#format =0
                sw zero 8(s4)#instr = 0
                addi t2 t2 1
                addi s5 s5 12
                j infor_loop    # jump
        case_rvc:    
            mv  t4  t2  
            slli t4  t4  2 # i*4
            add  t4  s9  t4 #get the address of code[i]
            sw   s6   0(t4) #code[i] = first
            addi t1 t1 1
            mv  t4  t2  
            slli t4  t4  2 # i*4
            add  t4  s11  t4 #the address of prefix[i]
            sw   t1  0(t4) #prefix[i] = temp
            addi t0 t0 1 #pos+=1
            add  s4  s5  s10 #the address of structure[i]
            li t4 1
            sw  t4  0(s4) #struct[i].type = 1
            mv  a2  s6 #mcode=first
            li  a3  1  #l=1
            li  a4  0  #r = 0
            jal ra get_bits
            case_CR_or_c_slli:
                li  t4  2
                bne a1  t4  case_CL_or_CS
                mv  a2  s6 #mcode=first
                li  a3  15  #l=15
                li  a4  12  #r = 12
                jal ra get_bits
                case_c_add_or_c_jalr:
                    li t4  9
                    bne a1 t4 case_c_mv_or_c_jr
                    mv  a2  s6 #mcode=first
                    li  a3  6  #l=1
                    li  a4  2  #r = 0
                    jal ra get_bits
                    sw  zero 4(s4) #struct[i].format = 0
                    case_c_jalr:
                        bne a1 zero case_c_add
                        li t4 4
                        sw t4 8(s4) #struct[i].type = 4
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                    case_c_add:
                        li t4 1
                        sw t4 8(s4) #struct[i].type = 1
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                case_c_mv_or_c_jr:
                    li t4 8
                    bne a1 t4  case_c_slli
                    mv  a2  s6 #mcode=first
                    li  a3  6  #l=6
                    li  a4  2  #r = 2
                    jal ra get_bits
                    sw  zero 4(s4) #struct[i].format = 0
                    case_c_jr:
                        bne a1 zero case_c_mv
                        li t4 3
                        sw t4 8(s4) #struct[i].type = 3
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                    case_c_mv:
                        li t4 2
                        sw t4 8(s4) #struct[i].type = 2
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                case_c_slli:
                        li t4 1
                        sw t4 4(s4) #struct[i].format = 1
                        li t4 8
                        sw t4 8(s4) #struct[i].type = 8
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
            case_CL_or_CS:
                bne a1 zero case_CI_CA_CB_CJ
                mv  a2  s6 #mcode=first
                li  a3  15  #l=15
                li  a4  13  #r = 13
                jal ra get_bits
                case_c_lw:
                    li t4 2
                    bne a1 t4 case_c_sw
                    li t4 2
                    sw t4 4(s4) #struct[i].format = 2
                    li t4 9
                    sw t4 8(s4) #struct[i].type = 9
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_sw:
                    li t4 3
                    sw t4 4(s4) #struct[i].format = 3
                    li t4 12
                    sw t4 8(s4) #struct[i].type = 12
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
            case_CI_CA_CB_CJ:
                mv  a2  s6 #mcode=first
                li  a3  15  #l=15
                li  a4  13  #r = 13
                jal ra get_bits
                case_c_li:
                    li t4 2
                    bne a1 t4 case_c_lui
                    li t4 1
                    sw t4 4(s4) #struct[i].format = 1
                    li t4 5
                    sw t4 8(s4) #struct[i].type = 5
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_lui:
                    li t4 3
                    bne a1 t4 case_c_addi
                    li t4 1
                    sw t4 4(s4) #struct[i].format = 1
                    li t4 6
                    sw t4 8(s4) #struct[i].type = 6
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_addi:
                    bne a1 zero case_c_beqz
                    li t4 1
                    sw t4 4(s4) #struct[i].format = 1
                    li t4 7
                    sw t4 8(s4) #struct[i].type = 7
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_beqz:
                    li t4  6
                    bne a1 t4 case_c_bnez
                    li t4 4
                    sw t4 4(s4) #struct[i].format = 4
                    li t4 17
                    sw t4 8(s4) #struct[i].type = 17
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_bnez:
                    li t4  7
                    bne a1 t4 case_c_j
                    li t4 4
                    sw t4 4(s4) #struct[i].format = 4
                    li t4 18
                    sw t4 8(s4) #struct[i].type = 18
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_j:
                    li t4 5
                    bne a1 t4 case_c_jal
                    li t4 5
                    sw t4 4(s4) #struct[i].format = 5
                    li t4 10
                    sw t4 8(s4) #struct[i].type = 10
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_c_jal:
                    li t4 1
                    bne a1 t4 case_other_rvc
                    li t4 5
                    sw t4 4(s4) #struct[i].format = 5
                    li t4 11
                    sw t4 8(s4) #struct[i].type = 11
                    addi t2 t2 1
                    addi s5 s5 12
                    j infor_loop    # jump
                case_other_rvc:
                    mv  a2  s6 #mcode=first
                    li  a3  11  #l=15
                    li  a4  10  #r = 12
                    jal ra get_bits
                    case_c_srli:
                        bne a1 zero case_c_srai
                        li t4 4
                        sw t4 4(s4) #struct[i].format = 4
                        li t4 19
                        sw t4 8(s4) #struct[i].type = 19
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                    case_c_srai:
                        li t4 1
                        bne a1 t4 case_c_andi
                        li t4 4
                        sw t4 4(s4) #struct[i].format = 4
                        li t4 20
                        sw t4 8(s4) #struct[i].type = 20
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                    case_c_andi:
                        li t4 2
                        bne a1 t4 case_CA
                        li t4 4
                        sw t4 4(s4) #struct[i].format = 4
                        li t4 21
                        sw t4 8(s4) #struct[i].type = 21
                        addi t2 t2 1
                        addi s5 s5 12
                        j infor_loop    # jump
                    case_CA:
                        mv  a2  s6 #mcode=first
                        li  a3  6  #l=6
                        li  a4  5  #r = 5
                        jal ra get_bits
                        li t4 6
                        sw t4 4(s4) #struct[i].format = 6
                        case_c_and:
                            li t4 3
                            bne a1 t4 case_c_or
                            li t4 13
                            sw t4 8(s4) #struct[i].type = 13
                            addi t2 t2 1
                            addi s5 s5 12
                            j infor_loop    # jump
                        case_c_or:
                            li t4 2
                            bne a1 t4 case_c_xor
                            li t4 14
                            sw t4 8(s4) #struct[i].type = 14
                            addi t2 t2 1
                            addi s5 s5 12
                            j infor_loop    # jump
                        case_c_xor:
                            li t4 1
                            bne a1 t4 case_c_sub
                            li t4 15
                            sw t4 8(s4) #struct[i].type = 15
                            addi t2 t2 1
                            addi s5 s5 12
                            j infor_loop    # jump
                        case_c_sub:
                            li t4 16
                            sw t4 8(s4) #struct[i].type = 16
                            addi t2 t2 1
                            addi s5 s5 12
                            j infor_loop    # jump
    exit_infor_loop:
    # recover other registers
        lw  s4  0(sp)
        lw  s5  4(sp)
        lw  s6  8(sp)
        lw  s7  12(sp)  # s7
        lw  ra  16(sp)
        lw  t0  20(sp)
        lw  t1  24(sp)  # t1
        lw  t2  28(sp)
        lw  t3  32(sp)
        lw  t4  36(sp)  # t4
        addi sp sp 40
        jr ra

# rvc machine code in a2, struct element address in a3, line_idx in a4, return risc-v machine code in a1, do extention and update offset for single line
rvc_extend:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -56    # adjust sp
	sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)   # s2
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw t0, 24(sp)   # t0
    sw t1, 28(sp)
    sw t2, 32(sp)
    sw t3, 36(sp)   # t3
    sw t4, 40(sp)
    sw t5, 44(sp)
    sw t6, 48(sp)   # t6
    sw s3, 52(sp)

    mv s0, a2   # store machine code in local variable
    mv s1, a3   # store address in local variable
    mv s2, a4   # store line_idx in local variable

    lw s3, 0(s1) # elememt.Type

    bne s3, x0, rvc_case  # if (element.Type == riscv)
    riscv_case:
        lw s4, 4(s1) # element.Format
        beq s4, x0, OT_case
        li t0, 1
        beq s4, t0, SB_case # else if riscv.format == SB
        j UJ_case       # else UJ case
        OT_case:
            mv a1, a2   # not do any modification
            j rvc_extend_return # return
        SB_case:
            jal ra, cal_sb_imm
            mv t0, a1   # t0 = cal_sb_imm
            li t1, 0    # jump_lines = 0
            bge t0, x0, sb_positive_offset  # offset > 0
            sb_negative_offset:    
                sb_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines - 1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)
                    beq t0, t4, sb_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j sb_negative_offset_loop
                sb_negative_offset_done:
                slli s5, t1, 2      # s5 = jump_bytes = jump_lines * 4
                li t0, -1
                mul s5, s5, t0      # multiply -1
                li  t0, 0x1FFF07F   # set large mask
                and s0, s0, t0  # and a mask to erase original imm
                mv a2, s5       # let a2 = s5 to pass to get_bits
                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s0, s0, a1   # machine_code |= return_value  << 7

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s0, s0, a1   # machine_code |= return_value  << 8

                li a3, 10       # l = 10
                li a4, 5        # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s0, s0, a1   # machine_code |= return_value  << 25

                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s0, s0, a1   # machine_code |= return_value  << 31
                mv a1, s0       # let a1 = s0 and return
                j rvc_extend_return
            sb_positive_offset:
                sb_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, sb_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j sb_positive_offset_loop
                sb_positive_offset_done:
                slli s5, t1, 2      # s5 = jump_bytes = jump_lines * 4
                li  t0, 0x1FFF07F   # set large mask
                and s0, s0, t0  # and a mask to erase original imm
                mv a2, s5       # let a2 = s5 to pass to get_bits
                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s0, s0, a1   # machine_code |= return_value  << 7

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s0, s0, a1   # machine_code |= return_value  << 8

                li a3, 10       # l = 10
                li a4, 5        # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s0, s0, a1   # machine_code |= return_value  << 25

                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s0, s0, a1   # machine_code |= return_value  << 31
                mv a1, s0       # let a1 = s0 and return
                j rvc_extend_return
        UJ_case:
            jal ra, cal_uj_imm
            mv t0, a1   # t0 = cal_uj_imm
            li t1, 0    # jump_lines = 0
            bge t0, x0, uj_positive_offset  # offset > 0
            uj_negative_offset:    
                uj_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines - 1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)

                    beq t0, t4, uj_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j uj_negative_offset_loop
                uj_negative_offset_done:
                slli s5, t1, 2      # s5 = jump_bytes = jump_lines * 4
                li t0, -1
                mul s5, s5, t0      # multiply -1
                li t0, 0x00000FFF   # set large mask
                and s0, s0, t0 # and a mask to erase original imm
                mv a2, s5       # let a2 = s5 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s0, s0, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s0, s0, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s0, s0, a1   # machine_code |= return_value  << 21

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s0, s0, a1   # machine_code |= return_value  << 31
                mv a1, s0       # let a1 = s0 and return
                j rvc_extend_return
            uj_positive_offset:
                uj_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, uj_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j uj_positive_offset_loop
                uj_positive_offset_done:
                slli s5, t1, 2      # s5 = jump_bytes = jump_lines * 4
                li t0, 0x00000FFF   # set large mask
                and s0, s0, t0 # and a mask to erase original imm
                mv a2, s5       # let a2 = s5 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s0, s0, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s0, s0, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s0, s0, a1   # machine_code |= return_value  << 21

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s0, s0, a1   # machine_code |= return_value  << 31
                mv a1, s0       # let a1 = s0 and return
                j rvc_extend_return
    rvc_case:
        lw s4, 8(s1) # element.instr_type
        # remember j rvc_extend_return
        c_add_case:
            li t0 1 # c_add hash value is 1
            bne s4 t0 c_mv_case # judge if it is c_add
            li  t0 51 # use t0 to store opcode which is 0b0110011
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            mv t3 t1 # use t3 to store rs1
            li a3 6 # a3 = l
            li a4 2 #a4 = r
            jal ra get_bits #call get_bits to get rs2
            mv t2 a1 #use t2 to store rs2
            mv a1 zero #machine code = 0
            add a1 a1 zero # machine code+=func7 and func7=0
            slli t2 t2 20 #rs2<<20
            add a1 a1 t2 # machine code += rs2<<20
            slli t3 t3 15 # rs1<<15
            add  a1 a1 t3 #machine code += rs1 << 15
            add  a1 a1 zero # machine code += func3 and  func3 = 0
            slli t1 t1 7 # rd<<7
            add a1 a1 t1 # machine code += rd<<7
            add a1 a1 t0 # machine code += opcode
            j rvc_extend_return
        c_mv_case:
            li t0 2 #c_mv hash value is 2
            bne s4 t0 c_jr_case
            li t0 51 #use t0 to store opcode which is 0b0110011
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            li a3 6 # a3 = l
            li a4 2 #a4 = r
            jal ra get_bits #call get_bits to get rs2
            mv t2 a1 #use t2 to store rs2
            mv a1 zero #machine code = 0
            add a1 a1 zero # machine code += func7 and func7 is 0
            slli t2 t2 20 #rs2<<20
            add a1 a1 t2 # machine code += rs2<<20
            add a1 a1 zero #machine code += rs1 and rs1  = 0
            add a1 a1 zero #machine code +=func3 and func3 = 0
            slli t1 t1 7 # rd<<7
            add a1 a1 t1 #machine code += rd<<7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_jr_case: 
            li t0 3 #c_jr hash value is 3
            bne s4 t0 c_jalr_case
            li t0 103 #use t0 to store opcode which is 0b1100111
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rs1
            mv t1 a1 #use t1 to store rs1
            mv a1 zero #machine code = 0
            add a1 a1 zero #machine code += imm  and imm = 0
            slli t1 t1 15 #rs1<15
            add a1, a1, t1  # machine_code += rs1
            add a1 a1 zero #machine code += func3 and func3 = 0
            add a1 a1 zero #machine code += rd and rd = 0
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_jalr_case:
            li t0 4 #c_jalr hash value is 4
            bne s4 t0 c_li_case
            li t0 103 # use t0 to store opcode which is 0b1100111
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rs1
            mv t1 a1 #use t1 to store rs1
            li t2 1 # rd =1
            mv a1 zero #machine code = 0
            add a1 a1 zero # machine code +=imm and  imm = 0
            slli t1 t1 15 # rs1<<15
            add a1 a1 t1 # machine code += rs1<<15
            add a1 a1 zero #machine code +=func3 and func3 = 0
            slli t2 t2 7 # rd<<7
            add a1 a1 t2 # machine code += rd<<7
            add a1 a1 t0 # machine code += opcode
            j rvc_extend_return
        c_li_case:
            li t0 5 #c_li hash value is 5
            bne s4 t0 c_lui_case
            li t0 19 #use t0 to store opcode which is 0b0010011
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm #get the imm
            mv  t3  a1 #use t3 to store the imm
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t4 a1 #use t4 to store rd   
            add a1 zero zero #machine code = 0
            slli t3 t3 20 # imm<<20
            add a1 a1 t3 #machine code += imm<<20
            add a1 a1 zero #machine code += rs1 and rs1 = 0
            add a1 a1 zero #machine code += func3 and func3 = 0
            slli t4 t4 7 # rd<<7
            add a1 a1 t4 #machine code += rd <<7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_lui_case:
            li t0 6
            bne s4 t0 c_addi_case
            li t0 55 # use t0 to store opcode which is 0b0110111
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm #get the imm
            mv  t3  a1 #use t3 to store the imm
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t4 a1 #use t4 to store rd   
            slli t3 t3 12 # imm<<12
            add a1 zero zero #machine code = 0
            add a1 a1 t3 #machine_code+=imm<<12
            slli t4 t4 7 # rd <<7
            add a1 a1 t4 #machine code += rd<<7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_addi_case:
            li t0 7 #c_addi hash value is 7
            bne s4 t0 c_slli_case
            li t0 19 # use t0 to store opcode which is 0b0010011
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm #get the imm
            mv  t3  a1 #use t3 to store the imm
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t4 a1 #use t4 to store rd   
            mv t5 t4 #use t5 to store rs1
            add a1 zero zero #machine code  = 0
            slli t3 t3 20 # imm<<20
            add a1 a1 t3 #machine code+=imm<<20
            slli t5 t5 15 #rs1<<15
            add a1 a1 t5 # machine code += rs1<<15
            add a1 a1 zero #machine code+=func3 and func3 is 0
            slli t4 t4 7 #rd<<7
            add a1 a1 t4 #machine code += rd<<7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_slli_case:
            li t0 8
            bne s4 t0 c_lw_case
            li t0 19 #use t0 to store opcode which is 0b0010011
            li t6 1 # use t6 to store func3 which is 1
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm #get the imm
            mv  t3  a1 #use t3 to store the imm
            mv a2 s0 # a2=origin number
            li a3 11 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t4 a1 #use t4 to store rd   
            mv t5 t4 #use t5 to store rs1
            add a1 zero zero #machine code = 0
            slli t3 t3 20 # imm<<20
            add a1 a1 t3 #machine code+=imm<<20
            slli t5 t5 15 #rs1<<15
            add a1 a1 t5 # machine code += rs1<<15
            slli t6 t6 12 #func3<<12
            slli t4 t4 7 #rd<<7
            add  a1 a1 t6 #machine code +=func3<<7
            add a1 a1 t4 #machine code += rd<<7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_lw_case:
            li t0 9
            bne s4 t0 c_sw_case
            mv a2 s0 # a2=origin number
            li a3 5 # a3 = l
            li a4 5 # a4 = r
            jal ra get_bits #call get bits to get imm[5]
            mv t1 a1 #use t1 to store imm[5]
            mv a2 s0 # a2=origin number
            li a3 12 # a3 = l
            li a4 10 # a4 = r
            jal ra get_bits #call get bits to get imm[12:10]
            mv t2 a1 #use t2 to store imm[12:10]
            mv a2 s0 # a2=origin number
            li a3 6 # a3 = l
            li a4 6 # a4 = r
            jal ra get_bits #call get bits to get imm[6]
            mv t3 a1 #use t3 to store imm[6]
            slli t1 t1 6 # imm[5]<<6
            slli t2 t2 3 # imm[12:10]<<3
            slli t3 t3 2 # imm[6]<<2
            add t4 t1 t2 # imm += imm[5]<<6+imm[12:10]<<3
            add t4 t4 t3 #imm+=imm[6]<<2
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            addi t1 t1 8 #rd +=8
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rs1
            mv t2 a1 #use t2 to store rs1
            addi t2 t2 8 #use t2 to store rs1
            li t0 3 #opcode = 0b010
            li t3 2 #func3 = 2
            add a1 zero zero #machine code = 0
            slli t4 t4 20 #imm<<20
            add a1 a1 t4 #machine code+=imm<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code +=rs1<<15
            slli t3 t3 12 #func3 <<12
            add a1 a1 t3 #machine code+=func3<<20
            slli t1 t1 7 #rd<<7
            add a1 a1 t1 #machine code+=rd << 7
            add a1 a1 t0 #machine code += opcode
            j rvc_extend_return
        c_sw_case:
            li t0 12
            bne s4 t0 c_and_case
            mv a2 s0 # a2=origin number
            li a3 5 # a3 = l
            li a4 5 # a4 = r
            jal ra get_bits #call get bits to get imm[5]
            mv t1 a1 #use t1 to store imm[5]
            mv a2 s0 # a2=origin number
            li a3 12 # a3 = l
            li a4 10 # a4 = r
            jal ra get_bits #call get bits to get imm[12:10]
            mv t2 a1 #use t2 to store imm[12:10]
            mv a2 s0 # a2=origin number
            li a3 6 # a3 = l
            li a4 6 # a4 = r
            jal ra get_bits #call get bits to get imm[6]
            mv t3 a1 #use t3 to store imm[6]
            slli t1 t1 6 # imm[5]<<6
            slli t2 t2 3 # imm[12:10]<<3
            slli t3 t3 2 # imm[6]<<2
            add t4 t1 t2 # imm += imm[5]<<6+imm[12:10]<<3
            add t4 t4 t3 #imm+=imm[6]<<2
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rs2
            mv t1 a1 #use t1 to store rs2
            addi t1 t1 8 #rs2 +=8
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rs1
            mv t2 a1 #use t2 to store rs1
            addi t2 t2 8 #use t2 to store rs1
            add a1 zero zero #machine code = 0
            mv  t3 a1 # use a temp reg to store a1
            mv a2 t4 # a2=origin number
            li a3 11 # a3 = l
            li a4 5 # a4 = r
            jal ra get_bits #call get bits to get imm
            mv t5 a1 #use t5 to store imm[11:5]
            mv a1 t3 #give back the value of a1
            slli t5 t5 25 #imm[11:5]<<25
            add a1 a1 t5 #machine code += imm[11:5]<<25
            mv t3 a1
            mv a2 t4 # a2=origin number
            li a3 4 # a3 = l
            li a4 0 # a4 = r
            jal ra get_bits #call get bits to get imm
            mv t5 a1 #use t5 to store imm[4:0]
            slli t5 t5 7 #imm[4:0]<<7
            mv a1 t3
            add a1 a1 t5 #machine code += imm[4:0]<<6
            slli t1 t1 20 # rs2<<20
            add a1 a1 t1 #machine code+=rs2<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code+=rs2<<15
            li t0 2 # use t0 to store func3 which is 0b010
            slli t0 t0 12 #func3<<12
            add a1 a1 t0 # machine code += func3<<12
            li t0 0b0100011 #opcode = 0b011
            add a1 a1 t0 #machine code+= opcode
            j rvc_extend_return
        c_and_case:
            li t0 13
            bne s4 t0 c_or_case
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            addi t1 t1 8
            mv  t2 t1 #rs1 = rd
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rs2
            mv t3 a1 #use t3 to store rs2
            addi t3 t3 8
            add a1 zero zero #machine code = 0
            slli t3 t3 20 #rs2<<20
            add a1 a1 t3 #machine code+= rs2<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code+=rs1<<15
            slli t1 t1 7 #rd<<7
            add a1 a1 t1 #machine code+=rd<<7
            li t0 51 #opcode = 51
            add a1 a1 t0 # machine code+= opcode
            li t0 7 #func3 = 0b111
            slli t0 t0 12 #func3<<12
            add a1 a1 t0 #machine code += func3<<12
            mv t0 zero #func7 = 0
            slli t0 t0 25 #func7<<25
            add a1 a1 t0 #machine code+=func7<<25
            j rvc_extend_return
        c_or_case:
            li t0 14
            bne s4 t0 c_xor_case
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            addi t1 t1 8
            mv  t2 t1 #rs1 = rd
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rs2
            mv t3 a1 #use t3 to store rs2
            addi t3 t3 8
            add a1 zero zero #machine code = 0
            slli t3 t3 20 #rs2<<20
            add a1 a1 t3 #machine code+= rs2<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code+=rs1<<15
            slli t1 t1 7 #rd<<7
            add a1 a1 t1 #machine code+=rd<<7
            li t0 51 #opcode = 51
            add a1 a1 t0 # machine code+= opcode
            li t0 6 #func3 = 0b110
            slli t0 t0 12 #func3<<12
            add a1 a1 t0 #machine code += func3<<12
            mv t0 zero #func7 = 0
            slli t0 t0 25 #func7<<25
            add a1 a1 t0 #machine code+=func7<<25
            j rvc_extend_return
        c_xor_case:
            li t0 15
            bne s4 t0 c_sub_case
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            addi t1 t1 8
            mv  t2 t1 #rs1 = rd
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rs2
            mv t3 a1 #use t3 to store rs2
            addi t3 t3 8
            add a1 zero zero #machine code = 0
            slli t3 t3 20 #rs2<<20
            add a1 a1 t3 #machine code+= rs2<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code+=rs1<<15
            slli t1 t1 7 #rd<<7
            add a1 a1 t1 #machine code+=rd<<7
            li t0 51 #opcode = 51
            add a1 a1 t0 # machine code+= opcode
            li t0 4 #func3 = 0b100
            slli t0 t0 12 #func3<<12
            add a1 a1 t0 #machine code += func3<<12
            mv t0 zero #func7 = 0
            slli t0 t0 25 #func7<<25
            add a1 a1 t0 #machine code+=func7<<25
            j rvc_extend_return
        c_sub_case:
            li t0 16
            bne s4 t0 c_beqz_case
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t1 a1 #use t1 to store rd
            addi t1 t1 8
            mv  t2 t1 #rs1 = rd
            mv a2 s0 # a2=origin number
            li a3 4 # a3 = l
            li a4 2 # a4 = r
            jal ra get_bits #call get bits to get rs2
            mv t3 a1 #use t3 to store rs2
            addi t3 t3 8
            add a1 zero zero #machine code = 0
            slli t3 t3 20 #rs2<<20
            add a1 a1 t3 #machine code+= rs2<<20
            slli t2 t2 15 #rs1<<15
            add a1 a1 t2 #machine code+=rs1<<15
            slli t1 t1 7 #rd<<7
            add a1 a1 t1 #machine code+=rd<<7
            li t0 51 #opcode = 51
            add a1 a1 t0 # machine code+= opcode
            li t0 0 #func3 = 0b000
            slli t0 t0 12 #func3<<12
            add a1 a1 t0 #machine code += func3<<12
            li t0 32 #func7 = 0b0100000
            slli t0 t0 25 #func7<<25
            add a1 a1 t0 #machine code+=func7<<25
            j rvc_extend_return
        c_beqz_case:
            li t0, 17
            bne s4, t0, c_bnez_case
            mv a2, s0   # a2 = rvc machine code

            li a3, 9    # l = 9
            li a4, 7    # r = 7
            jal ra, get_bits # get_bits(rvc_code, 9, 7)
            addi t1, a1, 8  # t1 = rs1 = get_bits + 8
            li t2, 0    # t2 = func3 = 0b000
            li t3, 99   # t3 = opcode = 0b1100011

            li s5, 0    # use s5 to store the machine_code
            slli t1, t1, 15 # rs1 << 15
            slli t2, t2, 12 # func3 << 12

            or s5, s5, t1   # add to result
            or s5, s5, t2   # add to result
            or s5, s5, t3   # add to result

            jal ra, cal_cb_imm
            mv t0, a1   # t0 = cal_cb_imm(rvc_code)
            bge t0, x0, beqz_positive_offset  # offset > 0
            beqz_negative_offset:   
                li t1, 0    # set jump_lines 0    
                beqz_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines -1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines -1]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)

                    beq t0, t4, beqz_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j beqz_negative_offset_loop
                beqz_negative_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                li t0, -1
                mul t1, t1, t0      # multiply -1
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31

                li a3, 10        # l = 10
                li a4, 5         # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s5, s5, a1   # machine_code |= return_value  << 25

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
            beqz_positive_offset:
                li t1, 0    # set jump_lines 0    
                beqz_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, beqz_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j beqz_positive_offset_loop
                beqz_positive_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31

                li a3, 10        # l = 10
                li a4, 5         # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s5, s5, a1   # machine_code |= return_value  << 25

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
        c_bnez_case:
            li t0, 18
            bne s4, t0, c_srli_case
            mv a2, s0   # a2 = rvc machine code

            li a3, 9    # l = 9
            li a4, 7    # r = 7
            jal ra, get_bits # get_bits(rvc_code, 9, 7)
            addi t1, a1, 8  # t1 = rs1 = get_bits + 8
            li t2, 1    # t2 = func3 = 0b001
            li t3, 99   # t3 = opcode = 0b1100011

            li s5, 0    # use s5 to store the machine_code
            slli t1, t1, 15 # rs1 << 15
            slli t2, t2, 12 # func3 << 12

            or s5, s5, t1   # add to result
            or s5, s5, t2   # add to result
            or s5, s5, t3   # add to result

            jal ra, cal_cb_imm
            mv t0, a1   # t0 = cal_cb_imm(rvc_code)
            bge t0, x0, bnez_positive_offset  # offset > 0
            bnez_negative_offset:   
                li t1, 0    # set jump_lines 0    
                bnez_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines - 1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)

                    beq t0, t4, bnez_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j bnez_negative_offset_loop
                bnez_negative_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                li t0, -1
                mul t1, t1, t0      # multiply -1
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31

                li a3, 10        # l = 10
                li a4, 5         # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s5, s5, a1   # machine_code |= return_value  << 25

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s5, s5, a1   # machine_code |= return_value  << 7
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
            bnez_positive_offset:
                li t1, 0    # set jump_lines 0    
                bnez_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, bnez_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j bnez_positive_offset_loop
                bnez_positive_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 12       # l = 12
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 12, 12)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31

                li a3, 10        # l = 10
                li a4, 5         # r = 5
                jal ra, get_bits    # get_bits(jump_bytes, 10, 5)
                slli a1, a1, 25
                or s5, s5, a1   # machine_code |= return_value  << 25

                li a3, 4        # l = 4
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 4, 1)
                slli a1, a1, 8
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 11       # l = 11
                li a4, 11       # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 7
                or s5, s5, a1   # machine_code |= return_value  << 7
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
        c_srli_case:
            li t0 19 #srli hash value is 19
            bne s4 t0 c_srai_case
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm   #get the imm      
            mv  t2  a1  #use t2 to store imm
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t3 a1 #use t3 to store rd
            addi t3 t3 8 #rd+=8
            mv t4 t3 #rs1 = rd
            add a1 zero zero #machine code = 0
            slli t2 t2 20 #imm<<20
            add a1 a1 t2 #machine code+=imm<<20
            slli t4 t4 15 #rs1<<15
            add a1 a1 t4 #machine code+=rs1<<15
            slli t3 t3 7#rd<<7
            add a1 a1 t3 #machine code+=rd<<7
            li t0 5#func3 =0b101
            slli t0 t0 12#func3<<12
            add a1 a1 t0 #machine code+=func3<<12
            li t0 19 #opcode=19
            add a1 a1 t0 #machine code+=opcode
            j rvc_extend_return
        c_srai_case:
            li t0 20 #srli hash value is 19
            bne s4 t0 c_andi_case
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm   #get the imm      
            mv  t2  a1  #use t2 to store imm
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t3 a1 #use t3 to store rd
            addi t3 t3 8 #rd+=8
            mv t4 t3 #rs1 = rd
            add a1 zero zero #machine code = 0
            slli t2 t2 20 #imm<<20
            add a1 a1 t2 #machine code+=imm<<20
            slli t4 t4 15 #rs1<<15
            add a1 a1 t4 #machine code+=rs1<<15
            slli t3 t3 7#rd<<7
            add a1 a1 t3 #machine code+=rd<<7
            li t0 5#func3 =0b101
            slli t0 t0 12#func3<<12
            add a1 a1 t0 #machine code+=func3<<12
            li t0 32 #func7=0b0100000
            slli t0 t0 25 #func7<<25
            add a1 a1 t0 #machine code+=func7<<25
            li t0 19 #opcode=19
            add a1 a1 t0 #machine code+=opcode
            j rvc_extend_return
        c_andi_case:
            li t0 21 #srli hash value is 19
            bne s4 t0 c_j_case
            mv a2 s0 # a2=origin number
            jal ra cal_ci_imm   #get the imm      
            mv  t2  a1  #use t2 to store imm
            mv a2 s0 # a2=origin number
            li a3 9 # a3 = l
            li a4 7 # a4 = r
            jal ra get_bits #call get bits to get rd
            mv t3 a1 #use t3 to store rd
            addi t3 t3 8 #rd+=8
            mv t4 t3 #rs1 = rd
            add a1 zero zero #machine code = 0
            slli t2 t2 20 #imm<<20
            add a1 a1 t2 #machine code+=imm<<20
            slli t4 t4 15 #rs1<<15
            add a1 a1 t4 #machine code+=rs1<<15
            slli t3 t3 7#rd<<7
            add a1 a1 t3 #machine code+=rd<<7
            li t0 7#func3 =0b111
            slli t0 t0 12#func3<<12
            add a1 a1 t0 #machine code+=func3<<12
            li t0 19 #opcode=19
            add a1 a1 t0 #machine code+=opcode
            j rvc_extend_return
        c_j_case:
            li t0, 10   # c_j hash value 10
            bne s4, t0, c_jal_case
            mv a2, s0   # a2 = s0 = rvc machine code
            li t1, 0b1101111  # opcode = 0b1101111
            li s5, 0    # use s5 to store final answer

            or s5, s5, t1   # add opcode
            jal ra, cal_cj_imm
            mv t0, a1   # t0 = cal_cb_imm(rvc_code)
            bge t0, x0, c_j_positive_offset  # offset > 0
            c_j_negative_offset:
                li t1, 0    # set jump_lines 0    
                c_j_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines - 1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)

                    beq t0, t4, c_j_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j c_j_negative_offset_loop
                c_j_negative_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                li t0, -1
                mul t1, t1, t0      # multiply -1
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s5, s5, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s5, s5, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
            c_j_positive_offset:
                li t1, 0    # set jump_lines 0    
                c_j_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, c_j_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j c_j_positive_offset_loop
                c_j_positive_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s5, s5, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s5, s5, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s5, s5, a1   # machine_code |= return_value  << 8

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
        c_jal_case:
            mv a2, s0   # a2 = s0 = rvc machine code
            li s5, 0    # use s5 to store final answer
            li t1, 1    # rd = 1
            slli t1, t1, 7
            or s5, s5, t1 # add rd
            li t1, 0b1101111  # opcode = 0b1101111
            or s5, s5, t1   # add opcode
            jal ra, cal_cj_imm
            mv t0, a1   # t0 = cal_cb_imm(rvc_code)
            bge t0, x0, c_jal_positive_offset  # offset > 0
            c_jal_negative_offset:
                li t1, 0    # set jump_lines 0    
                c_jal_negative_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    sub t3, s2, t1 # line_idx - jump_lines
                    addi t3, t3, -1 # line_idx - jump_lines - 1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx - jump_lines -1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx - jump_lines - 1]
                    addi t4, s2, -1 # t4 = line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx-1]
                    sub t3, t4, t3  # t3 = pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t3, t2  # t4 = -(jump_lines * 4 - (pre_sum[line_idx-1] - pre_sum[line_idx-jump_lines-1]) * 2)

                    beq t0, t4, c_jal_negative_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j c_jal_negative_offset_loop
                c_jal_negative_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                li t0, -1
                mul t1, t1, t0      # multiply -1
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s5, s5, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s5, s5, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s5, s5, a1   # machine_code |= return_value  << 21

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
            c_jal_positive_offset:
                li t1, 0    # set jump_lines 0    
                c_jal_positive_offset_loop:
                    slli t2, t1, 2 # jump_lines * 4
                    add t3, s2, t1 # line_idx + jump_lines - 1
                    addi t3, t3, -1
                    slli t3, t3, 2  # *4 to get address 
                    add t3, s11, t3 # get the address of pre_sum[line_idx + jump_lines - 1]
                    lw t3, 0(t3)    # t3 = pre_sum[line_idx + jump_lines - 1]
                    addi t4, s2, -1 # line_idx - 1
                    slli t4, t4, 2  # *4 to get address
                    add t4, s11, t4 # get the address of pre_sum[line_idx - 1]
                    lw t4, 0(t4)    # t4 = pre[line_idx]
                    sub t3, t3, t4  # t3 = pre_sum[line_idx+jump_lines-1] - pre_sum[line_idx - 1]
                    slli t3, t3, 1  # t3 *= 2
                    sub t4, t2, t3  # t4 = jump_lines * 4 - (pre_sum[line_idx + jump_lines - 1] - pre_sum[line_idx - 1]) * 2

                    beq t0, t4, c_jal_positive_offset_done
                    addi t1, t1, 1  # jump_lines ++
                    j c_jal_positive_offset_loop
                c_jal_positive_offset_done:
                slli t1, t1, 2      # t1 = jump_bytes = jump_lines * 4
                mv a2, t1       # let a2 = t1 to pass to get_bits
                li a3, 19       # l = 19
                li a4, 12       # r = 12
                jal ra, get_bits    # get_bits(jump_bytes, 19, 12)
                slli a1, a1, 12
                or s5, s5, a1   # machine_code |= return_value  << 12

                li a3, 11        # l = 11
                li a4, 11        # r = 11
                jal ra, get_bits    # get_bits(jump_bytes, 11, 11)
                slli a1, a1, 20
                or s5, s5, a1   # machine_code |= return_value  << 20

                li a3, 10       # l = 10
                li a4, 1        # r = 1
                jal ra, get_bits    # get_bits(jump_bytes, 10, 1)
                slli a1, a1, 21
                or s5, s5, a1   # machine_code |= return_value  << 21

                li a3, 20       # l = 20
                li a4, 20       # r = 20
                jal ra, get_bits    # get_bits(jump_bytes, 20, 20)
                slli a1, a1, 31
                or s5, s5, a1   # machine_code |= return_value  << 31
                mv a1, s5       # let a1 = s5 and return
                j rvc_extend_return
    rvc_extend_return:
	# Get back saved return address from stack and go to it
	lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)   # t2
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw t0, 24(sp)   # t0
    lw t1, 28(sp)
    lw t2, 32(sp)
    lw t3, 36(sp)   # t3
    lw t4, 40(sp)
    lw t5, 44(sp)
    lw t6, 48(sp)   # t6
    lw s3, 52(sp)
	addi sp, sp, 56
	jr ra   # return

# original number in a2, l in a3, r in a4, return value in a1
get_bits:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw t0, 0(sp)
    sw t1, 4(sp)

    sub t0, a3, a4  # len = l - r + 1
    addi t0, t0, 1
    li t1, 1        # t1 = 1
    sll t1, t1, t0  # t1 = 1000..00 
    addi t1, t1, -1 # t1 = 11...11 
    srl a1, a2, a4  # machinecode >>= r
    and a1, a1, t1  # res = machinecode & t1

    # Get back saved return address from stack and go to it
    lw t1, 4(sp)
	lw t0, 0(sp)
	addi sp, sp, 8
    jr ra

# risc-v machine code in a2, return value in a1
cal_sb_imm:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw ra, 0(sp)
    sw s0, 4(sp)

    li a3, 31   # set l = 31
    li a4, 31   # set r = 31
    jal ra, get_bits # get_bits(risc-v, 31, 31)

    bne a1, x0, sb_negative # judge the sign bit
        li s0, 0
        j sb_positive
    sb_negative:
        li s0, 0xFFFFF000   # do sign-extention
    sb_positive:

    li a3, 11   # set l = 11
    li a4, 8    # set r = 8
    jal ra, get_bits    # get_bits(risc-v, 11, 8)
    slli a1, a1, 1  # << 1 bit
    or s0, s0, a1

    li a3, 30   # set l = 30
    li a4, 25    # set r = 25
    jal ra, get_bits    # get_bits(risc-v, 30, 25)
    slli a1, a1, 5  # << 5 bit
    or s0, s0, a1

    li a3, 7   # set l = 7
    li a4, 7    # set r = 7
    jal ra, get_bits    # get_bits(risc-v, 7, 7)
    slli a1, a1, 11  # << 11 bit
    or s0, s0, a1
    
    mv a1, s0   # get return value

    # Get back saved return address from stack and go to it
    lw s0, 4(sp)
	lw ra, 0(sp)
	addi sp, sp, 8
	jr ra

# risc-v machine code in a2, return value in a1
cal_uj_imm:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw ra, 0(sp)
    sw s0, 4(sp)

    li a3, 31   # set l = 31
    li a4, 31   # set r = 31
    jal ra, get_bits # get_bits(risc-v, 31, 31)

    bne a1, x0, uj_negative # judge the sign bit
        li s0, 0
        j uj_positive
    uj_negative:
        li s0, 0xFFF00000   # do sign-extention
    uj_positive:

    li a3, 30   # set l = 30
    li a4, 21    # set r = 21
    jal ra, get_bits    # get_bits(risc-v, 30, 21)
    slli a1, a1, 1   # << 1 bit
    or s0, s0, a1

    li a3, 20   # set l = 20
    li a4, 20    # set r = 20
    jal ra, get_bits    # get_bits(risc-v, 20, 20)
    slli a1, a1, 11  # << 11 bit
    or s0, s0, a1

    li a3, 19   # set l = 19
    li a4, 12    # set r = 12
    jal ra, get_bits    # get_bits(risc-v, 19, 12)
    slli a1, a1, 12  # << 12 bit
    or s0, s0, a1

    mv a1, s0   # get return value

    # Get back saved return address from stack and go to it
	lw ra, 0(sp)
    lw s0, 4(sp)
	addi sp, sp, 8
	jr ra

# rvc machine code in a2, return value in a1
cal_cb_imm:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw ra, 0(sp)
    sw s0, 4(sp)

    li a3, 12   # set l = 12
    li a4, 12   # set r = 12
    jal ra, get_bits # get_bits(risc-v, 12, 12)

    bne a1, x0, cb_negative # judge the sign bit
        li s0, 0
        j cb_positive
    cb_negative:
        li s0, 0xFFFFFF00   # do sign-extention
    cb_positive:


    li a3, 4    # set l = 4
    li a4, 3    # set r = 3
    jal ra, get_bits    # get_bits(risc-v, 4, 3)
    slli a1, a1, 1   # << 1 bit
    or s0, s0, a1

    li a3, 11    # set l = 11
    li a4, 10    # set r = 10
    jal ra, get_bits    # get_bits(risc-v, 11, 10)
    slli a1, a1, 3  # << 3 bit
    or s0, s0, a1

    li a3, 2    # set l = 2
    li a4, 2    # set r = 2
    jal ra, get_bits    # get_bits(risc-v, 2, 2)
    slli a1, a1, 5  # << 5 bit
    or s0, s0, a1

    li a3, 6   # set l = 6
    li a4, 5   # set r = 5
    jal ra, get_bits    # get_bits(risc-v, 6, 5)
    slli a1, a1, 6  # << 6 bit
    or s0, s0, a1
    
    mv a1, s0   # get return value

    # Get back saved return address from stack and go to it
	lw ra, 0(sp)
    lw s0, 4(sp)
	addi sp, sp, 8
	jr ra

# rvc machine code in a2, return value in a1
cal_cj_imm:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw ra, 0(sp)
    sw s0, 4(sp)

    li a3, 12   # set l = 12
    li a4, 12   # set r = 12
    jal ra, get_bits # get_bits(risc-v, 12, 12)

    bne a1, x0, cj_negative # judge the sign bit
        li s0, 0
        j cj_positive
    cj_negative:
        li s0, 0xFFFFF800   # do sign-extention
    cj_positive:

    li a3, 5    # set l = 5
    li a4, 3    # set r = 3
    jal ra, get_bits    # get_bits(risc-v, 5, 3)
    slli a1, a1, 1   # << 1 bit
    or s0, s0, a1

    li a3, 11    # set l = 11
    li a4, 11    # set r = 11
    jal ra, get_bits    # get_bits(risc-v, 11, 11)
    slli a1, a1, 4  # << 4 bit
    or s0, s0, a1

    li a3, 2    # set l = 2
    li a4, 2    # set r = 2
    jal ra, get_bits    # get_bits(risc-v, 2, 2)
    slli a1, a1, 5  # << 5 bit
    or s0, s0, a1

    li a3, 7   # set l = 7
    li a4, 7   # set r = 7
    jal ra, get_bits    # get_bits(risc-v, 7, 7)
    slli a1, a1, 6  # << 6 bit
    or s0, s0, a1

    li a3, 6   # set l = 6
    li a4, 6   # set r = 6
    jal ra, get_bits    # get_bits(risc-v, 6, 6)
    slli a1, a1, 7  # << 7 bit
    or s0, s0, a1

    li a3, 10   # set l = 10
    li a4, 9    # set r = 9
    jal ra, get_bits    # get_bits(risc-v, 10, 9)
    slli a1, a1, 8  # << 8 bit
    or s0, s0, a1

    li a3, 8   # set l = 8
    li a4, 8   # set r = 8
    jal ra, get_bits    # get_bits(risc-v, 8, 8)
    slli a1, a1, 10  # << 10 bit
    or s0, s0, a1
    
    mv a1, s0   # get return value

    # Get back saved return address from stack and go to it
	lw ra, 0(sp)
    lw s0, 4(sp)
	addi sp, sp, 8
	jr ra

# rvc machine code in a2, return value in a1
cal_ci_imm:
    # Save the ra in the stack since it will be overwritten
	addi sp, sp, -8
	sw ra, 0(sp)
    sw s0, 4(sp)

    li a3, 12   # set l = 12
    li a4, 12   # set r = 12
    jal ra, get_bits # get_bits(risc-v, 12, 12)

    bne a1, x0, ci_negative # judge the sign bit
        li s0, 0
        j ci_positive
    ci_negative:
        li s0, 0xFFFFFFE0   # do sign-extention
    ci_positive:

    li a3, 6    # set l = 6
    li a4, 2    # set r = 2
    jal ra, get_bits    # get_bits(risc-v, 6, 2)
    or s0, s0, a1
    
    mv a1, s0   # get return value

    # Get back saved return address from stack and go to it
	lw ra, 0(sp)
    lw s0, 4(sp)
	addi sp, sp, 8
	jr ra
