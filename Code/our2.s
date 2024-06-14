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
fact:
    subu $sp,$sp,8
    sw $ra,4($sp)
    sw $fp,0($sp)
    addi $fp,$sp,8
    move $t0,$a0
    subu $sp,$sp,4
    sw $t0,-12($fp)
    lw $t0,-12($fp)
    move $t1,$t0
    subu $sp,$sp,4
    sw $t1,-16($fp)
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-20($fp)
    lw $t1,-16($fp)
    lw $t0,-20($fp)
    beq $t1,$t0,label1
    j label2
label1:
    lw $t0,-12($fp)
    move $v0,$t0
    lw $ra,-4($fp)
    lw $fp,-8($fp)
    addi $sp,$sp,8
    jr $ra
    j label3
label2:
    lw $t0,-12($fp)
    move $t1,$t0
    subu $sp,$sp,4
    sw $t1,-24($fp)
    lw $t1,-12($fp)
    move $t0,$t1
    subu $sp,$sp,4
    sw $t0,-28($fp)
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-32($fp)
    lw $t2,-28($fp)
    lw $t0,-32($fp)
    sub $t1,$t2,$t0
    subu $sp,$sp,4
    sw $t1,-36($fp)
    lw $t0,-36($fp)
    move $a0,$t0
    jal fact
    move $t1,$v0
    subu $sp,$sp,4
    sw $t1,-40($fp)
    lw $t3,-24($fp)
    lw $t1,-40($fp)
    mul $t0,$t3,$t1
    subu $sp,$sp,4
    sw $t0,-44($fp)
    lw $t0,-44($fp)
    move $v0,$t0
    lw $ra,-4($fp)
    lw $fp,-8($fp)
    addi $sp,$sp,8
    jr $ra
label3:

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
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-24($fp)
    lw $t1,-20($fp)
    lw $t0,-24($fp)
    bgt $t1,$t0,label4
    j label5
label4:
    lw $t1,-16($fp)
    move $t0,$t1
    subu $sp,$sp,4
    sw $t0,-28($fp)
    lw $t0,-28($fp)
    move $a0,$t0
    jal fact
    move $t1,$v0
    subu $sp,$sp,4
    sw $t1,-32($fp)
    lw $t1,-32($fp)
    move $t0,$t1
    subu $sp,$sp,4
    sw $t0,-36($fp)
    j label6
label5:
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-40($fp)
    lw $t1,-36($fp)
    lw $t0,-40($fp)
    move $t1,$t0
    sw $t1,-36($fp)
label6:
    lw $t1,-36($fp)
    move $t0,$t1
    subu $sp,$sp,4
    sw $t0,-44($fp)
    lw $t0,-44($fp)
    move $a0,$t0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    lw $ra,-4($fp)
    lw $fp,-8($fp)
    addi $sp,$sp,8
    move $v0,$0
    jr $ra

