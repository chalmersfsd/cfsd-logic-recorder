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

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

#include "logic-recorder.hpp"
#include <iostream>

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid") || 0 == commandlineArguments.count("freq")
        || 0 == commandlineArguments.count("scriptPath")
        || 0 == commandlineArguments.count("recordingsPath")) {
        std::cerr << argv[0] << "Module running recorder for Lynx based on AMI and knob readings" << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OpenDaVINCI session> --freq=<Microservice frequency> "
        << "--scriptPath=<Path to recording-up/down scripts> --recordingsPath=<Where to save recordings>" << std::endl;
        std::cerr << "Example: " << argv[0] << "--cid=111 --freq=2 --scriptPath=/home/cfsd/script/ --recordingsPath=/home/cfsd/recordings/" << std::endl;
        retCode = 1;
    } else {

        // Interface to a running OpenDaVINCI session.  
        cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};
        bool VERBOSE{static_cast<bool>(commandlineArguments.count("verbose"))};
        float FREQ{static_cast<float>(std::stof(commandlineArguments["freq"]))};
        std::string SCRIPTPATH{static_cast<std::string>(commandlineArguments["scriptPath"])};
        std::string RECORDINGSPATH{static_cast<std::string>(commandlineArguments["recordingsPath"])};

        Recorder recorder(od4, SCRIPTPATH, RECORDINGSPATH);

        auto onSwitchStateReading{[&recorder, &VERBOSE](cluon::data::Envelope &&envelope)
        {
            if (!recorder.m_initialized){
                return;
            }

            uint16_t senderStamp = envelope.senderStamp();
            if (senderStamp == 1401) {
                auto state = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
                recorder.m_asState = static_cast<asState>(state.state());
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] AS state reading: " << state.state() << std::endl;
                }

            } else if (senderStamp == 1406){
                auto mission = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
                recorder.m_asMission = static_cast<asMission>(mission.state());
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Mission reading: " << mission.state() << std::endl;
                }

            } else if (senderStamp == 1416){
                auto knobRight = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
                recorder.m_knobRight = knobRight.state();
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Right knob reading: " << knobRight.state() << std::endl;
                }

            }else if (senderStamp == 1417){
                auto knobLeft = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
                recorder.m_knobLeft = knobLeft.state();
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Left knob reading: " << knobLeft.state() << std::endl;
                }
            }
        }};
        od4.dataTrigger(opendlv::proxy::SwitchStateReading::ID(), onSwitchStateReading);

        auto onPulseWidthModulationRequest{[&recorder, &VERBOSE](cluon::data::Envelope &&envelope)
        {
            if (!recorder.m_initialized){
                return;
            }

            uint16_t senderStamp = envelope.senderStamp();
            if (senderStamp == 1300) {
                auto pwmSignal = cluon::extractMessage<opendlv::proxy::PulseWidthModulationRequest>(std::move(envelope));
                recorder.m_blueDuty = static_cast<uint32_t>(pwmSignal.dutyCycleNs());
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Blue pwm reading: " << pwmSignal.dutyCycleNs() << std::endl;
                }

            } else if (senderStamp == 1320){
                auto pwmSignal = cluon::extractMessage<opendlv::proxy::PulseWidthModulationRequest>(std::move(envelope));
                recorder.m_redDuty = static_cast<uint32_t>(pwmSignal.dutyCycleNs());
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Red pwm reading: " << pwmSignal.dutyCycleNs() << std::endl;
                }

            } else if (senderStamp == 1321){
                auto pwmSignal = cluon::extractMessage<opendlv::proxy::PulseWidthModulationRequest>(std::move(envelope));
                recorder.m_blueDuty = static_cast<uint32_t>(pwmSignal.dutyCycleNs());
                if (VERBOSE) {
                    std::cout << "[LOGIC-RECORDER] Green pwm reading: " << pwmSignal.dutyCycleNs() << std::endl;
                }
            }
        }};
        od4.dataTrigger(opendlv::proxy::PulseWidthModulationRequest::ID(), onPulseWidthModulationRequest);

        auto atFrequency{[&od4, &recorder, &VERBOSE]() -> bool
        {
            recorder.run();
            return true;
        }};

        od4.timeTrigger(FREQ, atFrequency);


    }
    return retCode;
}

