.data
game_name: .string "CHROME DINO!"
easy: .string "Easy"
hard: .string "Hard"
.section .text
.globl startUI
.type  startUI,@function
startUI:
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
    mv t0,a0 #the color of easy
    mv t1,a1 #thw color of hard
    #draw the name of the game
    li	a0,	35  #the x is at 15
    li	a1, 10  #thw y is at 10
    la	a2,	game_name  #show the name of the game
    li  a3, 0xF7BE  # Color
    jal	LCD_ShowString

    #draw the instruction
    li  a0, 35  #the x is at 15
    li  a1, 35  #thw y is at 10
    la  a2, easy #show the inst of the game
    mv  a3, t0  # Color
    jal  LCD_ShowString

    #draw the instruction
    li  a0, 95  #the x is at 15
    li  a1, 35  #thw y is at 10
    la  a2, hard #show the inst of the game
    mv  a3, t1 # Color
    jal  LCD_ShowString

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


