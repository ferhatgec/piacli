// MIT License
//
// Copyright (c) 2021 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//
// piacli - simple piano (or sth else) chord engine (supports C2 -> C7 chords)
// ------
// https://github.com/ferhatgec/piacli
//

#ifndef PIACLI_PIACLI_HPP
#define PIACLI_PIACLI_HPP

#ifdef __has_include
    #if __has_include(<alsa/asoundlib.h>)
        #include <alsa/asoundlib.h>
    #else
        #error "libalsa not found."
    #endif
#endif

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

enum channels : const unsigned {
    Mono = 1, Stereo = 2
};

constexpr unsigned start = 3500, add = 1200;
constexpr unsigned sampling_rate = 44100;

class piacli_device {
public:
    unsigned pcm, tmp, dir;
    snd_pcm_t* pcm_m;
    snd_pcm_hw_params_t* params;
    snd_pcm_uframes_t frames;
    std::string pcm_device = "default";
public:
    piacli_device() = default; ~piacli_device() = default;
};

class piacli {
public:
    unsigned channels = channels::Stereo;

    enum chords : const unsigned {
        C7 = sampling_rate,
        B6 = C7 - add, A6 = B6 - add,
        G6 = A6 - add, F6 = G6 - add,
        E6 = F6 - add, D6 = E6 - add,
        C6 = D6 - add, B5 = C6 - add,
        A5 = B5 - add, G5 = A5 - add,
        F5 = G5 - add, E5 = F5 - add,
        D5 = E5 - add, C5 = D5 - add,
        B4 = C5 - add, A4 = B4 - add,
        G4 = A4 - add, F4 = G4 - add,
        E4 = F4 - add, D4 = E4 - add,
        C4 = D4 - add, B3 = C4 - add,
        A3 = B3 - add, G3 = A3 - add,
        F3 = G3 - add, E3 = F3 - add,
        D3 = E3 - add, C3 = D3 - add,
        B2 = C3 - add, A2 = B2 - add,
        G2 = A2 - add, F2 = G2 - add,
        E2 = F2 - add, D2 = E2 - add,
        C2 = D2 - add, Rest = 0
    };

    class piacli_sequence {
    public:
        double seconds = 0.3;
        std::vector<piacli::chords> sequence;
    public:
        piacli_sequence() = default; ~piacli_sequence() = default;
    };

    piacli_device init;
    piacli_sequence sequence;
    unsigned buffer_size, loop_data;
    char* buffer_data;
    int file;
    std::string filename;
public:
    piacli(std::string filename) : filename(filename) {
        this->file = open(filename.c_str(), O_RDONLY);
    }; ~piacli() = default;

    void compile_sequence() noexcept {
        for(auto& arg: this->sequence.sequence) {
            if(arg == this->Rest) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(5));
                continue;
            }

            piacli y(filename);
            y.initialize(arg, this->channels);
        }
    }

    void initialize(unsigned __sampling_rate, unsigned __channels) noexcept {
        this->init.pcm = snd_pcm_open(&this->init.pcm_m,
                                      this->init.pcm_device.c_str(),
                                      SND_PCM_STREAM_PLAYBACK, 0);
        snd_pcm_hw_params_alloca(&this->init.params);
        snd_pcm_hw_params_any(this->init.pcm_m, this->init.params);

        this->init.pcm = snd_pcm_hw_params_set_access(this->init.pcm_m,
                                                      this->init.params,
                                                      SND_PCM_ACCESS_RW_INTERLEAVED);
        this->init.pcm = snd_pcm_hw_params_set_format(this->init.pcm_m,
                                                      this->init.params,
                                                      SND_PCM_FORMAT_S16_LE);
        this->init.pcm = snd_pcm_hw_params_set_channels(this->init.pcm_m,
                                                        this->init.params,
                                                        __channels);
        this->init.pcm = snd_pcm_hw_params_set_rate_near(this->init.pcm_m,
                                                         this->init.params,
                                                         &__sampling_rate,
                                                         0);
        this->init.pcm = snd_pcm_hw_params(this->init.pcm_m, this->init.params);

        snd_pcm_hw_params_get_channels(this->init.params, &this->init.tmp);
        snd_pcm_hw_params_get_rate(this->init.params, &this->init.tmp, 0);
        snd_pcm_hw_params_get_period_size(this->init.params, &this->init.frames, 0);

        this->buffer_size = this->init.frames * __channels * 2;
        this->buffer_data = new char[this->buffer_size];

        snd_pcm_hw_params_get_period_time(this->init.params, &this->init.tmp, nullptr);

        for(this->loop_data = (this->sequence.seconds * 1000000) / this->init.tmp; this->loop_data > 0; this->loop_data--) {
            this->init.pcm = read(this->file, this->buffer_data, this->buffer_size);

            if((this->init.pcm = snd_pcm_writei(this->init.pcm_m,
                                               this->buffer_data,
                                               this->init.frames) == -EPIPE)) {
                snd_pcm_prepare(this->init.pcm_m);
            }
        }

        snd_pcm_drain(this->init.pcm_m);
        snd_pcm_close(this->init.pcm_m);

        delete this->buffer_data;
    }
};

#endif // PIACLI_PIACLI_HPP
