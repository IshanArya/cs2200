!============================================================
! CS 2200 Homework 2 Part 1: mod
!
! Edit any part of this file necessary to implement the
! mod operation. Store your result in $v0.
!============================================================

mod:
    addi    $a0, $zero, 28      ! $a0 = 28, the number a to compute mod(a,b)
    addi    $a1, $zero, 13      ! $a1 = 13, the number b to compute mod(a,b)

    nand    $t0, $a1, $a1       ! not $a1
    addi    $t0, $t0, 1         ! make $a1 negative


loop:
    add     $a0, $a0, $t0       ! a0 = a0 - a1
    skplt   $a0, $a1            ! skip if a0 < a1
    goto    loop                ! loop
add     $v0, $zero, $a0         ! store result in $v0
halt

