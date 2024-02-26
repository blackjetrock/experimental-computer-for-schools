				; Comment

	X equ 02
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
	
	copy sign and rh six digits of R0 into R9
	copy sign and rh six digits of R2 into R6
	copy sign and rh six digits of R5 into R4
	copy sign and rh six digits of R7 into R2
	copy sign and rh six digits of R9 into R0
	
