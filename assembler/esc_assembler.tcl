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
    # Branches
    {"branchto([a-zA-z0-9_]+)"                        inst_1_branch    .4}
    
    # register addressing before literals
    {"R([0-9]+)[<][-]R([0-9]+)[+]R([0-9]+)"           inst_1_Rc_Rc_Rd  10}
    {"R([0-9]+)[<][-]R([0-9]+)[-]R([0-9]+)"           inst_1_Rc_Rc_Rd  11}
    {"R([0-9]+)[<][-]R([0-9]+)[-]R([0-9]+)"           inst_1_Rc_Rd_Rc  12}
    {"^R([0-9]+)[<][-]R([0-9]+)$"                     inst_1_Rc_Rd     13}
    
    # Literals
    {"^R([0-9]+)[<][-]R([0-9]+)[+]([0-9A-Za-z_]+$)"   inst_1_Rc_Rc_d   00}
    {"^R([0-9]+)[<][-]R([0-9]+)[-]([0-9A-Za-z_]+$)"   inst_1_Rc_Rc_d   01}
    {"^R([0-9]+)[<][-]([0-9A-Za-z_]+)$"               inst_1_Rc_d      03}


}

################################################################################

proc inst_error {e} {
    puts "\n*** ERROR *** $e"
    puts "\n$::LINE"
}

################################################################################

proc check_same {retval a b} {
    if { $a != $b } {
	inst_error "Rc not consistent (R$a should equal R$b)"
	return "0000"
    } else {
	return $retval
    }
}

proc check_register {retval a} {
    
    if { ($a > 9) } {
	inst_error "Register number out of range (R$a should be less than R9)"
	return "0000"
    } else {
	return $retval
    }
}

proc check_literal {retval a} {
    
    if { ($a > 9) } {
	inst_error "Literal out of range ($a should be less than 10)"
	return "0000"
    } else {
	return $retval
    }
}

################################################################################

set ::TABLE6_INFO {
    {2 "^([a-zA-Z0-9_]+)$" }
    {3 "^([a-zA-Z0-9_]+)+R3+$" }
    {4 "^([a-zA-Z0-9_]+)+R4$" }
    {5 "^([a-zA-Z0-9_]+)+R5$" }
    {6 "^(\([a-zA-Z0-9_]+\))$" }
}

proc determine_opcode_a {arg} {
    foreach t6 $::TABLE6_INFO {
	set opcode_a [lindex $t6 0]
	set fmt      [lindex $t6 1]

	if { [regexp -- $fmt $arg] } {
	    return $opcode_a
	}
    }
return "?"
}

################################################################################

proc inst_1_branch {line fmt opcode} {
    if { [regexp -- $fmt $line all dest] } {
	set dest [substitute_equates $dest]
	set dest [substitute_labels $dest]

	set opcode_a [determine_opcode_a $dest]
	set opcode "$opcode_a[lindex [split $opcode ""] 1]"

       	set retval "$opcode$dest"
	
	# Check arguments are valid

	# return the opcode
	return $retval
	
    } else {
	inst_error "INST failed:$fmt $line" 
    }
    
}

proc inst_1_Rc_Rc_d {line fmt opcode} {
    if { [regexp -- $fmt $line all a b c] } {

	set a [substitute_equates $a]
	set b [substitute_equates $b]
	set c [substitute_equates $c]

       	set retval "$opcode$a$c"
	
	# Check arguments are valid
	set retval [check_same $retval $a $b]

	set retval [check_register $retval $a]
	set retval [check_literal $retval $c]

	# return the opcode
	return $retval
	
    } else {
	inst_error "INST failed:$fmt $line" 
    }
    
}

proc inst_1_Rc_d {line fmt opcode} {
    if { [regexp -- $fmt $line all c d] } {

	set c [substitute_equates $c]
	set d [substitute_equates $d]

	set retval "$opcode$c[expr $d]"
	
	set retval [check_register $retval $c]
	set retval [check_literal $retval $d]

	# return the opcode
	return $retval
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
    
}

proc inst_1_Rc_Rd {line fmt opcode} {
    if { [regexp -- $fmt $line all c d] } {

	set c [substitute_equates $c]
	set d [substitute_equates $d]

	set retval "$opcode$c[expr $d]"
	
	set retval [check_register $retval $c]
	set retval [check_register $retval $d]

	# return the opcode
	return $retval
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
    
}


proc inst_1_Rc_Rc_Rd {line fmt opcode} {
    if { [regexp -- $fmt $line all a b c] } {

	set a [substitute_equates $a]
	set b [substitute_equates $b]
	set c [substitute_equates $c]

	set retval "$opcode$a$c"
	
	# Check arguments are valid
	set retval [check_same $retval $a $b]
	set retval [check_literal $retval $a]
	
	# return the opcode
	return $retval
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
}

proc inst_1_Rc_Rd_Rc {line fmt opcode} {
    if { [regexp -- $fmt $line all a b c] } {

	set a [substitute_equates $a]
	set b [substitute_equates $b]
	set c [substitute_equates $c]

	set retval "$opcode$a$c"
	
	# Check arguments are valid
	set retval [check_same $retval $a $c]
	set retval [check_register $retval $a]
	set retval [check_register $retval $c]
	
	# return the opcode
	return $retval
    } else {
	inst_error "INST failed:'$fmt' '$line'" 
    }
}

################################################################################

set ::EQUATE_NAMES {}

proc substitute_equates {in} {

    foreach eq $::EQUATE_NAMES {
	#puts "eq:'$eq'"
	set in [string map "$eq [set ::EQUATE_VALUE($eq)]" $in]
    }
    
return $in
}

proc substitute_labels {in} {

    foreach lab $::LABEL_NAMES {
	#puts "label:'$lab'"
	set in [string map "$lab [set ::LABEL_VALUE($lab)]" $in]
    }
    
return $in
}

################################################################################

set ::ADDRESS          0
set ::ADDRESS_A        0
set ::ADDRESS_A_CHAR " "

################################################################################

# Output goes to the .obj and .lst files


proc assemble {t pass} {

    
    foreach line [split $t "\n"] {
	set ::LINE $line
	
	#puts "<<'$line'"

	set original_line $line
	
	# Remove comments
	if { [regexp "(.*);(.*)" $line all a b] } {
	    set line $a
	}

	# Remove spaces
	set line [string map {" " "" "\t" ""} $line]
	#puts "---'$line'"
	
	# Handle equates
	if { [regexp "(.*)equ(.*)" $line all equate value] } {

	    # Next two lines arein this order so we don't try to substitute this eqyate's value
	    # befor eitis set up
	    set ::EQUATE_VALUE($equate) [substitute_equates $value]
	    lappend ::EQUATE_NAMES $equate
	    #puts "EQ $equate = $::EQUATE_VALUE($equate)"
	    continue
	}
	
	# Store and remove labels
	if { [regexp "(.*):(.*)" $line all label b] } {
	    lappend ::LABEL_NAMES $label

	    # Addresses (labels) always point to the first instruction in a word
	    
	    set ::LABEL_VALUE($label) $::ADDRESS
	    set line $b
	}

	set length [string length $line]
#	puts "\[$length\]>>$line "

	# If length is zero then ignore the line
	if { $length == 0 } {
	    continue
	}

	# ORG
	if { [regexp "org(.*)" $line all value] } {
	    set ::ADDRESS [substitute_equates $value]
	    set ::ADDRESS_A 0
	    set ::ADDRESS_A_CHAR " "
	    continue
	}
	
	# Find and assemble the instruction
	set found 0
	
	foreach inst $::INST_INFO {
	    set f [lindex $inst 0]
	    set p [lindex $inst 1]
	    set opcode [lindex $inst 2]
	    if { [regexp -- $f $line] } {
		set found 1
		set object [$p $line $f $opcode]
		puts -nonewline $::lstf [format "%04d%s %8s" $::ADDRESS $::ADDRESS_A_CHAR $object]
		break
	    }
	}

	if { !$found } {
	    puts -nonewline $::lstf [format "%04d%s %8s" $::ADDRESS $::ADDRESS_A_CHAR ""]
	}

	# Tidy up the source and add that to the end
	# Align with the colon of labels

	if { [regexp -- {(.+):(.*)} $original_line all a b] } {
	    puts $::lstf [format "%20s: %s" $a $b]
	} else {
	    puts $::lstf [format "%20s  %s" "" $original_line]
	}

	# Added complication is that there are 4 digit and 8 digit instructions. Two 4 digit
	# instructions are packed into a word, one 8 digit in a word. There is therefore an
	# 'A' flag which is used to address the second 4 digit instruction in a word. Branches cannot
	# jump to this second instruction, so NOPs have to be inserted if needed
	# If an 8 digfit instruction follows a four digit instruction in the first half of a word then
	# a NOP has to be inserted

	if { $::ADDRESS_A == 0 } {
	    set ::ADDRESS_A 1
	    set ::ADDRESS_A_CHAR A
	} else {
	    set ::ADDRESS_A 0
	    set ::ADDRESS_A_CHAR " "
	    incr ::ADDRESS 1
	}
    }
}

################################################################################
#
# Dump the label data
#
################################################################################

proc dump_labels {} {
    puts $::lstf ""
    puts $::lstf "Labels"
    puts $::lstf "------"
    puts $::lstf ""
    foreach labelname $::LABEL_NAMES {
	set labelvalue $::LABEL_VALUE($labelname)
	
	puts $::lstf [format "%20s: 0x%04X %d" $labelname $labelvalue $labelvalue]
    }
}

proc dump_equates {} {
    puts $::lstf ""
    puts $::lstf "Equates"
    puts $::lstf "-------"
    puts $::lstf ""
    foreach eqname $::EQUATE_NAMES {
	set equatevalue $::EQUATE_VALUE($eqname)
	
	puts $::lstf [format "%20s EQU 0x%04X %d" $eqname $equatevalue $equatevalue]
    }
}

################################################################################

set filename [lindex $argv 0]

set fn [open $filename]
set txt [read $fn]
close $fn

set objfn [string map {.asm .obj} $filename]
set lstfn [string map {.asm .lst} $filename]

puts $objfn
puts $lstfn

set ::objf [open $objfn w]
set ::lstf [open $lstfn w]
set done 0

for {set pass 1} {!$done} {incr pass 1} {

    assemble $txt $pass
    set done 1
}

dump_equates
dump_labels

close $::objf
close $::lstf


