/* ArduGrayLink, a modern yet simple TI Graph-Link cable
 *  https://github.com/MTres19/serial2ti83
 *  https://github.com/DSchndr/serial2ti83
 *  https://github.com/jw0k/serial2ti83
 * 
 * Copyright (C) 2017 jw0k
 * Copyright (C) 2018 DSchndr
 * Copyright (C) 2018 Matthew Trescott
 * 
 * MIT-licensed.
 */

// This version uses original code from jw0k, but with the Pin library from "https://github.com/fenichelar/Pin" 
//  which accelerates port operations on the uC.
// Direct port manipulation *is of course better* (4 times faster than Pin library) but with the Pin library 
//  you can change pins easily without the need of reading the AVR documentation.

//#include <Pin.h> //Uncomment to use Pin.h from libraries folder
#include "Pin.h"

// ***Config area***
#define TIP 6
#define RING 15

const unsigned long TXTIMEOUT = 50; //ms
const unsigned long RXTIMEOUT = 20; //ms

// ***Config area END***



// Program area, DO NOT THINK ABOUT TOUCHING THIS!

Pin PTip = Pin(TIP);
Pin PRing = Pin(RING);

void sendByte(uint8_t byte)
{
    unsigned long currentTime;

    for (int i = 0; i < 8; ++i)
    {
        bool bit = byte & 0x01;
        byte >>= 1;

        //poll both lines until they are both high, which means we're ready to send a bit
        currentTime = millis();
        while (PTip.getValue() == LOW || PRing.getValue() == LOW)
        {
            if (millis() - currentTime > TXTIMEOUT)
            {
                return;
            }
        }

        Pin *ourLine;
        Pin *oppositeLine;
        if (bit)
        {
            ourLine = &PRing;
            oppositeLine = &PTip;
        }
        else
        {
            ourLine = &PTip;
            oppositeLine = &PRing;
        }

        //send a bit by pulling appropriate line low
        ourLine->setOutput();
        ourLine->setLow();

        //wait for opposite line to become low
        currentTime = millis();
        while (oppositeLine->getValue() == HIGH)
        {
            if (millis() - currentTime > TXTIMEOUT)
            {
                ourLine->setInput();
                ourLine->setPullupOn();
                return;
            }
        }

        //release our line
            ourLine->setInput();
            ourLine->setPullupOn();
        //wait for opposite line to become high
        currentTime = millis();
        while (oppositeLine->getValue() == LOW)
        {
            if (millis() - currentTime > TXTIMEOUT)
            {
                return;
            }
        }
    }
}

bool getByte(uint8_t& byte)
{
    unsigned long currentTime;
    uint8_t result = 0;

    for (int i = 0; i < 8; ++i)
    {
        //poll both lines until one of them becomes low
        currentTime = millis();
        while (PTip.getValue() == HIGH && PRing.getValue() == HIGH)
        {
            if (millis() - currentTime > RXTIMEOUT)
            {
                return false;
            }
        }

        Pin *ourLine;
        Pin *oppositeLine;

        bool bit = (PRing.getValue() == LOW);
        result >>= 1;
        if (bit)
        {
            ourLine = &PTip;
            oppositeLine = &PRing;
            result |= 0x80; //bits are always transmitted LSb first (least significant bit)
        }
        else
        {
            ourLine = &PRing;
            oppositeLine = &PTip;
        }

        //acknowledge a bit by pulling appropriate line low
        ourLine->setOutput();
        ourLine->setLow();

        //wait for opposite line to become high
        currentTime = millis();
        while (oppositeLine->getValue() == LOW)
        {
            if (millis() - currentTime > RXTIMEOUT)
            {
                ourLine->setInput();
                ourLine->setPullupOn();
                return false;
            }
        }

        //release our line
        ourLine->setInput();
        ourLine->setPullupOn();
    }

    byte = result;
    return true;
}

void setup()
{
    //turn off the built-in LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(9600);
    Serial.setTimeout(10); // set serialtimeout lower for arduino w/o serial converter.

    //configure both lines to be in a high-impedance state and enable pull-up resistors
    PTip.setInput();
    PTip.setPullupOn();
    PRing.setInput();
    PRing.setPullupOn();
}

void loop()
{
    //forward incoming data from PC to the calculator
    while (Serial.available() > 0)
    {
        sendByte(Serial.read());
    }

    //forward incoming data from calculator to the PC
    uint8_t byteFromCalc;
    if (getByte(byteFromCalc))
    {
        Serial.write(byteFromCalc);
    }
}

// Program area END