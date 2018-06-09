cmake_minimum_required(VERSION 3.10)
project(utility)

add_library(
        logger
        SHARED
        ""
)

add_library(utils::logger ALIAS logger)

target_compile_features(
    logger
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        logger
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        logger
        PUBLIC
        $<BUILD_INTERFACE:${utility_SOURCE_DIR}/include>
)

target_sources(
        logger
        PRIVATE
        logger.cpp
        include/Log.h
)


add_library(
        unique_id_generators
        SHARED
        ""
)

add_library(utils::unique_id_generators ALIAS unique_id_generators)

target_compile_features(
    unique_id_generators
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        unique_id_generators
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        unique_id_generators
        PUBLIC
        $<BUILD_INTERFACE:${utility_SOURCE_DIR}/include>
)

target_sources(
        unique_id_generators
        PRIVATE
        definitions.cpp
        include/UniqueIdGenerator.hpp
)