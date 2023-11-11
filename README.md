# The Game of Life

## Files:

##### life.c

Facillitates primary function of Game of Life using  
functions from `set.c` and `universe.c`.

##### universe.c

Implements core of the *Universe* used for Game of Life.  
Included are the functions used to manipulate a universe,  
print it, etc.

##### universe.h

Header file for `universe.c`.

##### set.c

Implements simple set data type as unsigned integer and  
modified with turnary operators.

##### set.h

Header for `set.c`.

##### WRITEUP.pdf

Describes and shows what was learned and the main takeaways  
from this project.

##### DESIGN.pdf

Design document for the program that explains the layout  
of the project using pseudocode and some diagrams.

##### README.me

*This* document; describes briefly what each file does.

## Short Description:

This program implements the Game of Life in C. Data can  
be inputted/outputted using text files or standard in/out.

## Build:

To compile the program, run `$ make`, `$ make all`,  
or `$ make life`.  
This will generate a single executable in the current  
working directory along with several object files you can  
ignore.

## Clean:

To remove all the `.o` and executable files, run  
``$ make clean``. 

## Format:

To format all `.c` and `.h` files, execute `$ make format`.

## Scan-Build

To scan-build, run ``make scan-build``.

## Running:

To run the program, `$ ./life -[tsn:i:o:]`.  
Below is a description of the commandline options  
and their descriptions:  

t : makes universe toroidal
s : silence ncurses
n : number of generations - takes numerical argument
i : input file (stdin by default) - takes filepath
o : output file (stdout by default) - takes filepath
