#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#include    "main.h"
#include    "ioexp.h"
#include    "atask.h"
#include    "io.h"

#define MCP_PIN_NBR         16 

ioexp_st ioexp[IOEXP_NBR_OF] =
{
    [IOEXP_GPA0] = {INPUT_PULLUP, 0},
    [IOEXP_GPA1] = {INPUT_PULLUP, 0},
    [IOEXP_GPA2] = {INPUT_PULLUP, 0},
    [IOEXP_LED]  = {OUTPUT, HIGH},
    [IOEXP_INP1] = {INPUT_PULLUP, 0},
    [IOEXP_INP2] = {INPUT_PULLUP, 0},
    [IOEXP_INP3] = {INPUT_PULLUP, 0},
    [IOEXP_INP4] = {INPUT_PULLUP, 0},
    [IOEXP_ULN8] = {OUTPUT, 0},
    [IOEXP_ULN7] = {OUTPUT, 0},
    [IOEXP_ULN6] = {OUTPUT, 0},
    [IOEXP_ULN5] = {OUTPUT, 0},
    [IOEXP_ULN4] = {OUTPUT, 0},
    [IOEXP_ULN3] = {OUTPUT, 0},
    [IOEXP_ULN2] = {OUTPUT, 0},
    [IOEXP_ULN1] = {OUTPUT, 0}
};


Adafruit_MCP23X17 mcp;

void ioexp_initialize(void)
{
    Serial.println("MCP ...");
    if (!mcp.begin_I2C()) {
        Serial.println("Error.");
        while (1);
    } else {
        Serial.println(".. OK");
    }

    for (uint8_t i = 0; i < IOEXP_NBR_OF; i++) 
    {  
        if(ioexp[i].io_conf == INPUT_PULLUP)
            mcp.pinMode(i, INPUT_PULLUP);
        else {
            mcp.pinMode(i, OUTPUT);
            mcp.digitalWrite(i, ioexp[i].value);
        }
    }
}

uint8_t ioexp_read_pin(uint8_t mcp_pin)
{
    return mcp.digitalRead(mcp_pin);
}

void ioexp_write_pin(uint8_t mcp_pin, uint8_t value)
{
    mcp.digitalWrite(mcp_pin, value);
}

void ioexp_test(void)
{
    uint8_t inp = mcp.digitalRead(IOEXP_INP1);
    mcp.digitalWrite(IOEXP_LED, inp);

}
