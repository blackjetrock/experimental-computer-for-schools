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

set ::DEBUG      0
set ::PASS       0
set ::NUM_PASSES 3

set ::INST_INFO {
    {"^nop$"                                                              inst_1_nop       00}
    {"^dw([0-9]+)$"                                                       inst_1_dw        00}

    {"R([0-9]+)[<][-]R([0-9]+)[+]R([0-9]+)"                               inst_1_Rc_Rc_Rd  10}
    {"R([0-9]+)[<][-]R([0-9]+)[-]R([0-9]+)"                               inst_1_Rc_Rc_Rd  11}
    {"R([0-9]+)[<][-]R([0-9]+)[-]R([0-9]+)"                               inst_1_Rc_Rd_Rc  12}

    {"^R([0-9]+)[<][-]R([0-9]+)[+]([0-9A-Za-z_]+$)"                       inst_1_Rc_Rc_d   00}
    {"^R([0-9]+)[<][-]R([0-9]+)[-]([0-9A-Za-z_]+$)"                       inst_1_Rc_Rc_d   01}
    {"^R([0-9]+)[<][-]([0-9A-Za-z_]+)$"                                   inst_1_Rc_d      03}

    {"testR([0-9]+)([a-zA-Z0-9_=><]+)"                                    inst_1_test      05}
    {"^leftshiftR([0-9]+)by([0-9A-Za-z_]+)places$"                        inst_1_Rc_d      06}
    {"^rightshiftR([0-9]+)by([0-9A-Za-z_]+)places$"                       inst_1_Rc_d      07   "0. complete"}

    {"^R([0-9]+)[<][-]R([0-9]+)$"                                         inst_1_Rc_Rd     13}
    {"copysignandrhsixdigitsofR([0-9]+)intoR([0-9]+)"                     inst_1_Rd_Rc     14}
    
    {"^leftshiftR([0-9]+)byR([0-9]+)places$"                              inst_1_Rc_Rd     16}
    {"^rightshiftR([0-9]+)by[(]R([0-9]+)[)]places$"                       inst_1_Rc_Rd     17}
    {"^stopanddisplay[(]R([0-9]+)[)]and[(]R([0-9]+)[)]$"                  inst_1_Rc_Rd     19}
    {"^display[(]R([0-9]+)[)]and[(]R([0-9]+)[)]$"                         inst_1_Rc_Rd     19}
    {"^stopanddisplayR([0-9]+)andR([0-9]+)$"                              inst_1_Rc_Rd     19}
    {"^displayR([0-9]+)andR([0-9]+)$"                                     inst_1_Rc_Rd     19}

    
    {"[(]R0,R1[)]<-([a-zA-Z0-9_)(+]+)"                                    inst_1_branch    .0   "2. .. 6."}  
    {"([a-zA-Z0-9_)(+]+)<-[(]R0,R1[)]"                                    inst_1_branch    .1}
    {"branchto([a-zA-z0-9_]+)ifcl1"                                       inst_1_branch    .5}
    {"branchto([a-zA-z0-9_]+)ifcl0"                                       inst_1_branch    .6}
    {"branchto([a-zA-z0-9_]+)$"                                           inst_1_branch    .4}
    {"input([a-zA-Z0-9_]+)"                                               inst_1_branch    .8}


    {"^([0-9a-zA-Z_+)(]+)[<][-]([0-9A-Za-z_+)(]+[+][0-9A-Za-z_+)(]+)[+]([0-9A-Za-z_+)(]+[+][0-9A-Za-z_+)(]+)$"  inst_3_a_b_c      .0  "7. .. 9."}
    {"^([0-9a-zA-Z_+)(]+)[<][-]([0-9A-Za-z_)(]+)[+]([0-9A-Za-z_)(]+)$"  inst_3_a_b_c      .0  "7. .. 9."}   
    {"^([0-9a-zA-Z_+)(]+)[<][-]([0-9A-Za-z_+)(]+)[-]([0-9A-Za-z_+)(]+)$"  inst_3_a_b_c      .1}
    {"^([0-9a-zA-Z_+)(]+)[<][-]([0-9A-Za-z_+)(]+)[*]([0-9A-Za-z_+)(]+)$"  inst_3_a_b_c      .2}
    {"^([0-9a-zA-Z_+)(]+)[<][-]([0-9A-Za-z_+)(]+)[/]([0-9A-Za-z_+)(]+)$"  inst_3_a_b_c      .3}
    {"^branchto([0-9a-zA-Z_+)(]+)if([0-9a-zA-Z_+)(]+)[=]([0-9a-zA-Z_+)(]+)$"  inst_3_a_b_c  .4}
    {"^branchto([0-9a-zA-Z_+)(]+)if([0-9a-zA-Z_+)(]+)[>]([0-9a-zA-Z_+)(]+)$"  inst_3_a_b_c  .5}
    {"^branchto([0-9a-zA-Z_+)(]+)if[|]([0-9a-zA-Z_+)(]+)[|][>][|]([0-9a-zA-Z_+)(]+)[|]$"  inst_3_a_b_c  .6}
    {"^display([0-9a-zA-Z_+)(]+)[,]([0-9A-Za-z_+)(]+)[,]([0-9A-Za-z_+)(]+)$"  inst_3_a_b_c      .9}
}

################################################################################

proc dbg {str} {
    if { $::DEBUG } {
	puts $str
    }
}

################################################################################

set ::ERRORS 0

proc inst_error {e} {
    puts "\n*** ERROR *** $e"
    puts "\n$::LINE"

    incr ::ERRORS 1
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
    {3 "^([a-zA-Z0-9_]+)[+][(]R3[)]+$" }
    {4 "^([a-zA-Z0-9_]+)[+][(]R4[)]$" }
    {5 "^([a-zA-Z0-9_]+)[+][(]R5[)]$" }
    {6 "^[(]([a-zA-Z0-9_]+)[)]$" }
}

proc determine_opcode_a {arg} {
    dbg "Arg:'$arg'"
    
    foreach t6 $::TABLE6_INFO {
	set opcode_a [lindex $t6 0]
	set fmt      [lindex $t6 1]

	dbg "f='$fmt'"
	if { [regexp -- $fmt $arg all v] } {
	    dbg "Found"
	    return [list $opcode_a $v]
	}
    }
    
    return { "?" "--" }
}

set ::TABLE4_INFO {
    {0 "^=0$" }
    {1 "^>0$" }
    {2 "^<0$" }
    {3 "^LHD=0$" }
    {4 "^RHD=0$" }
}

proc determine_opcode_d_test {arg} {
    foreach t4 $::TABLE4_INFO {
	set opcode_d [lindex $t4 0]
	set fmt      [lindex $t4 1]

	if { [regexp -- $fmt $arg] } {
	    return $opcode_d
	}
    }
return "?"
}

################################################################################

proc inst_1_branch {line fmt opcode} {
    if { [regexp -- $fmt $line all dest] } {
	dbg "  Branch dest:$dest"
	set dest [substitute_equates $dest]
	dbg "  Branch dest:$dest"
	set dest [substitute_labels $dest]
	dbg "  Branch dest:$dest"

	set opcode_a_lst [determine_opcode_a $dest]

	dbg $opcode_a_lst
	
	set opcode_a [lindex $opcode_a_lst 0]
	set dest     [lindex $opcode_a_lst 1]
	
	set opcode "$opcode_a[lindex [split $opcode ""] 1]"

	if { 1 } {
	if { [catch {
	    set od [format "%02d" $dest]
	}]
	 } {
	    set od "--"
	}
	} else {
	    set od [format "%02d" $dest]
	}
	
	
       	set retval "$opcode$od"
	
	# Check arguments are valid

	# return the opcode
	return $retval
	
    } else {
	inst_error "INST failed:$fmt $line" 
    }
    
}

proc inst_1_test {line fmt opcode} {
    if { [regexp -- $fmt $line all c test] } {
	set c [substitute_equates $c]
	set c [substitute_labels $c]

	set opcode_d [determine_opcode_d_test $test]

       	set retval "$opcode$c$opcode_d"
	
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

proc inst_1_Rd_Rc {line fmt opcode} {
    if { [regexp -- $fmt $line all d c] } {

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

#-------------------------------------------------------------------------------

set ::OPCODE_A_3ADDR_INFO {
    {8 "R[3-5]"}
    {9 "[(][0-9a-zA_Z_]+[)]" }
    {7 "[0-9a-zA_Z_]+"}
}

set ::OPCODE_CD_3ADDR_INFO {

    {8 "([0-9a-zA_Z_]+)[+*/-][(]R[3-5][)]"}
    {9 "[(]([0-9a-zA_Z_]+)[)]"}
    {7 "([0-9a-zA_Z_]+)"}
}

proc determine_opcode_a_3addr {x} {
    foreach y $::OPCODE_A_3ADDR_INFO {
	set opcode [lindex $y 0]
	set fmt    [lindex $y 1]

	dbg "FMT:'$fmt' x:'$x'"
	
	if { [regexp -- $fmt $x] } {
	    return $opcode
	}
    }
    return "?"
}

proc determine_opcode_cd_3addr {x} {
    foreach y $::OPCODE_CD_3ADDR_INFO {
	set opcode [lindex $y 0]
	set fmt    [lindex $y 1]

	if { [regexp -- $fmt $x all v] } {
	    dbg "Found $fmt"
	    return $v
	}
    }
    return "--"
}

proc inst_3_a_b_c {line fmt opcode} {
    
    if { [regexp -- $fmt $line all a b c] } {
	dbg "a='$a' b='$b' c='$c' before subst"
	
	set a [substitute_equates $a]
	set a [substitute_labels  $a]
	
	set b [substitute_equates $b]
	set b [substitute_labels  $b]
	
	set c [substitute_equates $c]
	set c [substitute_labels  $c]

	dbg "a='$a' b='$b' c='$c' after eq/lab"

	# Save the original form of the argument so we can detect the addressing mode later
	set a1 $a
	
	set a [determine_opcode_cd_3addr $a]
	set b [determine_opcode_cd_3addr $b]
	set c [determine_opcode_cd_3addr $c]

	dbg "a='$a' b='$b' c='$c'"
	
	set a [format "%02d" $a]
	set b [format "%02d" $b]
	set c [format "%02d" $c]

	dbg "a='$a' b='$b' c='$c'"
	
	set opcode_a [determine_opcode_a_3addr $a1]
	set opcode "$opcode_a[lindex [split $opcode ""] 1]"
	
       	set retval "$opcode$a$b$c"

	# Skip an extra 4 digits on address increment
	address_skip_extra
	
	# return the opcode
	return $retval
	
    } else {
	inst_error "INST failed:$fmt $line" 
    }
    
}

#-------------------------------------------------------------------------------

proc inst_1_dw  {line fmt opcode} {
    if { [regexp -- $fmt $line all a] } {
	dbg "DW: a='$a'"
	set a1 [format "%04d" [string trimleft $a "0"]]
	dbg "'$a1'"
	return $a1
    }
    
    return "...."
}

proc inst_1_nop {line fmt opcode} {
    return "0000"
}

################################################################################

set ::EQUATE_NAMES {}

proc substitute_equates {in} {

    foreach eq $::EQUATE_NAMES {
	dbg "eq:'$eq'= [set ::EQUATE_VALUE($eq)]"
	set in [string map "$eq [set ::EQUATE_VALUE($eq)]" $in]
    }
    
return $in
}

proc substitute_labels {in} {

    foreach lab $::LABEL_NAMES {
	dbg "label:'$lab'  -> $::LABEL_VALUE($lab)"
	set in [string map "$lab [set ::LABEL_VALUE($lab)]" $in]
    }
    
return $in
}

################################################################################

set ::ADDRESS          0
set ::ADDRESS_A        0
set ::ADDRESS_A_CHAR " "
set ::ADDRESS_SKIP_EXTRA 0

proc address_skip_extra {{n 1}} {
    set ::ADDRESS_SKIP_EXTRA $n
}

proc next_address_1 {} {
    if { $::ADDRESS_A == 0 } {
	set ::ADDRESS_A 1
	set ::ADDRESS_A_CHAR A
    } else {
	set ::ADDRESS_A 0
	set ::ADDRESS_A_CHAR " "
	incr ::ADDRESS 1
    }
}

## Move address counter on by one and then the extra count
proc next_address {} {
    next_address_1

    for {set i 0} {$i<$::ADDRESS_SKIP_EXTRA} {incr i 1} {
	next_address_1
    }

    set ::ADDRESS_SKIP_EXTRA 0
}


################################################################################

proc lst_no_nl {str} {
    if { $::PASS == $::NUM_PASSES } {
	puts -nonewline $::lstf $str
    }
}

proc lst {str} {
    if { $::PASS == $::NUM_PASSES } {
	puts $::lstf $str
    }
}

#-------------------------------------------------------------------------------

# Output goes to the .obj and .lst files


proc assemble {t} {
    
    foreach line [split $t "\n"] {
	set ::LINE $line
	
	set original_line $line
	
	# Remove comments
	if { [regexp "(.*);(.*)" $line all a b] } {
	    lst [format "%20s  %s" "" $line]
	    set line $a
	}

	# Remove spaces
	set line [string map {" " "" "\t" ""} $line]
	
	# Handle equates
	if { [regexp "(.*)equ(.*)" $line all equate value] } {

	    # Next two lines arein this order so we don't try to substitute this eqyate's value
	    # before it is set up
	    set ::EQUATE_VALUE($equate) [substitute_equates $value]
	    if { [lsearch -exact $::EQUATE_NAMES $equate] == -1 } {
		lappend ::EQUATE_NAMES $equate
	    }
	    
	    #puts "EQ $equate = $::EQUATE_VALUE($equate)"
	    continue
	}
	
	# Store and remove labels
	if { [regexp "(.*):(.*)" $line all label b] } {
	    if { [lsearch -exact $::LABEL_NAMES $label] == -1 } {
		lappend ::LABEL_NAMES $label
	    }

	    # Addresses (labels) always point to the first instruction in a word
	    # We store the flag with the label to detect errors, or fixes to the
	    # branches
	    
	    set ::LABEL_VALUE($label) $::ADDRESS
	    set ::LABEL_A_VALUE($label) $::ADDRESS_A
	    set line $b
	}

	set length [string length $line]

	# If length is zero then ignore the line
	if { $length == 0 } {
	    continue
	}

	# ORG
	if { [regexp "org(.*)" $line all value] } {
	    set ::ADDRESS [substitute_equates $value]
	    set ::ADDRESS_A 0
	    set ::ADDRESS_A_CHAR " "
	    lst [format "%20s  %s" "" $line]
	    continue
	}
	
	# Find and assemble the instruction
	# Assemble all the passes and emit listing only on last pass
	
	set found 0

	# Tidy up the source and add that to the end
	# Align with the colon of labels
	
	if { [regexp -- {(.+):(.*)} $original_line all a b] } {
	    set src [format "%20s: %s" $a $b]
	} else {
	    set src [format "%20s  %s" "" $original_line]
	}
	
	foreach inst $::INST_INFO {
	    set f [lindex $inst 0]
	    set p [lindex $inst 1]
	    set opcode [lindex $inst 2]
	    
	    if { [regexp -- $f $line] } {
		set found 1
		dbg "----------------------------------------"
		dbg "ASM:Line:'$line'"
		dbg "ASM:FMT :'$f'"

		set object [$p $line $f $opcode]

		dbg "ASM: obj:'$object'"
		lst [format "%04d%s %8s  %s" $::ADDRESS $::ADDRESS_A_CHAR $object $src]

		emit_object $object
		break
	    }
	}
	
	if { !$found } {
	    lst [format "%04d%s %8s  %s" $::ADDRESS $::ADDRESS_A_CHAR "" $src]
	}
    	
	# Added complication is that there are 4 digit and 8 digit instructions. Two 4 digit
	# instructions are packed into a word, one 8 digit in a word. There is therefore an
	# 'A' flag which is used to address the second 4 digit instruction in a word. Branches cannot
	# jump to this second instruction, so NOPs have to be inserted if needed
	# If an 8 digfit instruction follows a four digit instruction in the first half of a word then
	# a NOP has to be inserted
	next_address
    }
}

################################################################################
#
# Emit object in various ways
#
#

proc start_emit_object {filename} {
    set ::ascobjfn  [string map {.asm .ascii.obj}  $filename]
    set ::cfnobjfn  [string map {.asm .cfn.c}      $filename]
    set ::fascobjfn [string map {.asm .fascii.obj} $filename]

    filename_display "C Function file"             $::cfnobjfn
    filename_display "ASCII object file"           $::ascobjfn
    filename_display "Formatted ASCII object file" $::fascobjfn

    set ::cfnobjf  [open $::cfnobjfn  w]
    set ::ascobjf  [open $::ascobjfn  w]
    set ::fascobjf [open $::fascobjfn w]

    puts $::cfnobjf "void cli_load_xxxx(void)"
    puts $::cfnobjf "    \{"
    puts $::cfnobjf "    "
    puts $::cfnobjf "  ESC_STATE *s = &esc_state;"
    puts $::cfnobjf "  int i = 0;"
    puts $::cfnobjf "  "
}

proc end_emit_object {} {

    # Complete data?
    
    if { $::c_collected != 0 } {
	set pad "0000"
	puts $::cfnobjf "  s->store[i++] = 0x$::c_collect$pad;"
    }
    
    puts $::cfnobjf "\}"
    puts $::cfnobjf ""

    close $::cfnobjf
    
    close $::ascobjf

    puts $::fascobjf ""
    
    close $::fascobjf
}

set ::b_count 0

set ::c_collected 0
set ::c_collect ""

proc emit_object {obj} {

    if { $::PASS != $::NUM_PASSES } {
	return
    }
    
    # Split the object into two character units (bytes, really for the simulation and probablky the real machine)
    
    foreach {a b} [split $obj ""] {

	# C Function output for CLI load
	set ::c_collect "$::c_collect$a$b"
	incr ::c_collected 2

	if { $::c_collected == 8 } {
	    puts $::cfnobjf "  s->store\[i++\] = 0x$::c_collect;"
	    set ::c_collect ""
	    set ::c_collected 0
	}
	
	# Ascii output
	puts -nonewline $::ascobjf "$a$b "

	# Formatted ascii
	if { ($::b_count % 8) == 0 } {
	    puts -nonewline $::fascobjf [format "\n%04d: " [expr $::b_count / 4]]
	}

	puts -nonewline $::fascobjf "$a$b "

	incr ::b_count 1
    }
}

################################################################################
#
# Dump the label data
#
################################################################################

proc dump_labels {} {
    lst ""
    lst "Labels"
    lst "------"
    lst ""
    
    foreach labelname $::LABEL_NAMES {
	set labelvalue  $::LABEL_VALUE($labelname)
	set labelavalue $::LABEL_A_VALUE($labelname)
	if { $labelavalue } {
	    set label_a "A"
	} else {
	    set label_a " "
	}
	
	lst [format "%20s: %04d%s" $labelname $labelvalue $label_a]
    }
}

proc dump_equates {} {
    lst ""
    lst "Equates"
    lst "-------"
    lst ""

    foreach eqname $::EQUATE_NAMES {
	set equatevalue $::EQUATE_VALUE($eqname)
	
	lst [format "%20s EQU %04d" $eqname $equatevalue]
    }
}

################################################################################

proc filename_display {desc fn} {
    set width 40
    set s [format "%-$width\s: %s" $desc $fn]
    puts $s
}

################################################################################

set filename [lindex $argv 0]

filename_display "Input file" $filename

set fn [open $filename]
set txt [read $fn]
close $fn

start_emit_object $filename

set lstfn [string map {.asm .lst} $filename]



filename_display  "List file" $lstfn


set ::lstf [open $lstfn w]

set ::LABEL_NAMES {}
set ::EQUATE_NAMES {}

for {set ::PASS 1} {$::PASS <= $::NUM_PASSES} {incr ::PASS 1} {
    set ::ERRORS 0
    assemble $txt
}

# Reset pass so following output comes out
set ::PASS $::NUM_PASSES

dump_equates
dump_labels


switch $::ERRORS {
    1 {
	set s ""
    }
    
    default {
	set s "s"
    }
}

puts "$::ERRORS error$s"
lst ""
lst  "$::ERRORS error$s"


close $::lstf

end_emit_object
