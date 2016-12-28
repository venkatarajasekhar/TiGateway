#ifndef __TIMER_OPERATIONS_H__
#define __TIMER_OPERATIONS_H__

extern volatile char TIMER2_OVERFLOW;

void BlinkWithTimer(void);
void Timer1IntHandler(void);
void Timer2IntHandler(void);
void Timer3IntHandler(void);
void StartTimer1A(void* ReloadValue);
void StartTimer2A(void* ReloadValue);
void StartTimer3A(void* ReloadValue);

#endif
