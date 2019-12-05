addi $t0, $zero, 1
add $t0, $t0, $t0
sw $t0, 5($zero)
lw $t1, 5($zero)
skpe $t0, $t1
add $t0, $t0, $t1
halt