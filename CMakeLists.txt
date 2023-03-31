cmake_minimum_required(VERSION 3.22)
project(SecondLevelFS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(SecondLevelFS
        Buf.h
        BufferManager.cpp
        BufferManager.h
        Utility.h
        DiskDriver.h
        File.cpp
        File.h
        FileSystem.cpp
        FileSystem.h
        INode.cpp
        INode.h
        main.cpp
        OpenFileManager.cpp
        OpenFileManager.h
        FileManager.cpp
        FileManager.h
        UserCall.cpp
        UserCall.h
        Kernel.cpp
        Kernel.h
        DiskDriver.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
