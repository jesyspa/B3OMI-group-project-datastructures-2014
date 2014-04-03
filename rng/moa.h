/***************************************************************************
                          moa.h  -  description
                             -------------------
    begin                : Thu Mar 7 2002
    copyright            : (C) 2002-2010 by Cleo Saulnier
    desc                 : MOA RNG algorithm

    Notes:
    Class to be used as the array initialiser for the r250 algorithm.

    Algorithm created by Dr. George Marsaglia.

    Dec 2010:
    I remember writing the code based on the equations.
    So I'm leaving my contribution in the public domain.
    
 ***************************************************************************/
#ifndef moaH
#define moaH
/*! \brief MOA RNG Algorithm.
 *
 */
class moa_rng
{
private:
  unsigned int X[4];
  unsigned int C;
public:
  moa_rng();
  ~moa_rng();
  void init(unsigned int seed);
  unsigned int rand();
};
#endif

