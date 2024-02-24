#!/usr/bin/tclsh

set fmt {R([0-9]+)[<][-]R([0-9]+)[+]R([0-9]+)}
set line R5<-R5+R6

    if { [regexp -- $fmt $line all a b c] } {
	
	puts "01$a$c"	
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
    
