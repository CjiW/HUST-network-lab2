cmake_minimum_required(VERSION 3.10)
project(StopWait)

SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src_stopwait SW_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src_gbn GBN_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src_sr SR_SRC_LIST)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

ADD_EXECUTABLE(stop_wait ${SW_SRC_LIST})
ADD_EXECUTABLE(gbn ${GBN_SRC_LIST})
ADD_EXECUTABLE(sr ${SR_SRC_LIST})

FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})