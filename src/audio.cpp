#include "audio.h"

#include <FreeRTOS.h>
#include <task.h>
#include <I2S.h>
#include <LittleFS.h>

#include "constants.h"

bool AudioTaskState::playing = false;
uint32_t AudioTaskState::playbackPosition = 0;

void AudioTask(void *pvParameters) {
    LittleFS.begin();
    File audioFile = LittleFS.open(AUDIO_FILE_PATH, "r");

    if (!audioFile) {
        Serial.println("Cannot find audio file: " AUDIO_FILE_PATH);
        vTaskDelete(NULL);
        return;
    }

    I2S i2s(OUTPUT);
    i2s.setBCLK(SPEAKER_SCL);
    // LRCLK is automatically set to SPEAKER_SCL + 1, which is SPEAKER_LRC in this case
    i2s.setDATA(SPEAKER_SDA);
    i2s.begin(AUDIO_FILE_SAMPLE_RATE);

    uint8_t buffer[AUDIO_BUFFER_SIZE];

    // Main audio processing loop
    while (1) {
        if (AudioTaskState::playing) {
            size_t bytesRead = audioFile.read(buffer, AUDIO_BUFFER_SIZE);
            if (bytesRead > 0) {
                i2s.write(buffer, bytesRead);
                AudioTaskState::playbackPosition += bytesRead;
            } else {
                AudioTaskState::stopPlayback();
                audioFile.seek(0);
            }
        } else if (audioFile.position() != 0) {
            audioFile.seek(0);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
