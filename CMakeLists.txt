cmake_minimum_required(VERSION 3.10)
project(SecondLevelFS)

# Add the executable target
add_executable(SecondLevelFS
        BufferManager.cpp
        BufferManager.h
        UserManager.cpp
        UserManager.h
        DiskDriver.cpp
        DiskDriver.h
        File.cpp
        File.h
        FileManager.cpp
        FileManager.h
        FileSystem.cpp
        FileSystem.h
        INode.cpp
        INode.h
        Kernel.cpp
        Kernel.h
        main.cpp
        OpenFileManager.cpp
        OpenFileManager.h
        User.cpp
        User.h
        KernelAPI.cpp)

add_executable(Client
        client.cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")



# Set compiler flags
if (MSVC)
    target_compile_options(SecondLevelFS PRIVATE /W4)
else()
    target_compile_options(SecondLevelFS PRIVATE -Wall -Wextra -pedantic)
endif()

# Add include directories
target_include_directories(SecondLevelFS PRIVATE .)

# Add link libraries if needed
# target_link_libraries(SecondLevelFS some_library)

# Link macOS frameworks
if (APPLE)
    target_link_libraries(SecondLevelFS PRIVATE "-framework CoreFoundation")
endif()
