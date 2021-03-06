#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

cmake_minimum_required(VERSION 2.8.1)

project(itkDataSHImageWriterPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION ${medInria_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)

include(${dtk_USE_FILE})
include(dtkPlugin)

##find_package(TTK REQUIRED)
##include(${TTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

SET(${PROJECT_NAME}_LIBRARIES
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGIPL
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOVTK
  ITKIOMRC
)

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkDataSHImageWriterPlugin.h
  itkDataSHImageWriterBase.h
  itkMetaDataSHImageWriter.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkDataSHImageWriterPlugin.cpp
  itkDataSHImageWriterBase.cpp
  itkMetaDataSHImageWriter.cpp)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
  ${${PROJECT_NAME}_LIBRARIES}
)
  
if(BUILD_DOCUMENTATION)
	dtk_build_doc(${PROJECT_NAME})
endif(BUILD_DOCUMENTATION)

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################

SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
