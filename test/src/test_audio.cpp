#include "al/core/io/al_AudioIO.hpp"
#include <cmath>

using namespace al;

TEST_CASE( "Audio Device Enum" ) {
    AudioDevice::printAll();
}

static void callback(AudioIOData &io) {
    static double phase = 0.0;
    static double phaseInc = M_2PI * 440.0 / io.framesPerSecond();
    while (io()) {
        io.out(0) = std::sin(phase);
        phase += phaseInc;
        if (phase > M_2PI) {
            phase -= M_2PI;
        }
    }
}


TEST_CASE( "Audio IO Object" ) {
    int userData = 5;
    AudioIO audioIO;
#ifdef AL_WINDOWS
	bool use_out = true;
	bool use_in = false;
	audioIO.initWithDefaults(callback, &userData, use_out, use_in);
#else
	audioIO.init(callback, &userData, 64, 44100.0, 2, 2);
#endif
    audioIO.print();
    REQUIRE(audioIO.user<int>() == 5);
    REQUIRE(audioIO.open());
    REQUIRE(audioIO.start());
    al_sleep(0.5);
    REQUIRE(audioIO.stop());
    REQUIRE(audioIO.close());
    REQUIRE(audioIO.user<int>() == 5);
}

TEST_CASE("Audio Channels/Virtual Channels") {
    AudioIO audioIO;
    audioIO.init(nullptr, nullptr, 256, 44100, 1, 1);

    // Make sure parameters match those passed to constructor
    audioIO.open();
    REQUIRE(audioIO.framesPerBuffer() == 256);
    REQUIRE(audioIO.fps() == 44100);
    REQUIRE(audioIO.channelsOut() == 1);
    REQUIRE(audioIO.channelsIn() == 1);
    audioIO.close();

    // Test virtual channels
    int maxChansOut = AudioDevice::defaultOutput().channelsOutMax();
    int maxChansIn  = AudioDevice::defaultInput().channelsInMax();
    audioIO.channelsOut(maxChansOut + 1);
    audioIO.channelsIn (maxChansIn  + 1);
    audioIO.open();
    REQUIRE(audioIO.channelsOutDevice() == maxChansOut); // opened all hardware channels?
    REQUIRE(audioIO.channelsOut() == (maxChansOut+1)); // got our extra virtual channel?
    REQUIRE(audioIO.channelsInDevice() == maxChansIn); // opened all hardware channels?
    REQUIRE(audioIO.channelsIn() == (maxChansIn+1)); // got our extra virtual channel?
    audioIO.close();
}

