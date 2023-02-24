// Redefine CS Pin Name
// SPI_CS0:     ESP32
// SS:          ESP32, nRF, RP2040
// SPI_CS:      Artemis
// PIN_SPI_SS:  STM32, SAMD51, nRF, RP2040

#ifndef PIN_SPI_SS
  // For Artemis
  #ifdef SS
    #define PIN_SPI_SS SPI_CS
  #endif
  // For ESP32
  #ifdef SPI_CS0
    #define PIN_SPI_SS SS
  #endif
#endif

// SX1276 pin connections:
//       | SLOT 0 | SLOT 1 |
//==========================
// cs    |   CS0  |   CS1  |
// dio0  |   D0   |   D1   |
// dio1  |   G2   |   G7   |
// dio2  |   G3   |   G8   |
// rst   |   G1   |   G6   |
// tx_en |  PWM0  |  PWM1  |
// rx_en |   G0   |   G5   |

#if defined(ARDUINO_RASPBERRY_PI_PICO)
  // MM RP2040 Processor Board (Using RP2040 Pico board definition)
  int pin_cs = 21;
  int pin_dio0 = 6;
  int pin_tx_enable = 13;
  int pin_rx_enable = 16;
  int pin_nrst = 17;
  int pin_dio1 = 18;
  
  // Redefine SPI pins
  int miso = 20;
  int mosi = 23;
  int sck = 22;

  // Custom SPI object
  MbedSPI SPI_mm(miso, mosi, sck);

  SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1, SPI_mm);

#else
  #if defined(ARDUINO_TEENSY_MICROMOD)
    // MM Teensy Processor Board
    int pin_cs = 10;
    int pin_dio0 = 4;
    int pin_tx_enable = 3;
    int pin_rx_enable = 40;
    int pin_nrst = 41;
    int pin_dio1 = 42;
  
  #else
    // All other MicroMod Processor Boards
    int pin_cs =        PIN_SPI_SS;
    int pin_dio0 =      D0;
    int pin_tx_enable = PWM0;
    int pin_rx_enable = G0;
    int pin_nrst =      G1;
    int pin_dio1 =      G2;
  #endif
  
  SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);
#endif