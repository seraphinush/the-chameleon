cmake_minimum_required(VERSION 3.1) 
project(chameleon)

# Set c++11
# https://stackoverflow.com/questions/10851247/how-to-activate-c-11-in-cmake
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()
set (CMAKE_CXX_STANDARD 11)

# nice hierarchichal structure in MSVC
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Find OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(IS_OS_MAC 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(IS_OS_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(IS_OS_WINDOWS 1)
else()
  message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif()

# Create executable target

# Generate the shader folder location to the header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/project_path.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/project_path.hpp")

# You can switch to use the file GLOB for simplicity but at your own risk
# file(GLOB SOURCE_FILES src/*.cpp src/*.hpp)

set(SOURCE_FILES
	src/common.cpp
  src/constants.hpp
	src/main.cpp
	src/spotter.cpp
  src/wanderer.cpp
	src/world.cpp
  src/char.cpp
  src/map.cpp
  src/start_screen.cpp
  src/control_screen.cpp  
  src/complete_screen.cpp
  src/hud.cpp       
  src/cutscene.cpp    
  src/shooter.cpp
  src/bullets.cpp
  src/overlay.cpp
  src/overlay.hpp
  src/level_screen.cpp
  src/particles.cpp
  src/pause_screen.cpp
  src/gameover_screen.cpp
  src/project_path.hpp
	src/common.hpp
	src/spotter.hpp
  src/wanderer.hpp
	src/world.hpp
  src/start_screen.hpp
  src/control_screen.hpp
  src/complete_screen.hpp
  src/char.hpp
	src/map.hpp
  src/hud.hpp      
  src/cutscene.hpp  
  src/particles.hpp
	src/shooter.hpp
	src/bullets.hpp
  src/level_screen.hpp
  src/pause_screen.hpp
  src/gameover_screen.hpp
  src/timer.cpp
  src/timer.hpp
	)

if (IS_OS_MAC)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

# Added this so policy CMP0065 doesn't scream
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS 0)


# External header-only libraries in the ext/

target_include_directories(${PROJECT_NAME} PUBLIC ext/stb_image/)
target_include_directories(${PROJECT_NAME} PUBLIC ext/gl3w)
target_include_directories(${PROJECT_NAME} PUBLIC ext/freetype/include)

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)

if (OPENGL_FOUND)
   target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
   target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
endif()

# glfw, sdl could be precompiled (on windows) or installed by a package manager (on OSX and Linux)

if (IS_OS_LINUX OR IS_OS_MAC)
    # Try to find packages rather than to use the precompiled ones
    # Since we're on OSX or Linux, we can just use pkgconfig.
    find_package(PkgConfig REQUIRED)
    
    pkg_search_module(GLFW REQURIED glfw3)

    pkg_search_module(SDL2 REQURIED sdl2)
    pkg_search_module(SDL2MIXER REQURIED SDL2_mixer)
    
    # Link Frameworks on OSX
    if (IS_OS_MAC)
       find_library(COCOA_LIBRARY Cocoa)
       find_library(CF_LIBRARY CoreFoundation)
       target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY} ${CF_LIBRARY})
    endif()
elseif (IS_OS_WINDOWS)
# https://stackoverflow.com/questions/17126860/cmake-link-precompiled-library-depending-on-os-and-architecture
    set(GLFW_FOUND TRUE)
    set(SDL2_FOUND TRUE)
    set(FREETYPE_FOUND TRUE)
    
    set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include")
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/include/SDL")
    set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/include")
    set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/include/config")

    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x64.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.lib")
	      set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.lib")
        set(FREETYPE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win64/freetype.lib")

	set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x64.dll")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.dll")
	set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.dll")
  set(FREETYPE_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win64/freetype.dll")
    else()
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x86.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.lib")
	set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.lib")
  set(FREETYPE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win32/freetype.lib")
	
	set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x86.dll")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.dll")
	set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.dll")
  set(FREETYPE_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win32/freetype.dll")
    endif()

    # Copy and rename dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDLMIXER_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FREETYPE_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/freetype.dll")
endif()

# Can't find the include and lib. Quit.
if (NOT GLFW_FOUND OR NOT SDL2_FOUND OR NOT FREETYPE_FOUND)
   if (NOT GLFW_FOUND)
      message(FATAL_ERROR "Can't find GLFW." )
  elseif (NOT FREETYPE_FOUND)
      message(FATAL_ERROR "Can't find Freetype." )
   else ()
      message(FATAL_ERROR "Can't find SDL." )
   endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${FREETYPE_LIBRARY} )

# Needed to add this
if(IS_OS_LINUX)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})
endif()
