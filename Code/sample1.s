.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
main:
    subu $sp,$sp,8
    sw $ra,4($sp)
    sw $fp,0($sp)
    addi $fp,$sp,8
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal read
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    move $t0,$v0
    subu $sp,$sp,4
    sw $t0,-12($fp)
    lw $t0,-12($fp)
    move $t1,$t0
    subu $sp,$sp,4
    sw $t1,-16($fp)
    lw $t1,-16($fp)
    move $t0,$t1
    subu $sp,$sp,4
    sw $t0,-20($fp)
    li $t0,0
    subu $sp,$sp,4
    sw $t0,-24($fp)
    lw $t1,-20($fp)
    lw $t0,-24($fp)
    bgt $t1,$t0,label1
    j label2
label1:
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-28($fp)
    lw $t0,-28($fp)
    move $a0,$t0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    j label3
label2:
    lw $t2,-16($fp)
    move $t1,$t2
    subu $sp,$sp,4
    sw $t1,-32($fp)
    li $t0,0
    subu $sp,$sp,4
    sw $t0,-36($fp)
    lw $t1,-32($fp)
    lw $t0,-36($fp)
    blt $t1,$t0,label4
    j label5
label4:
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-40($fp)
    lw $t0,-40($fp)
    sub $t1,$0,$t0
    subu $sp,$sp,4
    sw $t1,-44($fp)
    lw $t0,-44($fp)
    move $a0,$t0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    j label6
label5:
    li $t1,0
    subu $sp,$sp,4
    sw $t1,-48($fp)
    lw $t1,-48($fp)
    move $a0,$t1
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
label6:
label3:
    lw $ra,-4($fp)
    lw $fp,-8($fp)
    addi $sp,$sp,8
    move $v0,$0
    jr $ra

