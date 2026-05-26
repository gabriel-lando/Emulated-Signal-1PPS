#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/gpio.h>
#include <esp_timer.h>

/* ===================== PINS ===================== */

#define PPS_LED_PIN GPIO_NUM_8  // Built-in LED / 1PPS output
#define PPS_OUT_PIN GPIO_NUM_0  // 1PPS output, correct polarity
#define LED_ACTIVE_LEVEL HIGH   // LOW = active-low (e.g. ESP32-C3), HIGH = active-high

/* ===================== TIMING ===================== */

#define PPS_PERIOD_US 1000000ULL  // 1 s  (periodic)
#define PPS_PULSE_US 100000ULL    // 100 ms high

static esp_timer_handle_t s_pps_timer;
static esp_timer_handle_t s_pulse_end_timer;

#define LED_ON (LED_ACTIVE_LEVEL)
#define LED_OFF (!LED_ACTIVE_LEVEL)

static void pulse_end_cb(void* /*arg*/) {
  gpio_set_level(PPS_LED_PIN, LED_OFF);  // LED off
  gpio_set_level(PPS_OUT_PIN, 0);        // signal low
}

static void pps_cb(void* /*arg*/) {
  gpio_set_level(PPS_LED_PIN, LED_ON);  // LED on
  gpio_set_level(PPS_OUT_PIN, 1);       // signal high
  esp_timer_start_once(s_pulse_end_timer, PPS_PULSE_US);
}

static void pps_init(void) {
  // Configure LED and signal GPIOs as outputs
  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = (1ULL << PPS_LED_PIN) | (1ULL << PPS_OUT_PIN);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&io_conf);
  gpio_set_level(PPS_LED_PIN, LED_OFF);  // LED off
  gpio_set_level(PPS_OUT_PIN, 0);        // signal idle low

  // One-shot timer to end the pulse after 100 ms
  esp_timer_create_args_t end_args = {};
  end_args.callback = pulse_end_cb;
  end_args.name = "pps_end";
  esp_timer_create(&end_args, &s_pulse_end_timer);

  // Periodic 1 Hz timer to start each pulse
  esp_timer_create_args_t pps_args = {};
  pps_args.callback = pps_cb;
  pps_args.name = "pps";
  esp_timer_create(&pps_args, &s_pps_timer);
  esp_timer_start_periodic(s_pps_timer, PPS_PERIOD_US);
}

/* ===================== ARDUINO ===================== */

void setup() {
  // Reduce jitter
  esp_wifi_stop();
  esp_bt_controller_disable();

  pps_init();  // 1 PPS: LED on GPIO8, signal on GPIO0 (100 ms high, 900 ms low)
}

void loop() {
  delay(1000);  // Nothing to do
}
