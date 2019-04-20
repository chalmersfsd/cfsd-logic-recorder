/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RECORDER_H
#define RECORDER_H

#include "opendlv-standard-message-set.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <fstream>

enum asState {
    AS_OFF,
    AS_READY, 
    AS_DRIVING, 
    AS_FINISHED,
    AS_EMERGENCY,
    AS_MANUAL
 };

 enum asMission {
    AMI_NONE,
    AMI_ACCELERATION, 
    AMI_SKIDPAD, 
    AMI_TRACKDRIVE, 
    AMI_AUTOCROSS,
    AMI_BRAKETEST,
    AMI_INSPECTION,
    AMI_MANUAL,
    AMI_TEST
};

class Recorder {
  public:
    Recorder(cluon::OD4Session &od4, std::string scriptPath, std::string recordingsPath);
    ~Recorder();

  public:
    void startRecording();
    void stopRecording();
    void run();
    std::string getMissionName();
    void setUp();
    void tearDown();

  public:
    cluon::OD4Session &m_od4;
    bool m_recordingState;
    bool m_initialized;
    asState m_asState;
    asMission m_asMission;
    int16_t m_knobRight;
    int16_t m_knobLeft;
    std::string m_scriptPath;
    std::string m_recordingsPath;
    uint32_t m_blueDuty;
    uint32_t m_redDuty;
    uint32_t m_greenDuty;

};
#endif

