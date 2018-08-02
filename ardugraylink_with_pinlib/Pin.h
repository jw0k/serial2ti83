/**
  @file Pin.h
  @author Alec Fenichel
  @brief Fast operations on Arduino I/O pins
 */


#include "Arduino.h"


#pragma once


// ################################# Defines #################################

#define DDR_HIGH (*_DDR |= _offset)  ///< Set the DDR register to HIGH for the pin
#define DDR_TOGGLE (*_DDR ^= _offset)  ///< Set the DDR register to the inverse for the pin
#define DDR_LOW (*_DDR &= _ioffset)  ///< Set the DDR register to LOW for the pin

#define PORT_HIGH (*_PORT |= _offset)  ///< Set the PORT register to HIGH for the pin
#define PORT_TOGGLE (*_PORT ^= _offset)  ///< Set the PORT register to the inverse for the pin
#define PORT_LOW (*_PORT &= _ioffset)  ///< Set the PORT register to LOW for the pin

#define DDR_ON (*_DDR & _offset)  ///< Get the DDR register for the pin (HIGH, LOW) with other pins forced to 0
#define DDR_OFF (*_DDR | _ioffset)  ///< Get the DDR register for the pin (HIGH, LOW) with other pins forced to 1

#define PORT_ON (*_PORT & _offset)  ///< Get the PORT register for the pin (HIGH, LOW) with other pins forced to 0
#define PORT_OFF (*_PORT | _ioffset)  ///< Get the PORT register for the pin (HIGH, LOW) with other pins forced to 1

#define PIN_ON (*_PIN & _offset)  ///< Get the PIN register for the pin (HIGH, LOW) with other pins forced to 0
#define PIN_OFF (*_PIN | _ioffset)  ///< Get the PIN register for the pin (HIGH, LOW) with other pins forced to 1


/**
  Class for fast operations on Arduino I/O pins

  @author Alec Fenichel
 */
class Pin {
  public:
    // ################################# Constructors #################################
    
    /**
      Default constructor
     */
    Pin() {
      _number = 0;
      _offset = digitalPinToBitMask(_number);
      _ioffset = ~_offset;
      _timer = digitalPinToTimer(_number);
      _PIN = portInputRegister(digitalPinToPort(_number));
      _PORT = portOutputRegister(digitalPinToPort(_number));
      _DDR = portModeRegister(digitalPinToPort(_number));
    }

    /**
      Arduino supported board constructor

      @param number pin number written on board
     */
    Pin(uint8_t number) {
      _number = number;
      _offset = digitalPinToBitMask(_number);
      _ioffset = ~_offset;
      _timer = digitalPinToTimer(_number);
      _PIN = portInputRegister(digitalPinToPort(_number));
      _PORT = portOutputRegister(digitalPinToPort(_number));
      _DDR = portModeRegister(digitalPinToPort(_number));
    }

    /**
      Custom board constructor

      getAnalogValue() and setDutyCycle(int value) not supported

      @param number pin number written on board
      @param offset bit mask used to access pin in registers
      @param timer timer for pin
      @param PIN input register for pin
      @param PORT data register for pin
      @param DDR data direction register for pin
     */
    Pin(uint8_t number, uint8_t offset, uint8_t timer, volatile uint8_t* PIN, volatile uint8_t* PORT, volatile uint8_t* DDR) {
      _number = number;
      _offset = offset;
      _ioffset = ~_offset;
      _timer = timer;
      _PIN = PIN;
      _PORT = PORT;
      _DDR = DDR;
    }


    // ################################# Operators #################################

    /**
      Get the value of the pin from the PIN register

      @return true if the value of the pin is HIGH, false otherwise
     */
    operator bool() const {
      return bool(PIN_ON);
    }

    /**
      Set the pin state

      @param state the state of the pin (HIGH, LOW)
     */
    Pin& operator =(uint8_t state) {
      uint8_t oldSREG = SREG;
      cli();
      if (state == LOW) {
        PORT_LOW;
      } else {
        PORT_HIGH;
      }
      SREG = oldSREG;

      return *this;
    }


    // ################################# Getters #################################

    /**
      Get the pin number

      @return pin number
     */
    uint8_t getNumber() {
      return _number;
    }

    /**
      Get the pin offset

      @return pin offset
     */
    uint8_t getOffset() {
      return _offset;
    }

    /**
      Get the inverse pin offset

      @return inverse pin offset
     */
    uint8_t getInverseOffset() {
      return _ioffset;
    }

    /**
      Get the pin timer

      @return pin timer
     */
    uint8_t getTimer() {
      return _timer;
    }

    /**
      Get a pointer to the PIN register

      @return pointer to the PIN register
     */
    volatile uint8_t* getPIN() {
      return _PIN;
    }

    /**
      Get a pointer to the PORT register

      @return pointer to the PORT register
     */
    volatile uint8_t* getPORT() {
      return _PORT;
    }

    /**
      Get a pointer to the DDR register

      @return pointer to the DDR register
     */
    volatile uint8_t* getDDR() {
      return _DDR;
    }

    /**
      Get the mode of the pin from the DDR register

      @return mode of the pin (OUTPUT, INPUT)
     */
    uint8_t getMode() {
      if (DDR_ON) {
        return OUTPUT;
      } else {
        return INPUT;
      }
    }

    /**
      Get the state of the pin from the PORT register

      @return state of the pin (HIGH, LOW)
     */
    uint8_t getState() {
      if (PORT_ON) {
        return HIGH;
      } else {
        return LOW;
      }
    }

    /**
      Get the value of the pin from the PIN register

      @return value of the pin (HIGH, LOW)
     */
    uint8_t getValue() {
      if (PIN_ON) {
        return HIGH;
      } else {
        return LOW;
      }
    }

    /**
      Get the analog value of the pin

      @return analog value of the pin (0-1023)
     */
    uint16_t getAnalogValue() {
      return analogRead(_number);
    }


    // ################################# Setters #################################

    // #################### Generic ####################

    /**
      Set the pin mode and pin state

      @param mode the mode of the pin (OUTPUT, INPUT)
      @param state the state of the pin (HIGH, LOW)
     */
    void set(uint8_t mode, uint8_t state) {
      uint8_t oldSREG = SREG;
      cli();
      if (mode == INPUT) {
        DDR_LOW;
      } else {
        DDR_HIGH;
      }
      if (state == LOW) {
        PORT_LOW;
      } else {
        PORT_HIGH;
      }
      SREG = oldSREG;
    }

    /**
      Set the pin mode

      @param mode the mode of the pin (OUTPUT, INPUT)
     */
    void setMode(uint8_t mode) {
      uint8_t oldSREG = SREG;
      cli();
      if (mode == INPUT) {
        DDR_LOW;
      } else {
        DDR_HIGH;
      }
      SREG = oldSREG;
    }

    /**
      Set the pin state

      @param state the state of the pin (HIGH, LOW)
     */
    void setState(uint8_t state) {
      uint8_t oldSREG = SREG;
      cli();
      if (state == LOW) {
        PORT_LOW;
      } else {
        PORT_HIGH;
      }
      SREG = oldSREG;
    }

    // #################### Input ####################

    /**
      Set the pin mode to input
     */
    void setInput() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_LOW;
      SREG = oldSREG;
    }

    /**
      Set the pin pullup resistor to on
     */
    void setPullupOn() {
      uint8_t oldSREG = SREG;
      cli();
      PORT_HIGH;
      SREG = oldSREG;
    }

    /**
      Set the pin pullup resistor to off
     */
    void setPullupOff() {
      uint8_t oldSREG = SREG;
      cli();
      PORT_LOW;
      SREG = oldSREG;
    }

    /**
      Set the pin mode to input and the pin pullup resistor to on
     */
    void setInputPullupOn() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_LOW;
      PORT_HIGH;
      SREG = oldSREG;
    }

    /**
      Set the pin mode to input and the pin pullup resistor to off
     */
    void setInputPullupOff() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_LOW;
      PORT_LOW;
      SREG = oldSREG;
    }

    // #################### Output ####################

    /**
      Set the pin mode to output
     */
    void setOutput() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_HIGH;
      SREG = oldSREG;
    }

    /**
      Set the pin output to HIGH
     */
    void setHigh() {
      uint8_t oldSREG = SREG;
      cli();
      PORT_HIGH;
      SREG = oldSREG;
    }

    /**
      Set the pin output to LOW
     */
    void setLow() {
      uint8_t oldSREG = SREG;
      cli();
      PORT_LOW;
      SREG = oldSREG;
    }

    /**
      Set the pin mode to output and the pin output to HIGH
     */
    void setOutputHigh() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_HIGH;
      PORT_HIGH;
      SREG = oldSREG;
    }

    /**
      Set the pin mode to output and the pin output to LOW
     */
    void setOutputLow() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_HIGH;
      PORT_LOW;
      SREG = oldSREG;
    }

    /**
      Set the PWM duty cycle

      @param value the duty cycle (0-255)
     */
    void setDutyCycle(int value) {
      analogWrite(_number,value);
    }


    // ################################# Utilities #################################

    // #################### Toggle ####################

    /**
      Toggle the pin mode (OUTPUT -> INPUT, INPUT -> OUTPUT)
     */
    void toggleMode() {
      uint8_t oldSREG = SREG;
      cli();
      DDR_TOGGLE;
      SREG = oldSREG;
    }

    /**
      Toggle the pin state (HIGH -> LOW, LOW -> HIGH)
     */
    void toggleState() {
      uint8_t oldSREG = SREG;
      cli();
      PORT_TOGGLE;
      SREG = oldSREG;
    }


    // #################### RC Timer ####################

    /**
      Set the pin mode to input and decrement a counter until the pin goes HIGH or the counter reaches 0 then set the pin mode to output and return the counter value

      @param count the initial value for the counter to start at (0-65535)

      @return the value remaining on the counter when the pin state went to HIGH or 0 if the counter reached 0
     */
    volatile unsigned int rcTimer(volatile unsigned int count) {
      // ######## C ########
      /*
      pin.setInput();
      for (; count > 0; count--) {
        if ((pin.getValue() == HIGH) || (count == 0)) {
          pin.setOutput();
          return count;
        }
      }
      */

      // ######## Assembly ########
      uint8_t status;
      asm volatile (
        // Save interupt status and disable interupts
        "in %[status], __SREG__ \n\t"  // Store current interupt status in variable 's'
        "cli \n\t"  // Disable interupts

        // Set Pin to input mode to start charging capacitor
        "ld __tmp_reg__, %a[_DDR] \n\t"  // Load the DDR register into r0 (__tmp_reg__)
        "and __tmp_reg__, %[_ioffset] \n\t"  // Apply the bit mask (offset) to r0 (__tmp_reg__)
        "st %a[_DDR], __tmp_reg__ \n\t"  // Store r0 (__tmp_reg__) in the DDR register

        // Count time before Pin becomes high
        "loop%=: \n\t"  // Label for looping
          "ld __tmp_reg__,%a[_PIN] \n\t"  // Load the PIN register into r0 (__tmp_reg__)
          "and __tmp_reg__, %[_offset] \n\t"  // Apply the bit mask (offset) to r0 (__tmp_reg__)
          "brne end%= \n\t"  // End the loop if r0 (__tmp_reg__) is not equal to zero by branching to label 'end'
          "dec %[count] \n\t"  // Decrement the value of 'count' by one
          "brne loop%= \n\t"  // If the value of 'count' is not equal to zero continue the loop by branching to label 'loop'

        // Done counting
        "end%=: \n\t"  // Label for ending loop
          // Set Pin to output mode to start discharging capacitor
          "ld __tmp_reg__, %a[_DDR] \n\t"  // Load the DDR register into r0 (__tmp_reg__)
          "or __tmp_reg__, %[_offset] \n\t"  // Apply the bit mask (offset) to r0 (__tmp_reg__)
          "st %a[_DDR], __tmp_reg__ \n\t"  // Store r0 (__tmp_reg__) in the PORT register

          // Restore interupt status
          "out __SREG__, %[status] \n\t"  // Load interupt status from variable 's'

        // Outputs
        :
          [count] "+r" (count),  // The value the counter was at when the pin went high
          [status] "=&r" (status)  // The interupt status

        // Inputs
        :
          [_DDR] "e" (_DDR),  // The address of the DDR register for the pin
          [_PIN] "e" (_PIN),  // The address of the PIN register for the pin
          [_offset] "r" (_offset),  // The bit mask used to access pin in registers
          [_ioffset] "r" (_ioffset)  // The inverse bit mask used to access pin in registers
      );

      return count;
    }

  private:
    // Variables
    uint8_t _number;
    uint8_t _offset;
    uint8_t _ioffset;
    uint8_t _timer;
    volatile uint8_t* _PIN;
    volatile uint8_t* _PORT;
    volatile uint8_t* _DDR;
};
