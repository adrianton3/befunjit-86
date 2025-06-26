cmake_minimum_required(VERSION 3.31)
project(b86)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra)

add_executable(b86 src/main.cpp
        src/core/Vec2.h
        src/core/Cursor.h
        src/core/Pathlet.cpp
        src/core/Pathlet.h
        src/core/Field.cpp
        src/core/Field.h
        src/core/StaticBindings.h
        src/core/Graph.cpp
        src/core/Graph.h
        src/core/generate.cpp
        src/core/generate.h
        src/core/push.cpp
        src/core/push.h
        src/core/Path.cpp
        src/core/Path.h
        src/core/Vec2.cpp
        src/core/Cursor.cpp
        src/core/readPlayfield.cpp
        src/core/readPlayfield.h
        src/parts/readPlayfield.cpp
        src/parts/readPlayfield.h
        src/parts/findPathlet.cpp
        src/parts/findPathlet.h
        src/parts/findGraph.cpp
        src/parts/findGraph.h
        src/parts/runLine.cpp
        src/parts/runLine.h
        src/parts/run.cpp
        src/parts/run.h
        src/core/generateOpt.cpp
        src/core/generateOpt.h
        src/parts/findGraphOptimize.cpp
        src/parts/findGraphOptimize.h
        src/core/Binary.cpp
        src/core/Binary.h)
