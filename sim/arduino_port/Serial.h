#ifndef ___SERIAL__
#define ___SERIAL__

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#include <stdarg.h>

//dummy serial implementation.... just ignore for now...
class Serial_{
public:
	static void println(...){}
	static void print(...){}
};

Serial_ Serial;

#endif //___SERIAL__
