#ifndef __IOEXP_H__
#define __IOEXP_H__

typedef enum 
{
    IOEXP_GPA0 = 0,
    IOEXP_GPA1,
    IOEXP_GPA2,
    IOEXP_LED,
    IOEXP_INP1,
    IOEXP_INP2,
    IOEXP_INP3,
    IOEXP_INP4,
    IOEXP_ULN8,
    IOEXP_ULN7,
    IOEXP_ULN6,
    IOEXP_ULN5,
    IOEXP_ULN4,
    IOEXP_ULN3,
    IOEXP_ULN2,
    IOEXP_ULN1,
    IOEXP_NBR_OF,
} ioexp_et;


typedef struct
{
    uint8_t io_conf;
    uint8_t value;
} ioexp_st;

void ioexp_initialize(void);

uint8_t ioexp_read_pin(uint8_t mcp_pin);

void ioexp_write_pin(uint8_t mcp_pin, uint8_t value);

void ioexp_test(void);

#endif