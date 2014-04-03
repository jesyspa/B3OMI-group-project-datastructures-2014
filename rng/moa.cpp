/***************************************************************************
                          moa.cpp  -  description
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

//#include "pch.h"

#include "moa.h"
//---------------------------------------------------------------------------

moa_rng::moa_rng()
{
}

moa_rng::~moa_rng()
{
}

void moa_rng::init(unsigned int seed)
{
  for (int i=0;i<4;i++)
  {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    X[i] = seed;
  }
  seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
  C = seed;
}

unsigned int moa_rng::rand()
{
  unsigned long long int S = (unsigned long long int)2111111111 * (unsigned long long int)X[0];
  S += (unsigned long long int)1492 * (unsigned long long int)X[1];
  S += (unsigned long long int)1776 * (unsigned long long int)X[2];
  S += (unsigned long long int)5115 * (unsigned long long int)X[3];
  S += (unsigned long long int)C;
  X[0] = X[1];
  X[1] = X[2];
  X[2] = X[3];
  X[3] = (unsigned int)S;
  C = (unsigned int)(S>>32);
  return X[3];
}


