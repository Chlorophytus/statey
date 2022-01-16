#include "statey.hpp"

using namespace statey;

constexpr static auto VEC_PI =
    F32x4{std::numbers::pi_v<F32>, std::numbers::pi_v<F32>,
          std::numbers::pi_v<F32>, std::numbers::pi_v<F32>};
constexpr static auto VEC_ONE_P = F32x4{1.0f, 1.0f, 1.0f, 1.0f};
constexpr static auto VEC_ONE_N = -VEC_ONE_P;
constexpr static auto VEC_TWO = F32x4{2.0f, 2.0f, 2.0f, 2.0f};
constexpr static auto VEC_TWOPI = VEC_TWO * VEC_PI;
// const static auto VEC_HUGEVAL = _mm_set1_ps(999999.0f);

F32x4 F32x4_clampones(F32x4 what) {
  return _mm_max_ps(_mm_min_ps(what, VEC_ONE_P), VEC_ONE_N);
}

block::block(const role _type) : type{_type} {}

void block::render(F32x4 &L, F32x4 &R, std::vector<F32x4> &&params,
                   F32x4 rate) {
  switch (type) {
  case role::lpf:
  case role::hpf: {
    // delays
    constexpr auto delayL0 = 0;
    constexpr auto delayL1 = 1;
    constexpr auto delayR0 = 2;
    constexpr auto delayR1 = 3;
    // low pass
    constexpr auto loL = 4;
    constexpr auto loR = 5;
    // high pass
    constexpr auto hiL = 6;
    constexpr auto hiR = 7;
    // band pass
    constexpr auto bpL = 8;
    constexpr auto bpR = 9;
    // band reject
    constexpr auto brL = 10;
    constexpr auto brR = 11;

    // PRECALCULATED by the Python 3 scripts because State Variable Filters use
    // trig that is not supported in x86 SSE SIMD nor AVX, shame...
    const auto PC_f_coeff = params.at(0);
    const auto PC_q_coeff = params.at(1);

    // =========================================================================
    // left channel
    // =========================================================================
    _storage.at(hiL) = F32x4_clampones(
        (((_storage.at(loL) + (_storage.at(delayL0) * PC_q_coeff)) + L)));
    _storage.at(bpL) = F32x4_clampones(
        ((_storage.at(hiL) * PC_f_coeff) + _storage.at(delayL0)));
    _storage.at(loL) = F32x4_clampones(
        ((_storage.at(delayL0) * PC_f_coeff) + _storage.at(delayL1)));
    _storage.at(brL) = F32x4_clampones((_storage.at(hiL) + _storage.at(loL)));
    _storage.at(delayL0) = _storage.at(bpL);
    _storage.at(delayL1) = _storage.at(loL);
    // =========================================================================
    // right channel
    // =========================================================================
    _storage.at(hiL) = F32x4_clampones(
        (((_storage.at(loL) + (_storage.at(delayL0) * PC_q_coeff)) + L)));
    _storage.at(bpL) = F32x4_clampones(
        ((_storage.at(hiL) * PC_f_coeff) + _storage.at(delayL0)));
    _storage.at(loL) = F32x4_clampones(
        ((_storage.at(delayL0) * PC_f_coeff) + _storage.at(delayL1)));
    _storage.at(brL) = F32x4_clampones((_storage.at(hiL) + _storage.at(loL)));
    _storage.at(delayL0) = _storage.at(bpL);
    _storage.at(delayL1) = _storage.at(loL);

    switch (type) {
    case role::lpf: {
      L = _storage.at(loL);
      R = _storage.at(loR);
      break;
    }
    case role::hpf: {
      L = _storage.at(hiL);
      R = _storage.at(hiR);
      break;
    }
    default: {
      break;
    }
    }

    break;
  }
  case role::echo: {

    break;
  }
  case role::gen: {
    constexpr auto phase = 0;
    const auto morph = params.at(1);
    _storage.at(phase) = _storage.at(phase) + params.at(0);
    // const auto subtrahend =
    //     _mm_floor_ps(_mm_div_ps(_storage.at(phase), VEC_TWOPI));
    // _storage.at(phase) = _mm_div_ps(
    //     _mm_sub_ps(_storage.at(phase), _mm_add_ps(subtrahend, VEC_PI)),
    //     rate);
    const auto mix_saw = (_storage.at(phase) * (VEC_ONE_P - morph));
    // const auto clip_coeff = ((params.at(2) * VEC_HUGEVAL) - VEC_HUGEVAL);
    // const auto mix_pwm = (
    //     _mm_clampones_ps(_mm_sub_ps(_mm_mul_ps(_storage.at(phase),
    //     VEC_HUGEVAL), clip_coeff)) * morph);
    break;
  }
  default: {
    break;
  }
  }
}