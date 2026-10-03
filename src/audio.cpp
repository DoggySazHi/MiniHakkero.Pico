#include "audio.h"

#include <FreeRTOS.h>
#include <task.h>
#include <I2S.h>

#include "constants.h"

__asm__(
    ".section .rodata\n"
    ".global audio_wav_start\n"
    ".global audio_wav_end\n"
    ".balign 4\n"
    "audio_wav_start:\n"
    ".incbin \"" AUDIO_FILE_NAME "\"\n"
    "audio_wav_end:\n"
    ".byte 0\n"
    ".previous\n"
);

extern "C" const uint8_t audio_wav_start[];
extern "C" const uint8_t audio_wav_end[];

bool AudioTaskState::playing = false;
uint32_t AudioTaskState::playbackPosition = 0;

static uint32_t readLE32(const uint8_t *p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint16_t readLE16(const uint8_t *p) {
    return p[0] | (p[1] << 8);
}

struct WAVHeader {
    uint16_t format; // 1 for PCM, other values for garbage
    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bits;
};

WAVHeader garbageHeader = {0, 0, 0, 0};

// Find the WAV header, and if we have data, return a pointer to the data/length
static WAVHeader parseWav(const uint8_t *&data, uint32_t &dataLength) {
    const uint8_t *wav = audio_wav_start;
    const size_t size = audio_wav_end - audio_wav_start;

    if (size < 12 || memcmp(wav, "RIFF", 4) != 0 || memcmp(wav + 8, "WAVE", 4) != 0) {
        Serial.println("Embedded audio is not a WAV file");
        return garbageHeader;
    }

    bool formatOk = false;
    WAVHeader header = {0, 0, 0, 0};
    size_t offset = 12;
    while (offset + 8 <= size) {
        const uint8_t *chunk = wav + offset;
        uint32_t chunkSize = readLE32(chunk + 4);
        const uint8_t *body = chunk + 8;

        if (memcmp(chunk, "fmt ", 4) == 0 && chunkSize >= 16) {
            header.format = readLE16(body);
            header.channels = readLE16(body + 2);
            header.sampleRate = readLE32(body + 4);
            header.bits = readLE16(body + 14);

            formatOk = header.format == 1;
            if (!formatOk) {
                Serial.printf("Unexpected WAV format: fmt=%u ch=%u rate=%lu bits=%u\n", header.format, header.channels,
                              (unsigned long) header.sampleRate, header.bits);
                return garbageHeader;
            }
        } else if (memcmp(chunk, "data", 4) == 0) {
            if (!formatOk) {
                Serial.println("WAV data chunk found before fmt chunk?");
                return garbageHeader;
            }

            // Avoid read overflow
            if (offset + 8 + chunkSize > size) {
                chunkSize = size - offset - 8;
            }
            data = body;
            dataLength = chunkSize;
            return header;
        }

        // Chunks are padded to an even size
        offset += 8 + chunkSize + (chunkSize & 1);
    }

    Serial.println("WAV has no data chunk");
    return garbageHeader;
}

void AudioTask(void *pvParameters) {
    const uint8_t *audioData = nullptr;
    uint32_t audioLength = 0;

    WAVHeader wavHeader = parseWav(audioData, audioLength);
    if (wavHeader.format == 0) {
        // Oops, no audio...
        vTaskDelete(NULL);
        return;
    }

    I2S i2s(OUTPUT);
    i2s.setBCLK(SPEAKER_SCL);
    // LRCLK is automatically set to SPEAKER_SCL + 1, which is SPEAKER_LRC in this case
    i2s.setDATA(SPEAKER_SDA);
    i2s.setBitsPerSample(wavHeader.bits);
    i2s.begin(wavHeader.sampleRate);

    int16_t buffer[AUDIO_BUFFER_SIZE / sizeof(int16_t)];

    while (1) {
        if (AudioTaskState::playing) {
            uint32_t position = AudioTaskState::playbackPosition;
            if (position < audioLength) {
                size_t bytesToWrite = min((uint32_t) AUDIO_BUFFER_SIZE, audioLength - position);
                size_t sampleCount = bytesToWrite / sizeof(int16_t);
                const int16_t* samples = reinterpret_cast<const int16_t*>(audioData + position);
                for (size_t i = 0; i < sampleCount; i++) {
                    buffer[i] = (int32_t) samples[i] * AUDIO_VOLUME_PERCENT / 100;
                }
                i2s.write(reinterpret_cast<const uint8_t *>(buffer), sampleCount * sizeof(int16_t));
                AudioTaskState::playbackPosition += bytesToWrite;
            } else {
                AudioTaskState::stopPlayback();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
