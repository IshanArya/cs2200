! This program executes pow as a test program using the LC 2200 calling convention
! Check your registers ($v0) and memory to see if it is consistent with this program

! main:	addi $t0, $zero, 5
! lea $t1, BASE
addi $t1, $zero, 5
add $t0, $zero, $zero
lea $t2, loop
loop: addi $t0, $t0, 1
! sw $t0, 1($t1)
skplt $t1, $t0
jalr $s0, $t2
! main2: sw $t0, 2($t1)
! BASE:   .fill 4
! BASE_Y: .fill 0
! BASE_Z: .fill 1
halt