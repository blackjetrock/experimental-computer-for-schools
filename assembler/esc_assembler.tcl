#!/usr/bin/tclsh
#
# Assembler for the Experimental Schools Computer
#
#
# Features:
# ---------
#
# * Must beable to support all instructions
# * Must handle two instructions per word for single address instructions
# * Must handle (or error) attempts to branch to inaccessible instructions
#
################################################################################
#
# Assembles a block of source text
#
# Output is returned as a list of opcodes
#
# Everything is assembled on a line by line basis
#
#
#
# Assembly code syntax follows the first course on the schools computer
#
# Example mnemonics
#
# Input <x>; Display <y>, <z>
# <x> <- <immediate value> (uses <y> + <z>, y=z (0) as immediate value)
# <x> <- <y> + <z>
# <x> <- <y> - <z>
# <x> <- <y> * <z>
# <x> <- <y> / <z>
# Rc <- Rc + d
# Rc <- Rc + Rd
#
#
# LD  Rc, Rc+Rd
# ld  Rc, Rc+Rd
# add Rc, Rd
# mov Rc, Rd
# display Rd
# display (Aa)
# stopdisp Rc, Rd
# stopdisp (Aa1), (Aa2), (Aa3)
# input (Aa1); display (Aa1), (Aa2), (Aa3)
# ld (Aa), Rlink
# if (Aa1) <> 0; branch to (Aa2); ld (Aa3), Rlink
# copy (Aa) to R0, R1
# store R0, R1 in (Aa) as data
# store R0, R1 in (Aa) as instruction
#
# Floating point
# ==============
#
# (Aa1) <- (Aa2) - (Aa3)
# (Aa1) <- (Aa2) + (Aa3)
# (Aa1) <- (Aa2) / (Aa3)
# (Aa1) <- (Aa2) * (Aa3)

# Absolute addressing:
#
# 2 Address instructions
#
# a    Addressing
# 2    Ap
# 3    Ap+(R3)
# 4    Ap+(R4)
# 5    Ap+(R5)
# 6    (Ap)
#
# 3 address instructions
#
# a    Addressing
# 7    Ap1                   Ap2                Ap3
# 8    Ap1+ (R3)             Ap2+(R4)           Ap3+(R5)
# 9    (Ap1)                 (Ap2)              (Ap3)
#
#

set ::INST_INFO_3 {
    {"([0-9]+)<-([0-9]+)-([0-9]+)"}
    {"([0-9]+[(])<-([0-9]+)-([0-9]+))" }
}

set ::INST_INFO {
    {"(R[0-9]+)[<][-](R[0-9]+)[+]([0-9]+)"   inst_1_00}
    {"R([0-9]+)[<][-]R([0-9]+)[+]R([0-9]+)"  inst_1_01}

}

################################################################################

proc inst_error {e} {
    puts "\n*** ERROR *** $e"
}

################################################################################

proc inst_1_00 {line fmt} {
    if { [regexp -- $fmt $line all a b c] } {
	
	# Check arguments are valid
	if { $a != $b } {
	    inst_error "Rc not consistent (R$a should equal R$b)"
	    return "0000"
	}
	
	if { ($a > 9) } {
	    inst_error "Register number out of range (R$a should be less than R9)"
	    return "0000"
	}

	# return the opcode
	return "00$a$c"	
    } else {
	inst_error "INST failed:$fmt $line" 
    }
    
}

proc inst_1_01 {line fmt} {
    if { [regexp -- $fmt $line all a b c] } {
	
	# Check arguments are valid
	if { $a != $b } {
	    inst_error "Rc not consistent (R$a should equal R$b)"
	    return "0000"
	}
	
	if { ($a > 9) } {
	    inst_error "Number out of range ($a should be less than 9)"
	    return "0000"
	}

	# return the opcode
	return "01$a$c"	
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
    
}

################################################################################

proc assemble {t pass} {
    set ::ADDRESS 0
    
    foreach line [split $t "\n"] {
	puts "<<$line "
	
	# Remove comments
	if { [regexp "(.*);(.*)" $line all a b] } {
	    set line $a
	}

	# Remove spaces
	set line [string map {" " ""} $line]

	# Store and remove labels
	if { [regexp "(.*):(.*)" $line all label b] } {
	    lappend ::LABEL_NAMES $label
	    set ::LABEL_VALUE($label) $::ADDRESS
	    set line $b
	}

	set length [string length $line]
	puts "\[$length\]>>$line "

	# If length is zero then ignore the line
	if { $length == 0 } {
	    continue
	}
	
	# Find and assemble the instruction
	foreach inst $::INST_INFO {
	    set f [lindex $inst 0]
	    set p [lindex $inst 1]

	    set object [$p $line $f]
	    puts "OBJ:$object"
	}
	
	incr ::ADDRESS 1
    }
}

################################################################################
#
# Dump the label data
#
################################################################################

proc dump_labels {} {
    puts ""
    puts "labels"
    puts "------"
    puts ""
    foreach labelname $::LABEL_NAMES {
	set labelvalue $::LABEL_VALUE($labelname)
	
	puts [format "$labelname: 0x%04X %d" $labelvalue $labelvalue]
    }
    
}

################################################################################

set filename [lindex $argv 0]

set fn [open $filename]
set txt [read $fn]
close $fn

set done 0

for {set pass 1} {!$done} {incr pass 1} {

    assemble $txt $pass
    set done 1
}

dump_labels


