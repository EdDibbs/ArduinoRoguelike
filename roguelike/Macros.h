#ifndef __MACROS__
#define __MACROS__

#define DEBUGMODE

#ifdef DEBUGMODE
#define Sprintln(a) (Serial.println(a))
#define Sprint(a) (Serial.print(a));
#else
#define Sprintln(a)
#define Sprint(a)
#endif

#endif
