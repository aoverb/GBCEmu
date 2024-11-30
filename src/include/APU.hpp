#pragma once
#include "Timer.hpp"
#include "BusRWInterface.hpp"
#include "Common.hpp"

namespace GBCEmu {

class APU : public BusRWInterface {
public:
    APU();
    ~APU();
    bool init();
    bool initAudioRes();
    bool isEnabled()
    {
        return getBit(NR52Control_, 7);
    }
    void disable();
    void tick(Timer& timer);

    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;

protected:
    SDL_AudioDeviceID dev_;

    void tickDivAPU(Timer& timer);

    // Pulse Channel 1...
    uint8_t NR10Sweep_;
    uint8_t NR11DutyCycleLenTimer_;
    uint8_t NR12VolumeEnvelope_;
    uint8_t NR13PeriodLow_;
    uint8_t NR14PeriodHighControl_;

    uint8_t ch1SampleIndex_;
    uint8_t ch1Volume_;
    uint16_t ch1PeriodCounter_;
    float ch1OutputSample_;

    uint8_t ch1SweepIterCounter_;
    uint8_t ch1SweepIterPace_;

    bool ch1EnvelopeIterDecay_;
    uint8_t ch1EnvelopeIterCounter_;
    uint8_t ch1EnvelopeIterPace_;

    uint8_t ch1LenTimer_;

    inline bool ch1Enabled() const
    {
        return getBit(NR52Control_, 0);
    }

    inline bool ch1DACOn()
    {
        return (NR12VolumeEnvelope_ & 0xF8) != 0;
    }

    void enableCH1();
    void disableCH1();

    bool ch1LEnabled() const
    {
        return getBit(NR51Panning_, 4);
    }

    bool ch1REnabled() const
    {
        return getBit(NR51Panning_, 0);
    }

    uint8_t lVolume() const
    {
        return NR50Mixer_& 0x07;
    }

    uint8_t rVolume() const
    {
        return (NR50Mixer_& 0x70) >> 4;
    }

    inline uint8_t ch1SweepPace() const
    {
        return (NR10Sweep_ & 0x70) >> 4;
    }
    
    inline bool ch1SweepSub() const
    {
        return getBit(NR10Sweep_, 3);
    }

    inline uint8_t ch1SweepStep() const
    {
        return NR10Sweep_ & 0x07;
    }

    inline uint8_t ch1InitLenTimer() const
    {
        return NR11DutyCycleLenTimer_ & 0x3F;
    }

    inline uint8_t ch1WaveType() const
    {
        return (NR11DutyCycleLenTimer_ & 0xC0) >> 6;
    }

   inline uint8_t ch1EnvelopePace() const
    {
        return NR12VolumeEnvelope_ & 0x07;
    }

   inline bool ch1EnvelopeDecay() const
    {
        return !getBit(NR12VolumeEnvelope_, 3);
    }

    inline uint8_t ch1InitVolume() const
    {
        return (NR12VolumeEnvelope_ & 0xF0) >> 4;
    }

    inline uint16_t ch1Period() const
    {
        return static_cast<uint16_t>(NR13PeriodLow_) + ((static_cast<uint16_t>(NR14PeriodHighControl_) & 0x07) << 8);
    }

    void setCH1Period(uint16_t period)
    {
        NR13PeriodLow_ = static_cast<uint8_t>(period & 0xFF);
        NR14PeriodHighControl_ = (NR14PeriodHighControl_ & 0xF8) + static_cast<uint8_t>((period >> 8) & 0x07);
    }

    inline bool ch1LenEnabled() const
    {
        return getBit(NR14PeriodHighControl_, 6);
    }

    void tickCH1();
    void tickCH1Sweep();
    void tickCH1Envelope();
    void tickCH1Length();

    // Pulse Channel 2...
    uint8_t NR21DutyCycleLenTimer_;
    uint8_t NR22VolumeEnvelope_;
    uint8_t NR23PeriodLow_;
    uint8_t NR24PeriodHighControl_;

    uint8_t ch2SampleIndex_;
    uint8_t ch2Volume_;
    uint16_t ch2PeriodCounter_;
    float ch2OutputSample_;

    bool ch2EnvelopeIterDecay_;
    uint8_t ch2EnvelopeIterCounter_;
    uint8_t ch2EnvelopeIterPace_;

    uint8_t ch2LenTimer_;

    inline bool ch2Enabled() const
    {
        return getBit(NR52Control_, 1);
    }

    inline bool ch2DACOn()
    {
        return (NR22VolumeEnvelope_ & 0xF8) != 0;
    }

    void enableCH2();
    void disableCH2();

    bool ch2LEnabled() const
    {
        return getBit(NR51Panning_, 5);
    }

    bool ch2REnabled() const
    {
        return getBit(NR51Panning_, 1);
    }

    inline uint8_t ch2InitLenTimer() const
    {
        return NR21DutyCycleLenTimer_ & 0x3F;
    }

    inline uint8_t ch2WaveType() const
    {
        return (NR21DutyCycleLenTimer_ & 0xC0) >> 6;
    }

   inline uint8_t ch2EnvelopePace() const
    {
        return NR22VolumeEnvelope_ & 0x07;
    }

   inline bool ch2EnvelopeDecay() const
    {
        return !getBit(NR22VolumeEnvelope_, 3);
    }

    inline uint8_t ch2InitVolume() const
    {
        return (NR22VolumeEnvelope_ & 0xF0) >> 4;
    }

    inline uint16_t ch2Period() const
    {
        return static_cast<uint16_t>(NR23PeriodLow_) + ((static_cast<uint16_t>(NR24PeriodHighControl_) & 0x07) << 8);
    }

    inline bool ch2LenEnabled() const
    {
        return getBit(NR24PeriodHighControl_, 6);
    }

    void tickCH2();
    void tickCH2Envelope();
    void tickCH2Length();

    double globalTime_ = 0;

    // Wave Channel...

    uint8_t NR30DAC_;
    uint8_t NR31LenTimer_;
    uint8_t NR32OutputLevel_;
    uint8_t NR33PeriodLow_;
    uint8_t NR34PeriodHighControl_;

    uint8_t waveRam_[16];

    uint8_t ch3SampleIndex_;
    uint16_t ch3PeriodCounter_;
    float ch3OutputSample_;
    uint8_t ch3LenTimer_;


    inline bool ch3Enabled() const
    {
        return getBit(NR52Control_, 2);
    }

    bool ch3LEnabled() const
    {
        return getBit(NR51Panning_, 6);
    }

    bool ch3REnabled() const
    {
        return getBit(NR51Panning_, 2);
    }

    inline bool ch3DACOn()
    {
        return getBit(NR30DAC_, 7);
    }

    void enableCH3();
    void disableCH3();

    inline uint8_t ch3InitLenTimer() const
    {
        return NR31LenTimer_;
    }

    inline uint8_t ch3OutputLevel() const
    {
        return (NR32OutputLevel_ & 0X60) >> 5;
    }

    inline uint16_t ch3Period() const
    {
        return static_cast<uint16_t>(NR33PeriodLow_) + ((static_cast<uint16_t>(NR34PeriodHighControl_) & 0x07) << 8);
    }

    inline bool ch3LenEnabled() const
    {
        return getBit(NR34PeriodHighControl_, 6);
    }

    uint8_t ch3WavePattern(uint8_t index) const;
    void tickCH3();
    void tickCH3Length();

    // Noise channel...

    uint8_t NR41LenTimer_;
    uint8_t NR42VolumeEnvelope_;
    uint8_t NR43FreqRandom_;
    uint8_t NR44Control_;

    uint16_t ch4Lfsr_;
    uint8_t ch4Volume_;
    uint32_t ch4PeriodCounter_;
    float ch4OutputSample_;

    bool ch4EnvelopeIterDecay_;
    uint8_t ch4EnvelopeIterPace_;
    uint8_t ch4EnvelopeIterCounter_;
    uint8_t ch4LenTimer_;


    inline bool ch4Enabled() const
    {
        return getBit(NR52Control_, 3);
    }

    bool ch4LEnabled() const
    {
        return getBit(NR51Panning_, 7);
    }

    bool ch4REnabled() const
    {
        return getBit(NR51Panning_, 3);
    }

    void enableCH4();
    void disableCH4();

    inline uint8_t ch4InitLenTimer() const
    {
        return NR41LenTimer_ & 0x3F;
    }

    inline uint8_t ch4InitVolume() const
    {
        return (NR42VolumeEnvelope_ & 0xF0) >> 4;
    }

    inline uint32_t ch4Period() const
    {
        return static_cast<uint32_t>(ch4ClockDivider()) * (1 << ch4ClockShift()) * 4;
    }

    inline bool ch4LenEnabled() const
    {
        return getBit(NR44Control_, 6);
    }

    inline uint8_t ch4ClockDivider() const
    {
        return (NR43FreqRandom_ & 0x07);
    }

    inline uint8_t ch4LfsrWidth() const
    {
        return (NR43FreqRandom_ & 0x08) >> 3;
    }

    inline uint8_t ch4ClockShift() const
    {
        return (NR43FreqRandom_ & 0xF0) >> 4;
    }

    void updateLfsr(bool shortMode);
    void tickCH4();
    void tickCH4Length();
    void tickCH4Envelope();

    // Master Control...
    uint8_t NR50Mixer_;
    uint8_t NR51Panning_;
    uint8_t NR52Control_;

    uint8_t lastDiv_;
    uint8_t divAPU_;

    uint32_t tick_ = 0;
};
}


/*

I finally failed to manage to find a way to tackle the grainly pulse wave sound...
But I think it might be useful some day.

struct oscpulse
{
    double frequency = 0;
    double dutyCycle = 0;
    double amplitude = 1;
    double pi = 3.14159;
    double harmonics = 15;

    double sample(double t)
    {
        double a = 0;
        double b = 0;
        double p = dutyCycle * 2.0 * pi;
        auto approxsin = [](float t)
        {
            // 将输入归一化到 [0, 2π]
            constexpr float PI = 3.14159265358979323846f;
            constexpr float TWO_PI = 2.0f * PI;

            t = fmod(t, TWO_PI); // 确保 t 在 [0, 2π] 范围内
            if (t < 0) t += TWO_PI;

            // 使用更精确的三次插值近似公式
            float j = t / PI; // 归一化到 [0, 2]
            if (j > 1.0f) j = 2.0f - j; // 对称性简化正弦计算

            // 三次多项式近似公式（经过优化的正弦逼近）
            return j * (1.27323954f - 0.405284735f * j * j);
        };
        for (double n = 1; n < harmonics; n++) {
            double c = n * frequency * 2.0 * pi * t;
            a += -approxsin(c) / n;
            b += -approxsin(c - p * n) / n;
        }

        return (2.0 * amplitude / pi) * (a - b)+ 5;
    }
};

Usage:
    oscpulse pulse1;
    pulse1.amplitude = ch1Volume_;
    pulse1.frequency = 4194304.0 / (4.0 * (2048 - ch1Period()));
    pulse1.dutyCycle = ch1WaveType() == 3 ? 0.750 : (std::pow(2, ch1WaveType() - 1) * 0.25);
    double t = ch1SampleIndex_ / 4194304.0;

    ch1OutputSample_ = (pulse1.sample(t) + 1.0f);

*/