#ifndef ____randomseed__
#define ____randomseed__

unsigned int bitOut(void)
{
  static unsigned long firstTime=1, prev=0;
  unsigned long bit1=0, bit0=0, x=0, port=4, limit=99;
  if (firstTime)
  {
    firstTime=0;
    prev=analogRead(port);
  }
  while (limit--)
  {
    x=analogRead(port);
    bit1=(prev!=x?1:0);
    prev=x;
    x=analogRead(port);
    bit0=(prev!=x?1:0);
    prev=x;
    if (bit1!=bit0)
      break;
  }
  return bit1;
}

//------------------------------------------------------------------------------
unsigned long seedOut(unsigned int noOfBits)
{
  // return value with 'noOfBits' random bits set
  unsigned long seed=0;
  for (int i=0;i<noOfBits;++i)
    seed = (seed<<1) | bitOut();
  return seed;
}

//randomize doesn't work very well on arduino HW, this is a way around
void arduinoRandomize()
{
    int seed=seedOut(31);
    randomSeed(seed);
}

#endif ____randomseed__
