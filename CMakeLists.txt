cmake_minimum_required(VERSION 3.10)
project(lab2)

set(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src_stopwait SW_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src_gbn GBN_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src_sr SR_SRC_LIST)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(stop_wait ${SW_SRC_LIST})
add_executable(gbn ${GBN_SRC_LIST})
add_executable(sr ${SR_SRC_LIST})

find_library(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(stop_wait  ${NETSIM_LIB})
target_link_libraries(gbn  ${NETSIM_LIB})
target_link_libraries(sr  ${NETSIM_LIB})