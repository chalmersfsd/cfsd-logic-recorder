/*
 * Copyright (C) 2018  Love Mowitz
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

#include "cluon-complete.hpp"
#include "logic-recorder.hpp"

Recorder::Recorder(cluon::OD4Session &od4, std::string scriptPath, std::string recordingsPath)
  : m_od4{od4}
  , m_recordingState{false}
  , m_initialized{false}
  , m_asState{asState::AS_OFF}
  , m_asMission{asMission::AMI_NONE}
  , m_knobRight{0U}
  , m_knobLeft{0U}
  , m_scriptPath{scriptPath}
  , m_recordingsPath{recordingsPath}
  , m_blueDuty{0U}
  , m_redDuty{0U}
  , m_greenDuty{0U}
{
  setUp();
}

Recorder::~Recorder()
{
  Recorder::tearDown();
}

void Recorder::setUp()
{
  m_initialized = true;
}

void Recorder::tearDown()
{
}

void Recorder::run()
{
  if (!m_recordingState && m_asState != asState::AS_OFF && m_asMission != asMission::AMI_NONE && m_knobRight == 12 && m_knobLeft == 12) {
    startRecording();
  }

  if (m_recordingState && m_asMission == asMission::AMI_NONE) {
    stopRecording();
  }

}

void Recorder::startRecording()
{
  
  m_recordingState = true;
  std::cout << "Starting recording" << std::endl;

  cluon::data::TimeStamp timeStamp = cluon::time::now();

  std::string fileName = getMissionName();
  std::cout << "Filename: \""<< fileName << "\"" << std::endl;
  std::cout << "Write to: \"" << m_scriptPath + ".env" << "\"" << std::endl;

  std::ofstream myfile(m_scriptPath + ".env");
  if (myfile.is_open())
  {
    myfile << "NAME=" + fileName;
    myfile.close();
  }
  else {std::cout << "Unable to open file";}

  std::string systemCommand = "sshpass -p cfsd ssh -o StrictHostKeyChecking=no cfsd@172.17.0.1 ";

  std::string startRecCommand = "";
  startRecCommand += systemCommand;
  startRecCommand += " \"sh ";
  startRecCommand += m_scriptPath;
  startRecCommand += "recorder-up.sh\" &";

  std::cout << "Up command: " << startRecCommand << std::endl;
  
  // Runs a script for starting the recording in specified path
  std::cout << "Starting recording... " << system(startRecCommand.c_str()) << " done." << std::endl;

  cluon::data::TimeStamp sampleTime = cluon::time::now();


  // Flash the ASSI to green when starting recording
  opendlv::proxy::PulseWidthModulationRequest msgPwm;
  m_od4.send(msgPwm.dutyCycleNs(0U), sampleTime, 1300); // Blue 
  m_od4.send(msgPwm.dutyCycleNs(0U), sampleTime, 1320); // Red
  m_od4.send(msgPwm.dutyCycleNs(1000000000U), sampleTime, 1321); // Green

  usleep(200000);

  // Reset to settings from state machine
  m_od4.send(msgPwm.dutyCycleNs(m_blueDuty), sampleTime, 1300);
  m_od4.send(msgPwm.dutyCycleNs(m_redDuty), sampleTime, 1320);
  m_od4.send(msgPwm.dutyCycleNs(m_greenDuty), sampleTime, 1321);
}

void Recorder::stopRecording()
{
  
  m_recordingState = false;
  std::cout << "Stopping recording" << std::endl;

  std::string systemCommand = "sshpass -p cfsd ssh -o StrictHostKeyChecking=no cfsd@172.17.0.1 ";

  std::string stopRecCommand = "";
  stopRecCommand += systemCommand;
  stopRecCommand += "\"sh ";
  stopRecCommand += m_scriptPath;
  stopRecCommand += "recorder-down.sh\" &";

  // Runs a script for stopping the recording in specified path
  std::cout << "Stopping recording... " << system(stopRecCommand.c_str()) << " done." << std::endl;

  cluon::data::TimeStamp sampleTime = cluon::time::now();

  // Flash the ASSI to red when stopping recording
  opendlv::proxy::PulseWidthModulationRequest msgPwm;
  m_od4.send(msgPwm.dutyCycleNs(0U), sampleTime, 1300); // Blue
  m_od4.send(msgPwm.dutyCycleNs(1000000000U), sampleTime, 1320); // Red
  m_od4.send(msgPwm.dutyCycleNs(0U), sampleTime, 1321); // Green

  usleep(200000);

  // Reset to settings from state machine
  m_od4.send(msgPwm.dutyCycleNs(m_blueDuty), sampleTime, 1300);
  m_od4.send(msgPwm.dutyCycleNs(m_redDuty), sampleTime, 1320);
  m_od4.send(msgPwm.dutyCycleNs(m_greenDuty), sampleTime, 1321);
  
}

std::string Recorder::getMissionName()
{
  std::string missionName = "";

  switch(m_asMission) {
    case asMission::AMI_ACCELERATION:
      missionName += "ACC";
      break;
    
    case asMission::AMI_AUTOCROSS:
      missionName += "AX";
      break;
    
    case asMission::AMI_BRAKETEST:
      missionName += "BT";
      break;
    
    case asMission::AMI_INSPECTION:
      missionName += "IN";
      break;

    case asMission::AMI_MANUAL:
      missionName += "MAN";
      break;
    
    case asMission::AMI_SKIDPAD:
      missionName += "SP";
      break;
    
    case asMission::AMI_TEST:
      missionName += "TEST";
      break;

    case asMission::AMI_TRACKDRIVE:
      missionName += "TD";
      break;

    default:
      break;
  }

  return missionName;
}