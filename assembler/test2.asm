	b equ 03
	L equ 0
	J equ 1
	N   equ  30

	org 47
	
start:	N <- 1
	input b
	input L
	J <- b
	(R0,R1) <- L
	right shift R1 by (R0)
	R0 <- 0
	P <- [L]
	Q <- L - P
A:	;branch to B if P = 0
	P <- P - 1
	N <- N * b
	branch to A
B:	J <- sqrt J
	;branch to C if J = 1
	(R0,R1) <- J
	R1 <- R1 - 1
	test R1 = 0
	branch to C if cl 1
	   
	Q <- Q + Q
	   ;branch to b if Q - 1 < 0
	Q <- Q -1
	N <- N * J
	branch to B
C:	display b, L , N
	branch to start
	