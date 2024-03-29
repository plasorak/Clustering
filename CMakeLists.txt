cmake_minimum_required(VERSION 3.8)

project(DAQWork)
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net XMLIO)
find_program(LSB_RELEASE_EXEC lsb_release)
find_package(TBB REQUIRED)

include(${ROOT_USE_FILE})
string(CONCAT CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra -Werror -Wno-error=deprecated-copy")
#grabs the distribution
execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
  OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# set(Gperftools_ROOT_DIR "/Users/plasorak/Documents/Sussex-Postdoc/Physics/gperftools/")
# string(CONCAT CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/Users/plasorak/Documents/Sussex-Postdoc/Physics/Clustering/clustering/")

# if the distribution is scientific linux, then setup boost in a different way (not using the cmake file from boost)
string(FIND "${LSB_RELEASE_ID_SHORT}" "Scientific" _cmp)
if (_cmp GREATER -1)
  set(Boost_NO_BOOST_CMAKE ON)
else()
  string(CONCAT CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -std=c++1y")
endif()

find_package(Boost REQUIRED)
#find_package(Gperftools REQUIRED) 
#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/BurstTrigger)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Algorithm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Analyser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/BurstTrigger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Clustering)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/DataContainer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Dumper)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/IO)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QuickAnalyses)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/RadiologicalCheatGenerator)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TDRPlots)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TriggerDecision)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Utils)
