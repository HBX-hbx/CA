CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(riscv-manycore-simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options("-Wall" "-W" "-Wextra")

set(SIMULATOR_INCLUDE_DIRECTORIES include)

aux_source_directory(./common COMMON_SRCS)
add_library(CommonLibrary ${COMMON_SRCS})
target_include_directories(CommonLibrary PUBLIC ${SIMULATOR_INCLUDE_DIRECTORIES})

add_executable(instruction-test ${PROJECT_SOURCE_DIR}/program/instruction_test.cpp)
target_link_libraries(instruction-test PUBLIC CommonLibrary)

add_executable(elf-test ${PROJECT_SOURCE_DIR}/program/elf_test.cpp)
target_link_libraries(elf-test PUBLIC CommonLibrary)

aux_source_directory(./frontend FRONTEND_SRCS)
add_library(FrontendLibrary ${FRONTEND_SRCS})
target_include_directories(FrontendLibrary PUBLIC ${SIMULATOR_INCLUDE_DIRECTORIES})
target_link_libraries(FrontendLibrary PUBLIC CommonLibrary)

aux_source_directory(./backend BACKEND_SRCS)
add_library(BackendLibrary ${BACKEND_SRCS})
target_include_directories(BackendLibrary PUBLIC ${SIMULATOR_INCLUDE_DIRECTORIES})
target_link_libraries(BackendLibrary PUBLIC CommonLibrary)

add_executable(processor-test ${PROJECT_SOURCE_DIR}/program/processor_test.cpp)
target_link_libraries(processor-test 
                        PUBLIC CommonLibrary
                        PUBLIC FrontendLibrary
                        PUBLIC BackendLibrary)

add_executable(checker ${PROJECT_SOURCE_DIR}/program/checker.cpp)
target_include_directories(checker PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty)
target_link_libraries(checker
                        PUBLIC CommonLibrary
                        PUBLIC FrontendLibrary
                        PUBLIC BackendLibrary)

add_subdirectory(test) # NOTE: Simply comment out this line if you don't have riscv toolchain
