#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

#define AUDIO_FILE_PATH "master_spark.wav"
#define AUDIO_FILE_SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 1024

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