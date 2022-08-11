cmake_minimum_required(VERSION 3.22)

project(factorio_calculator)

set(CMAKE_CXX_STANDARD 17)

#
# gtest configuration after: https://crascit.com/2015/07/25/cmake-gtest/
#

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)

# executes at configure time! this means googletest's own CMakeLists.txt is
# downloaded and ready for add_subdirectory() further down
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds the following targets:
# gtest, gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build"
)

# Now simply link your own targets against gtest, gmock,
# etc. as appropriate

add_executable(factorio_test test.cpp)

# set_property(TARGET factorio_test PROPERTY
#              MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_dependencies(factorio_test gtest)
target_link_libraries(factorio_test PUBLIC gtest)
target_link_libraries(factorio_test PUBLIC gtest_main)

# makes tests runnable with
#   $ make test
enable_testing()
add_test(NAME    factorio_test
         COMMAND factorio_test)