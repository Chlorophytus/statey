#include "statey_object.hpp"

using namespace statey;

void state_variable_filter::render(float inL, float inR, float &outL,
                                   float &outR) {
  auto L = inL;
  auto R = inR;
  outL = 0.0f;
  outR = 0.0f;
  auto f_coeff = std::sin((M_PI * frequency) / SAMPLE_RATE) * 2.0f;
  auto q_coeff = 1.0f / q_factor;

  highpassL = truncate(-(lowpassL + (delaysL[0] * q_coeff)) + L);
  bandpassL = truncate((highpassL * f_coeff) + delaysR[0]);
  lowpassL = truncate((delaysL[0] * f_coeff) + delaysL[1]);
  bandrejectL = truncate(highpassL + lowpassL);
  delaysL[0] = bandpassL;
  delaysL[1] = lowpassL;

  highpassR = truncate(-(lowpassR + (delaysR[0] * q_coeff)) + R);
  bandpassR = truncate((highpassR * f_coeff) + delaysR[0]);
  lowpassR = truncate((delaysR[0] * f_coeff) + delaysR[1]);
  bandrejectR = truncate(highpassR + lowpassR);
  delaysR[0] = bandpassR;
  delaysR[1] = lowpassR;

  switch (mux) {
  case output_mux_t::lp: {
    outL = lowpassL;
    outR = lowpassR;
    break;
  }
  case output_mux_t::hp: {
    outL = highpassL;
    outR = highpassR;
    break;
  }
  case output_mux_t::bp: {
    outL = bandpassL;
    outR = bandpassR;
    break;
  }
  case output_mux_t::br: {
    outL = bandrejectL;
    outR = bandrejectR;
    break;
  }
  }
}

void oscillator::render(float inL, float inR, float &outL, float &outR) {
  auto vib = std::cos(vibrato_phase);
  vibrato_phase += vibrato_freqattenuate;
  vibrato_phase = std::fmod(vibrato_phase + M_PI, 2.0f * M_PI) - M_PI;
  auto saw = phase;
  auto pulse = (phase < duty ? 1.0f : -1.0f);
  phase += (frequency_multi * (frequency_multi2coarse + frequency_multi2fine) *
            frequency * M_PI * 2.0f) /
           SAMPLE_RATE;
  phase += vib * vibrato_amplitude * 0.001f;
  phase = std::fmod(phase + M_PI, 2.0f * M_PI) - M_PI;
  auto mixed = ((1.0f - morph) * saw) + (morph * pulse);
  outL = mixed / M_PI;
  outR = mixed / M_PI;
}

void amplifier::render(float inL, float inR, float &outL, float &outR) {
  outL = inL * constantL;
  outR = inR * constantR;
}

void enhanced_amplifier::render(float inL, float inR, float &outL,
                                float &outR) {
  switch (state) {
  case envelope_t::off: {
    if (hold) {
      state = envelope_t::attack;
    }
    break;
  }
  case envelope_t::attack:
  case envelope_t::hold:
  case envelope_t::release: {
    switch (state) {
    case envelope_t::attack: {
      envelope_amplitude += attack_rate;
      if (envelope_amplitude > 1.0f) {
        envelope_amplitude = 1.0f;
        state = envelope_t::hold;
      }
      break;
    }
    case envelope_t::hold: {
      if (!hold) {
        state = envelope_t::release;
      }
      break;
    }
    case envelope_t::release: {
      envelope_amplitude -= release_rate;
      if (envelope_amplitude < 0.0f) {
        envelope_amplitude = 0.0f;
        state = envelope_t::off;
      }
      break;
    }
    default: {
      break;
    }
    }
    break;
  }
  }
  outL = inL * constantL * envelope_amplitude;
  outR = inR * constantR * envelope_amplitude;
}
