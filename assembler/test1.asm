				; Comment

	X equ 02
	INC equ 1
	
start:	
	R5  <-   X              ; Comment
	R6 <-     INC
loop:	R5 <-      R5 + R6
	branch to  loop

	copy sign and rh six digits of R0 into R9
	copy sign and rh six digits of R2 into R6
	copy sign and rh six digits of R5 into R4
	copy sign and rh six digits of R7 into R2
	copy sign and rh six digits of R9 into R0
	
