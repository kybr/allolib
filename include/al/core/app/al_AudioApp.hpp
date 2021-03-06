#ifndef INCLUDE_AL_AUDIOAPP_HPP
#define INCLUDE_AL_AUDIOAPP_HPP

/*  Keehong Youn, 2017, younkeehong@gmail.com
*/

#include "al/core/io/al_AudioIO.hpp"

namespace al {

class AudioApp {
public:
  AudioIO mAudioIO;
  AudioIO& audioIO(){ return mAudioIO; }
  const AudioIO& audioIO() const { return mAudioIO; }

  /// Initialize audio
  /// @param[in] sampleRate. Unsupported values will use default rate of device.
  /// @param[in] blockSize. Number of sample frames to process per callback
  /// @param[in] outputChannels. Number of output channels to open. -1 for all
  /// @param[in] inputChannels. Number of input channels to open. -1 for all
  void initAudio(
    double audioRate, int audioBlockSize,
    int audioOutputs, int audioInputs
  );

  // initialize audio with default values from default device
  enum AudioIOConfig : unsigned int {
      // binary literal supported since c++14
      IN_ONLY = 0b1,
      OUT_ONLY = 0b10,
      IN_AND_OUT = 0b11
  };
  void initAudio(AudioIOConfig config = OUT_ONLY);

  bool usingAudio() const;
  void beginAudio();
  void endAudio();

  virtual void onSound(AudioIOData& io) {}
    
};

}

#endif // !INCLUDE_AL_AUDIOAPP_HPP

