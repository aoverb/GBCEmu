#include <cmath>
#include "APU.hpp"
#include <algorithm>  // for std::clamp
#include <type_traits> // for std::is_arithmetic

namespace GBCEmu {

constexpr uint8_t MAX_AUDIO_BUFFER_SIZE = 65535;

// Pulse Wave...
constexpr uint8_t PULSE_WAVE[4][8] = {
    {1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1}
};

template <typename T>
T clamp(T value, T min_value, T max_value)
{
    return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}

template <typename T>
T cubic_lerp(T a, T b, T t)
{
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type (e.g., int, float, double)");

    // 限制 t 在 [0, 1] 范围内
    t = clamp(t, static_cast<T>(0), static_cast<T>(1));

    // B-Spline 插值公式
    T t2 = t * t;
    T t3 = t2 * t;

    // 简化版 B-Spline 插值
    return (2 * t3 - 3 * t2 + 1) * a + (-2 * t3 + 3 * t2) * b;
}

inline float dac(uint8_t sample)
{
    return cubic_lerp(-1.0f, 1.0f, ((float)(15 - sample)) / 15.0f);
}
SDL_AudioSpec spec;
static SDL_mutex* mutex;
static std::deque<float> audioBufferL;
static std::deque<float> audioBufferR;
static void SDLCALL playbackCallback(void* userdata, uint8_t* stream, int len)
{
    SDL_LockMutex(mutex);
    int i;
    int sampleLen = len / (sizeof(float) * 2);
    for(i = 0; i < sampleLen; i++) {
        double timestamp = static_cast<double>(i) / static_cast<double>(spec.freq);
        double sampleIndex = timestamp * 1048576.0;
        uint32_t sample1Index = static_cast<uint32_t>(std::floor(sampleIndex));
        uint32_t sample2Index = static_cast<uint32_t>(std::ceil(sampleIndex));
        if (sample2Index >= audioBufferL.size()) break;
        if (sample2Index >= audioBufferR.size()) break;
        float sample1L = audioBufferL[sample1Index];
        float sample2L = audioBufferL[sample2Index];
        float sample1R = audioBufferR[sample1Index];
        float sample2R = audioBufferR[sample2Index];
        
        ((float*)stream)[i * 2] = cubic_lerp(sample1L, sample2L, static_cast<float>(sampleIndex) - static_cast<float>(sample1Index));
        ((float*)stream)[i * 2 + 1] = cubic_lerp(sample1R, sample2R, static_cast<float>(sampleIndex) - static_cast<float>(sample1Index));
    }
    if (i) {
        double deltaTime = static_cast<double>(i) / static_cast<double>(spec.freq);
        uint32_t numSamples = static_cast<uint32_t>(deltaTime * 1048576.0);
        numSamples = std::min(numSamples, static_cast<uint32_t>(audioBufferL.size()));
        audioBufferL.erase(audioBufferL.begin(), audioBufferL.begin() + numSamples);
        audioBufferR.erase(audioBufferR.begin(), audioBufferR.begin() + numSamples);
    }
    SDL_UnlockMutex(mutex);
    return;
}

APU::APU()
{
    std::cout << "APU Init...\n";
    if (!init()) {
        std::cerr << "APU Init Failed!\n";
    }
}

APU::~APU()
{
}

bool APU::initAudioRes()
{
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_F32;
    spec.samples = 5000;
    spec.channels = 2;
    spec.callback = playbackCallback;

    mutex = SDL_CreateMutex();

    const char* deviceName = SDL_GetAudioDeviceName(3, 0);
    dev_ = SDL_OpenAudioDevice(deviceName, 0, &spec, &spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (dev_ == 0) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
        return false;
    }
    printf("Opened audio device: %s\n", deviceName);
    std::cerr << "Device: " << dev_ << "\n";
    SDL_PauseAudioDevice(dev_, 0);
    return true;
}

void APU::disable()
{
    NR10Sweep_ = 0;
    NR11DutyCycleLenTimer_ = 0;
    NR12VolumeEnvelope_ = 0;
    NR13PeriodLow_ = 0;
    NR14PeriodHighControl_ = 0;

    NR50Mixer_ = 0;
    NR51Panning_ = 0;
    NR52Control_ = 0;
    lastDiv_ = 0;
    divAPU_ = 0;
    tick_ = 0;
    ch1OutputSample_ = 0;
}

void APU::tick(Timer& timer)
{
    tick_++;
    if (!isEnabled()) {
        return;
    }

    tickDivAPU(timer);

    if (tick_ % 4 == 0) {
        if (ch1Enabled()) {
            tickCH1();
        }
        if (ch2Enabled()) {
            tickCH2();
        }
        if (ch3Enabled()) {
            tickCH3();
        }
        if (ch4Enabled()) {
            tickCH4();
        }

        float sampleL = 0;
        float sampleR = 0;
        if (ch1DACOn() && ch1LEnabled()) {
            sampleL += ch1OutputSample_;
        }
        if (ch1DACOn() && ch1REnabled()) {
            sampleR += ch1OutputSample_;
        }
        if (ch2DACOn() && ch2LEnabled()) {
            sampleL += ch2OutputSample_;
        }
        if (ch2DACOn() && ch2REnabled()) {
            sampleR += ch2OutputSample_;
        }
        if (ch3DACOn() && ch3LEnabled()) {
            sampleL += ch3OutputSample_;
        }
        if (ch3DACOn() && ch3REnabled()) {
            sampleR += ch3OutputSample_;
        }
        if (ch4LEnabled()) {
            sampleL += ch4OutputSample_;
        }
        if (ch4REnabled()) {
            sampleR += ch4OutputSample_;
        }

        sampleL /= 4.0f;
        sampleR /= 4.0f;
        sampleL *= (static_cast<float>(lVolume())) / 7.0f;
        sampleR *= (static_cast<float>(rVolume())) / 7.0f;

        // 简单的一阶高通滤波器
        static float prevInputL = 0.0f;
        static float prevOutputL = 0.0f;
        static float prevInputR = 0.0f;
        static float prevOutputR = 0.0f;

        // 调整alpha值以适应Game Boy音频特性
        const float alpha = 0.999f;

        float filteredL = alpha * (prevOutputL + sampleL - prevInputL);
        float filteredR = alpha * (prevOutputR + sampleR - prevInputR);

        prevInputL = sampleL;
        prevOutputL = filteredL;
        prevInputR = sampleR;
        prevOutputR = filteredR;

        SDL_LockMutex(mutex);
        if (audioBufferL.size() > 131072) {
            audioBufferL.pop_front();
        }
        if (audioBufferR.size() > 131072) {
            audioBufferR.pop_front();
        }
        audioBufferL.push_back(filteredL);
        audioBufferR.push_back(filteredR);
        
        SDL_UnlockMutex(mutex);
    }
}

void APU::tickDivAPU(Timer& timer)
{
    uint8_t div = timer.getDiv();
    if (getBit(lastDiv_, 4) && !getBit(div, 4)) {
        
        ++divAPU_;
        if ((divAPU_ % 2) == 0) {
            tickCH1Length();
            tickCH2Length();
            tickCH3Length();
            tickCH4Length();
        }

        if ((divAPU_ % 4) == 0) {
            tickCH1Sweep();
        }

        if ((divAPU_ % 8) == 0) {
            tickCH1Envelope();
            tickCH2Envelope();
            tickCH4Envelope();
        }
    }
    lastDiv_ = div;
}

uint8_t APU::busRead(uint16_t addr)
{
    if (addr >= 0xFF30 && addr <= 0xFF3F) {
        return waveRam_[addr - 0xFF30];
    }
    if (addr == 0xFF10) {
        return NR10Sweep_;
    }
    if (addr == 0xFF11) {
        return NR11DutyCycleLenTimer_ & 0xC0; // lower 6 bits write-only...
    }
    if (addr == 0xFF12) {
        return NR12VolumeEnvelope_;
    }
    // NR13 Write only...
    if (addr == 0xFF14) {
        return NR14PeriodHighControl_ & 0x40; // all bits except 6th bit write-only...
    }

    if (addr == 0xFF16) {
        return NR21DutyCycleLenTimer_ & 0xC0; // lower 6 bits write-only...
    }
    if (addr == 0xFF17) {
        return NR22VolumeEnvelope_;
    }

    if (addr == 0xFF1A) {
        return NR30DAC_;
    }
    // NR31 Write only...

    if (addr == 0xFF1C) {
        return NR32OutputLevel_;
    }   
    if (addr == 0xFF1D) {
        return NR33PeriodLow_;
    }   
    if (addr == 0xFF1E) {
        return NR34PeriodHighControl_ & 0x40;
    }
    // NR41 Write only...
    if (addr == 0xFF21) {
        return NR42VolumeEnvelope_;
    }

    if (addr == 0xFF22) {
        return NR43FreqRandom_;
    }

    if (addr == 0xFF23) {
        return NR44Control_ & 0x40;
    }
    // NR23 Write only...
    if (addr == 0xFF19) {
        return NR24PeriodHighControl_ & 0x40; // all bits except 6th bit write-only...
    }

    if (addr == 0xFF24) {
        return NR50Mixer_;
    }
    if (addr == 0xFF25) {
        return NR51Panning_;
    }
    if (addr == 0xFF26) {
        return NR52Control_;
    }
    return 0xFF;
}

void APU::busWrite(uint16_t addr, uint8_t value)
{
    if (isEnabled()) {
        if (addr == 0xFF10) {
            if ((NR10Sweep_ & 0x70) == 0 && (value & 0x70) != 0) {
                ch1SweepIterCounter_ = 0;
                ch1SweepIterPace_ = (value & 0x70) >> 4;
            }
            NR10Sweep_ = value;
        }
        if (addr == 0xFF12) {
            NR12VolumeEnvelope_ = value;
        }
        if (addr == 0xFF13) {
            NR13PeriodLow_ = value;
        }
        if (addr == 0xFF14) {
            if (getBit(value, 7)) {
                enableCH1();
            }
            
            NR14PeriodHighControl_ = value;
        }
        if (addr == 0xFF17) {
            NR22VolumeEnvelope_ = value;
        }
        if (addr == 0xFF18) {
            NR23PeriodLow_ = value;
        }
        if (addr == 0xFF19) {
            if (getBit(value, 7)) {
                enableCH2();
            }
            
            NR24PeriodHighControl_ = value;
        }

        if (addr == 0xFF1A) {
            NR30DAC_ = value;
        }

        if (addr == 0xFF1C) {
            NR32OutputLevel_ = value;
        }   

        if (addr == 0xFF1D) {
            NR33PeriodLow_ = value;
        }   

        if (addr == 0xFF1E) {
            if (getBit(value, 7)) {
                enableCH3();
                value &= 0x7F;
            }
            NR34PeriodHighControl_ = value;
        }   

        if (addr == 0xFF24) {
            NR50Mixer_ = value;
        }
        if (addr == 0xFF25) {
            NR51Panning_ = value;
        }
        if (addr == 0xFF11) {
            NR11DutyCycleLenTimer_ = value;
        }
        if (addr == 0xFF13) {
            NR13PeriodLow_ = value;
        }
        if (addr == 0xFF16) {
            NR21DutyCycleLenTimer_ = value;
        }
        if (addr == 0xFF21) {
            NR42VolumeEnvelope_ = value;
        }

        if (addr == 0xFF22) {
            NR43FreqRandom_ = value;
        }

        if (addr == 0xFF23) {
            if (getBit(value, 7)) {
                enableCH4();
                value &= 0x7F;
            }
            NR44Control_ = value;
        }
        if (addr == 0xFF25) {
            NR51Panning_ = value;
        }
    } else {
        if (addr == 0xFF11) {
            NR11DutyCycleLenTimer_ = value;
        }
        if (addr == 0xFF16) {
            NR21DutyCycleLenTimer_ = value;
        }
        if (addr == 0xFF20) {
            NR41LenTimer_ = value;
        }
        if (addr == 0xFF1B) {
            NR31LenTimer_ = value;
        }
    }
    if (addr >= 0xFF30 && addr <= 0xFF3F) {
        waveRam_[addr - 0xFF30] = value;
    }
    if (addr == 0xFF26) {
        
        bool prevEnabled = isEnabled();
        NR52Control_ = (value & 0x80) | (NR52Control_ & 0x7F);
        if (prevEnabled && !isEnabled()) {
            disable();
        }
    }
    return;
}

void APU::tickCH1Length()
{
    if (ch1Enabled() && ch1LenEnabled())
    {
        ++ch1LenTimer_;
        if (ch1LenTimer_ >= 64) {
            disableCH1();
        }
    }
}

void APU::tickCH2Length()
{
    if (ch2Enabled() && ch2LenEnabled())
    {
        ++ch2LenTimer_;
        if (ch2LenTimer_ >= 64) {
            disableCH2();
        }
    }
}

uint8_t APU::ch3WavePattern(uint8_t index) const
{
    uint8_t base = index >> 1;
    return index & 0x1 ? (waveRam_[base] & 0x0F) : ((waveRam_[base] >> 4) & 0x0F);
}

void APU::tickCH3()
{
    if (!ch3DACOn()) {
        disableCH3();
    }

    for (uint32_t i = 0; i < 2; ++i) {
        ++ch3PeriodCounter_;
        if (ch3PeriodCounter_ >= 0x800) {
            ch3SampleIndex_ = (ch3SampleIndex_ + 1) % 32;
            ch3PeriodCounter_ = ch3Period();
        }
    }

    uint8_t wave = ch3WavePattern(ch3SampleIndex_);
    uint8_t level = ch3OutputLevel();
    if (level != 0) {
        wave >>= (level - 1);
    } else {
        wave = 0;
    }
    ch3OutputSample_ = dac(wave);
}

void APU::tickCH3Length()
{
    if (ch3Enabled() && ch3DACOn()) {
        ++ch3LenTimer_;
        if (ch3LenTimer_ >= 256) {
            disableCH3();
        }
    }
}

void APU::tickCH4Length()
{
    if (ch4Enabled() && ch4LenEnabled()) {
        ++ch4LenTimer_;
        if (ch4LenTimer_ >= 64) {
            disableCH4();
        }
    }
}

void APU::tickCH1Sweep()
{
    if (ch1Enabled() && ch1SweepPace())
    {
        ++ch1SweepIterCounter_;
        if (ch1SweepIterCounter_ == ch1SweepIterPace_)
        {
            int32_t period = static_cast<int32_t>(ch1Period());
            uint8_t step = ch1SweepStep();
            
            if (ch1SweepSub())
            {
                period -= period / (1 << step);
            } else {
                period += period / (1 << step);
            }

            if (period > 0x07FF || period <= 0) // Overflow, disable Channel...
            {
                disableCH1();
            } else {
                setCH1Period(static_cast<uint16_t>(period));
            }
            ch1SweepIterCounter_ = 0;
            ch1SweepIterPace_ = ch1SweepPace();
        }
    }
}

void APU::tickCH1Envelope()
{
    if (ch1Enabled() && ch1EnvelopePace())
    {
        ++ch1EnvelopeIterCounter_;
        if (ch1EnvelopeIterCounter_ >= ch1EnvelopeIterPace_)
        {
            if (ch1EnvelopeDecay())
            {    
                if (ch1Volume_ > 0) {
                    --ch1Volume_;
                }
            } else {
                if (ch1Volume_ < 15) {
                    ++ch1Volume_;
                }
            }
            ch1EnvelopeIterCounter_ = 0;
        }
    }
}


void APU::tickCH4Envelope()
{
    if (ch4Enabled() && ch4EnvelopeIterPace_)
    {
        ++ch4EnvelopeIterCounter_;
        if (ch4EnvelopeIterCounter_ >= ch4EnvelopeIterPace_)
        {
            if (ch4EnvelopeIterDecay_)
            {
                if (ch4Volume_ > 0) {
                    --ch4Volume_;
                }
            } else {
                if (ch4Volume_ < 15) {
                    ++ch4Volume_;
                }
            }
            ch4EnvelopeIterCounter_ = 0;
        }
    }
}


void APU::tickCH2Envelope()
{
    if (ch2Enabled() && ch2EnvelopePace())
    {
        ++ch2EnvelopeIterCounter_;
        if (ch2EnvelopeIterCounter_ >= ch2EnvelopeIterPace_)
        {
            if (ch2EnvelopeDecay())
            {
                if (ch2Volume_ > 0) {
                    --ch2Volume_;
                }
            } else {
                if (ch2Volume_ < 15) {
                    ++ch2Volume_;
                }
            }
            ch2EnvelopeIterCounter_ = 0;
        }
    }
}

void APU::enableCH1()
{
    setBit(NR52Control_, 0, 1);
    ch1SampleIndex_ = 0;
    ch1PeriodCounter_ = ch1Period();
    ch1Volume_ = ch1InitVolume();
    ch1SweepIterCounter_ = 0;
    ch1SweepIterPace_ = ch1SweepPace();
    ch1EnvelopeIterDecay_ = ch1EnvelopeDecay();
    ch1EnvelopeIterPace_ = ch1EnvelopePace();
    ch1EnvelopeIterCounter_ = 0;
    ch1LenTimer_ = ch1InitLenTimer();
}

void APU::disableCH1()
{
    setBit(NR52Control_, 0, 0);
}


void APU::enableCH2()
{
    setBit(NR52Control_, 1, 1);
    ch2SampleIndex_ = 0;
    ch2Volume_ = ch2InitVolume();
    ch2PeriodCounter_ = 0;
    ch2EnvelopeIterDecay_ = ch2EnvelopeDecay();
    ch2EnvelopeIterPace_ = ch2EnvelopePace();
    ch2EnvelopeIterCounter_ = 0;
    ch2LenTimer_ = ch2InitLenTimer();
}

void APU::disableCH2()
{
    setBit(NR52Control_, 1, 0);
}

void APU::enableCH3()
{
    setBit(NR52Control_, 2, 1);
    ch3PeriodCounter_ = 0;
    ch3SampleIndex_ = 1;
    ch3LenTimer_ = ch3InitLenTimer();
}

void APU::disableCH3()
{
    setBit(NR52Control_, 2, 0);
}

void APU::enableCH4()
{
    setBit(NR52Control_, 3, 1);
    ch4PeriodCounter_ = 0;
    ch4LenTimer_ = ch4InitLenTimer();
    ch4Volume_ = ch4InitVolume();
    ch4EnvelopeIterDecay_ = !getBit(NR42VolumeEnvelope_, 3);
    ch4EnvelopeIterPace_ = NR42VolumeEnvelope_ & 0x07;
    ch4EnvelopeIterCounter_ = 0;
    ch4Lfsr_ = 0;
}

void APU::disableCH4()
{
    setBit(NR52Control_, 3, 0);
}

float state = 0.0f;  // 滤波器的状态，需要在每次调用filter函数时保持一致
float cutoff = 5000.0f;  // 滤波器的截止频率，可以根据需要调整

float filter(float input, float& state, float cutoff)
{
    float alpha = 2.0f * M_PI * cutoff / 48000.0f;  // 假设采样率为44100Hz
    state = alpha * input + (1.0f - alpha) * state;
    return state;
}

void APU:: tickCH1()
{
    if (!ch1DACOn()) {
        disableCH1();
        return;
    }

    ++ch1PeriodCounter_;

    if (ch1PeriodCounter_ >= 0x800)
    {
        // get new sample...
        ch1SampleIndex_ = (ch1SampleIndex_ + 1) % 8;
        ch1PeriodCounter_ = ch1Period();
    }

    uint8_t sample = PULSE_WAVE[ch1WaveType()][ch1SampleIndex_];

    float rawSample = dac(sample * ch1Volume_);
    ch1OutputSample_ = rawSample;
}


void APU::tickCH2()
{
    if (!ch2DACOn()) {
        disableCH2();
        return;
    }

    ++ch2PeriodCounter_;

    if (ch2PeriodCounter_ >= 0x800)
    {
        // get new sample...
        ch2SampleIndex_ = (ch2SampleIndex_ + 1) % 8;
        ch2PeriodCounter_ = ch2Period();
    }

    uint8_t sample = PULSE_WAVE[ch2WaveType()][ch2SampleIndex_];

    float rawSample = dac(sample * ch2Volume_);

    ch2OutputSample_ = rawSample;
}

void APU::updateLfsr(bool shortMode)
{
    uint8_t bit0 = ch4Lfsr_ & 0x01;
    uint8_t bit1 = (ch4Lfsr_ & 0x02) >> 1;
    bool res = (bit0 == bit1);
    ch4Lfsr_ = (ch4Lfsr_ & 0x7FFF) + (res ? 0x8000 : 0);
    if (shortMode) {
        ch4Lfsr_ = (ch4Lfsr_ & 0xFF7F) + (res ? 0x80 : 0);
    }
    ch4Lfsr_ >>= 1;
}

void APU::tickCH4()
{
    ++ch4PeriodCounter_;

    if (ch4PeriodCounter_ >= ch4Period())
    {
        // get new sample...
        updateLfsr(!!ch4LfsrWidth());
        ch4PeriodCounter_ = 0;
    }

    uint8_t wave = (ch4Lfsr_ & 0x01) * ch4Volume_;
    ch4OutputSample_ = dac(wave);
}

bool APU::init(){
    if (SDL_Init(SDL_INIT_AUDIO) < 0){
        return false;
    }
    if(!initAudioRes()){
        return false;
    }
    disable();
    return true;
}

}