#include "pins_arduino.h" 
#include <avr/pgmspace.h>
#include <FastLED.h>

// Define the I2C address of the PCF8575
#define PCF8575_ADDRESS 0x20


#define HAS_LEDS
#ifdef HAS_LEDS
#define DATA_PIN A5
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];
#endif

#include <Keypad_Matrix.h> //Library to support Matrix Keypad
const byte ROWS = 4; //ROWs for Keypad
const byte COLS = 5; //Colums for Keypad

// how the keypad has its keys laid out
const char keys[ROWS][COLS] = {
  {'0', '1', '2', '3', '4'},
  {'5', '6', '7', '8', '9'},
  {':', ';', '<', '=', '>'},
  {'?', '@', 'A', 'B', 'C'}
};
const byte rowPins[ROWS] = { 0, 1, 3, 4}; //connect to the row pinouts of the keypad
const byte colPins[COLS] = {  5,  6,  7, 8, 9}; //connect to the column pinouts of the keypad
  // Create the Keypad
Keypad_Matrix kpd = Keypad_Matrix( makeKeymap (keys), rowPins, colPins, ROWS, COLS );

#define HAS_TM1637_DISPLAY  // comment out if you're not using a TM1637 4 digits 7 segment display
#ifdef  HAS_TM1637_DISPLAY
#include <TM1637Display.h>
#define TM_CLKPIN A3
#define TM_DIOPIN A4
#define TM_BRIGHTNESS 5   // (0..7), sets the brightness of the display. 0=min, 7=max
TM1637Display display = TM1637Display(TM_CLKPIN, TM_DIOPIN);
uint8_t TM_data[] = { 0x00, 0x00, 0x00, 0x00 };
const uint8_t TM_race[] = { 0x50, 0x77, 0x58, 0x79 };
const uint8_t TM_crc[] = {0x39, 0x50, 0x39};
#endif

//#define DISPLAY_CRC8_MATCH_ON_SERIAL             // keep this and the next line uncommented, until you get many more "crc8 match" lines on the serial port, than crc8 mismatch lines. (I am mostly getting somewhere between 10 to 200 "match" lines before a single "mismatch" line)
//#define DISPLAY_CRC8_MISMATCH_ON_SERIAL          // once you know the SPI port is mostly receiving data correctly, you can comment out both lines (or use them for easy debugging)
//#define DISPLAY_CRC8_MISMATCH_ON_ALPHANUMERIC    // use this if you'd like to get a 'CrC' message displayed on the alphanumeric display, each time there's a crc8 mismatch, which means bad SPI communication with the wheelbase. My advice: always keep this commented. If you do have some SPI communication issues, this might help you to know when communication is faulty without looking at the serial monitor.
//#define DISPLAY_ALPHANUMERIC_DATA_WHEN_MISMATCH_ON_SERIAL  // When getting some (likely broken) alphanumeric text on a packet with mismatched crc8, you might want to view the text on the serial monitor. In that case uncomment this line. (keep this commented to tidy things up)

//#define HAS_ANALOG_DPAD
//#define DEBUG_ANALOG_DPAD // print the DPAD value on the alphanumeric display. Only relevant if you have an analog dpad, and only when trying to get the DPAD working. once it's working - uncomment this. (Dpad value is always sent to serial monitor)
#ifdef  HAS_ANALOG_DPAD
#define DPADPIN A0
int prevDpadVal = 0;
#endif

#define HAS_ANALOG_SW1
//#define DEBUG_ANALOG_SW1  // print the DPAD value on the alphanumeric display. Only relevant if you have an analog dpad, and only when trying to get the DPAD working. once it's working - uncomment this. (Dpad value is always sent to serial monitor)
#ifdef  HAS_ANALOG_SW1
#define SW1PIN A0
int prevSW1Val = 0;
#endif


#define HAS_ANALOG_SW2
//#define DEBUG_ANALOG_SW2  // print the DPAD value on the alphanumeric display. Only relevant if you have an analog dpad, and only when trying to get the DPAD working. once it's working - uncomment this. (Dpad value is always sent to serial monitor)
#ifdef  HAS_ANALOG_SW2
#define SW2PIN A1
int prevSW2Val = 0;
#endif


#define HAS_ANALOG_SW3
//#define DEBUG_ANALOG_SW3  // print the DPAD value on the alphanumeric display. Only relevant if you have an analog dpad, and only when trying to get the DPAD working. once it's working - uncomment this. (Dpad value is always sent to serial monitor)
#ifdef  HAS_ANALOG_SW3
#define SW3PIN A2
int prevSW3Val = 0;
#endif

#define dataLength 33
#define CS_ISR 2    // connect SPI Cable Select pin to Arduino's D2 & D10. (See schematics)
#define PRINTBIN(Num) for (uint32_t t = (1UL << (sizeof(Num) * 8) - 1); t; t >>= 1) Serial.write(Num& t ? '1' : '0'); // Prints a binary number with leading zeros (Automatic Handling)

//#define HASBUTTONS  // use digital and analog pins for steering wheel buttons. Note: Pins A7 and A6 do not have an internal pullup resistor. Serial data pins: D0, D1. SPI pins: D2, D10, D11, D12, D13. Display Pins: D3, D4. The rest of digital and analog pins can be used for buttons. (also AFAIK A6 and A7 can only be used for analogRead()  )
#ifdef HASBUTTONS
#define buttonsnum 1                  // how many buttons are you using?
uint8_t buttonsPins[] = { A3, A4};    // what arduino pins are you using? 
uint8_t buttonsBits[] = { 22, 12};    // what bits do you want each button to affect? use the last comment in this file as reference.
#endif

#define KEYPAD  // use digital and analog pins for steering wheel buttons. Note: Pins A7 and A6 do not have an internal pullup resistor. Serial data pins: D0, D1. SPI pins: D2, D10, D11, D12, D13. Display Pins: D3, D4. The rest of digital and analog pins can be used for buttons. (also AFAIK A6 and A7 can only be used for analogRead()  )
#ifdef KEYPAD
#define keypadnum 20                  // how many buttons are you using?
uint8_t keypadVal[] = {   0,  1, 2, 3,  4, 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};    // what arduino pins are you using? 
uint8_t keypadBits[] = { 13, 10, 6, 8, 15,14,  7, 20, 11, 22,  3,  1,  4,  2, 18,  3,  1,  4,  2, 19};    // what bits do you want each button to affect? use the last comment in this file as reference.
#endif

uint8_t mosiBuf[dataLength];  // buffer for the incoming data on the mosi line. 
volatile unsigned long lastPrintMillis = 0;
int selectedButtonByte = 2;   // button bytes are 3rd to 5th. initialize to 1st relevant byte.
uint8_t incByte, prevPrintedByte, prevAlphaDisp[3];
volatile uint8_t isrIndex = 0;
unsigned long delayMillis = 400; // wait at least the delay time since last spi communication before printing out whatever came in.

// CRC lookup table with polynomial of 0x131
PROGMEM const unsigned char _crc8_table[256] = {
  0, 94, 188, 226, 97, 63, 221, 131,
  194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30,
  95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160,
  225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61,
  124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197,
  132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88,
  25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230,
  167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123,
  58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15,
  78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146,
  211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44,
  109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177,
  240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73,
  8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212,
  149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106,
  43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247,
  182, 232, 10, 84, 215, 137, 107, 53
};

// Uni hub packet (change to other steering wheels in setup() )
uint8_t returnData[dataLength] = { 0xA5, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc };

// return CRC8 from buf
uint8_t crc8(const uint8_t* buf, uint8_t length) {
  uint8_t crc = 0xff;
  while (length) {
    crc = pgm_read_byte_near(_crc8_table + (*buf ^ crc));
    buf++;
    length--;
  }
  return crc;
}


void setup()
{
  pinMode(MISO, OUTPUT);    // usually when using SPI, arduino will automatically define MISO as output, but not when doing SPI slave.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(CS_ISR, INPUT);   // connecting CS_ISR pin (D2) to the SS pin (D10) is crucial. See the schematics.
  pinMode(SS, INPUT);     

  kpd.begin (); //init Keypad
  kpd.setKeyDownHandler (keyDown); //set Keypad handler
  kpd.setKeyUpHandler   (keyUp);
  
#ifdef HASBUTTONS
  for (int i = 0; i < buttonsnum; i++) {
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }
#endif

  returnData[1] = {0x03}; // for ClubSport Porshe 918 RSR - This is the only option that I'm implementing right now.
        //    {0x01} for BMW M3 GT2, {0x02} for ClubSport FORMULA, (0x04) for Uni hub
   //Serial.begin(250000);

#ifdef HAS_ANALOG_DPAD
  pinMode(DPADPIN, INPUT);
#endif

#ifdef KEYPAD
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }  
#endif

#ifdef HAS_ANALOG_SW1
  pinMode(SW1PIN, INPUT);
#endif

#ifdef HAS_ANALOG_SW2
  pinMode(SW2PIN, INPUT);
#endif

#ifdef HAS_ANALOG_SW3
  pinMode(SW3PIN, INPUT);
#endif


#ifdef HAS_TM1637_DISPLAY
  display.clear();
  display.setBrightness(TM_BRIGHTNESS);
  display.setSegments(TM_race);
  delay(1000);
  display.clear();
#endif

#ifdef HAS_LEDS
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
#endif
  
  attachInterrupt(digitalPinToInterrupt(CS_ISR), cableselect, RISING);
  // SPCR BYTE should be: 11000100   note to self: by raw_capture.ino and fanatec.cpp spi settings, of btClubSportWheel by Darknao, SPI settings are SPI_Mode0 & MSBFIRST. but with logic scope I see that CPHA 1 (falling!) is used by wheel base, which means SPI_MODE1. (and MSBFIRST)
  // (Mode 1 - clock is normally low (CPOL = 0), and the data is sampled on the transition from high to low (trailing edge) (CPHA = 1))
  SPCR |= _BV(SPE);   // turn on SPI
  SPCR |= _BV(SPIE);    // turn on interrupts
  SPCR |= _BV(CPHA);    //turns on CPHA. as I found the CSW wheelbase to use it, with logic scope.
  Serial.print("SPCR byte (binary):");
  Serial.println(SPCR, BIN);   //prints byte as binary
}

void cableselect() {          // When CS line goes high - the rim should get ready to transmit the next returnData buffer, right from the begining. (so that the first byte (0xA5) will be sent out on the first Clock cycle (and after CS line went Low)
  SPCR &= ~_BV(SPIE);         // turn OFF interrupts
  SPDR = returnData[0];       // load first byte into SPDR single-byte's buffer
  isrIndex = 0;           // on next SPI interrupt(SPI_STC_vect), load the 2nd byte
  SPCR |= _BV(SPIE);          // turn on interrupts
}

// SPI interrupt routine
ISR(SPI_STC_vect)       // Note: this routine takes 6 us, and happens once 8 bits are recieved through SPI.
{ 
  byte c = SPDR;
  mosiBuf[isrIndex] = c;
  isrIndex++;
  if (isrIndex >= dataLength) {
    isrIndex = 0;
  }
  SPDR = returnData[isrIndex];
}

void loop() {
  readSerial();
  readButtons();
  calcOutgoingCrc();
 //  printButtonByteToSerial();
  bool crc8Stat = checkIncomingCrc();
  refreshAlphanumericDisplays(crc8Stat);

  leds[0] = CRGB::YellowGreen; FastLED.show(); 
  leds[1] = CRGB::Green; FastLED.show();
  leds[2] = CRGB::Yellow; FastLED.show();
  leds[3] = CRGB::Red; FastLED.show();
  leds[4] = CRGB::Teal; FastLED.show();
  leds[5] = CRGB::Teal; FastLED.show(); 
  leds[6] = CRGB::Yellow; FastLED.show();
  leds[7] = CRGB::Green; FastLED.show();
  leds[8] = CRGB::Red; FastLED.show();
  leds[9] = CRGB::Purple; FastLED.show();
  leds[10] = CRGB::White; FastLED.show(); 
  leds[11] = CRGB::Red; FastLED.show();
}

void keyDown (const char which)
  {
  int i=which - '0';
  Serial.print (F("Key down: "));
  Serial.println (which);
  Serial.print (F("bit up: "));Serial.println (i);
  Serial.println (keypadBits[i]);
  buttonBitChange(keypadBits[i], true);
  }
  
void keyUp (const char which)
  {
  int i=which - '0';
  Serial.print (F("Key up: "));
  Serial.println (which);
  Serial.print (F("bit down: "));Serial.println (i);
  Serial.println (keypadBits[i]);
  buttonBitChange(keypadBits[i], false);
  }
  
void readButtons() {
#ifdef HASBUTTONS
    for (int i = 0; i < buttonsnum; i++) {
      if (!digitalRead(buttonsPins[i])) {       // if this input is low the button is pressed. Reverse logic: all button inputs use the internal input pullup, and pressing the button completes the path to ground)
        buttonBitChange(buttonsBits[i], true);    // raise the correct button bit, as defined in buttonsBits[]
      }
      else {
        buttonBitChange(buttonsBits[i], false);   // drop bit
      }
    }
#endif

#ifdef KEYPAD
  kpd.scan (); //scan for which Key is pressed
#endif

#ifdef HAS_ANALOG_DPAD
/*
  VALUE   MIDPOINT  BUTTON              DpadVal
    1023  933.5   nothing             0
    844   763     right             1 
    682   639.5   left              2
    597   505.5   left+right (calibrate center) 3
    414   299     down              4
    184   164.5   up                5
    145   72.5    up+down (menu)          6
    0   0     Dpad button           7
*/

//In case you've used different resistors to what I used, uncomment the serial print command here to find your analog read values: 
//Serial.println(analogRead(DPADPIN));
  int DpadRead = analogRead(DPADPIN);
  int DpadVal = 0;
  if (DpadRead > 933) DpadVal = 0; else
    if (DpadRead > 763) DpadVal = 1; else
      if (DpadRead > 639) DpadVal = 2; else
        if (DpadRead > 505) DpadVal = 3; else
          if (DpadRead > 299) DpadVal = 4; else
            if (DpadRead > 134) DpadVal = 5; else
              if (DpadRead > 72) DpadVal = 6; else
                DpadVal = 7;

  if (DpadVal != prevDpadVal) {   // only if button has changed, do something
    Serial.print("Dpad button:");
    Serial.println(DpadVal);
#if defined HAS_ANALOG_DPAD && defined HAS_TM1637_DISPLAY && defined DEBUG_ANALOG_DPAD
    display.showNumberDec(DpadVal, false,1,3);    // print the dpad value on the lcd screen
#endif  
    dPadBitChange(prevDpadVal, false);  // turn the previous Dpad button bit off.
    dPadBitChange(DpadVal, true);   // turn the current Dpad button bit on.   
  }
  prevDpadVal = DpadVal;
#endif




#ifdef HAS_ANALOG_SW1
/*
  VALUE   MIDPOINT  BUTTON          SW1Val
   1023   976.5                       0
   930    884                         1
   838    791                         2
   744    698                         3
   652    605                         4
   558    512.5                       5
   467    415.5                       6
   364    322.5                       7
   281    234.5                       8
   188    134                         9
   92     46                         10
   0                                 11
*/
//In case you've used different resistors to what I used, uncomment the serial print command here to find your analog read values: 
Serial.println(analogRead(SW1PIN));
  int SW1Read = analogRead(SW1PIN);
  int SW1Val = 0;
  if (SW1Read > 976) SW1Val = 1; else
    if (SW1Read > 884) SW1Val = 2; else
      if (SW1Read > 791) SW1Val = 3; else
        if (SW1Read > 698) SW1Val = 4; else
          if (SW1Read > 605) SW1Val = 5; else
            if (SW1Read > 512) SW1Val = 6; else
              if (SW1Read > 415) SW1Val = 7; else
                if (SW1Read > 322) SW1Val = 8; else
                  if (SW1Read > 234) SW1Val = 9; else
                    if (SW1Read > 134) SW1Val = 10; else
                      if (SW1Read > 46) SW1Val = 11; else
                        SW1Val = 12;
  if(prevSW1Val==0){prevSW1Val=SW1Val;}                      

  if (SW1Val != prevSW1Val) {   // only if button has changed, do something
    Serial.print("SW1 button:");
    Serial.println(SW1Val);
#if defined HAS_ANALOG_SW1 && defined HAS_TM1637_DISPLAY && defined DEBUG_ANALOG_SW1
    display.showNumberDec(SW1Val, false,1,3);    // print the dpad value on the lcd screen
#endif  
  if (SW1Val > prevSW1Val){
    SW1BitChange(1, true);   // turn the current Dpad button bit on.   
  }
  if (SW1Val < prevSW1Val){
    SW1BitChange(2, true);   // turn the current Dpad button bit on.   
  }
  
  }
  else{delay(50);SW1BitChange(2, false);SW1BitChange(1, false);}
  prevSW1Val = SW1Val;
#endif

#ifdef HAS_ANALOG_SW2
/*
  VALUE   MIDPOINT  BUTTON          SW2Val
   1023   976.5                       0
   930    884                         1
   838    791                         2
   744    698                         3
   652    605                         4
   558    512.5                       5
   467    415.5                       6
   364    322.5                       7
   281    234.5                       8
   188    134                         9
   92     46                         10
   0                                 11
*/
//In case you've used different resistors to what I used, uncomment the serial print command here to find your analog read values: 
Serial.println(analogRead(SW2PIN));
  int SW2Read = analogRead(SW2PIN);
  int SW2Val = 0;
  if (SW2Read > 976) SW2Val = 1; else
    if (SW2Read > 884) SW2Val = 2; else
      if (SW2Read > 791) SW2Val = 3; else
        if (SW2Read > 698) SW2Val = 4; else
          if (SW2Read > 605) SW2Val = 5; else
            if (SW2Read > 512) SW2Val = 6; else
              if (SW1Read > 415) SW2Val = 7; else
                if (SW2Read > 322) SW2Val = 8; else
                  if (SW2Read > 234) SW2Val = 9; else
                    if (SW2Read > 134) SW2Val = 10; else
                      if (SW2Read > 46) SW2Val = 11; else
                        SW2Val = 12;
  if(prevSW2Val==0){prevSW2Val=SW2Val;}                      

  if (SW2Val != prevSW2Val) {   // only if button has changed, do something
    Serial.print("SW2 button:");
    Serial.println(SW2Val);
#if defined HAS_ANALOG_SW2 && defined HAS_TM1637_DISPLAY && defined DEBUG_ANALOG_SW2
    display.showNumberDec(SW2Val, false,1,3);    // print the dpad value on the lcd screen
#endif  
  if (SW2Val > prevSW2Val){
    SW2BitChange(1, true);   // turn the current Dpad button bit on.   
  }
  if (SW2Val < prevSW2Val){
    SW2BitChange(2, true);   // turn the current Dpad button bit on.   
  }
  
  }
  else{delay(50);SW2BitChange(2, false);SW2BitChange(1, false);}
  prevSW2Val = SW2Val;
#endif


#ifdef HAS_ANALOG_SW3
/*
  VALUE   MIDPOINT  BUTTON          SW3Val
   1023   976.5                       0
   930    884                         1
   838    791                         2
   744    698                         3
   652    605                         4
   558    512.5                       5
   467    415.5                       6
   364    322.5                       7
   281    234.5                       8
   188    134                         9
   92     46                         10
   0                                 11
*/
//In case you've used different resistors to what I used, uncomment the serial print command here to find your analog read values: 
Serial.println(analogRead(SW3PIN));
  int SW3Read = analogRead(SW3PIN);
  int SW3Val = 0;
  if (SW3Read > 976) SW3Val = 1; else
    if (SW3Read > 884) SW3Val = 2; else
      if (SW3Read > 791) SW3Val = 3; else
        if (SW3Read > 698) SW3Val = 4; else
          if (SW3Read > 605) SW3Val = 5; else
            if (SW3Read > 512) SW3Val = 6; else
              if (SW3Read > 415) SW3Val = 7; else
                if (SW3Read > 322) SW3Val = 8; else
                  if (SW3Read > 234) SW3Val = 9; else
                    if (SW3Read > 134) SW3Val = 10; else
                      if (SW3Read > 46) SW3Val = 11; else
                        SW3Val = 12;
  if(prevSW3Val==0){prevSW3Val=SW3Val;}                      

  if (SW3Val != prevSW3Val) {   // only if button has changed, do something
    Serial.print("SW3 button:");
    Serial.println(SW3Val);
#if defined HAS_ANALOG_SW3 && defined HAS_TM1637_DISPLAY && defined DEBUG_ANALOG_SW3
    display.showNumberDec(SW3Val, false,1,3);    // print the dpad value on the lcd screen
#endif  
  if (SW3Val > prevSW3Val){
    SW3BitChange(1, true);   // turn the current Dpad button bit on.   
  }
  if (SW3Val < prevSW3Val){
    SW3BitChange(2, true);   // turn the current Dpad button bit on.   
  }
  
  }
  else{delay(25);SW3BitChange(2, false);SW3BitChange(1, false);}
  prevSW3Val = SW3Val;
#endif
}

void dPadBitChange(uint8_t dPadVal, bool bitOn) {
  // sets (raises or lowers) the correct button bits according to the Dpad button that was pressed or released
  switch (dPadVal) {
  case 0: break;
  case 1: // right
    buttonBitChange(3, bitOn); break;
  case 2: // left
    buttonBitChange(2, bitOn); break;
  case 3: // left+right (calibrate wheel center point)
    buttonBitChange(18, bitOn);     // Dpad button
    buttonBitChange(19, bitOn); break;  // Joystick button
  case 4: // down
    buttonBitChange(4, bitOn); break;
  case 5: // up
    buttonBitChange(1, bitOn); break;
  case 6: // up+down (menu)
    buttonBitChange(22, bitOn); break;
  case 7: // Dpad button
    buttonBitChange(18, bitOn); break;
  }
}

void SW1BitChange(uint8_t dPadVal, bool bitOn) {
  // sets (raises or lowers) the correct button bits according to the Dpad button that was pressed or released
  switch (dPadVal) {
  case 0: break;
  case 1: // right
    buttonBitChange(5, bitOn); break;
  case 2: // left
    buttonBitChange(16, bitOn); break;
  }
}

void SW2BitChange(uint8_t dPadVal, bool bitOn) {
  // sets (raises or lowers) the correct button bits according to the Dpad button that was pressed or released
  switch (dPadVal) {
  case 0: break;
  case 1: // right
    buttonBitChange(21, bitOn); break;
  case 2: // left
    buttonBitChange(17, bitOn); break;
  }
}

void SW3BitChange(uint8_t dPadVal, bool bitOn) {
  // sets (raises or lowers) the correct button bits according to the Dpad button that was pressed or released
  switch (dPadVal) {
  case 0: break;
  case 1: // right
    buttonBitChange(24, bitOn); break;
  case 2: // left
    buttonBitChange(23, bitOn); break;
  }
}

void buttonBitChange(uint8_t buttonBit, bool bitOn) {
    // changes a selected bit in one of the 3 button bytes to either on (1) or off (0). Valid values for buttonBit is 1 to 24. See buttons list in last comment.
        if (bitOn) {
            if (buttonBit < 23) returnData[2 + ((buttonBit - 1) / 8)] |= (1 << (((buttonBit - 1) % 8)));
        else {
            returnData[7] |= (1 << (30-buttonBit));
            //returnData[7] |= (1 << (24-buttonBit));
        }
        }
        else {
            if (buttonBit < 23) returnData[2 + ((buttonBit - 1) / 8)] &= ~(1 << (((buttonBit - 1) % 8)));
        else {
            returnData[7] &= ~(1 << (30-buttonBit));
            //returnData[7] &= ~(1 << (24-buttonBit));
        }
        }
}


void readSerial()
  // reads user inputs from serial connection. i = print mosI data, o = print misO data, d = view what is displayed on the rim's alphanumeric display. letters (A,B,C) = selects corresponding button byte (first second or third), numbers (1..7) = changes bits inside the selected byte. So user can send button presses to the wheelbase without having any physical buttons attached.
{
  if (Serial.available() > 0) {
    incByte = Serial.read();
    Serial.println((char)incByte);
    if (incByte == 'i')
      printmosibuf();
    if (incByte == 'o')
      printmisobuf();
    if (incByte == 'd')
      refreshAlphanumericDisplays(true);
    if ((incByte >= 'A') && (incByte <= 'J')) {   // send A,B,C to choose 1st, 2nd, 3rd button byte
      selectedButtonByte = (incByte - 'A' + 2);   // first button byte is 3rd in array, or cell no 2 to cell no 4 in array
      Serial.print("Selected button byte:");
      Serial.println(selectedButtonByte);
      PRINTBIN(returnData[selectedButtonByte]);
      Serial.print("      ");
      printHex(returnData[selectedButtonByte], 2);
      Serial.println();
    }
    if ((incByte >= '1') && (incByte <= '8'))
    {
      returnData[selectedButtonByte] ^= (1 << (incByte - '1')); // toggles bit correcsponding to number read by serial print
      printmisobuf();
    }
  }
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];
  sprintf(format, "0x%%.%dX", precision);
  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void refreshAlphanumericDisplays(bool crc8Stat) {
  // display current alphanumeric information through serial port / TM1637 display
  bool displaychanged = false;
  for (int i = 2; i <= 4; i++) {            // cells 2 to 4 in miso data is alphanumeric data
    if (mosiBuf[i] != prevAlphaDisp[i - 2]) {   // if anything on the display changed, reprint
      displaychanged = true;
      break;
    }
  }
#ifdef    DISPLAY_ALPHANUMERIC_DATA_WHEN_MISMATCH_ON_SERIAL   // print to serial in any case (even if packet has bad crc8). (only print to alphanumeric display if it has good crc8)
  if (displaychanged) {                     
#else                                                         // prints to serial monitor and alphanumeric display only if packet has good crc8
  if (crc8Stat && displaychanged) {                     
#endif


    for (int i = 2; i <= 4; i++) {
      uint8_t p = mosiBuf[i] & 0x7F;        // remove the . (dot) bit
      switch (p) {
      case 0x3f:  Serial.print("0"); break;
      case 0x6:   Serial.print("1"); break;
      case 0x5b:  Serial.print("2"); break;
      case 0x4f:  Serial.print("3"); break;
      case 0x66:  Serial.print("4"); break;
      case 0x6d:  Serial.print("S"); break;
      case 0x7d:  Serial.print("6"); break;
      case 0x7:   Serial.print("7"); break;
      case 0x7f:  Serial.print("8"); break;
      case 0x6f:  Serial.print("9"); break;
      case 0x39:  Serial.print("C"); break;
      case 0x38:  Serial.print("L"); break;
      case 0x8:   Serial.print("_"); break;
      case 0x79:  Serial.print("E"); break;
      case 0x54:  Serial.print("n"); break;
      case 0x71:  Serial.print("F"); break;
      case 0x76:  Serial.print("H"); break;
      case 0x5c:  Serial.print("o"); break;
      case 0x77:  Serial.print("A"); break;
      case 0x7c:  Serial.print("b"); break;
      case 0x5e:  Serial.print("d"); break;
      case 0x50:  Serial.print("r"); break;
      case 0x30:  Serial.print("I"); break;
      case 0x73:  Serial.print("P"); break;
      case 0x78:  Serial.print("t"); break;
      case 0x40:  Serial.print("-"); break;
      case 0x3e:  Serial.print("u"); break;
      case 0x2A:  Serial.print("*"); break;   // this should really be a floating 'o' (displayed on the top square) but I couldn't find an ascii character for that
      default:  printHex(p, 2);    break;
      }
      if (mosiBuf[i] & 0x80)              // select only the . (dot) bit
        Serial.print(". ");
      else
        Serial.print("  ");

      if (crc8Stat)
        prevAlphaDisp[i - 2] = mosiBuf[i];    // only update the previous display array if current packet has good crc8. (If not doing this test, text will not print out to alphanumeric display properly)
    }
    if (!crc8Stat)
      Serial.println(" [crc8 mismatch]");
    else
      Serial.println();
#ifdef HAS_TM1637_DISPLAY
    if (crc8Stat) {                            // Only refresh the alphanumeric display if the current packet is intact (has proper crc8 attached to it) {
      for (int i = 2; i <= 4; i++) {
        TM_data[i-2] = mosiBuf[i];
      }
      display.setSegments(TM_data);
    }
#endif
  }
}

void printButtonByteToSerial() {
// Prints the selected button byte to Serial, if it was changed. The button byte can be selected by sending either A,B or C through the serial monitor. The bits in the selected button byte can either be changed by the serial monitor (sending 1 to 7 raises / lowers a specific bit,) or by pressing one of the physical buttons, if you have any attached.
  if (millis() > lastPrintMillis + delayMillis) {
    if (prevPrintedByte != returnData[selectedButtonByte]) {
      PRINTBIN(returnData[selectedButtonByte]);
      Serial.print("      ");
      printHex(returnData[selectedButtonByte], 2);
      Serial.println();
    }
    prevPrintedByte = returnData[selectedButtonByte];
    lastPrintMillis = millis();
  }
}

void printmosibuf() {
  Serial.print("MOSI:");
  for (int i = 0; i < dataLength; i++)
  {
    printHex(mosiBuf[i], 2);
    //Serial.print(mosiBuf[i],HEX);
    Serial.print(" ");

  }
  Serial.println();
}

void printmisobuf() {
  Serial.print("MISO:");
  for (int i = 0; i < dataLength; i++) {
    printHex(returnData[i], 2);
    //Serial.print(returnData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void calcOutgoingCrc() {
  returnData[dataLength - 1] = crc8(returnData, dataLength - 1);    // calculates crc8 for outgoing packet
}

bool checkIncomingCrc() {   // crc check for incoming data. returns true if correct crc8 is found in the incoming data
  uint8_t crc = crc8(mosiBuf, dataLength - 1);    //Serial.print("calc crc");     //Serial.println(crc,HEX);
  if (crc != mosiBuf[dataLength - 1]) {
#ifdef DISPLAY_CRC8_MISMATCH_ON_SERIAL
    Serial.println("_____________incoming data crc8 mismatch!________");
    printmosibuf();
#endif
#ifdef DISPLAY_CRC8_MISMATCH_ON_ALPHANUMERIC
    display.setSegments(TM_crc);
#endif
  return false;
  }
  else
#ifdef DISPLAY_CRC8_MATCH_ON_SERIAL
  Serial.println("crc8 match!");
#endif
  return true;
}


/*
Clubsport wheel buttons, by bit, from 3rd byte on miso array to 5th byte.
The buttons numbers appear in the fanatec driver (mouse settings tab), and also in the 'clubsport Porsche 911 buttons.png' file.

A
1 D-pad Up
2 D-pad Left
3 D-pad Right
4 D-pad Down
5   Button 11
6   Button 3
7   Button 6
8   Button 4

B
9(1)    Right Paddle
10(2) Button 2      (top left, right one)
11(3)   Button 8      (bottom left, lower)
12(4)   Left paddle
13(5)   Button 1      (top left, left one)
14(6)   Button 5      (bottom left, higher)
15(7)   Button 9      (3 buttons in triangle, left)
16(8)   Button 10     (3 buttons in triangle, right)

C
17(1) Button 21 in contentmanager. Does not show up in fanatec's driver
18(2) D-pad Button
19(3) Joystick Button
20(4) Button 7      (3 buttons in triangle, top)
21(5) Button 27 in contentmanager. Does not show up in fanatec's driver
22(6) Menu button. Button 28 in contentmanager. Does not show up in fanatec's driver)
23(7)
24(8)

example: button 8 on fanatec's driver, number 11 on this list, is controlled by 3rd bit on the 2nd byte, of the 3 bytes that control buttons.

C6 - enters menu
B8C6 - enters shifter calibration mode

To calibrate wheel center, 
A. Using the serial monitor, send:
C6 6 23 23 6 6. (each space here means: hit return)

B. Using the Dpad:
Enter the menu by pressing both up+down bubttons on the Dpad,
S_1 will appear on the serial monitor and on the alphanumeric display,
Press both left+right buttons on the Dpad to calibrate wheel center,
Press both up+down on the Dpad to exit menu.

*/
