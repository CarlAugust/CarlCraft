# CMakeList.txt : CMake project for CarlCraft, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CarlCraft")
find_package(raylib CONFIG REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source to this project's executable.

file(GLOB MY_SOURCES "src/*.c")
add_executable (CarlCraft ${MY_SOURCES})
target_link_libraries(CarlCraft PRIVATE raylib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CarlCraft PROPERTY CXX_STANDARD 20)
endif()

