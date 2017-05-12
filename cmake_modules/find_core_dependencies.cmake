# al_path needs to be set prior to calling this script

find_package(OpenGL REQUIRED)
if (WINDOWS)
  set(lib_path ${al_path}/dependencies)
  set(GLFW_INCLUDE_DIRS ${lib_path}/glfw/include)
  set(GLFW_LIBRARIES ${lib_path}/glfw/glfw3dll.lib)
  set(GLEW_INCLUDE_DIRS ${lib_path}/glew/include)
  set(GLEW_LIBRARIES ${lib_path}/glew/glew32.lib)

  if (USE_PORTAUDIO)
    set(PORTAUDIO_INCLUDE_DIRS ${lib_path}/portaudio/include)
    set(PORTAUDIO_LIBRARIES ${lib_path}/portaudio/portaudio_x64.lib)
  endif (USE_PORTAUDIO)

  set(APR_INCLUDE_DIRS ${lib_path}/apr/include)
  set(APR_LIBRARIES ${lib_path}/apr/libapr-1.lib)
else ()
  find_package(GLEW REQUIRED)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW REQUIRED glfw3)

  if (USE_PORTAUDIO)
    pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)
  endif (USE_PORTAUDIO)

  pkg_search_module(APR REQUIRED apr-1)

  if(MACOS)
    # for rtaudio
    find_library(COREAUDIO_LIB CoreAudio)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    # also pthread will be added
  endif(MACOS)
endif (WINDOWS)