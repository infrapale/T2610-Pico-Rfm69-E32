#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#include    "main.h"
#include    "ioexp.h"
#include    "atask.h"
#include    "io.h"

#define MCP_PIN_NBR         16 

typedef struct 
{
    uint8_t         pin;
    uint8_t         value;    
} ioexp_st;

Adafruit_MCP23X17 mcp;

void ioexp_initialize(void)
{
    for (uint8_t i = 0; i < MCP_PIN_NBR; i++) {  
        if(butt[i].status == BUTT_STATUS_IDLE)
        {
            mcp.pinMode(butt[i].pin, INPUT_PULLUP);
            mcp.setupInterruptPin(butt[i].pin, LOW);
        }
    }
}