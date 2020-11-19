#include <stdint.h>
#include <Arduino.h>
#include "randombytes.h"

//TODO Maybe we do not want to use the hardware RNG for all randomness, but instead only read a seed and then expand that using fips202.

extern "C" {
  int randombytes(uint8_t *obuf, size_t len)
  {
      union
      {
          unsigned char aschar[4];
          uint32_t asint;
      } rnd;

      while (len > 4)
      {
          rnd.asint = random(2147483647);
          *obuf++ = rnd.aschar[0];
          *obuf++ = rnd.aschar[1];
          *obuf++ = rnd.aschar[2];
          *obuf++ = rnd.aschar[3];
          len -= 4;
      }
      if (len > 0)
      {
          for (rnd.asint = random(2147483647); len > 0; --len)
          {
              *obuf++ = rnd.aschar[len - 1];
          }
      }

      return 0;
  }
}
