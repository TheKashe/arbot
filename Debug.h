//
//  Debug.h
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _Debug_h
#define _Debug_h

#define STDOUT(x)		(Serial.print(x))
#define STDOUTLN(x)     (Serial.println(x))

#ifdef DEBUG
#define DEBUG_STDERR(x) (Serial.print(x))
#define DEBUG_STDOUT(x) (Serial.print(x))

//... etc
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
//... etc
#endif

#endif
