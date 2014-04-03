/***************************************************************************
                           r250.h  -  description
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
#ifndef r250H
#define r250H

// These are default values for R250
//#define R250_SIZE 250
//#define R250_X2 103

// For bigger cycles, use size=1279 and x2=418
#define R250_SIZE 1279
#define R250_X2 418

/*! \brief R250 Algorithm.
 *
 */
class r250
{
private:
  unsigned int r250_buffer[R250_SIZE];
  int r250_index;
public:
  r250();
  ~r250();
  void init();
  void init(unsigned int seed);
  unsigned int rand();
  double drand();
};


#endif

