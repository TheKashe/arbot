#ifndef ____randomseed__
#define ____randomseed__
//randomize doesn't work very well on arduino HW
//in browser we jst use srand
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

void arduinoRandomize()
{
    srand (time(NULL));
}

#endif // ____randomseed__