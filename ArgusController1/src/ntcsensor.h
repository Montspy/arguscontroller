//---------------------------------------------------------
// Argus Controller (Open Hardware)
// ntcsensor.h
// Copyright 2020-2021 Argotronic UG (haftungsbeschraenkt)
//
// License: CC BY-SA 4.0
// https://creativecommons.org/licenses/by-sa/4.0/
// You are free to Share & Adapt under the following terms:
// Give Credit, ShareAlike
//---------------------------------------------------------
// Default: NTC on Vcc side
//         __       __
// Vcc o--|__|--|--|__|---| Gnd
//      10k NTC |  10k R
//              |
//          Arduino ADC
//---------------------------------------------------------
// Define NTC_TO_GROUND for NTC on GND side
//         __       __
// Vcc o--|__|--|--|__|---| Gnd
//        10k R | 10k NTC
//              |
//          Arduino ADC
//---------------------------------------------------------

#ifndef _NTCSENSOR_H_
#define _NTCSENSOR_H_

#define MAX_NTC 6

class NTCSENSOR {

public:
    NTCSENSOR()
        : _temperature{ 0 }
        , _adcpin{ 0 }
        , _tempCount(0)
    {
#ifdef NTC_USE_EXTERNAL_REF
      analogReference(EXTERNAL);
#endif
    }

    bool addPin(uint8_t pin)
    {
        if (_tempCount < MAX_NTC) {
            _adcpin[_tempCount] = pin;
            _tempCount++;
            return true;
        }
        return false;
    }

    void read()
    {
        for (uint8_t i = 0; i < _tempCount; i++) {
            if (_adcpin[i] != 0) {
                float analogValue   = analogRead(_adcpin[i]);
#ifdef NTC_TO_GROUND
                float resistorValue = analogValue * 10000.0 / (1023.0 - analogValue);
#else
                float resistorValue = 1023.0 / analogValue - 10000.0;
#endif
                double temp         = log(resistorValue);
                double kelvin       = 1 / (0.001215004541947 + (0.00020533494946 * temp) + (0.000003191763165 * temp * temp) + (-0.00000002937520103 * temp * temp * temp));
                _temperature[i]     = (int16_t)((kelvin - 273.15) * 10.0);
            } else {
                break;
            }
        }
    }

    int16_t temperature(uint8_t channel)
    {
        if (channel < _tempCount) {
            return _temperature[channel];
        }
        return 0;
    }

private:
    int16_t _temperature[MAX_NTC];
    uint8_t _adcpin[MAX_NTC];
    uint8_t _tempCount;
};

#endif
