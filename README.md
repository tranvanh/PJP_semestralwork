# Semestral Work

Implementation of a simple Mila compiler (base on LLVM) for CTU FIT BI-PJP subject

### LLVM version

LLVM 6.0.1_3

## Build

```
cd cmake-build-debug &&
cmake ..
make
```

**To rebuild:**
```
cd cmake-build-debug &&
make
```

##RUN

**Compile all sample files**

```
./test
```
all shell executable files will be saved in "./sample" directory

**Compile single file**
```
./cmake-build-debug/mila <source file path>
clang++ output.o -o <shell executable name>
./<shell executable name>
```
