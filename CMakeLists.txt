CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(textSynth)
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

Include(UseCXX11)

add_definitions(-DSRC_PATH=\"${CMAKE_SOURCE_DIR}/\")

IF(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
  SET(CMAKE_EXE_LINKER_FLAGS "/LARGEADDRESSAWARE")
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_NO_CXX11_SCOPED_ENUMS")
  find_package(Qt4 REQUIRED QtCore QtGui)
  INCLUDE(${QT_USE_FILE})
ENDIF()

#SET(Mosek_Dir $ENV{MOSEK_8_1_INSTALLDIR})
#INCLUDE_DIRECTORIES( ${Mosek_Dir}/tools/platform/win64x86/h )
SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)
SET(Eigen_Dir $ENV{EIGEN_DIR})
include_directories(.
          ${Eigen_Dir}
                    ${Eigen_Dir}/../
                    ${LIBSL_INCLUDE_DIR}
                    )



SET(CMAKE_BUILD_TYPE Debug)

set(SOURCES 
	main.cpp
	renderer.cpp
	renderer.h
	FileDialog.cpp
	FileDialog.h
	Synthetizer.cpp
	Synthetizer.h
	shader/render.fp
	shader/render.vp
	shader/render.h
)

AUTO_BIND_SHADERS( shader/render )

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

IF(NOT APPLE)
ADD_DEFINITIONS(-DUSE_GLUX)
ELSE()
ADD_DEFINITIONS(-DAPPLE)
ENDIF(NOT APPLE)

if(WIN32)
set(USER $ENV{USERNAME})
else(WIN32)
set(USER $ENV{USER})
endif(WIN32)

ADD_EXECUTABLE(textSynth ${SOURCES} ${HEADERS})
IF(WIN32)
TARGET_LINK_LIBRARIES(textSynth  ${LIBSL_LIBRARIES} ${LIBSL_GL_LIBRARIES})
ELSE(WIN32)
TARGET_LINK_LIBRARIES(textSynth  LibSL LibSL_gl pthread)
ENDIF(WIN32)



add_definitions(-DPATHTOSRC="${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(textSynth PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

