				; Comment

	X equ 02
	VAR_Y equ 78
	
	INC equ 1

	org 45
	
start:	
	R5  <-   X              ; Comment
	R6 <-     INC
loop:	R5 <-      R5 + R6
	branch to  loop
	branch to  loop if cl 1	
        branch to  loop	if cl 0
	display X
	display R5 and R6
	stop and display R6 and R7
	display (R6) and (R8)
	stop and display (R2) and (R3)
	(R0, R1) <- 42
	(R0, R1) <- 42+(R3)
	(R0, R1) <- 42+(R4)
	(R0, R1) <- 42+(R5)
	(R0, R1) <- (42)	
	(R0, R1) <- VAR_Y
	(R0, R1) <- VAR_Y+(R3)
	(R0, R1) <- VAR_Y+(R4)
	(R0, R1) <- VAR_Y+(R5)
	(R0, R1) <- (VAR_Y)	

	42 <- (R0, R1)
	42+(R3) <- (R0, R1)
	42+(R4) <- (R0, R1)
	42+(R5) <- (R0, R1)
	(42)	 <- (R0, R1)
	VAR_Y <- (R0, R1)
	VAR_Y+(R3) <- (R0, R1)
	VAR_Y+(R4) <- (R0, R1)
	VAR_Y+(R5) <- (R0, R1)
	(VAR_Y)	 <- (R0, R1)
	copy sign and rh six digits of R0 into R9
	copy sign and rh six digits of R2 into R6
	copy sign and rh six digits of R5 into R4
	copy sign and rh six digits of R7 into R2
	copy sign and rh six digits of R9 into R0

         23  <-  24  +  25
         23+(R3)  <-  24+(R3)  + 25+(R3)
         23  <-  24  + (R4)
         23  <-  24  + (R5)
         23  <-  24  + (R3)
        (23) <- (24) + (25)

