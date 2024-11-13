cmake_minimum_required(VERSION 3.10)

project(opendaq-bridge)

# Ensure correct handling for Windows builds
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-pragmas")
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

# Set build type and C++ standard
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the openDAQ package (make sure openDAQ is installed and available)
find_package(openDAQ REQUIRED)

# Define module path (required for the OpenDAQ setup)
add_compile_definitions(MODULE_PATH="${OPENDAQ_MODULES_DIR}")

if (WIN32)
    set(DRIVER_SRC
        dlfcn-win32/src/dlfcn.c
        driver.c)
else()
    set(DRIVER_SRC
        driver.c)
endif ()  

# List of source files for the shared library
set(SHARED_LIB_SRC
    lib.cpp
    BoilerplateImpl/app_channel.cpp
    BoilerplateImpl/app_descriptor.cpp
    BoilerplateImpl/app_device.cpp
    BoilerplateImpl/app_signal.cpp
    BoilerplateImpl/app_function_block.cpp
    BoilerplateImpl/app_property_object.cpp
    BoilerplateImpl/app_input_port.cpp
    BoilerplateImpl/app_sync.cpp
    BoilerplateImpl/stdout_redirect.cpp
    BoilerplateImpl/OpenDaqObject.cpp
)


#set_source_files_properties(BoilerplateImpl/app_signal.cpp PROPERTIES COMPILE_OPTIONS ${SANITIZE_OPTIONS})

#set_source_files_properties(BoilerplateImpl/app_signal.cpp PROPERTIES COMPILE_OPTIONS "-fno-sanitize=vptr")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Create the shared library (DLL in Windows)
add_library(opendaq-bridge SHARED ${SHARED_LIB_SRC})

# Link the openDAQ library to the shared library
target_link_libraries(opendaq-bridge PRIVATE daq::opendaq nlohmann_json::nlohmann_json)
# Add executables
add_executable(driver ${DRIVER_SRC})
add_executable(test-polygon test_polygon.cpp)

# Link the openDAQ library to the test-polygon executable
target_link_libraries(test-polygon PRIVATE daq::opendaq)
