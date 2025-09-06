cmake_minimum_required(VERSION 3.14)
include(FetchContent)

FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG 4160d259d961cd393fd8d67590a8c7d210207348
)

FetchContent_MakeAvailable(cli11_proj)

project(Arm7 LANGUAGES CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES src/main.cpp)
add_executable(Arm7 ${SOURCES})
target_link_libraries(Arm7 PRIVATE CLI11::CLI11)

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
