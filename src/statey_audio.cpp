#include "statey_audio.hpp"

using namespace statey;

audio::audio() {
  RtAudio::StreamParameters params;
  params.deviceId = dac.getDefaultOutputDevice();
  params.firstChannel = 0;
  params.nChannels = 2;
  auto numFrames = (unsigned int){512};

  for (auto &&voice : voices) {
    voice = std::make_unique<statey::voice>();
    voice->svf0.mux = statey::state_variable_filter::output_mux_t::hp;
    voice->svf1.mux = statey::state_variable_filter::output_mux_t::lp;
  }

  dac.openStream(&params, nullptr, RTAUDIO_SINT16, 44100, &numFrames,
                 audio::callback, this);
  dac.startStream();
}

void audio::render_channels(int16_t &left, int16_t &right) {
  float floatL = 0.0f;
  float floatR = 0.0f;
  for (auto &&voice : voices) {
    float floatLosc0;
    float floatRosc0;
    float floatLosc1;
    float floatRosc1;
    float floatLsvfH;
    float floatRsvfH;
    float floatLsvfL;
    float floatRsvfL;
    float floatLamp;
    float floatRamp;
    voice->osc0.render(0.0f, 0.0f, floatLosc0, floatRosc0);
    voice->osc1.render(0.0f, 0.0f, floatLosc1, floatRosc1);
    float floatLoscs = (floatLosc0 + floatLosc1) / 2.0f;
    float floatRoscs = (floatRosc0 + floatRosc1) / 2.0f;
    voice->svf0.render(floatLoscs, floatRoscs, floatLsvfH, floatRsvfH);
    voice->svf1.render(floatLsvfH, floatRsvfH, floatLsvfL, floatRsvfL);
    voice->amp0.render(floatLsvfL, floatRsvfL, floatLamp, floatRamp);
    floatL += floatLamp;
    floatR += floatRamp;
  }
  floatL /= 2.0f;
  floatR /= 2.0f;
  left = static_cast<int16_t>(16384.0f * floatL);
  right = static_cast<int16_t>(16384.0f * floatR);
}

int audio::callback(void *out, void *inp, unsigned int frame_cnt, double time,
                    RtAudioStreamStatus status, void *userdata) {
  auto self = reinterpret_cast<audio *>(userdata);
  auto audio = reinterpret_cast<int16_t *>(out);
  for (auto i = 0; i < frame_cnt * 2; i += 2) {
    auto l = int16_t{0};
    auto r = int16_t{0};
    self->render_channels(l, r);
    audio[i + 0] = l;
    audio[i + 1] = r;
  }
  return 0;
}

void audio::step_on(uint8_t midnote, float velocity) {
  auto note = 440.0f * std::pow(std::pow(2.0f, 1.0f / 12.0f),
                                static_cast<int>(midnote) - 49);
  auto &&vco = voices.at(on);
  vco->osc0.frequency = note;
  vco->osc1.frequency = note * (vco->osc1_detune_fine + vco->osc1_detune);
  vco->amp0.constantL = velocity;
  vco->amp0.constantR = velocity;
  vco->amp0.hold = true;
  on++;
  on %= voices.size();
}
void audio::step_off() {
  voices.at(off)->amp0.hold = false;
  off++;
  off %= voices.size();
}

audio::~audio() {
  dac.stopStream();
  if (dac.isStreamOpen())
    dac.closeStream();
}