#pragma once
#include "statey.hpp"
#include "statey_object.hpp"
namespace statey {
struct voice {
  oscillator osc0{};
  oscillator osc1{};
  state_variable_filter svf0{};
  state_variable_filter svf1{};
  enhanced_amplifier amp0{};

  float osc1_detune = 1.0f;
  float osc1_detune_fine = 0.0f;
};
struct audio {
  uint8_t on = 0;
  uint8_t off = 0;
  void step_on(uint8_t, float);
  void step_off();

  audio();
  ~audio();
  RtAudio dac;
  void render_channels(int16_t &, int16_t &);
  static int callback(void *, void *, unsigned int, double, RtAudioStreamStatus,
                      void *);
  std::array<std::unique_ptr<voice>, 8> voices{};
};
} // namespace statey