# ===============
# COMMON SETTINGS
# ===============
PROJECT(tulip)

# =========
# CMAKE 2.8
# =========
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# =================
# SET PYTHON OPTION
# =================
OPTION(buildPythonInterface "Build Python Interface" ON)

# ===================
# BUILD DOCUMENTATION
# ===================
OPTION(buildDocumentation "Build Documentation" OFF)

# ===================
# INCLUDES TEST CASES
# ===================
OPTION(buildTests "Build tulip Test Suite" OFF)

# ====================================================================
# Includes Bertini model with equation parser (compilation takes time)
# ====================================================================
OPTION(addModelBertini "Add Bertini parser model" OFF)

# ===============================
# SET FOLDERS FOR CMAKE UTILITIES
# ===============================
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# =========================================
# SET OUTPUTS FOR EXECUTABLES AND LIBRARIES
# =========================================
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ===================
# DEFINE SOURCE FILES
# ===================
# ADD SOURCES IN CURRENT FOLDER
FILE(GLOB UQ_SRC_C "${CMAKE_CURRENT_SOURCE_DIR}/src/nets/*.cpp" 
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/actions/*.cpp" 
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/odeModels/*.cpp"  
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/odeIntegrators/*.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/*.cpp"                    
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/data/*.cpp" 
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/*.cpp" 
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/thirdParty/*.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/uqCore/*.cpp")

# LIST OF HEADER FILES
FILE(GLOB UQ_SRC_H "${CMAKE_CURRENT_SOURCE_DIR}/src/nets/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/actions/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/odeModels/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/odeIntegrators/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/models/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/data/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/thirdParty/*.h"
                   "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/uqCore/*.h")

# ADD OTHER BUBDIRECTORIES FOR SPECIFIC OPTIONS
IF(addModelBertini)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/models/bertini)
  FILE(GLOB SRC_BERTINI_C "${CMAKE_CURRENT_SOURCE_DIR}/src/models/bertini/*.cpp")
  FILE(GLOB SRC_BERTINI_H "${CMAKE_CURRENT_SOURCE_DIR}/src/models/bertini/*.h")
  SET(UQ_SRC_C ${UQ_SRC_C} ${SRC_BERTINI_C})
  SET(UQ_SRC_H ${UQ_SRC_H} ${SRC_BERTINI_H})
ENDIF()

# ====================
# SWIG INTERFACE FILES
# ====================
# UQ functionalities - Uncertainty Quantification
FILE(GLOB PY_I_UQ "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/uqCore/tulipUQ.i")

# CM functionalities - Computational Models for Hemodynamics
IF(addModelBertini)
  FILE(GLOB PY_I_CM "${CMAKE_CURRENT_SOURCE_DIR}/src/models/bertini/tulipCM.i")
ELSE()
  FILE(GLOB PY_I_CM "${CMAKE_CURRENT_SOURCE_DIR}/src/models/tulipCM.i")
ENDIF()

# AC functionalities - High level algorithms for uncertainty analysis
FILE(GLOB PY_I_AC "${CMAKE_CURRENT_SOURCE_DIR}/src/actions/tulipAC.i")

# DA functionalities - Data structures
FILE(GLOB PY_I_DA "${CMAKE_CURRENT_SOURCE_DIR}/src/data/tulipDA.i")

# NT functionalities - Inference Networks
FILE(GLOB PY_I_NT "${CMAKE_CURRENT_SOURCE_DIR}/src/nets/tulipNT.i")

# COPY TEST FOLDER
file(COPY "${CMAKE_SOURCE_DIR}/test/." DESTINATION "${CMAKE_BINARY_DIR}/test/.")

# COPY PYTHON LIBRARIES
file(COPY "${CMAKE_SOURCE_DIR}/py/." DESTINATION "${CMAKE_BINARY_DIR}/py/.")

# DEFINE FOLDERS VARIABLES
SET(LPN_SRCS_DIR "./src")

# ADD SUBDIRECTORY FOR DOCUMENTATION
SET(LPN_DOCS_DIR "./docs")

# =====================
# INCLUDE MPI LIBRARIES
# =====================
FIND_PACKAGE(MPI REQUIRED)
IF(MPI_FOUND)
  SET(CMAKE_C_COMPILER ${MPI_C_COMPILER})
  SET(CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
ELSE()
  MESSAGE(FATAL_ERROR "ERROR: MPI Libraries required.")
ENDIF()

# =====================
# INCLUDE BOOST LIBRARY
# =====================
FIND_PACKAGE(Boost 1.40 REQUIRED)
IF(Boost_FOUND)
MESSAGE(STATUS "Boost Library Included")
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ELSE()
  MESSAGE(FATAL_ERROR "ERROR: Boost Library required.")
ENDIF()

# ================================
# INCLUDE LINEAR ALGEBRA LIBRARIES
# ================================
FIND_PACKAGE(Armadillo REQUIRED)
IF(ARMADILLO_FOUND)
  MESSAGE(STATUS "Armadillo Library Included")
  INCLUDE_DIRECTORIES(${ARMADILLO_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "ERROR: Armadillo Library required.")
ENDIF()

# COMPILER FLAGS
ADD_DEFINITIONS("-g -m64 -O3 -std=c++14 -fPIC -Werror")

# INCLUDE FOLDERS
INCLUDE_DIRECTORIES(
  "${CMAKE_SOURCE_DIR}/src/"
  "${CMAKE_SOURCE_DIR}/src/nets/"
  "${CMAKE_SOURCE_DIR}/src/actions/"
  "${CMAKE_SOURCE_DIR}/src/models/odeModels/"
  "${CMAKE_SOURCE_DIR}/src/models/odeIntegrators/"
  "${CMAKE_SOURCE_DIR}/src/models/"
  "${CMAKE_SOURCE_DIR}/src/data/"
  "${CMAKE_SOURCE_DIR}/src/tools/"
  "${CMAKE_SOURCE_DIR}/src/tools/thirdParty"
  "${CMAKE_SOURCE_DIR}/src/tools/uqCore")

# =================
# BUILD C++ LIBRARY
# =================
ADD_SUBDIRECTORY(${LPN_SRCS_DIR})

# ================================
# INSTALL DOXYGEN IF NOT AVAILABLE
# ================================
IF(buildDocumentation)
  
  OPTION(USE_SYSTEM_DOXYGEN "Use the pre-compiled version of Doxygen configured for your system" ON)

  # INSTALL DOXYGEN IF REQUIRED
  IF( NOT USE_SYSTEM_DOXYGEN )
    INCLUDE(installDoxy)
  ENDIF()

  # SET UP CUSTOM TARGET: MAKE DOC
  ADD_SUBDIRECTORY(docs)

ENDIF()

# ======================
# INSTALL PYTHON MODULES
# ======================
IF(buildPythonInterface)

  OPTION(USE_SYSTEM_SWIG "Use a pre-compiled version of SWIG 2.0 previously configured for your system" ON)

  # INSTALL SWIG IF REQUIRED
  IF( NOT USE_SYSTEM_SWIG )
    INCLUDE(installSwig)
    LIST(APPEND ${CMAKE_PROJECT_NAME}_DEPENDENCIES Swig)
  ENDIF()

  # USE SWIG TO GENERATE THE PYTHON LIBRARIES
  ADD_SUBDIRECTORY(py)

ENDIF()

# ================
# BUILD TEST SUITE
# ================
IF(buildTests)

  ADD_SUBDIRECTORY(test)

ENDIF()
