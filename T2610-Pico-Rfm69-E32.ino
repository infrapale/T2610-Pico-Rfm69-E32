/*****************************************************************************
T2608_Ruuvi_RFM_GWt 
*******************************************************************************

HW: Pi Pico 2W + RFM69

Receive Ruuvi Tag data and send via UART

??Optional: Receive RFM69 Date and time and send via UART

*******************************************************************************
https://github.com/infrapale/T2310_RFM69_TxRx
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
*******************************************************************************

*******************************************************************************

*******************************************************************************
**/

#include <Arduino.h>
#include "main.h"
#include <Wire.h>
#include "secrets.h"
#include "atask.h"
#include "io.h"
#include "r69.h"
#include "autom.h"
#include "ioexp.h"
#include "clock.h"


//*********************************************************************************************
#define SERIAL_BAUD   9600
#define IO_TICK_INTERVAL    (100)

#define WIRE Wire


main_ctrl_st ctrl = {0};

void debug_print_task(void);
void rfm_receive_task(void); 
void test_i2c_scan(void);

atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};

void initialize_tasks(void)
{
    atask_initialize();
    io_initialize();
    clock_initialize();
    atask_add_new(&debug_print_handle);
    //uart_initialize();
    #if defined(SENSOR_VILLA_ASTRID)
    r69_initialize();
    #endif
}


void setup() 
{
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer

    SerialExt.setTX(PIN_TX0 );   // UART0
    SerialExt.setRX(PIN_RX0);

    SerialTft.setTX(PIN_TX1 );   // UART1
    SerialTft.setRX(PIN_RX1);
    Serial.begin(9600);
    SerialExt.begin(9600);
    SerialTft.begin(9600);
    delay(2000);
    WIRE.setSDA(PIN_I2C0_SDA);
    WIRE.setSCL(PIN_I2C0_SCL);
    WIRE.begin();
    Serial.print(__APP__); Serial.print(F(" Compiled: "));
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.println();

    test_i2c_scan();
    initialize_tasks();
    Serial.println("Task Initialized!");
    autom_initialize();
    ioexp_initialize();
}

void setup1(){
    io_initialize();
    ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
}


void loop() 
{
    atask_run();  
    // ioexp_test();
}

void loop1()
{
    if(millis() > ctrl.next_io_tick){
        ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
        io_task();
    }
}


void run_100ms(void)
{
    io_task();
}

void debug_print_task(void)
{
    atask_print_status(true);
    clock_debug_print();
}

void test_i2c_scan(void) {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}


