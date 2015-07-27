# CMakeLists.txt generated 2015/07/23 18:26:08
# by vcproj05.pl from C:\GTools\tools\Sudoku\Sudoku.sln

# local CMake Scripts.
# set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules )

cmake_minimum_required (VERSION 2.8.8)

project (Sudoku)

# Set EXE type
set( EXE_TYPE WIN32 )   # set default WIN32 GUI (Uses WinMain() instead of main())
option( BUILD_WIN32_EXE   "Set OFF to build a console app, using main()"  ON  )
# Allow developer to select is Dynamic or static library built
set( LIB_TYPE STATIC )  # set default static
option( BUILD_SHARED_LIB  "Set ON to build shared (DLL) Libraries"        OFF )

# Setup MSVC 3rd party directories if available and needed
# include( ConfigureMsvc3rdParty )

if(CMAKE_COMPILER_IS_GNUCXX)
    set(WARNING_FLAGS_CXX "-Wall")
    set(WARNING_FLAGS_C   "-Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
   set (WARNING_FLAGS_CXX "-Wall -Wno-overloaded-virtual")
   set (WARNING_FLAGS_C   "-Wall")
endif() 

if(WIN32 AND MSVC)
    set(MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS")
    # turn off various warnings
    # foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
    foreach(warning 4996)
        set(MSVC_FLAGS "${MSVC_FLAGS} /wd${warning}")
    endforeach()
    
    # to distinguish between debug and release lib in windows
    set( CMAKE_DEBUG_POSTFIX "d" ) # little effect in unix
else()
    set(MSVC_FLAGS "${MSVC_FLAGS} -Wno-write-strings -Wno-unused-variable -Wno-unused-but-set-variable")
    set(BUILD_WIN32_EXE NO)
    #  CMAKE_THREAD_LIBS_INIT     - the thread library
    find_package(Threads REQUIRED)
    list( APPEND add_LIBS ${CMAKE_THREAD_LIBS_INIT} )
endif()

# Uncomment to REDUCE the Windows configurations buildable
# set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE) # Disables MinSizeRel & MaxSpeedRel

set (BOOST_CXX_FLAGS "-DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION -DBOOST_BIMAP_DISABLE_SERIALIZATION")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS_C} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS_CXX} ${MSVC_FLAGS} -D_REENTRANT ${BOOST_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}")

if (BUILD_WIN32_EXE)
    set( EXE_TYPE WIN32 )
    add_definitions( -DBUILD_WIN32_EXE )
    message(STATUS "*** Building WIN32 GUI EXE - EXE_TYPE=${EXE_TYPE}")
else ()
    set( EXE_TYPE )
    message(STATUS "*** Building console binary (uses main())")
endif ()

if (BUILD_SHARED_LIB)
    set( LIB_TYPE SHARED )
    message(STATUS "*** Building shared (DLL) Library - LIB_TYPE=${LIB_TYPE}")
else ()
    set( LIB_TYPE STATIC )
    message(STATUS "*** Building static Library - LIB_TYPE=${LIB_TYPE}")
endif ()

set(name OpenSudoku)
set(dir src)
set( ${name}_SRCS
    ${dir}/Sudo_AIC.cxx
    ${dir}/Sudo_BMP.cxx
    ${dir}/Sudo_Change.cxx
    ${dir}/Sudo_Color.cxx
    ${dir}/Sudo_Color2.cxx
    ${dir}/Sudo_Debug.cxx
    ${dir}/Sudo_Dialogs.cxx
    ${dir}/Sudo_FIle2.cxx
    ${dir}/Sudo_File.cxx
    ${dir}/Sudo_Ini.cxx
    ${dir}/Sudo_Keys.cxx
    ${dir}/Sudo_List.cxx
    ${dir}/Sudo_Locked.cxx
    ${dir}/Sudo_Logic.cxx
    ${dir}/Sudo_Menu.cxx
    ${dir}/Sudo_NakedPTQ.cxx
    ${dir}/Sudo_Paint.cxx
    ${dir}/Sudo_PntColors.cxx
    ${dir}/Sudo_Print.cxx
    ${dir}/Sudo_SET.cxx
    ${dir}/Sudo_SLink2.cxx
    ${dir}/Sudo_SLink3.cxx
    ${dir}/Sudo_SLinks.cxx
    ${dir}/Sudo_Scraps.cxx
    ${dir}/Sudo_Strat.cxx
    ${dir}/Sudo_Thread.cxx
    ${dir}/Sudo_Time.cxx
    ${dir}/Sudo_Timer.cxx
    ${dir}/Sudo_URect.cxx
    ${dir}/Sudo_URect2.cxx
    ${dir}/Sudo_Utils.cxx
    ${dir}/Sudo_XBrowse.cxx
    ${dir}/Sudo_XCycle3.cxx
    ${dir}/Sudo_XCycles.cxx
    ${dir}/Sudo_XYChain.cxx
    ${dir}/Sudo_XYZWings.cxx
    ${dir}/sprtf.cxx
    )
set( ${name}_HDRS
    ${dir}/Sudo_AIC.hxx
    ${dir}/Sudo_BMP.hxx
    ${dir}/Sudo_Change.hxx
    ${dir}/Sudo_Color.hxx
    ${dir}/Sudo_Color2.hxx
    ${dir}/Sudo_Debug.hxx
    ${dir}/Sudo_Dialogs.hxx
    ${dir}/Sudo_File.hxx
    ${dir}/Sudo_Ini.hxx
    ${dir}/Sudo_Keys.hxx
    ${dir}/Sudo_List.hxx
    ${dir}/Sudo_Locked.hxx
    ${dir}/Sudo_Logic.hxx
    ${dir}/Sudo_Menu.hxx
    ${dir}/Sudo_NakedPTQ.hxx
    ${dir}/Sudo_Paint.hxx
    ${dir}/Sudo_PntColors.hxx
    ${dir}/Sudo_Print.hxx
    ${dir}/Sudo_SET.hxx
    ${dir}/Sudo_SLink2.hxx
    ${dir}/Sudo_SLink3.hxx
    ${dir}/Sudo_SLinks.hxx
    ${dir}/Sudo_Scraps.hxx
    ${dir}/Sudo_Strat.hxx
    ${dir}/Sudo_Thread.hxx
    ${dir}/Sudo_Time.hxx
    ${dir}/Sudo_Timer.hxx
    ${dir}/Sudo_URect.hxx
    ${dir}/Sudo_URect2.hxx
    ${dir}/Sudo_Utils.hxx
    ${dir}/Sudo_XBrowse.hxx
    ${dir}/Sudo_XCycle3.hxx
    ${dir}/Sudo_XCycles.hxx
    ${dir}/Sudo_XYChain.hxx
    ${dir}/Sudo_XYZWings.hxx
    ${dir}/sprtf.hxx
    ${dir}/targetver.h
    )
add_library( ${name} ${LIB_TYPE} ${${name}_SRCS} ${${name}_HDRS} )
list( APPEND add_LIBS ${name} )

### 1 executables #############################################
# Project [Sudoku], type [Application], 39 C/C++, 40 Hdrs, 0 Other, 0 INCS, 5 DEFS
# File dir [C:\GTools\tools\Sudoku\], out [C:\GTools\tools\Sudoku], rel = []
set(name Sudoku)
set(dir src)
set( ${name}_SRCS
    ${dir}/Sudoku.cxx
    )
set( ${name}_HDRS
    ${dir}/Sudoku.hxx
    )
if (WIN32)
    list(APPEND ${name}_SRCS ${dir}/Sudoku.rc)
    list(APPEND  ${name}_HDRS ${dir}/resource.h ${dir}/unistd.h )
else ()
    list(APPEND ${name}_SRCS ${dir}/unixglue.cxx)
    list(APPEND ${name}_HDRS ${dir}/unixglue.hxx)
endif ()
add_executable( ${name} ${EXE_TYPE} ${${name}_SRCS} ${${name}_HDRS} )
if (add_LIBS)
    message(STATUS "*** Linking with libs ${add_LIBS}")
    target_Link_libraries(${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
if (NOT BUILD_WIN32_EXE)
    set_target_properties(${name} PROPERTIES OUTPUT_NAME Sudo-con)
endif ()

# deal with install, if required
# install(TARGETS ${inst_LIBS}
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib )
# install(TARGETS ${inst_BINS} DESTINATION bin)
# install(FILES ${inst_HDRS} DESTINATION include)

# eof
