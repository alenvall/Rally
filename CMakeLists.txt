#/*
#-----------------------------------------------------------------------------
#Filename:    CMakeLists.txt
#-----------------------------------------------------------------------------
#*/
cmake_minimum_required(VERSION 2.6)

	set(CMAKE_USER_MAKE_RULES_OVERRIDE
		${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
	set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
		${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)

project(Rally)

if(WIN32)
	if(NOT DEFINED ENV{BOOST_LIBRARYDIR})
		message(FATAL_ERROR "BOOST_LIBRARYDIR not set?")
	endif()
	
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
endif(WIN32)
 
#TODO(Joel): Fix this...
if(UNIX_NONONONONO)
	if(EXISTS "/usr/local/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

	elseif(EXISTS "/usr/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one

	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX_NONONONONO)
# Really bad assumptions above, these seem to work... On my machine...
if(UNIX)
    set(CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/modules/;${CMAKE_MODULE_PATH}")
    set(OGRE_SAMPLES_INCLUDEPATH "/usr/include/OGRE")
endif(UNIX)
 
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
 
set(CMAKE_DEBUG_POSTFIX "_d")
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 
find_package(OGRE REQUIRED)
 
#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#  message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
#endif()
 
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()
 
# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	if (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" "1.55.0")
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time system)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	find_package(Boost QUIET)
 
	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()
 
if(WIN32)
	if(NOT BULLET_LIBS)
		set(BULLET_LIBS $ENV{BULLET_HOME}/lib)
	endif()
	if(NOT BULLET_HEADERS)
		set(BULLET_HEADERS $ENV{BULLET_HOME}/src)
	endif()
else()
	if(NOT BULLET_LIBS)
		set(BULLET_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../bullet/lib)
	endif()
	if(NOT BULLET_HEADERS)
		set(BULLET_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../bullet-src/src)
	endif()
 endif()
 
set(HDRS )

# Pretty confused about this stuff... seems to be working, eh.. /Erik
if(WIN32 AND MSVC)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	
	file (GLOB GAME_SOURCES
		src/*.h
		src/*.cpp
		src/controller/*.h
		src/controller/*.cpp
		src/model/*.h
		src/model/*.cpp
		src/view/*.h
		src/view/*.cpp
		src/util/*.h
		src/util/*.cpp
	)

	source_group(main src/*)
	source_group(controller src/controller/*)
	source_group(model src/model/*)
	source_group(view src/view/*)
	source_group(util src/util/*)
	add_executable(Rally WIN32 ${GAME_SOURCES})
	
	foreach( src_file ${GAME_SOURCES} )
		set_source_files_properties(
			${src_file}
			PROPERTIES
			COMPILE_FLAGS "/Yustdafx.h /Zm239 /FIstdafx.h"
		)
	endforeach( src_file ${GAME_SOURCES} )
	
	set_source_files_properties(src/stdafx.cpp
		PROPERTIES
		COMPILE_FLAGS "/Ycstdafx.h /Zm239"
	)
    list(APPEND GAME_SOURCES stdafx.cpp)
else()
	aux_source_directory(src SRCS)
	aux_source_directory(src/model SRCS)
	aux_source_directory(src/view SRCS)
	aux_source_directory(src/controller SRCS)
	aux_source_directory(src/util SRCS)
	
	add_executable(Rally WIN32 ${HDRS} ${SRCS})
endif()

#set(SRCS
#	./src/main.cpp
#)

if(NOT MYGUI_LIBS)
	set(MYGUI_LIBS $ENV{MYGUI_ROOT}/build/lib/Debug)
endif()
if(NOT MYGUI_INCLUDE_DIR)
	set(MYGUI_INCLUDE_DIR $ENV{MYGUI_ROOT}/MyGUIEngine/include)
endif()
if(NOT MYGUI_INCLUDE_OGRE_DIR)
	set(MYGUI_INCLUDE_OGRE_DIR $ENV{MYGUI_ROOT}/Platforms/Ogre/OgrePlatform/include )
endif()
if(NOT MYGUI_PLUGIN_DIR)
	file( TO_CMAKE_PATH $ENV{MYGUI_ROOT}/build/bin MYGUI_PLUGIN_DIR )
endif()

 
include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${OGRE_Overlay_INCLUDE_DIRS}
	${OGRE_Terrain_INCLUDE_DIRS}
	${BULLET_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MYGUI_INCLUDE_DIR}
    ${MYGUI_INCLUDE_OGRE_DIR}
)
 
 
set_target_properties(Rally PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)
	set_target_properties(Rally PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(Rally PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(Rally PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(Rally PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(Rally PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
	set_target_properties(Rally PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
   
	target_link_libraries(Rally
		${OGRE_LIBRARIES}
		${OIS_LIBRARIES}
		${OGRE_Overlay_LIBRARIES}
		${OGRE_Terrain_LIBRARIES}
		//${BULLET_LIBS}/BulletDynamics_vs2010_debug.lib
		//${BULLET_LIBS}/BulletCollision_vs2010_debug.lib
		//${BULLET_LIBS}/LinearMath_vs2010_debug.lib
		//${BULLET_LIBS}/../Extras/lib/BulletWorldImporter_vs2010_debug.lib
		//${BULLET_LIBS}/../Extras/lib/BulletFileLoader_vs2010_debug.lib
		${MYGUI_LIBS}/MyGUIEngine_d.lib
		${MYGUI_LIBS}/MyGUI.OgrePlatform_d.lib
		)
	)
	
	SET(LINK_LIBRARY1 optimized ${BULLET_LIBS}/BulletDynamics_vs2010.lib debug ${BULLET_LIBS}/BulletDynamics_vs2010_debug.lib)
	SET(LINK_LIBRARY2 optimized ${BULLET_LIBS}/BulletCollision_vs2010.lib debug ${BULLET_LIBS}/BulletCollision_vs2010_debug.lib)
	SET(LINK_LIBRARY3 optimized ${BULLET_LIBS}/LinearMath_vs2010.lib debug ${BULLET_LIBS}/LinearMath_vs2010_debug.lib)
	SET(LINK_LIBRARY4 optimized ${BULLET_LIBS}/../Extras/lib/BulletWorldImporter_vs2010.lib debug ${BULLET_LIBS}/../Extras/lib/BulletWorldImporter_vs2010_debug.lib)
	SET(LINK_LIBRARY5 optimized ${BULLET_LIBS}/../Extras/lib/BulletFileLoader_vs2010.lib debug ${BULLET_LIBS}/../Extras/lib/BulletFileLoader_vs2010_debug.lib)
	
	target_link_libraries(Rally ${LINK_LIBRARY1})
	target_link_libraries(Rally ${LINK_LIBRARY2})
	target_link_libraries(Rally ${LINK_LIBRARY3})
	target_link_libraries(Rally ${LINK_LIBRARY4})
	target_link_libraries(Rally ${LINK_LIBRARY5})
	target_link_libraries(Rally ${LINK_LIBRARY6})
else() 
	target_link_libraries(Rally
		${OGRE_LIBRARIES}
		${OIS_LIBRARIES}
		${OGRE_Overlay_LIBRARIES}
		${OGRE_Terrain_LIBRARIES}
		${BULLET_LIBS}/libBulletWorldImporter.a
		${BULLET_LIBS}/libBulletFileLoader.a
		${BULLET_LIBS}/libBulletDynamics.a
		${BULLET_LIBS}/libBulletCollision.a
		${BULLET_LIBS}/libLinearMath.a
		)
	#    -lGL -lGLU)
 endif()
 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin/debug)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin/release)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

# post-build copy for win32
if(WIN32 AND NOT MINGW)
	add_custom_command( TARGET Rally PRE_BUILD
		COMMAND if not exist .\\dist\\bin\\"$(ConfigurationName)\" mkdir .\\dist\\bin\\"$(ConfigurationName)\" )
	add_custom_command( TARGET Rally POST_BUILD
		COMMAND copy \"$(TargetPath)\" .\\dist\\bin\\"$(ConfigurationName)\\" )
endif(WIN32 AND NOT MINGW)

if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin/debug)
endif(MINGW OR UNIX)
 
if(WIN32)
 
	install(TARGETS Rally
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/ogre.cfg
		DESTINATION bin/release
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/ogre.cfg
		DESTINATION bin/debug
		CONFIGURATIONS Debug
	)
 
        # NOTE: for the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
        # so you'll have to change that to make it work with 1.7.1
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/OgreTerrain.dll
		${OGRE_PLUGIN_DIR_REL}/OgrePaging.dll
	#	${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
	#	${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D11.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_BSPSceneManager.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_PCZSceneManager.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_OctreeZone.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_OctreeSceneManager.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		${MYGUI_PLUGIN_DIR}/release/MyGUIEngine.dll
		DESTINATION bin/release
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.dll
	#	${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
	#	${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D11_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_BSPSceneManager_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_PCZSceneManager_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeZone_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		${MYGUI_PLUGIN_DIR}/debug/MyGUIEngine_d.dll
		DESTINATION bin/debug
		CONFIGURATIONS Debug
	)
 
   # as of sdk 1.7.2 we need to copy the boost dll's as well
   # because they're not linked statically (it worked with 1.7.1 though)
   install(FILES 
	#	${Boost_DATE_TIME_LIBRARY_RELEASE}
	#	${Boost_THREAD_LIBRARY_RELEASE}
		${Boost_LIBRARY_DIR}/boost_system-vc110-mt-1_55.dll	
		DESTINATION bin/release
		CONFIGURATIONS Release RelWithDebInfo
   )

   install(FILES
	#	${Boost_DATE_TIME_LIBRARY_DEBUG}
	#	${Boost_THREAD_LIBRARY_DEBUG}
		${Boost_LIBRARY_DIR}/boost_system-vc110-mt-gd-1_55.dll
		DESTINATION bin/debug
		CONFIGURATIONS Debug
   )
endif(WIN32)

if(UNIX)
 
	install(TARGETS Rally
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins-linux.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin/release
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
	
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins-linux.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin/debug
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
	
	
 
endif(UNIX)
 
