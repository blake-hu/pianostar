#include "pwm_speaker.h"
#include "notes.h"

// Maximum number of notes that can play at the same time
pianostar_note_t notes_playing[PIANOSTAR_MAX_NOTES] = {0};

// Holds a pre-computed sine wave
uint16_t sine_buffer[SINE_BUFFER_SIZE] = {0};

// Sample data configurations
// Note: this is a 32 kB buffer (about 25% of RAM)
uint16_t samples[BUFFER_SIZE] = {0}; // stores PWM duty cycle values

// PWM configuration
const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

/*** Initialization & handling code ***/

void gpio_init(void) {
  // Initialize pins
  // Microphone pin MUST be high drive
  nrf_gpio_pin_dir_set(LED_MIC, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_cfg(LED_MIC, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
               NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);

  // Enable microphone
  nrf_gpio_pin_set(LED_MIC);
}

void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 16 MHz
  // Set a countertop value based on sampling frequency and repetitions
  nrfx_pwm_config_t config = {
      // Connect to PIN 13 on breakout adaptor
      .output_pins = {EDGE_P13, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED,
                      NRFX_PWM_PIN_NOT_USED},
      .irq_priority = 4,
      .base_clock = NRF_PWM_CLK_16MHz,
      .count_mode = NRF_PWM_MODE_UP,
      // anything for now: topvalue
      .top_value = 16000000 / (SAMPLING_FREQUENCY * (REPEATS + 1)),
      .load_mode = NRF_PWM_LOAD_COMMON,
      .step_mode = NRF_PWM_STEP_AUTO};

  nrfx_pwm_init(&PWM_INST, &config, NULL);
}

void pwm_play() {
  nrf_pwm_sequence_t pwm_sequence = {
      .values.p_common = samples,
      .length = BUFFER_SIZE,
      .repeats = REPEATS,
      .end_delay = 0,
  };
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void pwm_stop() { nrfx_pwm_stop(&PWM_INST, true); }

void compute_sine_wave(uint16_t max_value) {
  for (int i = 0; i < SINE_BUFFER_SIZE; i++) {
    // what percent into the sine wave are we?
    float percent = (float)i / (float)SINE_BUFFER_SIZE;

    // get sine value
    float twopi = 2.0 * 3.14159;
    float radians = twopi * percent;
    float sine_value = sin(radians);

    // translate from "-1 to 1" into "0 to 2"
    float abs_sine = sine_value + 1.0;

    // scale from "0 to 2" into "0 to 1"
    float mag_1_sine = abs_sine / 2.0;

    // scale to the max_value desired ("0 to max_value")
    // and truncate to an integer value
    uint16_t value = (uint16_t)(max_value * mag_1_sine);

    // save value in buffer
    sine_buffer[i] = value;
  }
}

static void _add_note_to_buffer(uint16_t frequency, float volume) {

  // determine number of sine values to "step" per played sample
  // units are (sine-values/cycle) * (cycles/second) / (samples/second) =
  // (sine-values/sample)
  float step_size =
      (float)SINE_BUFFER_SIZE * (float)frequency / (float)SAMPLING_FREQUENCY;

  // Fill sample buffer based on frequency
  // Each element in the sample buffer will be an element from the sine_buffer
  // The first should be sine_buffer[0],
  //    then each successive sample will be "step_size" further along the sine
  //    wave
  // Be sure to convert the cumulative step_size into an integer to access an
  // item in sine_buffer If the cumulative steps are greater than
  // SINE_BUFFER_SIZE, wrap back around zero
  //    but don't just set it to zero, as that would be discontinuous
  // TODO

  for (int step = 0; step < BUFFER_SIZE; step++) {
    samples[step] += (uint16_t)(sine_buffer[(uint16_t)(step * step_size) %
                                            SINE_BUFFER_SIZE] *
                                volume);
  }
}

static void _clear_buffer() {
  for (int step = 0; step < BUFFER_SIZE; step++) {
    samples[step] = 0;
  }
}

/*** API for playing multiple notes ***/

void play_updated_notes() {
  printf("Playing notes: ");
  _clear_buffer();

  for (int i = 0; i < PIANOSTAR_MAX_NOTES; i++) {
    pianostar_note_t note = notes_playing[i];
    if (note.frequency == 0) {
      continue;
    }
    _add_note_to_buffer(note.frequency, note.volume);
    printf("%i, ", note.frequency);
  }
  printf("\n");
}

pianostar_note_t *add_note(pianostar_note_t new_note) {
  for (int i = 0; i < PIANOSTAR_MAX_NOTES; i++) {
    if (notes_playing[i].frequency == 0) {
      notes_playing[i].frequency = new_note.frequency;
      notes_playing[i].volume = new_note.volume;
      return &notes_playing[i];
    }
  }
  return NULL;
}

bool delete_note(pianostar_note_t *note) {
  if (note == NULL) {
    return false;
  }
  note->frequency = 0;
  note->volume = 0;
  return true;
}

bool set_note_volume(pianostar_note_t *note, float volume) {
  // volume = 0 will silence the note without deleting it
  if (note == NULL) {
    return false;
  }
  note->volume = volume;
  return true;
}

void clear_notes() {
  for (int i = 0; i < PIANOSTAR_MAX_NOTES; i++) {
    notes_playing[i].frequency = 0;
    notes_playing[i].volume = 0;
  }
}