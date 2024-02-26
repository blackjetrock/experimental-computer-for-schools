	b equ 03
	L equ 0
	J equ 1
	VAR_N   equ  2
        CONST1 equ 30
	VAR_P  equ 4
	VAR_Q  equ 7
	
	org 31
	
start:	VAR_N <- CONST1 / CONST1
	input b
	input L

        ;J <- b

	(R0,R1) <- b
	J <- (R0,R1)
	
	(R0,R1) <- L	
	right shift R1 by (R0) places

        R0 <- 0
	;VAR_P <- [L]
	VAR_P <- (R0,R1)
	
	VAR_Q <- L - VAR_P
	;branch to B if VAR_P = 0
	
A:      (R0,R1) <- VAR_P
        test R1 = 0
        branch to B if cl 1
        nop
	
	;VAR_P <- VAR_P - CONST1
	R1 <- R1 - 1
	VAR_P <- (R0,R1)
	
	VAR_N <- VAR_N * b
	branch to A
	nop
	
B:	;J <- sqrt J
	;branch to C if J = 1
	(R0,R1) <- J
	R1 <- R1 - CONST1
	test R1 = 0
	branch to C if cl 1
	   
	VAR_Q <- VAR_Q + VAR_Q
	   ;branch to b if VAR_Q - 1 < 0
	VAR_Q <- VAR_Q - CONST1
	VAR_N <- VAR_N * J
	branch to B
C:	display b, L , VAR_N
	branch to start
	
