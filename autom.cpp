#include <sys/_stdint.h>
#include "main.h"
#include "io.h"
#include "ioexp.h"
#include "atask.h"
#include "clock.h"
#include "autom.h"
#include "r69.h"


typedef struct
{
    uint8_t     indx;
} autom_inp_cntrl_st;

typedef struct
{
    uint8_t     indx;
} autom_out_cntrl_st;

typedef struct
{
    autom_inp_cntrl_st inp;
    autom_out_cntrl_st out;
} autom_cntrl_st;

extern main_ctrl_st main_ctrl;

autom_cntrl_st actrl =
{
    .inp = {.indx = 0},
    .out = {.indx = 0},
};

extern basic_time_st   clock_basic;

void autom_inp_task(void);
void autom_out_task(void);

atask_st inp_th         = {"Input Autom    ", 1000,0, 0, 255, 0, 1, autom_inp_task};
atask_st out_th         = {"Output Autom   ", 1000,0, 0, 255, 0, 1, autom_out_task};

typedef struct 
{
    uint32_t    state;
    uint32_t    timeout;
} one_inp_sm_st;

typedef struct 
{
    uint32_t    state;
    uint32_t    timeout;
} one_out_sm_st;


one_inp_sm_st inp_sm[AUTOM_INP_NBR_OF] = {0};
one_out_sm_st out_sm[AUTOM_OUT_NBR_OF] = {0};

autom_outp_st autom_out[AUTOM_OUT_NBR_OF] =
{
    [AUTOM_OUT_INDX_ENTRANCE]   = {"Katos",     true,   IOEXP_ULN1, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_2]          = {"Out2",      false,  IOEXP_ULN2, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_3]          = {"Out3",      false,  IOEXP_ULN3, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_4]          = {"Out4",      false,  IOEXP_ULN4, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_DING_DONG]  = {"DingDong",  true,   IOEXP_ULN5, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_ALARM]      = {"Alarm",     true,   IOEXP_ULN6, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_7]          = {"Out7",      false,  IOEXP_ULN7, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_8]          = {"Out8",      false,  IOEXP_ULN8, 0, 0, OUT_EVENT_UNDEFINED, 0},
    [AUTOM_OUT_INDX_LED]        = {"LED",       false,  IOEXP_LED,  0, 0, OUT_EVENT_UNDEFINED, 0}
};

autom_inp_st autom_inp[AUTOM_INP_NBR_OF] =
{
    [AUTOM_INP_INDX_PIR_ENTRANCE]   = {"Katos", true, IOEXP_INP1, LOW, INP_STATE_OFF, 0, 0},
    [AUTOM_INP_INDX_PIR_1]          = {"PIR_1", true, IOEXP_INP2, LOW, INP_STATE_OFF, 0, 0},
    [AUTOM_INP_INDX_PIR_2]          = {"PIR_2", true, IOEXP_INP3, LOW, INP_STATE_OFF, 0, 0},
    [AUTOM_INP_INDX_PIR_3]          = {"PIR_3", true, IOEXP_INP4, LOW, INP_STATE_OFF, 0, 0}
};


char week_day[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const uint32_t dark_time[MONTHS_PER_YEAR] =
{   
    //                --------222211111111110000000000
    //                --------321098765432109876543210
    [JANUARY]    = {0b00000000111111110000000111111111},  
    [FEBRUARY]   = {0b00000000111111100000000011111111},
    [MARCH]      = {0b00000000111110000000000001111111},
    [APRIL]      = {0b00000000111100000000000000111111},
    [MAY]        = {0b00000000111000000000000000001111},
    [JUNE]       = {0b00000000110000000000000000000011},
    [JULY]       = {0b00000000110000000000000000000011},
    [AUGUST]     = {0b00000000111000000000000000000111},
    [SEPTEMBER]  = {0b00000000111100000000000000001111},
    [OCTOBER]    = {0b00000000111111000000000001111111},
    [NOVEMBER]   = {0b00000000111111110000001111111111},
    [DECEMBER]   = {0b00000000111111111000011111111111},
};

void dark_test(void);

void autom_initialize(void)   //uint8_t hour, uint8_t minute)
{
    atask_add_new(&inp_th);
    atask_add_new(&out_th);
    dark_test();
}  

bool autom_get_is_dark(uint8_t month, uint8_t hour)
{
    bool is_dark = false;
    if ((month < MONTHS_PER_YEAR) && (hour < 24))
    {
        if((dark_time[month] & (1UL << hour)) != 0) is_dark = true;
    }
    return is_dark;
}


void autom_set_out_event(uint8_t out_indx, autom_out_event_et event, uint32_t duration, bool at_dark_only)
{
    if ((autom_get_is_dark(clock_basic.month, clock_basic.hour)) || !at_dark_only)
    {
        autom_out[out_indx].event = event;
        autom_out[out_indx].duration = duration;
    }

}

void autom_send_inp_event(uint8_t inp_indx)
{
    char buff[ R69_MSG_SIZE ];
    sprintf(buff,"<E;%s;C;%d>",
        autom_inp[inp_indx].label,
        autom_inp[inp_indx].event_cntr);
    r69_send(buff);

}

void autom_one_inp_sm(uint8_t inp_indx)
{
    switch(inp_sm[inp_indx].state)
    {
        case 0:
            inp_sm[inp_indx].state = 10;
            break;
        case 10:
            autom_inp[inp_indx].value = ioexp_read_pin(autom_inp[inp_indx].pin);
            if (autom_inp[inp_indx].value == autom_inp[inp_indx].on_level){
                inp_sm[inp_indx].state = 50;
                Serial.printf("%s ON\n",autom_inp[inp_indx].label);
                autom_inp[inp_indx].event_cntr++;
                switch(inp_indx)
                {
                    case AUTOM_INP_INDX_PIR_ENTRANCE:
                        autom_set_out_event(AUTOM_OUT_INDX_ENTRANCE, OUT_EVENT_SWITCH_ON, 5000, true);
                        autom_set_out_event(AUTOM_OUT_INDX_LED, OUT_EVENT_SWITCH_ON, 5000, false);
                        autom_send_inp_event(inp_indx);
                        break;
                    case AUTOM_INP_INDX_PIR_1:
                        autom_set_out_event(AUTOM_OUT_INDX_DING_DONG, OUT_EVENT_SWITCH_ON, 2000, false);
                        break;
                    case AUTOM_INP_INDX_PIR_2:
                        autom_set_out_event(AUTOM_OUT_INDX_DING_DONG, OUT_EVENT_SWITCH_ON, 5000, false);
                        break;
                    case AUTOM_INP_INDX_PIR_3:
                        autom_set_out_event(AUTOM_OUT_INDX_ALARM, OUT_EVENT_SWITCH_ON, 0, false);
                        break;
                }

            }
            break;
        case 50:
            autom_inp[inp_indx].value = ioexp_read_pin(autom_inp[inp_indx].pin);
            if (autom_inp[inp_indx].value != autom_inp[inp_indx].on_level){
                inp_sm[inp_indx].state = 100;
                autom_inp[inp_indx].timeout = millis() + 10000;
                Serial.printf("%s OFF\n",autom_inp[inp_indx].label);
                switch(inp_indx)
                {
                    case AUTOM_INP_INDX_PIR_ENTRANCE:
                        break;
                    case AUTOM_INP_INDX_PIR_1:
                        break;
                    case AUTOM_INP_INDX_PIR_2:
                        break;
                    case AUTOM_INP_INDX_PIR_3:
                        autom_set_out_event(AUTOM_OUT_INDX_ALARM, OUT_EVENT_SWITCH_OFF, 0, false);
                        break;
                }

            }
            break;
        case 100:
            if(millis() > autom_inp[inp_indx].timeout )
            {
                inp_sm[inp_indx].state = 10;
            }
            break;

    }
}

void autom_inp_task(void)
{
    switch(inp_th.state)
    {
        case 0:
            inp_th.state = 10;
            actrl.inp.indx = 0;
            break;
        case 10:
            //Serial.printf("Indx %d\n",actrl.inp.indx);
            if(autom_inp[actrl.inp.indx].active){
                autom_one_inp_sm(actrl.inp.indx);                
            }
            if(actrl.inp.indx < AUTOM_INP_NBR_OF-1 ) actrl.inp.indx++;
            else actrl.inp.indx = 0;
            break;
        case 20:
            inp_th.state = 10;
            break;
        case 100:
            inp_th.state = 10;
            break;
    }
}

void autom_one_out_sm(uint8_t out_indx)
{
    switch(out_sm[out_indx].state)
    {
        case 0:
            out_sm[out_indx].state = 10;
            break;
       case 10:
            switch(autom_out[out_indx].event)
            {
                case OUT_EVENT_UNDEFINED:
                    break;
                case OUT_EVENT_SWITCH_ON:
                    ioexp_write_pin( autom_out[out_indx].pin, HIGH);
                    Serial.printf("%s -> ON\n",autom_out[out_indx].label);
                    autom_out[out_indx].event = OUT_EVENT_UNDEFINED;
                    if(autom_out[out_indx].duration > 0){
                        autom_out[out_indx].timeout = millis()+ autom_out[out_indx].duration;
                        out_sm[out_indx].state = 50;
                    } 
                    break;
                case OUT_EVENT_SWITCH_OFF:
                    ioexp_write_pin( autom_out[out_indx].pin, LOW);
                    autom_out[out_indx].event = OUT_EVENT_UNDEFINED;
                    Serial.printf("%s -> OFF\n",autom_out[out_indx].label);
                    if(autom_out[out_indx].duration > 0){
                        autom_out[out_indx].timeout = millis()+ autom_out[out_indx].duration;
                        out_sm[out_indx].state = 60;
                    } 
                    break;
            }       
            break;
        case 50:
            if( millis() > autom_out[out_indx].timeout)
            {
                ioexp_write_pin( autom_out[out_indx].pin, LOW);   
                out_sm[out_indx].state = 10;
            }
            break;
        case 60:
            if( millis() > autom_out[out_indx].timeout)
            {
                out_sm[out_indx].state = 10;
            }
            break;
     }
}

void autom_out_task(void)
{
    switch(out_th.state)
    {
        case 0:
            out_th.state = 10;
            actrl.out.indx = 0;
            break;
        case 10:
            if(autom_out[actrl.out.indx].active){
                autom_one_out_sm(actrl.out.indx);                
            }
            if(actrl.out.indx < AUTOM_OUT_NBR_OF-1 ) actrl.out.indx++;
            else actrl.out.indx = 0;
            break;
        case 20:
            inp_th.state = 10;
            break;

        case 100:
            out_th.state = 10;
            break;
        case 110:
            out_th.state = 10;
            break;
        case 120:
            out_th.state = 10;
            break;
        case 200:
            out_th.state = 10;
            break;
    }
}

void dark_test(void)
{
    for(uint8_t month=0; month < MONTHS_PER_YEAR; month++)
    {
        for(uint8_t hour = 0; hour < 24; hour++)
        {
            if(autom_get_is_dark(month, hour)) Serial.print("#");
            else Serial.print("-");
        }
        Serial.println("");
    }
}
