.globl main 

.equ N, 10
.data
A: .word 0,1,2,7,-8,4,5,-12,11,-2 //0x2160 0x28
B: .word 0,1,2,7,-8,4,5,12,-11,-2 //0x2188 0x28
.bss
C: .space 4*N //0x21C0 0x28 
//this numbers may change between different exe but 
//we can check the position of the tables by oppening the 
//memory in 0x8000140c +0x8
//add break point to sw a3,0(t2) to see the table changing 
//by running and open the memory in 0x21C0 0x28 
.text

main:
//display the position of all the tables in 0x8000140c +0x8  to know where to open the memory
    la t0, A 
    lui a5,0x80001
    sw t0,1036(a5)
    la t0,B 
    sw t0,1040(a5)
    la t0,C
    sw t0,1044(a5)

nice:
    li a0,0 //init the counter to run this N times
    li a1,4*(N) //top end
    la t0, A //take the position of all the tables in t0,t1,t2
    la t1, B
    la t2, C
loop_make_c:
    lw a3,0x0(t0) //take the element of A that t0 points to =>start from 0
    lw a4,0x24(t1) // take the element of B that t1 points to => start from the 24 the N-1 element

    add a3,a3,a4 //add the 2 elements of the tables
    blt a3,zero,abs_C // if the result is below zero make it positive
return_form_abs_C:
    sw a3,0(t2) //store the result in the position the t2 points to
    add t0,t0,4 // make the pointer points to the next element of the array (we are going forward)
    add t1, t1,-4 //make the pointer points to the previous element of the array(we are going backword)
    add t2,t2,4 //make the pointer of the result points to the next element to fill up
    add a0,a0,4 // add the counter to run N times the for loop
    bne a0,a1,loop_make_c // if we haven't reach the top end repeat
j nice

abs_C:
    not a3,a3 // take the supplement of 1 of the result
    add a3,a3,1 // take the supplement of 2 of the result
    j return_form_abs_C



.end