#!/usr/bin/tclsh

set f [open version.h]
set txt [read $f]
close $f

if { [regexp {\#define VERSION_INC ([0-9]+)} $txt all val]}  {
    puts "Value:$val"
}

set next_val [expr $val + 1]
puts "Next value:$next_val"

regsub {\#define VERSION_INC ([0-9]+)} $txt "\#define VERSION_INC $next_val" newtxt

puts $newtxt

set f [open version.h w]

puts -nonewline $f "$newtxt"
