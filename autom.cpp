#include "main.h"
#include "io.h"
//#include "relay.h"
#include "atask.h"
#include "autom.h"

typedef struct
{
   relay_prog_et  program;
   atask_st  *th;
   time_st  last_time;
   bool     is_dark_baseline;
   uint8_t  prev_state_index;
   bool     set_time;
   uint8_t  relay_indx;
   uint32_t iter_cntr;
   uint32_t next_random;
   uint32_t next_relay_ms;

} autom_cntrl_st;

extern main_ctrl_st main_ctrl;

atask_st autom_task_handle         = {"Automation     ", 1000,0, 0, 255, 0, 1, autom_task};

autom_cntrl_st autom_cntrl;

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

void autom_initialize(uint8_t hour, uint8_t minute)
{
    atask_add_new(&autom_task_handle);
    autom_cntrl.last_time.hour = 0;
    autom_cntrl.last_time.minute = 0;
    autom_cntrl.program = RELAY_PROG_AT_HOME;
    autom_cntrl.relay_indx = 0;
    autom_cntrl.iter_cntr = 0;
    autom_cntrl.prev_state_index = 0;
    autom_cntrl.next_relay_ms = 0;
    autom_cntrl.next_random = 0;
    autom_cntrl.th = atask_get_task(TASK_AUTOM);
    //autom_task_handle. = 0;
    autom_cntrl.set_time = false;

    for (uint8_t i = VA_RELAY_UNDEF; i < VA_RELAY_NBR_OF; i++)
    {
        autom_relay[i].hindx = 0;
        autom_relay[i].autom_state = RELAY_AUTOM_STATE_OFF;
    }
}  

bool autom_get_is_dark(void)
{
    return autom_cntrl.is_dark_baseline;
}
