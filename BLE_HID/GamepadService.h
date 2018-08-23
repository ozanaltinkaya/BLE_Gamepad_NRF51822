/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
 * Modifications copyright (C) 2017 Ozan ALTINKAYA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"

#include "HIDServiceBase.h"

enum ButtonState
{
    BUTTON_UP,
    BUTTON_DOWN
};

enum GamepadButton
{
    GAMEPAD_BUTTON_A       = 0x1,
    GAMEPAD_BUTTON_B       = 0x2,
    GAMEPAD_BUTTON_C       = 0x4,
    GAMEPAD_BUTTON_D       = 0x8,
    GAMEPAD_BUTTON_E       = 0x10,
    GAMEPAD_BUTTON_F       = 0x20,
    GAMEPAD_BUTTON_G       = 0x40,
    GAMEPAD_BUTTON_H       = 0x80,
    
    GAMEPAD_BUTTON_AXIS_Xp       = 0x01,
    GAMEPAD_BUTTON_AXIS_Xn       = 0x03,
    GAMEPAD_BUTTON_AXIS_Yp       = 0x04,
    GAMEPAD_BUTTON_AXIS_Yn       = 0x0C,
    
};

report_map_t GAMEPAD_REPORT_MAP = {
    USAGE_PAGE(1),      0x01,         // Generic Desktop
    USAGE(1),           0x05,         // Gamepad
    COLLECTION(1),      0x01,         // Application
    USAGE(1),           0x01,         // Pointer
    COLLECTION(1),      0x00,         // Physical
    USAGE(1),           0x30,          // X Axis
    USAGE(1),           0x31,         // Y Axis
    LOGICAL_MINIMUM(1), 0xFF,         // -1
    LOGICAL_MAXIMUM(1), 0x01,         // 1
    REPORT_COUNT(1),    0x02,         // 2 Fields, X-Y
    REPORT_SIZE(1),     0x02,         // 2 bits per Axis
    INPUT(1),           0x02,         // Data, Variable, Absolute
    END_COLLECTION(0),
    
    REPORT_COUNT(1),    0x04,         // 4 Bit Padding
    REPORT_SIZE(1),     0x01,         // 4 Bit Padding
    INPUT(1),           0x03,         // Constant, Variable, Absolute
    
    USAGE_PAGE(1),      0x09,         //   Buttons
    USAGE_MINIMUM(1),   0x01,
    USAGE_MAXIMUM(1),   0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_COUNT(1),    0x08,        // 2 bits (Buttons)
    REPORT_SIZE(1),     0x01,
    INPUT(1),           0x02,         // Data, Variable, Absolute
    
//    REPORT_COUNT(1),    0x02,         // 2 Bit Padding
//    REPORT_SIZE(1),     0x01,         // 2 Bit Padding
//    INPUT(1),           0x03,         // Constant, Variable, Absolute
    END_COLLECTION(0),
};

uint8_t report[] = { 0, 0 };

class GamepadService: public HIDServiceBase
{
public:
    GamepadService(BLE &_ble) :
        HIDServiceBase(_ble,
                       GAMEPAD_REPORT_MAP, sizeof(GAMEPAD_REPORT_MAP),
                       inputReport          = report,
                       outputReport         = NULL,
                       featureReport        = NULL,
                       inputReportLength    = sizeof(inputReport),
                       outputReportLength   = 0,
                       featureReportLength  = 0,
                       reportTickerDelay    = 6),
        buttonsState (0),
        axisState (0),
        failedReports (0)
    {

        startReportTicker();
    }

    int setXAxis(GamepadButton button, ButtonState state)
    {
        if (state == BUTTON_DOWN)
            XaxisState = button;
        else
        XaxisState = 0x00;

        return 0;
    }
    
    int setYAxis(GamepadButton button, ButtonState state)
    {
        if (state == BUTTON_DOWN)
            YaxisState = button;
        else
        YaxisState = 0x00;

        return 0;
    }

    int setButton(GamepadButton button, ButtonState state)
    {
        if (state == BUTTON_UP)
            buttonsState &= ~(button);
        else
            buttonsState |= button;

        return 0;
    }
    

    virtual void sendCallback(void) {
        if (!connected)
            return;

        report[1] = buttonsState;
        report[0] = YaxisState | XaxisState;


        if (send(report))
            failedReports++;
    }

protected:
    uint8_t buttonsState;
    uint8_t axisState;
    uint8_t XaxisState;
    uint8_t YaxisState;
    

public:
    uint32_t failedReports;
};
