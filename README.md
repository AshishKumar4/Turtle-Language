# Turtle-Language
An attempt at writing a simple Python-like interpreted programing language. Everything from scratch, No Regex.

## Currrently, support for Float, Boolean Operators, Classes, Arbitary length integers, lists, dicts and For loops not added

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