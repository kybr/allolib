# sets:
#   RTMIDI_INCLUDE_DIR
#   RTMIDI_SRC
#   RTMIDI_LIBRARIES
#   RTMIDI_DEFINITIONS

if(IS_DIRECTORY "${al_path}/external/rtmidi")
  if (AL_VERBOSE_OUTPUT)
    message("Using RtMidi")
  endif()

  set(RTMIDI_INCLUDE_DIR ${al_path}/external/rtmidi)
  set(RTMIDI_SRC external/rtmidi/RtMidi.cpp)

  if(AL_LINUX)
    # Use settings from RtAudio as they might conflict if different
    if(RtAudioLinuxAPI STREQUAL "jack")
      set(RTMIDI_DEFINITIONS -D__UNIX_JACK__)
      list(APPEND RTMIDI_LIBRARIES
        jack pthread
        )
    elseif(RtAudioLinuxAPI STREQUAL "alsa")
        set(RTMIDI_DEFINITIONS -D__LINUX_ALSA__)
        list(APPEND RTMIDI_LIBRARIES
          asound pthread
          )
    endif()
  endif(AL_LINUX)

  if(AL_MACOS)
    # add_definitions(-D__MACOSX_CORE__)
    set(RTMIDI_DEFINITIONS -D__MACOSX_CORE__)
    find_library(COREAUDIO_LIB CoreAudio)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    find_library(COREMIDI_LIB CoreMidi)
    list(APPEND RTMIDI_LIBRARIES
      ${COREAUDIO_LIB}
      ${COREFOUNDATION_LIB}
      ${COREMIDI_LIB}
      pthread
    )
  endif(AL_MACOS)

  if(AL_WINDOWS)
    set(RTMIDI_DEFINITIONS -D__WINDOWS_WASAPI__)
    list(APPEND RTMIDI_LIBRARIES
      ole32
      winmm
      ksuser
      uuid
    )
  endif(AL_WINDOWS)
endif() # RTMIDI
