				; Comment

	VAR_X equ 77
	VAR_Y equ 78
	VAR_Z equ 79

	INC equ 1

	org 45
	
start:
        R4 <- 0
	R5  <-  1              ; Comment
	R6 <-   2
	R6 <-   3
	R6 <-   4
	R6 <-   5
	R6 <-   6
	R6 <-   7
	R6 <-   8
	R6 <-   9

	R6 <-     INC
	
loop:	R5 <-      R5 + R6
	branch to  loop
	branch to  loop if cl 1	
        branch to  loop	if cl 0
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
        (23) <- (24) + (25)

        display VAR_X, VAR_Y, VAR_Z
	display VAR_X+(R3), VAR_Y+(R4), VAR_Z+(R5)
	display (VAR_X), (VAR_Y), (VAR_Z)
	
        23  <-  24  -  25	

        branch to loop if VAR_X = VAR_Y
	branch to loop if VAR_X > VAR_Y
	branch to loop if |VAR_X| > |VAR_Y|

        branch to loop+(R3) if VAR_X+(R4) = VAR_Y+(R5)
	branch to loop+(R3) if VAR_X+(R4) > VAR_Y+(R5)
	branch to loop+(R3) if |VAR_X+(R4)| > |VAR_Y+(R5)|
	
