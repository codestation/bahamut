find_package(Threads REQUIRED)
find_package(PCAP REQUIRED)
find_package(LibUSB REQUIRED)

add_subdirectory(core)

FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} BAHAMUT)
  MESSAGE("Current revision is ${BAHAMUT_WC_REVISION}")
  add_definitions( -DSVN_REV="${BAHAMUT_WC_REVISION}" )
ENDIF(Subversion_FOUND)

file(GLOB bahamut_SOURCES *.cpp)
file(GLOB to_remove DeviceBridge.cpp)
list(REMOVE_ITEM bahamut_SOURCES ${to_remove})

include_directories(${PCAP_INCLUDE_DIRS})
include_directories(${bahamut_SOURCE_DIR}/src/core)

link_directories(${bahamut_SOURCE_DIR}/src/core)

add_executable(bahamut-engine ${bahamut_SOURCES})

target_link_libraries(bahamut-engine bahamut-core)
target_link_libraries(bahamut-engine ${LIBUSB_LIBRARIES})
target_link_libraries(bahamut-engine ${PCAP_LIBRARIES})
if(WIN32)
target_link_libraries(bahamut-engine wsock32 iphlpapi ws2_32)
endif(WIN32)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-O2 -pipe -Wall -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
endif()

#set(CMAKE_BUILD_TYPE Release)
