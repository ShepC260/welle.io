/*
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *    This file is part of the welle.io.
 *    Many of the ideas as implemented in welle.io are derived from
 *    other work, made available through the GNU general Public License.
 *    All copyrights of the original authors are recognized.
 *
 *    welle.io is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    welle.io is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with welle.io; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __RTL_TCP_CLIENT
#define __RTL_TCP_CLIENT

#include <QByteArray>
#include <QHostAddress>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include <QtNetwork>
#include <array>

#include "CVirtualInput.h"
#include "DabConstants.h"
#include "MathHelper.h"
#include "ringbuffer.h"
#include "CRadioController.h"

typedef struct { /* structure size must be multiple of 2 bytes */
    char magic[4];
    uint32_t tuner_type;
    uint32_t tuner_gain_count;
} dongle_info_t;

class CRTL_TCP_Client : public CVirtualInput {
    Q_OBJECT
public:
    CRTL_TCP_Client(CRadioController &RadioController);
    ~CRTL_TCP_Client(void);

    // Interface methods
    void setFrequency(int32_t);
    bool restart(void);
    void stop(void);
    int32_t getSamples(DSPCOMPLEX* V, int32_t size);
    int32_t getSpectrumSamples(DSPCOMPLEX* V, int32_t size);
    int32_t getSamplesToRead(void);
    void reset(void);
    float setGain(int32_t gain);
    int32_t getGainCount(void);
    void setAgc(bool AGC);
    void setHwAgc(bool hwAGC);
    bool isHwAgcSupported();
    QString getName(void);
    CDeviceID getID(void);

    // Specific methods
    void setIP(QString IPAddress);
    void setPort(uint16_t Port);

    CRadioController *RadioController;

private slots:
    void readData(void);
    void disconnected(void);
    void TCPConnectionWatchDogTimeout(void);
    void AGCTimerTimeout(void);

private:
    QTcpSocket TCPSocket;
    QTimer TCPConnectionWatchDog;
    QTimer AGCTimer;

    int32_t LastFrequency;
    float CurrentGain;
    uint16_t CurrentGainCount;
    uint8_t MinValue;
    uint8_t MaxValue;
    bool isAGC;
    bool isHwAGC;
    int32_t Frequency;
    RingBuffer<uint8_t>* SampleBuffer;
    RingBuffer<uint8_t>* SpectrumSampleBuffer;
    bool connected;
    bool stopped;
    QHostAddress serverAddress;
    uint16_t serverPort;

    bool FirstData;
    dongle_info_t DongleInfo;

    // Gain values for the different tuners
    const std::array<float, 14> e4k_gains {{-1.0, 1.5, 4.0, 6.5, 9.0, 11.5, 14.0, 16.5, 19.0, 21.5,
            24.0, 29.0, 34.0, 42.0}};

    const std::array<float, 5> fc0012_gains {{ -9.9, -4.0, 7.1, 17.9, 19.2 }};

    const std::array<float, 23>  fc0013_gains {{ -9.9, -7.3, -6.5, -6.3, -6.0, -5.8, -5.4, 5.8, 6.1,
                       6.3, 6.5, 6.7, 6.8, 7.0, 7.1, 17.9, 18.1, 18.2,
                       18.4, 18.6, 18.8, 19.1, 19.7 }};

    const std::array<float, 1>  fc2580_gains {{ 0 /* no gain values */ }};

    const std::array<float, 29>  r82xx_gains {{ 0.0, 0.9, 1.4, 2.7, 3.7, 7.7, 8.7, 12.5, 14.4, 15.7,
                     16.6, 19.7, 20.7, 22.9, 25.4, 28.0, 29.7, 32.8,
                     33.8, 36.4, 37.2, 38.6, 40.2, 42.1, 43.4, 43.9,
                     44.5, 48.0, 49.6 }};

    void sendVFO(int32_t frequency);
    void sendRate(int32_t theRate);
    void setGainMode(int32_t gainMode);
    void sendCommand(uint8_t cmd, int32_t param);
    float getGainValue(uint16_t GainCount);
};

#endif
