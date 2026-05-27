#include <avr/io.h>
#include <avr/interrupt.h>

/* ===================== PINS ===================== */

#define PPS_LED_PIN LED_BUILTIN  // Arduino Uno built-in LED (D13 / PB5)
#define PPS_OUT_PIN 2            // 1PPS output pin (D2 / PD2)

/* ===================== TIMING ===================== */

// Timer1: 16 MHz / 64 = 250 kHz -> 4 us per tick
// 100 ms = 25,000 ticks -> OCR1A = 24,999 in CTC mode
#define TIMER1_OCR_100MS 24999U

static volatile uint8_t s_slot_100ms = 0;

ISR(TIMER1_COMPA_vect) {
  // Slot 0: start pulse, Slot 1: end pulse, Slots 2-9: keep low
  if (s_slot_100ms == 0) {
    PORTD |= _BV(PORTD2);  // D2 high
    PORTB |= _BV(PORTB5);  // D13 high
  } else if (s_slot_100ms == 1) {
    PORTD &= ~_BV(PORTD2);  // D2 low
    PORTB &= ~_BV(PORTB5);  // D13 low
  }

  s_slot_100ms++;
  if (s_slot_100ms >= 10) {
    s_slot_100ms = 0;
  }
}

static void timer1_init_100ms_tick(void) {
  cli();

  // Optional: stop Arduino's Timer0 ISR (millis/micros/delay) to reduce extra jitter.
  TIMSK0 = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = TIMER1_OCR_100MS;
  TCCR1B |= _BV(WGM12);             // CTC mode
  TCCR1B |= _BV(CS11) | _BV(CS10);  // prescaler = 64
  TIMSK1 |= _BV(OCIE1A);            // enable compare A interrupt

  sei();
}

/* ===================== ARDUINO ===================== */

void setup() {
  pinMode(PPS_LED_PIN, OUTPUT);
  pinMode(PPS_OUT_PIN, OUTPUT);
  digitalWrite(PPS_LED_PIN, LOW);
  digitalWrite(PPS_OUT_PIN, LOW);

  timer1_init_100ms_tick();
}

void loop() {
  // Timing is handled entirely in Timer1 ISR.
}
