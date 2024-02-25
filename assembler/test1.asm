				; Comment

	X equ 02
	INC equ 1
	
start:	
	R5  <-   X              ; Comment
	R6 <-     INC
loop:	R5 <-      R5 + R6
	BR loop

	
