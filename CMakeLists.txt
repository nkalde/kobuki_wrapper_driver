CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (kobuki_wrapper_driver)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

INCLUDE (FindPkgConfig)

SET (PLAYERCORE_LIBRARY_DIRS )
LIST (APPEND PLAYERCORE_LIBRARY_DIRS "/usr/local/lib64")

SET (PLAYERCORE_INCLUDE_DIRS )
LIST (APPEND PLAYERCORE_INCLUDE_DIRS "/opt/ros/indigo/include" "/usr/include/eigen3" "/usr/local/include/player-3.0" )


PLAYER_ADD_PLUGIN_DRIVER (kobukiwrapperdriver SOURCES kobuki_wrapper_driver.cc kobuki_wrapper_driver.h kobuki_player.h kobuki_player.cc LIBDIRS /opt/ros/indigo/lib LINKFLAGS -lkobuki)
#diagnostics.hpp diagnostics.cpp)

ADD_LIBRARY(libkobuki SHARED IMPORTED)
SET_TARGET_PROPERTIES(libkobuki PROPERTIES IMPORTED_LOCATION 
/opt/ros/indigo/lib/libkobuki.so)
TARGET_LINK_LIBRARIES(kobukiwrapperdriver libkobuki)

