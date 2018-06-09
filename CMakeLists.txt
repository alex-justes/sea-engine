cmake_minimum_required(VERSION 3.10)
project(engine)

add_library(engine SHARED "")
add_library(core::engine ALIAS engine)

target_compile_features(
    engine
    PUBLIC
        cxx_std_17
)

add_subdirectory(utility)

find_package(SDL2 REQUIRED)

target_link_libraries(
        engine
        PRIVATE
        SDL2::SDL2
)

target_include_directories(
        engine
        PUBLIC
        $<BUILD_INTERFACE:${engine_SOURCE_DIR}/include>
)
target_link_libraries(
        engine
        PUBLIC
        utils::unique_id_generators
        utils::logger
        stdc++fs
)

target_sources(engine PRIVATE
        include/helpers/Configuration.h
        include/core/Events.h
        include/core/Drawable.h
        src/Drawable.cpp
        src/Events.cpp
        include/core/Context.h
        src/Context.cpp
        include/core/Screen.h
        src/Screen.cpp
        include/core/Camera.h
        src/Camera.cpp
        src/Configuration.cpp
        include/core/CollisionDetectors.hpp
        include/core/BasicBehaviors.hpp
        src/BasicBehaviors.cpp
        include/helpers/Storage.hpp
        include/helpers/Containers.hpp
        include/core/Engine.h
        include/core/BasicActors.h
        src/Engine.cpp
        include/helpers/BasicContext.h
        src/BasicContext.cpp
        include/core/Types.h
        )


