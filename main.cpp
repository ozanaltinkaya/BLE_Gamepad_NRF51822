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

#include "ble/BLE.h"
#include "KeyboardService.h"
#include "GamepadService.h"
#include "examples_common.h"

/**
 * This program implements a complete HID-over-Gatt Profile:
 *  - HID is provided by KeyboardService
 *  - Battery Service
 *  - Device Information Service
 *
 * Complete strings can be sent over BLE using printf. Please note, however, than a 12char string
 * will take about 500ms to transmit, principally because of the limited notification rate in BLE.
 * KeyboardService uses a circular buffer to store the strings to send, and calls to putc will fail
 * once this buffer is full. This will result in partial strings being sent to the client.
 */

DigitalOut waiting_led(P0_2);
DigitalOut connected_led(P0_19);

DigitalIn axisXp(P0_19);
DigitalIn axisXn(P0_20);
DigitalIn axisYp(P0_18);
DigitalIn axisYn(P0_17);

DigitalIn buttonA(P0_22);
DigitalIn buttonB(P0_24);
DigitalIn buttonC(P0_5);
DigitalIn buttonD(P0_28);
DigitalIn buttonE(P0_29);
DigitalIn buttonF(P0_13);
DigitalIn buttonG(P0_25);
DigitalIn buttonH(P0_23);

//Serial pc(USBTX, USBRX); // tx, rx

BLE ble;
GamepadService *kbdServicePtr;


static const char DEVICE_NAME[] = "ArcadeGamepad";
static const char SHORT_DEVICE_NAME[] = "gpd1";

static void onDisconnect(const Gap::DisconnectionCallbackParams_t *params)
{
//    pc.printf("disconnected\r\n");
    connected_led = 0;

    ble.gap().startAdvertising(); // restart advertising
}

static void onConnect(const Gap::ConnectionCallbackParams_t *params)
{
//    pc.printf("connected\r\n");
    waiting_led = false;
}

static void waiting() {
    if (!kbdServicePtr->isConnected())
        waiting_led = !waiting_led;
    else
        waiting_led = 1;
}


void update(){
    if(buttonA){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_A, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_A, BUTTON_UP);
    }
    
    if(buttonB){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_B, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_B, BUTTON_UP);
    }
    
    if(buttonC){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_C, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_C, BUTTON_UP);
    }
    
    if(buttonD){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_D, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_D, BUTTON_UP);
    }
    
    if(buttonE){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_E, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_E, BUTTON_UP);
    }
    
    if(buttonF){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_F, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_F, BUTTON_UP);
    }
    
    if(buttonG){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_G, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_G, BUTTON_UP);
    }
    
    if(buttonH){
        kbdServicePtr->setButton(GAMEPAD_BUTTON_H, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setButton(GAMEPAD_BUTTON_H, BUTTON_UP);
    }
    
    
    
    
    //  AXIS CHECK
    
    
 
    
    if((axisXp) || (axisXn)){
        if(axisXp)
            kbdServicePtr->setXAxis(GAMEPAD_BUTTON_AXIS_Xp, BUTTON_DOWN);
        if(axisXn)
            kbdServicePtr->setXAxis(GAMEPAD_BUTTON_AXIS_Xn, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setXAxis(GAMEPAD_BUTTON_AXIS_Xp, BUTTON_UP);
    }
    
    if((axisYp)||(axisYn)){
        if(axisYp)
            kbdServicePtr->setYAxis(GAMEPAD_BUTTON_AXIS_Yp, BUTTON_DOWN);
        if(axisYn)
            kbdServicePtr->setYAxis(GAMEPAD_BUTTON_AXIS_Yn, BUTTON_DOWN);
    }
    
    else{
        kbdServicePtr->setYAxis(GAMEPAD_BUTTON_AXIS_Yp, BUTTON_UP);
    }     
}


int main()
{
    Ticker heartbeat;
    Ticker updater;

//    pc.printf("initialising ticker\r\n");

    heartbeat.attach(waiting, 1);
    updater.attach(update, 0.006);

//    pc.printf("initialising ble\r\n");
    ble.init();

    ble.gap().onDisconnection(onDisconnect);
    ble.gap().onConnection(onConnect);

    initializeSecurity(ble);

//    pc.printf("adding hid service\r\n");
    GamepadService kbdService(ble);
    kbdServicePtr = &kbdService;

//    pc.printf("adding device info and battery service\r\n");
    initializeHOGP(ble);

//    pc.printf("setting up gap\r\n");
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GAMEPAD);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME,
                                           (const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                           (const uint8_t *)SHORT_DEVICE_NAME, sizeof(SHORT_DEVICE_NAME));

    ble.gap().setDeviceName((const uint8_t *)DEVICE_NAME);

//    pc.printf("advertising\r\n");
    ble.gap().startAdvertising();

    while (true) {
        ble.waitForEvent();
    }
}