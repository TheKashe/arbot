//
//  Debug.h
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _Debug_h
#define _Debug_h

#include <iostream>

#define STDOUT(x) (std::cout << (x))
#define STDOUTLN(x) do{std::cout << (x); std::cout << "\n";}while(0)

#ifdef DEBUG
#define DEBUG_STDERR(x) (std::cerr << (x))
#define DEBUG_STDOUT(x) (std::cout << (x))

//... etc
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
//... etc
#endif


#endif
