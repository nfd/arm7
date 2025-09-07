cmake_minimum_required(VERSION 3.14)

set(APPLE FALSE) # disable apple specific flags
set(CMAKE_SYSTEM_NAME Generic)

# use arm-none-eabi-gcc toolchain
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_RANLIB arm-none-eabi-ranlib)

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

project(Demos LANGUAGES C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES start.S helloworld_c.c)
add_executable(helloworld_c ${SOURCES})

target_compile_options(helloworld_c PRIVATE -march=armv4t -mcpu=arm7tdmi -nostdlib -ffreestanding -O2)
target_link_options(helloworld_c PRIVATE -T ${CMAKE_SOURCE_DIR}/linker.lds -nostdlib -ffreestanding -O2)
add_custom_command(TARGET helloworld_c POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary helloworld_c helloworld_c.bin
	COMMENT "Creating helloworld_c.bin"
)
