!============================================================
! CS 2200 Homework 2 Part 2: fib
!
! Apart from initializing the stack,
! please do not edit main's functionality.
!============================================================

main:
    lea     $sp, stack          ! load ADDRESS of stack label into $sp

    lw      $sp, 0($sp)         ! TODO: Here, you need to initialize the stack
                                ! using the label below by loading its
                                ! VALUE into $sp (CHANGE THIS INSTRUCTION)

    lea     $at, fib            ! load address of fib label into $at
    addi    $a0, $zero, 5       ! $a0 = 5, the number a to compute fib(n)
    jalr    $ra, $at            ! jump to fib, set $ra to return addr
    halt                        ! when we return, just halt

fib:
    add     $zero, $zero, $zero ! TODO: perform post-call portion of
                                ! the calling convention. Make sure to
                                ! save any registers you'll be using!
    addi    $sp, $sp, -4        ! decrement stack pointer
    sw      $ra, 3($sp)         ! store old return address
    sw      $fp, 2($sp)         ! store old frame pointer
    addi    $fp, $sp, 2         ! save stack pointer address + 2 in fp
    sw      $s0, 1($sp)         ! save $s0 in stack
    sw      $s1, 0($sp)         ! save $s1 in stack
    

    
    add     $zero, $zero, $zero ! TODO: Implement the following pseudocode in assembly:
                                ! IF (a0 <= 1)
                                !    GOTO BASE
                                ! ELSE
                                !    GOTO ELSE

    addi    $t0, $zero, 1
    
    skplt   $t0, $a0            ! check if a0 > 1
    goto    base                ! go to base if a0 <= 1
    goto    else

base:
    add     $zero, $zero, $zero ! TODO: If $a0 is less than 0, set $a0 to 0
    skplt   $zero, $a0          ! check if a0 less than 0
    addi    $a0, $zero, 0       ! set $a0 to 0 if a0 <= 0
    addi    $v0, $a0, 0         ! return a
    goto    teardown            ! teardown the stack

else:
    add     $zero, $zero, $zero ! TODO: Save the value of the $a0 into a saved  register

    addi    $s0, $a0, 0         ! store a0 in s0

    addi    $a0, $a0, -1        ! $a0 = $a0 - 1 (n - 1)

    add     $zero, $zero, $zero ! TODO: Implement the recursive call to fib
                                ! You should not have to set any of the argument registers here.
    lea     $at, fib            ! load address of fib label into $at
    jalr    $ra, $at            ! jump to fib, set $ra to return addr

    add     $zero, $zero, $zero ! TODO: Save the return value of the fib call into a register
    addi    $s1, $v0, 0         ! save return value into $s1

    add     $zero, $zero, $zero ! TODO: Restore the old value of $a0 that we saved earlier
    addi    $a0, $s0, 0

    addi    $a0, $a0, -2        ! $a0 = $a0 - 2 (n - 2)

    add     $zero, $zero, $zero ! TODO: Implement the recursive call to fib
                                ! If your previous recursive call worked correctly,
                                ! you should be able to copy and paste it here :)
    lea     $at, fib            ! load address of fib label into $at
    jalr    $ra, $at            ! jump to fib, set $ra to return addr

    add     $zero, $zero, $zero ! TODO: Compute fib(n - 1) [stored from earlier] + fib(n - 2) [just computed]
                                ! Place the sum of those two values into $v0
    add     $v0, $s1, $v0       ! add fib(n - 1) + fib(n - 2)
    goto    teardown            ! return
    
teardown:
    add     $zero, $zero, $zero ! TODO: perform pre-return portion
                                !       of the calling convention
    lw      $ra, 1($fp)         ! restore old return address
    lw      $s1, -2($fp)        ! restore $s1
    lw      $s0, -1($fp)        ! restore $s0
    addi    $sp, $fp, 2         ! move $sp to before current function's additional parameters
    lw      $fp, 0($fp)         ! restore $fp

    jalr    $zero, $ra          ! return to caller

stack: .word 0xFFFF             ! the stack begins here
