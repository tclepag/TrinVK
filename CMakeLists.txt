cmake_minimum_required(VERSION 3.30)
project(TrinVK)

set(CMAKE_CXX_STANDARD 20)

add_executable(TrinVK main.cpp
        Engine/Source/Helpers/Console.h
        Engine/Source/Runtime/Core/Engine.cpp
        Engine/Source/Runtime/Core/Engine.h)
