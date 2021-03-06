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

project(itkFiltersPlugin)

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

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################
set(${PROJECT_NAME}_HEADERS
  itkFiltersProcessBase_p.h
  itkFiltersAddProcess_p.h
  itkFiltersDivideProcess_p.h
  itkFiltersGaussianProcess_p.h
  itkFiltersInvertProcess_p.h
  itkFiltersMedianProcess_p.h
  itkFiltersMultiplyProcess_p.h
  itkFiltersNormalizeProcess_p.h
  itkFiltersShrinkProcess_p.h
  itkFiltersSubtractProcess_p.h
  itkFiltersWindowingProcess_p.h
  itkFiltersDilateProcess_p.h
  itkFiltersErodeProcess_p.h
  itkFiltersCloseProcess_p.h
  itkFiltersOpenProcess_p.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  itkFiltersPlugin.h
  itkFiltersToolBox.h
  itkMorphologicalFiltersToolBox.h
  itkFiltersProcessBase.h
  itkFiltersAddProcess.h
  itkFiltersSubtractProcess.h
  itkFiltersMultiplyProcess.h
  itkFiltersDivideProcess.h 
  itkFiltersGaussianProcess.h
  itkFiltersNormalizeProcess.h
  itkFiltersMedianProcess.h
  itkFiltersInvertProcess.h
  itkFiltersShrinkProcess.h
  itkFiltersWindowingProcess.h
  itkFiltersDilateProcess.h
  itkFiltersErodeProcess.h
  itkFiltersCloseProcess.h
  itkFiltersOpenProcess.h
)

set(${PROJECT_NAME}_SOURCES
  itkFiltersPlugin.cpp
  itkFiltersToolBox.cpp
  itkMorphologicalFiltersToolBox.cpp
  itkFiltersProcessBase.cpp
  itkFiltersAddProcess.cpp
  itkFiltersSubtractProcess.cpp
  itkFiltersMultiplyProcess.cpp
  itkFiltersDivideProcess.cpp
  itkFiltersGaussianProcess.cpp
  itkFiltersNormalizeProcess.cpp
  itkFiltersMedianProcess.cpp
  itkFiltersInvertProcess.cpp
  itkFiltersShrinkProcess.cpp
  itkFiltersWindowingProcess.cpp
  itkFiltersDilateProcess.cpp
  itkFiltersErodeProcess.cpp
  itkFiltersCloseProcess.cpp
  itkFiltersOpenProcess.cpp
)

## #################################################################
## Build rules
## #################################################################


configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/itkFiltersDefinitions.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/itkFiltersDefinitions.h
      )

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore dtkLog
  medCore
  medGui
  ITKCommon
  )

#dtk_build_doc(itkAddConstantToImageFilterPlugin)

# add_executable(${PROJECT_NAME}Testing
#   itkAddConstantToImageFilterTestingApp.cpp
# )
#
# target_link_libraries(${PROJECT_NAME}Testing
#   ${PROJECT_NAME}
# )

# Due to the large number of template functions, windows needs an extra compile flag.
# Currently this only affects x64/debug builds, but enabling this flag for others does no harm.
if (MSVC)
    set(bigobjFile itkFilters.cpp)
    get_source_file_property(bigobjFile_COMPILE_FLAGS ${bigobjFile} COMPILE_FLAGS)
    if (${bigobjFile_COMPILE_FLAGS} STREQUAL NOTFOUND)
        set_source_files_properties(${bigobjFile} PROPERTIES COMPILE_FLAGS "/bigobj")
    else()
        set_source_files_properties(${bigobjFile} PROPERTIES COMPILE_FLAGS "${bigobjFile_COMPILE_FLAGS} /bigobj")
    endif()
endif()

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)

