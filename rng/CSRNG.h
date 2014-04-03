/*
   Description: Header for CS::RNG

   Copyright 2010 Cleo Saulnier

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef CSRNGH
#define CSRNGH

#include <stdio.h>
#include "r250.h"

namespace CS
{

class RNG
{
private:
  static r250 *rng;
  static int counter;
public:
  RNG()
  {
    counter++;
    if (rng==NULL)
    {
      rng=new r250();
      rng->init();
    }
  }

  ~RNG()
  {
    counter--;
    if (counter==0)
    {
      delete rng;
      rng = NULL;      
    }
  }

  unsigned int rand() {return rng->rand();}
  double drand() {return rng->drand();}
};

class RNG_threaded
{
private:
  r250 rng;
public:
  RNG_threaded()
  {
    rng.init();
  }

  ~RNG_threaded()
  {
  }

  unsigned int rand() {return rng.rand();}
  double drand() {return rng.drand();}
};

}

#endif
