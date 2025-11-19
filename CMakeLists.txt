cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем статическую линковку MinGW (Windows)
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# Собираем exe из всех .cpp файлов проекта
file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

add_executable(SoftwareRenderer ${SRC_FILES})
