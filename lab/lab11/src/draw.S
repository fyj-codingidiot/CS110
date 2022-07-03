.data
    cs: .string "CS110P"

.section .text
.globl draw
.type draw, @function
draw:
	# YOUR CODE HERE
	#circle
	li a0, 25;
    li a1, 30;
    li a2, 25;
    li a3, 0b0000000000011111;
    jal LCD_DrawCircle

	# square
    li a0, 55;
    li a1, 25;
    li a2, 95;
    li a3, 65;
    li a4, 0b1111100000000000;
    jal LCD_DrawRectangle;

	# string
	la t1, cs;
    li a0, 100;
    li a1, 32;
    mv a2, t1;
    li a3, 0b0000011111100000;
    jal LCD_ShowString;