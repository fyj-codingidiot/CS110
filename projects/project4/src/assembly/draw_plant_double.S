.section .text
.globl draw_plant_double
.type draw_plant_double ,@function
draw_plant_double:
    addi sp, sp, -52
    sw ra, 0(sp)    # store ra
    sw s0, 4(sp)    # store s0
    sw s1, 8(sp)    # store s1
    sw s2, 12(sp)   # store s2
    sw s3, 16(sp)   # store s3
    sw s4, 20(sp)   # store s4
    sw s5, 24(sp)   # store s5
    sw s6, 28(sp)   # store s6
    sw s7, 32(sp)   # store s7
    sw s8, 36(sp)   # store s8
    sw s9, 40(sp)   # store s9
    sw s10, 44(sp)  # store s10
    sw s11, 48(sp)  # store s11

    mv t0, a0  #the x1 of plant
    mv t1, a1  #the y1 of plant
    mv t2, a2  #the address of 1st plant
    mv t3, a3  #the address of 2nd plant
    #draw the plant
    mv a0, t0  #the x1 of  plant
    mv a1, t1  #the y1 of plant
    addi a2, a0, 11  #the x2 of plant
    addi a3, a1, 19  #the y2 of plant
    mv a4, t2  #the address of plant
    jal LCD_ShowPic

    addi a0,t0,12  #the x1 of  plant
    mv a1,t1       #the y1 of plant
    addi a2,a0,11  #the x2 of plant
    addi a3,a1,19  #the y2 of plant
    mv a4,t3       #the address of plant
    jal LCD_ShowPic

    lw ra, 0(sp)    # load ra
    lw s0, 4(sp)    # load s0
    lw s1, 8(sp)    # load s1
    lw s2, 12(sp)   # load s2
    lw s3, 16(sp)   # load s3
    lw s4, 20(sp)   # load s4
    lw s5, 24(sp)   # load s5
    lw s6, 28(sp)   # load s6
    lw s7, 32(sp)   # load s7
    lw s8, 36(sp)   # load s8
    lw s9, 40(sp)   # load s9
    lw s10, 44(sp)  # load s10
    lw s11, 48(sp)  # load s11
    addi sp, sp, 52
    ret