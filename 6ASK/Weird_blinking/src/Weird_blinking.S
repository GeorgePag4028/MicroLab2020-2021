.globl main 
.text
main:
//correct values:
/*
rotate_left:
00 01 02 04 08 10 20 40 80 81 82 84 88 90 A0 C0 C1
C2 C4 C8 D0 E0 E1 E2 E4 E8 F0 F1 F2 F4 F8 F9 FA FC FD FE FF

rotate_right:
FF 7F BF DF EF F7 FB FD FE 7E BE DE EE F6 FA FC 7C BC DC EC F4 F8 78 B8 D8 E8 F0 70 B0 D0 E0 60 A0 C0 40 80 00

*/
//we have to run the programm with break point 10/13/19/27/34/45
//lines and open memory in 0x80001400
    li a0,0 //a0 the led
    li a1,0x7 //init the number of times we move the led
    li a2,0x7
    lui a5,0x80001
    
    sw a0,1024(a5) //store the led value
rotate_left:
    add a0,a0,1 // add one to the led
    sw a0,1024(a5) //store it
    add a1,a2,0 //a1=a2 // init the number of slli we need to reach to the msb we want to change
    add a3,zero,1 // init the add value we use to move the leds
how_many_times_left:
    add a0,a0,a3 // move the led
    slli a3,a3,1 // move the add value to correct the value of the leds
    sw a0,1024(a5) // store the led value
    add a1,a1,-1 //sub the number of slli we need
    bne a1,zero,how_many_times_left // do it for the number of slli in order to reach the correct position

    add a2,a2,-1 // we have fixed the msb and now the move to the previous in line
    bne a2,zero,rotate_left // do it while all the leds are not fixed

add a0,a0,1 // add the last vaue to reach FF
sw a0,1024(a5) // store the ff
li a2,0x8 // add new value to fix the lights in different dir
rotate_right:
    add a1,a2,0 // init the times of we move the led until msb
    add a3,zero,0x80; //fix the sub value
how_many_times_right:
    sub a0,a0,a3 // we sub the correct number
    sw a0,1024(a5) //store it
    add a0,a0,a3 // add it again 
    add a4,a3,0 // keep the last value of the sub value
    srli a3,a3,1 // move the sub value to correct position
    add a1,a1,-1 //sub the number of slli we need
    bne a1,zero,how_many_times_right// do it for the number of slli in order to reach the correct position

    add a2,a2,-1// we have fixed the msb and now the move to the previous in line
    sub a0,a0,a4 //in the end just sub the last sub value to correct the number (last iteration we dont want to add it again in the line 36)
    bne a2,zero,rotate_right// do it while all the leds are not fixed

sw a0,1024(a5) // show the 0x00
j main

.end