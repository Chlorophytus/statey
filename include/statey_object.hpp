#pragma once
#include "statey.hpp"
namespace statey {
struct object {
  static float truncate(float a) {
    return std::max(std::min(a, 1.0f), -1.0f);
  }
  virtual void render(float, float, float &, float &) = 0;
};
struct state_variable_filter : object {
  float frequency = 1000.0f;
  float q_factor = 0.707f; // ranges anywhere from 0.5f to 6.0f
  enum class output_mux_t {
    hp, // high pass
    lp, // low pass
    bp, // band pass
    br, // band reject
  };
  output_mux_t mux{output_mux_t::lp};
  float delaysL[2]{0.0f, 0.0f};
  float delaysR[2]{0.0f, 0.0f};

  float highpassL = 0.0f;
  float lowpassL = 0.0f;
  float bandpassL = 0.0f;
  float bandrejectL = 0.0f;
  float highpassR = 0.0f;
  float lowpassR = 0.0f;
  float bandpassR = 0.0f;
  float bandrejectR = 0.0f;

  void render(float, float, float &, float &) override;
};
struct oscillator : object {
  float frequency = 440.0f;
  float frequency_multi = 1.0f;
  float frequency_multi2coarse = 1.0f;
  float frequency_multi2fine = 0.0f;
  float vibrato_amplitude = 0.0f;
  float vibrato_freqattenuate = 0.0f;
  float vibrato_phase = 0.0f;
  float morph = 1.0f;
  float phase = 0.0f;
  float duty = 0.0f;
  void render(float, float, float &, float &) override;
};
struct amplifier : object {
  float constantL = 0.0f;
  float constantR = 0.0f;
  void render(float, float, float &, float &) override;
};
struct enhanced_amplifier : amplifier {
  float attack_rate = 1.0f;
  float release_rate = 1.0f;
  float envelope_amplitude = 0.0f;
  bool hold = false;
  enum class envelope_t {
    off,
    attack,
    hold,
    release
  };
  envelope_t state{envelope_t::off};
  void render(float, float, float &, float &) override;
};
} // namespace statey