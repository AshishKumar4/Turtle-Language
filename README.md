# Turtle-Language
An attempt at writing a simple Python-like interpreted programing language. Everything from scratch, No Regex.

## Currrently, support for Classes, Arbitary length integers, dicts and For loops not added

# Build Instructions -->
## Install Cmake, make, gcc-9, g++9
```
git clone https://github.com/AshishKumar4/Turtle-Language
cd Turtle-Language
mkdir build
cd build
cmake ..
make -j8
./Turtle
./Turtle ../examples/test.ty
```

## COMMENTS HAVE NOT BEEN IMPLEMENTED YET! PLEASE DON'T USE THE '//' COMMENT AS USED IN BELOW EXAMPLES!
## ITS VERY VERY YOUNG AND IMMATURE, CAN BREAK VERY EASILY. Please Raise an Issue if a bug found.
# Examples: 
## General
Most of the inspiration comes from C++, Python and NumPy Python library
Everything is case sensitive i.e thisWorks != ThisWorks.
Every block of code needs to be terminated with a semicolon ';'. This rule though isn't needed for 
inline code in Interactive Interpreter mode.
## Variables
They are Place holders, can store any element, And context sensitive
```
a = 1+2*3 + (8/3)   // --> a = 9.666667
```
## Tuples
Every block of code encapsulated in Round Brackets forms a tuple. Tuples are solved at Symbolic Execution stage. They are Immutable.
```
a = (4,5,6);
(x,y,z) = a;    // --> x = 5, y = 5, z = 6
```
## Lists
MORE SUPPORT COMING SOON! Lists are container objects similar to tuples, and can hold any type of items, but unlike tuples, are mutable. Aarray Indexing is done using a List followed by another list containing ONLY INT type values corresponding to indexes of the first list. Indexes start from 0
```
a = [5,2,7,[8,10],1,4];
b = a + 5;       // --> Element wise Addition b = [10,7,12,[13,15,],6,9,]
h = b[2,3,4];    // Array Indexing, h = [12,[13,15,],6,]
```
## CodeBlock
Every block of code encapsulated in Curly Brackets form a CodeBlock. CodeBlocks have their own private contexts, which have higher priority than the outer contexts.
## Functions
Functions are defined using the keyword 'define' followed by a 'function name', 'parameters' as tuple,
and a 'CodeBlock' which may or may not use a 'return' statement, in which case the last statement's output is used as the function's output.
```
define someCrazyFunction(a,b) 
{
    o = a * a + b;
};

someCrazyFunction(4,5); // --> Gives output as 21
```
## Function Pointers
Functions can be wrapped into variable names as well, like the concept of Function Pointers of C/C++ and python.
```
define wow(a) 
{
    return a + 4;
};

a = wow;
a(4); // --> Outputs 8
```
## Lambda Functions
Lambda Functions are anonymous functions which are not tied to any contexts, but otherwise operate very similar to that of normal functions. Defined using the keyword 'lambda'
```
a = lambda (a,b){ a*a + b };
a(4,5); // --> Outputs 21
```
## Support for Higher Order Functions
Functions and Lambdas can take as argument any function, as well as return them. These are the Mathematical concept of 'Higher Order Functions'
```
define wow(a)
{
    a('This Works!');
};

wow( lambda (x) {"WOW!, " + x} );    // --> outputs 'WOW! This Works!'
```
## Conditional Statements
They have a grammer of 'if<tuple><codeblock> elif<tuple><codeblock>...else<codeblock>'. A Codeblock is only executed iff ALL the elements of the tuple evaluate to boolean true i.e, like an AND operator
```
a = 0;
b = 0;
if(b)
{
    a = 1;
}
elif (b, 1)
{
    a = 2;
}
elif (b + 1, 0)
{
    a = 3;
}
elif (b + 1, 1)
{
    a = 4;
}
else 
{
    a = 5;
};      // Remember, Semicolon is IMPORTANT. Also, REMOVE THIS COMMENT, THESE ARE NOT SUPPORTED!

a;  // --> Gives output a = 4
```
## While Loop
They follow the simple grammer 'while<tuple><codeblock>', wherein each value of tuple should compute to a boolean true for the codeblock to execute
```
a = 0;
sum = 0;
while (a < 10)
{
    sum = sum + a;
    a = a + 1;
};  
sum;        // --> Gives output 45 i.e, 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9
```
## Dicts
## ARBITARY LENGTH INTEGERS
## STRING MANIPULATIONS
## FOR LOOP (FOR EACH)
## LIST COMPREHENTION
## CLASSES
<TO BE IMPLEMENTED!>

