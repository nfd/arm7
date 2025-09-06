## ARM7?

This is an experiment with C++ templating which happens at the same time to emulate a very small
subset of ARM7 to prove the concept.

# Requirements

* CMake 3.14+
* Clang
* arm-none-eabi-* for cross-compilation

# Building

    mkdir build
    cmake ..
    make

# Run
    ./Arm7 helloworld.bin


