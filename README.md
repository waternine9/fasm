# fasm

Ever felt assembly was too hard? Well here's a mock ARM assembly.

## Guide

You can build the C++ file then run it, and follow instructions.

## Ok, cool, but how do I use this thing?

Here's a tutorial:

`.vars:`  
Here you store your variables, for example:  
`variable FF 1`  
What FF means is that, that variable is an array with 255 elements (converted from hexadecimal). However, that variable will only store the first element of that array and its not a pointer either. First element gets set to 1, as specified by the last hexadecimal.  
`pointer 1 variable`  
Here we declared a pointer to the first element of the array we initialized, since pointers are single variables we set the number of elements to 1. Note how we use the name of the variable to point to, to declare a pointer to that variable.  
`.inst:`  
Here you store your actual instructions, for example:  
`mov r0 *pointer`  
`int 2`  
Woah, what is this wizardry? I will explain. mov instruction moves a constant or a variable/register's contents to a destination which must not be constant, in this case register 0 (also, there are 8 registers 0 - 7) but it can be a variable as well, and vice-versa. The * operator simply dereferences the pointer.  
Now lets talk about the int instruction. It has 4 codes which you can pass in: 0 (print register 0 as an integer), 1 (exit program), 2 (print register 0 as character), and 3 (take a single character input and push it onto the stack). In this case we use 2 to print out register 0 as an integer.  
`pmov pointer DB`  
This instruction is a more specialized mov instruction, where it takes the source and copies it to the dereferenced address of the destination instead. So in this case we would be moving the hexadecimal value DB into the first element of that array we initialized earlier.
`inc pointer`  
What the inc instruction does is just increments a non-constant value by 1. In this case we are incrementing pointer to go to the next element in the array.  
`push pc`  
Two new things here. The push instruction, all it does is push things to the stack. In this case we are pushing the program counter, which is what line the CPU is reading. You can modify pc using mov, inc, dec, and other stuff but it is advised not to.  
`cmp pc, variable`  
The CMP instruction compares the first argument with the second by subtracting the second argument from the first and putting the result in a special register.  
`jmp eq 1A`  
What the jmp instruction does is it jumps to a certain line (last argument, can be constant or non-constant value) under a certain condition. In this case the condition is eq, which means that it returns True if the special register mentioned earlier equals 0, gt is for greater than (which returns True if the special register is over 0), and lt (which is the same thing as gt, but it checks if the special register is less than zero)  
`pop r0`  
What the pop instruction does is it pops the last element of the stack, and stores the value which was popped in the first argument passed in, which in this case is register 0  
`int 1`  
Finally, exit the program.
