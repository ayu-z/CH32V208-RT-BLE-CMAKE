set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.20)

option(BUILD_WITH_BLE "=== Project USE BLE ===" ON)

# 工具链设置
set(TOOLPATH ${CMAKE_SOURCE_DIR}/Toolchain/RISC-V\ Embedded\ GCC/bin/riscv-none-embed-)

if (WIN32)
    MESSAGE(STATUS "Now is windows!")
    set(CMAKE_C_COMPILER ${TOOLPATH}gcc.exe)
    set(CMAKE_CXX_COMPILER ${TOOLPATH}g++.exe)
    set(CMAKE_ASM_COMPILER ${TOOLPATH}gcc.exe)
    set(CMAKE_AR ${TOOLPATH}ar.exe)
    set(CMAKE_OBJCOPY ${TOOLPATH}objcopy.exe)
    set(CMAKE_OBJDUMP ${TOOLPATH}objdump.exe)
    set(SIZE ${TOOLPATH}size.exe)
# elseif (UNIX)
#     MESSAGE(STATUS "Now is UNIX-like OS!")
#     set(CMAKE_C_COMPILER ${TOOLPATH}gcc)
#     set(CMAKE_CXX_COMPILER ${TOOLPATH}g++)
#     set(CMAKE_ASM_COMPILER ${TOOLPATH}gcc)
#     set(CMAKE_AR ${TOOLPATH}ar)
#     set(CMAKE_OBJCOPY ${TOOLPATH}objcopy)
#     set(CMAKE_OBJDUMP ${TOOLPATH}objdump)
#     set(SIZE ${TOOLPATH}size)
else ()
    MESSAGE(FATAL_ERROR "Unsupported system!")
endif ()

project(ch32v208-freertos C CXX ASM)
# 定义芯片型号宏
add_compile_definitions(CH32V20x_D8W)

# 项目设置
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# 编译参数 一般不用改
add_compile_options(-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore)
add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common)

# 链接参数
add_link_options(-T ${CMAKE_SOURCE_DIR}/Core/Ld/Link.ld)
add_link_options(-march=rv32imac -mabi=ilp32 -nostartfiles -Xlinker --gc-sections -Wl,--print-memory-usage -Wl,-Map,${PROJECT_NAME}.map --specs=nano.specs --specs=nosys.specs)

# 编译等级
add_compile_options(-O2 -g)

# 编译信息等级
add_compile_options(-Wall)

# 定义SDK公共头文件路径变量
set(SDK_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Core/RVMSIS
    ${CMAKE_SOURCE_DIR}/Drivers/Debug
    ${CMAKE_SOURCE_DIR}/Drivers/Debug/printf
    ${CMAKE_SOURCE_DIR}/Drivers/Peripheral/inc
    ${CMAKE_SOURCE_DIR}/Kernel
    ${CMAKE_SOURCE_DIR}/Kernel/FreeRTOS
    ${CMAKE_SOURCE_DIR}/Kernel/FreeRTOS/include
    ${CMAKE_SOURCE_DIR}/Kernel/FreeRTOS/portable/GCC/RISC-V
)

# 添加BLE相关头文件路径
if(BUILD_WITH_BLE)
    list(APPEND SDK_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/Drivers/BLE/LIB
        ${CMAKE_SOURCE_DIR}/Drivers/BLE/HAL/include
    )
endif()

set(SDK_INCLUDE_DIRS ${SDK_INCLUDE_DIRS} CACHE INTERNAL "SDK include directories")

include_directories(${SDK_INCLUDE_DIRS})



# SDK源码文件
file(GLOB_RECURSE SOURCES 
                "Core/RVMSIS/*.c"
                "Core/Startup/*.S"
                "Drivers/Debug/*.c"
                "Drivers/Debug/printf/*.c"
                "Drivers/Peripheral/src/*.c"
                "Kernel/FreeRTOS/*.c"
                "Kernel/FreeRTOS/portable/GCC/RISC-V/*.c"
                "Kernel/FreeRTOS/portable/GCC/RISC-V/*.S"
                "Kernel/FreeRTOS/portable/MemMang/heap_4.c"
                )




add_executable(${PROJECT_NAME}.elf)
target_sources(${PROJECT_NAME}.elf
                PUBLIC
                    ${SOURCES}
                )

# 添加APP子目录
add_subdirectory(APP)
target_link_libraries(${PROJECT_NAME}.elf app_lib)

# 使用BLE
if(BUILD_WITH_BLE)
    MESSAGE(STATUS "Build with BLE!")
include_directories(
                    Drivers/BLE/LIB
                    Drivers/BLE/HAL/include
                    )   
file(GLOB_RECURSE HAL_SOURCES 
                "Drivers/BLE/HAL/*.c"
                "Drivers/BLE/LIB/*.S"
                )
target_sources(${PROJECT_NAME}.elf
                PUBLIC
                    ${HAL_SOURCES}
                )                
target_link_libraries(${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/Drivers/BLE/LIB/libwchble.a)

endif()

# 链接静态库
# target_link_libraries(${PROJECT_NAME}.elf printfloat)

# 输出hex和bin
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LST_FILE}
        COMMAND ${SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
)
