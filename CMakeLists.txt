cmake_minimum_required(VERSION 3.14)

project(Arm7 LANGUAGES CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES src/main.cpp)
add_executable(Arm7 ${SOURCES})

# cross-compile arm asm from demos/
add_custom_command(OUTPUT helloworld.o
	COMMAND arm-none-eabi-as -o helloworld.o ${CMAKE_SOURCE_DIR}/demos/helloworld.s
	DEPENDS ${CMAKE_SOURCE_DIR}/demos/helloworld.s
	COMMENT "Assembling helloworld.s to helloworld.o"
)

add_custom_command(OUTPUT helloworld.bin
	COMMAND arm-none-eabi-objcopy -O binary -j .text helloworld.o helloworld.bin
	DEPENDS helloworld.o
	COMMENT "Creating helloworld.bin"
)

add_custom_target(helloworld ALL
	DEPENDS helloworld.bin
	COMMENT "Building helloworld demo"
)
