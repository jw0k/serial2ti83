const int TIP = 2; //arduino port where the tip of the stereo jack plug is connected
const int RING = 3; //arduino port where the middle part of the stereo jack plug is connected
const unsigned long TXTIMEOUT = 50; //ms
const unsigned long RXTIMEOUT = 5; //ms

void sendByte(uint8_t byte)
{
    unsigned long currentTime;

    for (int i = 0; i < 8; ++i)
    {
        bool bit = byte & 0x01;
        byte >>= 1;

        //poll both lines until they are both high, which means we're ready to send a bit
        currentTime = millis();
        while (digitalRead(TIP) == LOW || digitalRead(RING) == LOW)
        {
            if (millis() - currentTime > TXTIMEOUT)
            {
                return;
            }
        }

        int ourLine;
        int oppositeLine;
        if (bit)
        {
            ourLine = RING;
            oppositeLine = TIP;
        }
        else
        {
            ourLine = TIP;
            oppositeLine = RING;
        }

        //send a bit by pulling appropriate line low
        pinMode(ourLine, OUTPUT);
        digitalWrite(ourLine, LOW);

        //wait for opposite line to become low
        currentTime = millis();
        while (digitalRead(oppositeLine) == HIGH)
        {
            if (millis() - currentTime > TXTIMEOUT)
            {
                pinMode(ourLine, INPUT_PULLUP);
                return;
            }
        }

        //release our line
        pinMode(ourLine, INPUT_PULLUP);

        //wait for opposite line to become high
        currentTime = millis();
        while (digitalRead(oppositeLine) == LOW)
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
        while (digitalRead(TIP) == HIGH && digitalRead(RING) == HIGH)
        {
            if (millis() - currentTime > RXTIMEOUT)
            {
                return false;
            }
        }

        int ourLine;
        int oppositeLine;
        bool bit = (digitalRead(RING) == LOW);
        result >>= 1;
        if (bit)
        {
            ourLine = TIP;
            oppositeLine = RING;
            result |= 0x80; //bits are always transmitted LSb first (least significant bit)
        }
        else
        {
            ourLine = RING;
            oppositeLine = TIP;
        }

        //acknowledge a bit by pulling appropriate line low
        pinMode(ourLine, OUTPUT);
        digitalWrite(ourLine, LOW);

        //wait for opposite line to become high
        currentTime = millis();
        while (digitalRead(oppositeLine) == LOW)
        {
            if (millis() - currentTime > RXTIMEOUT)
            {
                pinMode(ourLine, INPUT_PULLUP);
                return false;
            }
        }

        //release our line
        pinMode(ourLine, INPUT_PULLUP);
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

    //configure both lines to be in a high-impedance state and enable pull-up resistors
    pinMode(TIP, INPUT_PULLUP);
    pinMode(RING, INPUT_PULLUP);
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
