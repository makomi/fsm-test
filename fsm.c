// gcc -std=c99 -Wall -Wextra -Wpedantic fsm.c -o fsm &&  ./fsm

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>      // uint8_t


//------------------------------------------------------------------------------
// FSM data types


typedef enum eBacklightEvent
{
  eBE_systemStart,
  eBE_userAction,
  eBE_userInactive,
  eBE_count
} teBacklightEvent;


typedef enum eBacklightState
{
  eBS_warmup,
  eBS_on,
  eBS_shutdown,
  eBS_off,
  eBS_count
} teBacklightState;


const char* stringifyBacklightEvent(teBacklightEvent event)
{
  switch (event)
  {
    case eBE_systemStart:
      return "eBE_systemStart";
    case eBE_userAction:
      return "eBE_userAction";
    case eBE_userInactive:
      return "eBE_userInactive";
    default:
      return "invalid event";
  }
}


//------------------------------------------------------------------------------
// FSM helper functions

void printFsmEvents(void)
{
  for (uint16_t event = 0; event < eBE_count; ++event)
    printf("%u: %s\n", event, stringifyBacklightEvent((teBacklightEvent)event));

  printf("\n");
}

//------------------------------------------------------------------------------
// FSM's public API

// Normally a systemEventHandler would call the FSM handler
// by receiving the system ivents and calling the FSM handler
// with the appropriate backlight events


/// Initial FSM state
teBacklightState fsmState = eBS_off;


/// Inform the FSM about a new event
void fsmEventHandler(teBacklightEvent event)
{
  if (event < 0 || event >= eBE_count)
    return;

  printf("event: %s\n", stringifyBacklightEvent((teBacklightEvent)event));
}


/// Check if a recognized touch is valid
uint8_t backlightTouchIsValid(void)
{
  return (fsmState == eBS_on) ? 1 : 0;
}


//------------------------------------------------------------------------------


/// Get a single character without the need to hit 'return'
char getch(void)
{
  char   ch;
  struct termios oldt;
  struct termios newt;

  tcgetattr(STDIN_FILENO, &oldt);            // store old settings
  newt = oldt;                               // copy old settings to new settings
  newt.c_lflag &= ~(ICANON | ECHO);          // make one change to old settings in new settings
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);   // apply the new settings immediatly
  ch = (char) getchar();                     // standard getchar call
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);   // reapply the old settings

  return ch;                                 // return received char
}


/// User Interface
uint8_t runUi(void)
{
  char ch = ' ';

  // Get user input
  printf("Choose the next event:");
  ch = getch();
  printf("\33[2K\r");                        // clearline EL2: Clear entire line 

  // Exit program
  if ('q' == ch)
    return 1;

  // Update state
  fsmEventHandler((teBacklightEvent)((int32_t)ch-48));

  return 0;
}


//------------------------------------------------------------------------------


int main(void)
{
  printf("Finite State Machine\n");
  printf("--------------------\n\n");

  printFsmEvents();

  // Get and process user input
  while (runUi() == 0);

  printf("\nexit program\n");

  return 0;
}
