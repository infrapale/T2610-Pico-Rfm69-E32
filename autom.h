#ifndef __AUTOM_H__
#define __AUTOM_H__

#define MONTHS_PER_YEAR 12
#define AUTOM_LABEL_LEN 16

typedef enum
{
  JANUARY = 0,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
} month_et;

typedef enum
{
    INP_STATE_OFF,
    INP_STATE_ON
}   inp_state_st;

typedef enum 
{
    AUTOM_INP_INDX_PIR_ENTRANCE  = 0,
    AUTOM_INP_INDX_PIR_1,
    AUTOM_INP_INDX_PIR_2,
    AUTOM_INP_INDX_PIR_3,
    AUTOM_INP_NBR_OF
} autom_inp_et;

typedef enum 
{
    AUTOM_OUT_INDX_ENTRANCE  = 0,
    AUTOM_OUT_INDX_2,
    AUTOM_OUT_INDX_3,
    AUTOM_OUT_INDX_4,
    AUTOM_OUT_INDX_DING_DONG,
    AUTOM_OUT_INDX_ALARM,
    AUTOM_OUT_INDX_7,
    AUTOM_OUT_INDX_8,
    AUTOM_OUT_INDX_LED,
    AUTOM_OUT_NBR_OF
} autom_outp_indx_et;

typedef enum
{
    OUT_EVENT_UNDEFINED = 0,
    OUT_EVENT_SWITCH_ON,
    OUT_EVENT_SWITCH_OFF,
} autom_out_event_et;

typedef struct
{
    char            label[AUTOM_LABEL_LEN];
    bool            active;
    uint8_t         pin;
    uint8_t         on_level;
    inp_state_st    state;
    uint8_t         value;
    uint32_t        timeout;
    uint16_t        event_cntr;
} autom_inp_st;


typedef struct
{
    char        label[AUTOM_LABEL_LEN];
    bool        active;
    uint8_t     pin;
    uint8_t     value;
    uint32_t    timeout;
    autom_out_event_et  event;
    uint32_t    duration;
} autom_outp_st;


void autom_initialize();

bool autom_get_is_dark(uint8_t month, uint8_t hour);

#endif