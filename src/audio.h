#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

#define AUDIO_FILE_NAME "master_spark_mono.wav"
#define AUDIO_BUFFER_SIZE 1024
// 0-100, does not go over 100 because no clamping
#define AUDIO_VOLUME_PERCENT 100
// 1 for loop, 0 for no loop
// Useful for testing if you soldered the MAX98357A correctly...
#define AUDIO_LOOP 0

void AudioTask(void *pvParameters);

class AudioTaskState {
public:
    static bool playing;
    static uint32_t playbackPosition;

    static void startPlayback() {
        playing = true;
        playbackPosition = 0;
    }

    static void stopPlayback() {
        playing = false;
    }
};

#endif // AUDIO_H