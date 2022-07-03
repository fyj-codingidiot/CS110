# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

.text  # code starts here

.globl main
main:
    la a2 str1
    la a3 str2
    lw a4 len1
    lw a5 len2
    jal func
    add a1 a0 zero
    addi a0 zero 1
    ecall
    li a0 10
    ecall

func:
    add a0 zero zero #ans = 0
    add a1 zero zero #k=0
    addi sp sp -4 # add 4 to sp
    sw  ra  0(sp) #store ra
    jal ra solution # call solution
    lw  ra  0(sp)  #load back ra
    addi sp sp 4
    jr ra #return to main

solution:
    addi sp sp -4 #add -4
    sw  ra  0(sp) # store ra
    add t0 zero zero #i=0
for1tst:
    bge t0 a4  exit1 #if i>=len1 go to exit1
    add t1 zero zero #j=0
for2tst:
    bge t1 a5  exit2 #if j>=len2 go to exit2
    add a1 zero zero #k = 0
for3tst:
    add t2 t0  a1 #i+k
    bge t2 a4  exit3 # if i+k>=len1 ,go to exit3
    add t3 t1  a1 #j+k
    bge t3 a5  exit3 #if j+k>=len2,go to exit3
    slli t4 t2 2 # (i+k)*4
    add t4  t4  a2 # the address  of the i+kth element of str1
    lw  a6  0(t4) # a6 = str1[i+k]
    slli t5 t3 2 #(j+k)*4
    add t5  t5  a3 #the address of the j+k th element of str2
    lw  a7  0(t5) #a7 = str2[j+k]
    bne  a6  a7  exit3 #go to exit3 if str2[j+k] != str1[i+k]
    addi a1 a1 1 #k+=1
    j for3tst #begin new iter
exit3:
    bge a0  a1  exit4 # if ans>=k,do nothing
    add a0 a1 zero #else ans = k
    addi t1 t1 1 #j+=1
    j for2tst #begin new inner loop
exit4:
    add a0 a0 zero #do nothing to ans
    addi t1  t1 1 # j+=1
    j for2tst #begin new inner loop
exit2:
    addi t0 t0 1 #i+=1
    j for1tst #begin new outer loop
exit1:
    lw  ra   0(sp) # load ra back
    addi sp sp 4 #sp+4
    jr ra # jump and link






