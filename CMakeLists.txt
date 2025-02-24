cmake_minimum_required(VERSION 3.10)
project(VehicleSim)

set(CMAKE_CXX_STANDARD 17)

# Set vcpkg toolchain file if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
endif()

# Add include directory
include_directories(include)

# Find JSON library (nlohmann/json)
include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Option for Kafka support
option(USE_KAFKA "Build with Kafka support" OFF)

# Set source files
set(SOURCES
        src/main.cpp
        src/FilePublisher.cpp
)

if(USE_KAFKA)
    # Find RdKafka package
    find_package(RdKafka CONFIG)
    if(RdKafka_FOUND)
        add_definitions(-DUSE_KAFKA)
        list(APPEND SOURCES src/KafkaPublisher.cpp)
        message(STATUS "Found librdkafka: ${RdKafka_VERSION}")

        # For MinGW, we need to use the DLL directly
        if(MINGW)
            set(RDKAFKA_LIBRARIES
                    "${CMAKE_PREFIX_PATH}/bin/rdkafka.dll"
                    "${CMAKE_PREFIX_PATH}/bin/rdkafka++.dll")
        endif()
    else()
        message(WARNING "librdkafka not found, building without Kafka support")
        set(USE_KAFKA OFF)
    endif()
endif()

# Add executable
add_executable(vehicle_sim ${SOURCES})

# Link libraries
target_link_libraries(vehicle_sim PRIVATE nlohmann_json::nlohmann_json)

if(USE_KAFKA AND RdKafka_FOUND)
    target_link_libraries(vehicle_sim PRIVATE RdKafka::rdkafka RdKafka::rdkafka++)
endif()