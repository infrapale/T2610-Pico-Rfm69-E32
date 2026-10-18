#ifndef __AUTOM_H__
#define __AUTOM_H__

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

void autom_initialize(uint8_t hour, uint8_t minute);

bool autom_get_is_dark(void);

#endif