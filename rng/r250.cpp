/***************************************************************************
                          r250.cpp  -  description
                             -------------------

    begin                : ~2001
    copyright            : (C) 2001-2010 by Cleo Saulnier
    desc                 : r250 RNG class.

    Notes:
    Class to support r250 RNG algorithm.
    Produces full 32 bit random numbers or normalized double value.

    Code is from Maier in Dr. Dobbs Journal, 1991.
    I adapted it for 32 bit and ported to C++ class.
    I'm leaving my contribution in the public domain.

    Here is the comment from the original code.
***************************************************************************
* Module: r250.cpp Description: implements R250 random number generator,
* from S. Kirkpatrick and E. Stoll, Journal of Computational Physics, 40,
* p. 517 (1981). Written by: W. L. Maier
***************************************************************************

    Maier was contacted and said,
    "Yes, you can use the r250 code however you like."

    Usage:
    1. Create instance.  ex: r250 *r = new r250();  *or* r250 r;
    2. Initialize seed.  ex: r->init(time(NULL));
    3. Get random values.  ex: int number = r->rand();  *or* double number = r->drand();
    4. delete instance if dynamic allocation.  ex: delete r;

 ***************************************************************************/
//#include "pch.h"
#include <stdio.h>
#include <time.h>

#include "r250.h"
#include "moa.h"
//---------------------------------------------------------------------------

r250::r250()
{
}

r250::~r250()
{
}

// Auto init.
// Uses clock as seed if available, time(NULL) otherwise.
void r250::init()
{
  clock_t c = clock();
  time_t t = time(NULL);
  if (c==-1) init((unsigned int)t);
  else init((unsigned int)(c+t));
}

void r250::init(unsigned int seed)
{
  int j, k;
  unsigned long int mask, msb;
  unsigned int x;

	moa_rng *moa = new moa_rng();
	moa->init(seed);

	r250_index = 0;
	for (j = 0; j < R250_SIZE; j++) // fill r250 buffer with 32 bit values
	{
		x = moa->rand();
		r250_buffer[j] = x;
	}
	delete moa;
	moa = NULL;
  msb = 0x80000000;   // turn on diagonal bit
  mask = 0x7fffffff;        // turn off the leftmost bits

  for (j = 0; j < 32; j++)
  {
    k = 7 * j + 3;    // select a word to operate on
    r250_buffer[k] &= mask; // turn off bits left of the diagonal
    r250_buffer[k] |= msb;    // turn on the diagonal bit
    mask >>= 1;
    msb  >>= 1;
  }
}

/**** Function:  r250  Description: returns a random unsigned integer. ****/
unsigned int r250::rand()
{
/*------------------------------------------------------------------------*/
    register int    j;
    register unsigned int new_rand;
/*------------------------------------------------------------------------*/
    if (r250_index >= R250_SIZE-R250_X2)
        j = r250_index - (R250_SIZE-R250_X2);      /* Wrap pointer around */
    else
        j = r250_index + R250_X2;

    new_rand = r250_buffer[r250_index] ^ r250_buffer[j];
    r250_buffer[r250_index] = new_rand;
    if (r250_index >= R250_SIZE-1)      /* Increment pointer for next time */
        r250_index = 0;
    else
        r250_index++;

    return new_rand;
}

/**** Function:  r250  Description: returns a random double in range 0-1. ****/
double r250::drand()
{
/*------------------------------------------------------------------------*/
    register int    j;
    register unsigned int new_rand;
/*------------------------------------------------------------------------*/
    if (r250_index >= R250_SIZE-R250_X2)
        j = r250_index - (R250_SIZE-R250_X2);     /* Wrap pointer around */
    else
        j = r250_index + R250_X2;

    new_rand = r250_buffer[r250_index] ^ r250_buffer[j];
    r250_buffer[r250_index] = new_rand;
    if (r250_index >= R250_SIZE-1)      /* Increment pointer for next time */
        r250_index = 0;
    else
        r250_index++;
    return new_rand / (double)0xffffffff;   /* Return a number in 0.0 to 1.0 */
}


