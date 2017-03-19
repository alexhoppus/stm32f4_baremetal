#!/usr/bin/expect
spawn telnet localhost 4444
expect -re ".*>"
send "reset halt\r"
expect -re ".*>"
send "flash write_image erase $env(PWD)/main.elf\r"
expect -re ".*>"
send "reset\r"
expect -re ".*>"
send "exit\r"
