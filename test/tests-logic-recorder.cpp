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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

#include "logic-recorder.hpp"

// #include <string>
// #include <vector>

TEST_CASE("Test recording start with right knob settings") {
    cluon::OD4Session od4{111};

    Recorder recorder(od4, "/home/cfsd/scripts/", "/home/cfsd/recordings/");
    recorder.m_asState = asState::AS_READY;
    recorder.m_asMission = asMission::AMI_ACCELERATION;
    recorder.m_knobRight = 12;
    recorder.m_knobLeft = 12;

    recorder.run();

    REQUIRE(recorder.m_recordingState == true);
}

TEST_CASE("Test recording start with wrong knob settings") {
    cluon::OD4Session od4{111};

    Recorder recorder(od4, "/home/cfsd/scripts/", "/home/cfsd/recordings/");
    recorder.m_asState = asState::AS_READY;
    recorder.m_asMission = asMission::AMI_ACCELERATION;
    recorder.m_knobRight = 11;
    recorder.m_knobLeft = 12;

    recorder.run();

    REQUIRE(recorder.m_recordingState == false);
}

TEST_CASE("Test recording stop") {
    cluon::OD4Session od4{111};

    Recorder recorder(od4, "/home/cfsd/scripts/", "/home/cfsd/recordings/");
    recorder.m_asState = asState::AS_OFF;
    recorder.m_asMission = asMission::AMI_NONE;
    recorder.m_recordingState = true;
    recorder.m_knobRight = 12;
    recorder.m_knobLeft = 12;

    recorder.run();

    REQUIRE(recorder.m_recordingState == false);
}
