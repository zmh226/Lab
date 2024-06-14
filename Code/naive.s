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
    li $t0,2
    subu $sp,$sp,4
    sw $t0,-12($fp)
    lw $t0,-12($fp)
    move $a0,$t0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t1,0
    subu $sp,$sp,4
    sw $t1,-16($fp)
    lw $t1,-16($fp)
    move $a0,$t1
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t0,1
    subu $sp,$sp,4
    sw $t0,-20($fp)
    lw $t0,-20($fp)
    move $a0,$t0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t1,7
    subu $sp,$sp,4
    sw $t1,-24($fp)
    lw $t1,-24($fp)
    move $a0,$t1
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

