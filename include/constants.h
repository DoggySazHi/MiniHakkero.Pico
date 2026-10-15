/*
 * Voltage sense pins (analog)
 */

#define BAT_SENSE 26
#define STAT_SENSE 27
#define USB_SENSE 28

/*
 * Buttons
 */

// Physical buttons
#define BUTTON_1 20
#define BUTTON_2 21
// Not actually used, just... exists?
#define BUTTON_5 10

// Touch-capacitive buttons
#define BUTTON_3 12
#define BUTTON_4 13

/*
 * Debug pins
 */
#define DEBUG_TX 0
#define DEBUG_RX 1

/*
 * Speaker pins
 */
#define SPEAKER_SDA 2
#define SPEAKER_SCL 3
#define SPEAKER_LRC 4
#define SPEAKER_SD 6

/*
 * WS2812B LED pin
 */
#define WS2812B_DIN 8
#define WS2812B_EN 11
// Number of LEDs in the outer ring (ready)
#define WS2812B_LED_OUTER 8
// Number of LEDs in the inner ring (firing)
#define WS2812B_LED_INNER 8

#define WS2812B_LED_TOTAL (WS2812B_LED_OUTER + WS2812B_LED_INNER)
