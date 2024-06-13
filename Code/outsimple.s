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
    li $t0,0
    li $t1,1
    li $t2,0
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal read
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    move $t3,$v0
    move $t4,$t3
label1:
    move $t5,$t2
    move $t6,$t4
    blt $t5,$t6,label2
    j label3
label2:
    move $t7,$t0
    move $s0,$t1
    add $s1,$t7,$s0
    move $s2,$t1
    move $a0,$s2
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    move $s3,$t1
    move $t0,$s3
    move $s4,$s1
    move $t1,$s4
    move $s5,$t2
    li $s6,1
    add $s7,$s5,$s6
    move $t2,$s7
    j label1
label3:
    move $v0,$0
    jr $ra
