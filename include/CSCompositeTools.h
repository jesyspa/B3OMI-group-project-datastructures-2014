/*
   Description: Header file for MACROS and other tools used in
                composite and delegate skiplists.

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
#ifndef CSCompositeToolsH
#define CSCompositeToolsH

#include <exception>
#include <stddef.h>
#include <math.h>
#include <iterator>
#include "CSRNG.h"

namespace CS
{

#define CSXDefineIteratorEQ(it) \
    bool operator==(const it& other) const \
    { \
      return(FNode == other.FNode); \
    } \
 \
    bool operator!=(const it& other) const \
    { \
      return !(FNode == other.FNode); \
    }

#define CSXDefineIteratorInc(it) \
    it& operator++() \
    { \
      node_type* node1; \
      if ((FNode != NULL)&&((node1 = FNode->forward(X,0)) != NULL)) \
      { \
        FNode = node1; \
CSINDEX(Findex++,); \
      } \
      return *this; \
    } \
 \
 CSDefineIteratorIncPost(it)

#define CSXDefineIteratorDecBidi(it) \
    it& operator--() \
    { \
      node_type* node1; \
      node_type* node2; \
      if ((FNode != NULL)&&((node1 = FNode->backward(X,0)) != NULL)&&((node2 = node1->backward(X,0)) != NULL)) \
      { \
        FNode = node1; \
CSINDEX(Findex--,); \
      } \
      return *this; \
    } \
 \
 CSDefineIteratorDecPost(it)

#define CSXDefineIteratorAccess \
    reference operator*() const \
    { \
      return FNode->object; \
    } \
 \
    pointer operator->() const \
    { \
      return &FNode->object; \
    }

#define CSXDefineIteratorLTNodeCompare(it) \
  CSINDEX(CSDefineIteratorLTNodeIndexCompare(it), \
          CSXDefineIteratorLTNodeMultiKeyCompare(it)) \

// Compares two iterators via nodes.
// Supports comparison by multikey.
#define CSXDefineIteratorLTNodeMultiKeyCompare(it) \
    bool operator<(const it &other) const \
    { \
      bool a = (FNode->forward(X,0)==NULL); \
      bool b = (other.FNode->forward(X,0)==NULL); \
      if (a||b) \
      { \
        if (a) return false; \
        return true; \
      } \
 \
      container_type *container = getContainer(); \
      if (container->value_comp()(FNode->object,other.FNode->object)) return true; \
      if (container->value_comp()(other.FNode->object,FNode->object)) return false; \
 \
      /* keys are equal.  Scan until *this reaches other or until *this<node. */ \
      node_type *node1 = FNode->forward(X,0); \
      node_type *node2 = node1->forward(X,0); \
      for(;;) \
      { \
        if (other.FNode==node1) return true; /* reached other node. */ \
        if (node2==NULL) return false; /* reached end(). */ \
        if (container->value_comp()(FNode->object,node1->object)) return false; /* reached non-equal element */ \
        node1 = node2; \
        node2 = node2->forward(X,0); \
      } \
    }


#define CSXDefineIteratorLTNodeKeyCompare(it) \
    bool operator<(const it &other) const \
    { \
      bool a = (FNode->forward(X,0)==NULL); \
      bool b = (other.FNode->forward(X,0)==NULL); \
      if (a||b) \
      { \
        if (a) return false; \
        return true; \
      } \
 \
      return getContainer()->value_comp()(FNode->object,other.FNode->object); \
    }

#define CSXDefineIteratorRefreshBidi \
    size_type refresh() \
    { \
      if ((FNode==NULL)||(FNode->backward(X,0)==NULL)) \
      { \
        Findex = -1; \
        return Findex; \
      } \
      Findex = 0; \
      node_type* node1=FNode->backward(X,0); \
 \
      unsigned int level = 0; \
      while(node1->backward(X,level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        node1 = node1->backward(X,level); \
        Findex+=node1->skip(X,level); \
      } \
      return Findex; \
    }


#define CSXDefineIteratorArray(it) \
    reference operator[](difference_type off) const \
    { \
      it tmp(*this); \
      tmp+=off; \
      return *tmp; \
    }

#define CSXDefineIteratorForwardArbitraryAccess(it) \
    it& operator+=(difference_type off) \
    { \
      if (off==0) return *this; \
CSLEVEL(if (off<0) return operator-=(-off),); \
      if (off==1) return operator++(); \
      /* This will use head which has more levels and */ \
      /* will advance much quicker. */ \
CSLEVEL( \
      if (Findex==0) \
      { \
        FNode = data->head; \
        off++; \
        Findex--; \
      } \
      /* Go forward. */ \
      size_type adv = 0; \
      unsigned int level = FNode->level; \
      node_type* node1=FNode; \
      /* Try going up in levels if possible. */ \
      while(node1->forward(X,level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        if (adv+node1->skip(X,level)>off) break; /* Start going down in levels. */ \
        adv+=node1->skip(X,level); \
        node1 = node1->forward(X,level); \
      } \
      /* Try going down in levels. */ \
      while(node1->forward(X,level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->skip(X,level)>off)) \
        { \
          level--; \
        } \
        if (adv+node1->skip(X,level)>off) break; /* DONE! */ \
        adv+=node1->skip(X,level); \
        node1 = node1->forward(X,level); \
      } \
      /* We're at the correct node. */ \
      FNode = node1; \
      Findex+=adv; \
      \
, \
      \
      difference_type newIndex = Findex+off; \
      if (newIndex<0) \
      { \
        Findex = 0; \
        FNode = data->head->forward(X,0); \
        return *this; \
      } \
      if (newIndex>=data->items) \
      { \
        Findex = data->items; \
        FNode = data->tail; \
        return *this; \
      } \
 \
      Findex = newIndex; \
      FNode = data->head; \
      difference_type adv = -1; \
      node_type* node1=FNode; \
      unsigned int level = data->level; \
      while(node1->forward(X,level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->skip(X,level)>off)) \
        { \
          level--; \
        } \
        if (adv+node1->skip(X,level)>off) break; /* DONE! */ \
        adv+=node1->skip(X,level); \
        node1 = node1->forward(X,level); \
      } \
      /* We're at the correct node. */ \
      FNode = node1;) \
      return *this; \
    }

#define CSXDefineIteratorReverseArbitraryAccess(it) \
    it& operator-=(difference_type off) \
    { \
      if (off==0) return *this; \
      if (off<0) return operator+=(-off); \
      if (off==1) return operator--(); \
 \
      /* Go backward. */ \
      size_type adv = 0; \
      unsigned int level = FNode->level; \
      node_type* node1=FNode; \
      /* Try going up in levels if possible.*/ \
      while(node1->backward(X,level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        if (adv+node1->backward(X,level)->skip(X,level)>off) break; /* Start going down in levels. */ \
        adv+=node1->backward(X,level)->skip(X,level); \
        node1 = node1->backward(X,level); \
      } \
      /* Try going down in levels. */ \
      while(node1->backward(X,level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->backward(X,level)->skip(X,level)>off)) \
        { \
          level--; \
        } \
        if (adv+node1->backward(X,level)->skip(X,level)>off) break; /* DONE! */ \
        adv+=node1->backward(X,level)->skip(X,level); \
        node1 = node1->backward(X,level); \
      } \
      /* Check that we're not at the dummy node. */ \
      if (node1->backward(X,0)==NULL) \
      { \
        adv--; \
        node1 = node1->forward(X,0); \
      } \
      /* We're at the correct node. */ \
      FNode = node1; \
      Findex-=adv; \
      return *this; \
    }

#define CSXDefineBeginEnd \
iterator begin() \
{ \
  return CSINDEX(iterator(data,data->head->forward(X,0),0),iterator(data,data->head->forward(X,0))); \
} \
 \
iterator end() \
{ \
  return CSINDEX(iterator(data,data->tail,data->items),iterator(data,data->tail)); \
} \
 \
const_iterator begin() const \
{ \
  return CSINDEX(const_iterator(data,data->head->forward(X,0),0),const_iterator(data,data->head->forward(X,0))); \
} \
 \
const_iterator end() const \
{ \
  return CSINDEX(const_iterator(data,data->tail,data->items),const_iterator(data,data->tail)); \
}

#define CSXDefineSize \
size_type size() const \
{ \
  return data->items; \
}

#define CSXDefineEmpty \
bool empty() const \
{ \
  return (data->items==0); \
}

#define CSXDefineFront \
reference front() \
{ \
  return data->head->forward(X,0)->object; \
} \
 \
const_reference front() const \
{ \
  return data->head->forward(X,0)->object; \
}

#define CSXDefineBackBidi \
reference back() \
{ \
  return data->tail->backward(X,0)->object; \
} \
 \
const_reference back() const \
{ \
  return data->tail->backward(X,0)->object; \
}

#define CSXDefineMaxSize \
size_type max_size() const \
{ \
  size_type a = -1; \
  double da = (double)a; \
  da+=1.0; \
  size_type n = sizeof(node_type); \
  double dn = (double)n; \
  double c = data->probability; \
  double total = 0.0; \
  for(unsigned int i=1;i<=data->maxLevel;i++) \
  { \
    total+=c; \
    c*=data->probability; \
  } \
  n = sizeof(typename node_type::ptr_type); \
  dn+=((double)n)*total; \
  da/=dn; \
  return (size_type)da; \
}

#define CSXDefineClear \
void clear() \
{ \
  node_type *t1 = data->head->forward(0,0); \
  while((t1)&&(t1!=data->tail)) \
  { \
    node_type *t2 = t1->forward(0,0); \
    data->Free(t1); \
    t1 = t2; \
  } \
 \
  for (unsigned int i=0;i<data->getListCount();i++) \
  { \
    if (i<data->getIndexCount()) \
    { \
      data->head->skip(i,0) = 1; \
    } \
    data->scan_index[i] = -1; \
    data->head->forward(i,0) = data->tail; \
CSBIDI(data->tail->backward(i,0) = data->head,); \
  } \
 \
  data->level = 0; \
  data->items = 0; \
CSLEVEL(data->head->level = 0;,) \
CSLEVEL(data->tail->level = 0;,) \
}

#define CSXDefineClear2 \
void clear() \
{ \
  node_type *t1 = data.head->forward(0,0); \
  while((t1)&&(t1!=data.tail)) \
  { \
    node_type *t2 = t1->forward(0,0); \
    data.Free(t1); \
    t1 = t2; \
  } \
 \
  for (unsigned int i=0;i<data.getListCount();i++) \
  { \
    if (i<data.getIndexCount()) \
    { \
      data.head->skip(i,0) = 1; \
/*      data.scan_index[i] = -1; */ \
    } \
    data.scan_index[i] = -1; \
    data.head->forward(i,0) = data.tail; \
CSBIDI(data.tail->backward(i,0) = data.head,); \
  } \
 \
  data.level = 0; \
  data.items = 0; \
CSLEVEL(data.head->level = 0;,) \
CSLEVEL(data.tail->level = 0;,) \
}

#define CSXDefineDestroy2 \
void destroy() \
{ \
  node_type *t1 = data.head->forward(0,0); \
  while((t1)&&(t1!=data.tail)) \
  { \
    node_type *t2 = t1->forward(0,0); \
    delete t1->object; \
    data.Free(t1); \
    t1 = t2; \
  } \
 \
  for (unsigned int i=0;i<data.getListCount();i++) \
  { \
    if (i<data.getIndexCount()) \
    { \
      data.head->skip(i,0) = 1; \
/*      data.scan_index[i] = -1; */ \
    } \
    data.scan_index[i] = -1; \
    data.head->forward(i,0) = data.tail; \
CSBIDI(data.tail->backward(i,0) = data.head,); \
  } \
 \
  data.level = 0; \
  data.items = 0; \
CSLEVEL(data.head->level = 0;,) \
CSLEVEL(data.tail->level = 0;,) \
}

#define CSXDefineDestroy \
void destroy() \
{ \
  node_type *t1 = data->head->forward(0,0); \
  while((t1)&&(t1!=data->tail)) \
  { \
    node_type *t2 = t1->forward(0,0); \
    delete t1->object; \
    data->Free(t1); \
    t1 = t2; \
  } \
 \
  for (unsigned int i=0;i<data->getListCount();i++) \
  { \
    if (i<data->getIndexCount()) \
    { \
      data->head->skip(i,0) = 1; \
/*      data.scan_index[i] = -1; */ \
    } \
    data->scan_index[i] = -1; \
    data->head->forward(i,0) = data->tail; \
CSBIDI(data->tail->backward(i,0) = data->head,); \
  } \
 \
  data->level = 0; \
  data->items = 0; \
CSLEVEL(data->head->level = 0;,) \
CSLEVEL(data->tail->level = 0;,) \
}

// For XMultiAutoSkipList and XMultiSkipList
#define CSXDefineFind \
iterator find(const key_type& keyval) \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos += cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos += cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  if ((cursor==data->tail)||(KeyComp(keyval,key(cursor->object)))) \
  { \
    /* Match not found. */ \
    return end(); \
  } \
 \
  return CSINDEX(iterator(data,cursor,pos),iterator(data,cursor)); \
} \
 \
const_iterator find(const key_type& keyval) const \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos += cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos += cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  if ((cursor==data->tail)||(KeyComp(keyval,key(cursor->object)))) \
  { \
    /* Match not found. */ \
    return end(); \
  } \
 \
  return CSINDEX(const_iterator(data,cursor,pos),const_iterator(data,cursor)); \
}

// For XMultiAutoSkipList and XMultiSkipList
#define CSXDefineLowerBound \
iterator lower_bound(const key_type& keyval) \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos+=cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  return CSINDEX(iterator(data,cursor,pos),iterator(data,cursor)); \
} \
 \
const_iterator lower_bound(const key_type& keyval) const \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos+=cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  return CSINDEX(const_iterator(data,cursor,pos),const_iterator(data,cursor)); \
}

#define CSXDefineUpperBound \
iterator upper_bound(const key_type& keyval) \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(!KeyComp(keyval,key(node1->object)))) \
    { \
CSINDEX(pos += cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  return CSINDEX(iterator(data,cursor,pos),iterator(data,cursor)); \
} \
 \
const_iterator upper_bound(const key_type& keyval) const \
{ \
  node_type *cursor = data->head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=data->level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(X,i); \
    while ((node1!=data->tail)&&(!KeyComp(keyval,key(node1->object)))) \
    { \
CSINDEX(pos += cursor->skip(X,i),); \
      cursor = node1; \
      node1 = node1->forward(X,i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(X,0),); \
  cursor = cursor->forward(X,0); \
 \
  return CSINDEX(const_iterator(data,cursor,pos),const_iterator(data,cursor)); \
}

#define CSXDefinePopFront \
void pop_front() \
{ \
  if (data->items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  node_type *cursor = data->head->forward(X,0); \
 \
  data->erase(cursor); \
} \
 \
void destroy_front() \
{ \
  if (data->items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  node_type *cursor = data->head->forward(X,0); \
 \
  mapped_type obj = value(cursor->object); \
 \
  data->erase(cursor); \
 \
  delete obj; \
}

#define CSXDefinePopBackBidi \
void pop_back() \
{ \
  if (data->items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  node_type *cursor = data->tail->backward(X,0); \
 \
  data->erase(cursor); \
} \
 \
void destroy_back() \
{ \
  if (data->items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  node_type *cursor = data->tail->backward(X,0); \
 \
  mapped_type obj = value(cursor->object); \
 \
  data->erase(cursor); \
 \
  delete obj; \
}

#define CSXInitCore(xProbability, xMaxLevel) \
  data.probability = xProbability; \
  data.maxLevel = xMaxLevel; \
  data.tmp_container = new typename data_type::tmp_container_type(xProbability,xMaxLevel); \
  for(unsigned int i=0;i<N;i++) \
  { \
    data.update[i] = new std::pair<size_type,node_type*>[xMaxLevel+1]; \
    data.scan_index[i] = -1; \
  } \
  data.level = 0; \
  data.items = 0; \
 \
  data.head = data.Alloc(xMaxLevel); \
  data.tail = data.Alloc(xMaxLevel); \
 \
  for (unsigned int j=0; j<N; j++) \
  { \
    for (unsigned int i=0; i<=xMaxLevel; i++) \
    { \
CSINDEX(if (j<S) { data.head->skip(j,i) = 1; \
                   data.tail->skip(j,i) = 0;},) \
      data.head->forward(j,i) = data.tail; \
      data.tail->forward(j,i) = NULL; \
CSBIDI(data.tail->backward(j,i) = data.head; \
       data.head->backward(j,i) = NULL;,) \
    } \
  } \
CSLEVEL(data.head->level = 0;,) \
CSLEVEL(data.tail->level = 0;,)


#define CSXDefineInit \
void Init(double xProbability,size_type xMaxLevel) \
{ \
  CSXInitCore(xProbability, xMaxLevel) \
}

#define CSXDefineOperatorEqual \
container_type& operator=(const container_type &source) \
{ \
  if (this==&source) return *this; \
  if (data==source.data) return *this; /* Will not work between delegate containers. */ \
  clear(); \
 \
  for(typename container_type::const_iterator i=source.begin();i!=source.end();++i) \
  { \
    insert(*i); \
  } \
}

#define CSXDefineScanNode \
void scan(node_type *nodex) \
{ \
  data->scan(nodex,X); \
} \
 \
void scanAll(node_type *nodex) \
{ \
  data->scanAll(nodex); \
}

#define CSXDefineScanVal \
void scan_val(const value_type &val) const \
{ \
  value_compare ValueComp = value_comp(); \
 \
  node_type *node = data->head; \
 \
CSINDEX(size_type pos = -1,); \
 \
  for (int i=data->level; i>=0; i--) \
  { \
    node_type *node1 = node->forward(X,i); \
    while ((node1!=data->tail)&&(ValueComp(node1->object,val))) \
    { \
CSINDEX(pos+=node->skip(X,i),); \
      node = node1; \
      node1 = node1->forward(X,i); \
    } \
 \
CSINDEX(data->update[X][i].first = pos,); \
    data->update[X][i].second = node; \
  } \
 \
CSINDEX(data->scan_index[X] = data->update[X][0].first+1,); \
} \
 \
void scanAll_val(const value_type &val) const \
{ \
  scan_val(val); \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
  data->scanAll(cursor); \
}

#define CSXDefineScanKey \
void scan_key(const key_type &val) const \
{\
  key_compare KeyComp = key_comp(); \
 \
  node_type *node = data->head; \
 \
CSINDEX(size_type pos = -1,); \
 \
  for (int i=data->level; i>=0; i--) \
  { \
    node_type *node1 = node->forward(X,i); \
    while ((node1!=data->tail)&&(KeyComp(key(node1->object),val))) \
    { \
CSINDEX(pos+=node->skip(X,i),); \
      node = node1; \
      node1 = node1->forward(X,i); \
    } \
 \
CSINDEX(data->update[X][i].first = pos,); \
    data->update[X][i].second = node; \
  } \
 \
CSINDEX(data->scan_index[X] = data->update[X][0].first+1,); \
} \
 \
void scanAll_key(const key_type &val) const \
{ \
  scan_key(val); \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
  data->scanAll(cursor); \
}

// This sets all update[] arrays.
#define CSXDefineScanAllIndex \
void scanAll(size_type index) const \
{ \
  if (index>data->items) index = data->items; \
  if (data->scan_index[X]==index) \
  { \
    node_type *cursor = data->update[X][0].second->forward(X,0); \
    data->scanAll(cursor); \
    return; \
  } \
 \
  node_type *cursor = data->head; \
 \
  size_type total = -1; \
  size_type tmp_total; \
 \
  for (int i=data->level; i>=0; i--) \
  { \
    while ((tmp_total=total+cursor->skip(X,i))<index) \
    { \
      total=tmp_total; \
      cursor = cursor->forward(X,i); \
    } \
 \
		data->update[X][i].first = total; \
    data->update[X][i].second = cursor; \
  } \
 \
  data->scan_index[X] = index; \
  cursor = cursor->forward(X,0); \
 \
  data->scanAll(cursor); \
}

// This sets current update[] array.
// Only scans current container.
#define CSXDefineScanIndex \
void scan(size_type index) const \
{ \
  if (index>data->items) index = data->items; \
  if (data->scan_index[X]==index) return; \
 \
  node_type *cursor = data->head; \
 \
  size_type total = -1; \
  size_type tmp_total; \
 \
  for (int i=data->level; i>=0; i--) \
  { \
    while ((tmp_total=total+cursor->skip(X,i))<index) \
    { \
      total=tmp_total; \
      cursor = cursor->forward(X,i); \
    } \
 \
    data->update[X][i].first = total; \
    data->update[X][i].second = cursor; \
  } \
 \
  data->scan_index[X] = index; \
}

#define CSXDefineScanIterator \
void scan(const iterator &where) const \
{ \
CSINDEX(scan(where.Findex),scan(where.node,X)); \
}

#define CSXDefineScanAllIterator \
void scanAll(const iterator &where) const \
{ \
CSINDEX(scanAll(where.Findex),scanAll(where.node)); \
}

#define CSXDefineErase \
iterator erase(const iterator &where) \
{ \
  if CSINDEX((where.Findex>=data->items),((where.FNode==data->tail)||(where.FNode==data->head))) return end(); \
 \
  node_type *cursor = where.FNode; \
  node_type *cursor2 = cursor->forward(X,0); \
 \
  data->erase(cursor); \
 \
  return CSINDEX(iterator(data,cursor2,where.Findex),iterator(data,cursor2)); \
} \
 \
iterator destroy(const iterator &where) \
{ \
  mapped_type tmp = value(where.FNode->object); \
  iterator i = erase(where); \
  delete tmp; \
  return i; \
}

/* erasekey */
#define CSXDefineEraseKey \
size_type erase(const key_type &keyval) \
{ \
  scan_key(keyval); \
 \
  if ((data->update[X][0].second==data->tail)||(data->update[X][0].second->forward(X,0)==data->tail)) return 0;  /* Error */ \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  node_type *cursor2 = cursor->forward(X,0); \
  size_type cnt = 0; \
  while((cursor!=data->tail)&&(!key_comp()(keyval,key(cursor->object)))) \
  { \
    data->erase(cursor); \
    cursor = cursor2; \
    cursor2 = cursor2->forward(X,0); \
    cnt++; \
  } \
 \
  return cnt; \
} \
 \
size_type destroy(const key_type &keyval) \
{ \
  scan_key(keyval); \
 \
  if ((data->update[X][0].second==data->tail)||(data->update[X][0].second->forward(X,0)==data->tail)) return 0;  /* Error */ \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  node_type *cursor2 = cursor->forward(X,0); \
  size_type cnt = 0; \
  while((cursor!=data->tail)&&(!key_comp()(keyval,key(cursor->object)))) \
  { \
    mapped_type tmp = value(cursor->object); \
    data->erase(cursor); \
    delete tmp; \
    cursor = cursor2; \
    cursor2 = cursor2->forward(X,0); \
    cnt++; \
  } \
 \
  return cnt; \
} \
 \
size_type erase(const key_type &keyval, iterator &next) \
{ \
  scan_key(keyval); \
 \
  if ((data->update[X][0].second==data->tail)||(data->update[X][0].second->forward(X,0)==data->tail)) return 0;  /* Error */ \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  node_type *cursor2 = cursor->forward(X,0); \
CSINDEX(size_type idx = data->scan_index[X];,) \
  size_type cnt = 0; \
  while((cursor!=data->tail)&&(!key_comp()(keyval,key(cursor->object)))) \
  { \
    data->erase(cursor); \
    cursor = cursor2; \
    cursor2 = cursor2->forward(X,0); \
    cnt++; \
  } \
 \
  next = CSINDEX(iterator(data,cursor,idx+cnt),iterator(data,cursor)); \
  return cnt; \
} \
 \
size_type destroy(const key_type &keyval, iterator &next) \
{ \
  scan_key(keyval); \
 \
  if ((data->update[X][0].second==data->tail)||(data->update[X][0].second->forward(X,0)==data->tail)) return 0;  /* Error */ \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  node_type *cursor2 = cursor->forward(X,0); \
CSINDEX(size_type idx = data->scan_index[X];,) \
  size_type cnt = 0; \
  while((cursor!=data->tail)&&(!key_comp()(keyval,key(cursor->object)))) \
  { \
    mapped_type tmp = value(cursor->object); \
    data->erase(cursor); \
    delete tmp; \
    cursor = cursor2; \
    cursor2 = cursor2->forward(X,0); \
    cnt++; \
  } \
 \
  next = CSINDEX(iterator(data,cursor,idx+cnt),iterator(data,cursor)); \
  return cnt; \
}

// Removes [first,last) and puts it into right.
// Only current layer is used.
// Does not update size or other containers.
// Must be re-inserted into the current layer only.
// right must be empty.
// 'last' is still valid, 'first' is not.
#define CSXDefineCut \
void cut(const iterator &first, const iterator &last, tmp_container_type& right) \
{ \
  if (first==last) return; \
 \
  if (data->level>right.maxLevel) \
    throw level_exception(); \
 \
  difference_type diff = last.Findex-first.Findex; \
 \
  if (data->scan_index[X]!=first.Findex) scan(first.Findex); \
 \
  node_type *cursor = data->update[X][data->level].second; \
 \
  size_type total = -1; \
  size_type tmp_total; \
  right.level = data->level; \
  right.head->level = data->level; \
  right.tail->level = data->level; \
  for (int i=data->level; i>=0; i--) \
  { \
    while ((tmp_total=total+cursor->skip(X,i))<last.Findex) \
    { \
      total=tmp_total; \
      cursor = cursor->forward(X,i); \
    } \
 \
    /* Create the link */ \
    if (data->update[X][i].second==cursor) \
    { \
      cursor->skip(X,i) -= diff; \
      right.head->forward(X,i) = right.tail; \
      right.tail->backward(X,i) = right.head; \
      right.head->skip(X,i) = diff+1; \
    } \
    else \
    { \
      right.head->forward(X,i) = data->update[X][i].second->forward(X,i); \
      right.head->forward(X,i)->backward(X,i) = right.head; \
      right.head->skip(X,i) = data->update[X][i].second->skip(X,i)-(first.Findex-data->update[X][i].first)+1; \
      data->update[X][i].second->forward(X,i) = cursor->forward(X,i); \
      data->update[X][i].second->forward(X,i)->backward(X,i) = data->update[X][i].second; \
      data->update[X][i].second->skip(i) = first.Findex-data->update[X][i].first+cursor->skip(X,i)-(last.Findex-total); \
      cursor->forward(X,i) = right.tail; \
      right.tail->backward(X,i) = cursor; \
      cursor->skip(X,i) = (last.Findex-total); \
    } \
  } \
 \
  last.Findex = first.Findex; \
  right.items = diff; \
  data->items -= diff; \
}

// This just goes through one at a time.
#define CSXDefineEraseITIT \
iterator erase(const iterator &first, const iterator &last) \
{ \
  if (first==last) return last; \
 \
  for(const_iterator i=first;i!=last;) \
  { \
    i = erase(i); \
  } \
 \
CSINDEX(last.Findex = first.Findex,); \
  return last; \
} \
 \
iterator destroy(const iterator &first, const iterator &last) \
{ \
  if (first==last) return last; \
 \
  for(const iterator i=first;i!=last;) \
  { \
    i = destroy(i); \
  } \
 \
CSINDEX(last.Findex = first.Findex,); \
  return last; \
}

#define CSXDefineEraseIndex \
iterator erase_index(size_type index) \
{ \
  if (index>=data->items) return end(); \
 \
  /* Find the node. */ \
  scan(index); \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
  node_type *cursor2 = cursor->forward(X,0); \
 \
  data->erase(cursor); \
 \
  return CSINDEX(iterator(data,cursor2,index),iterator(data,cursor2)); \
} \
 \
iterator destroy_index(size_type index) \
{ \
  if (index>=data->items) return end(); \
 \
  /* Find the node. */ \
  scan(index); \
 \
  node_type *cursor = data->update[X][0].second->forward(X,0); \
  node_type *cursor2 = cursor->forward(X,0); \
 \
  mapped_type tmp = value(cursor->object); \
  data->erase(cursor); \
  delete tmp; \
 \
  return CSINDEX(iterator(data,cursor2,index),iterator(data,cursor2)); \
}

#define CSXDefineOperatorArrayIndex \
mapped_type_reference operator[](size_type index) \
{ \
  return value(*(begin()+index)); \
} \
 \
const_mapped_type_reference operator[](size_type index) const \
{ \
  return value(*(begin()+index)); \
} \
 \
mapped_type_reference at(size_type off) \
{ \
  if (off>data->items) throw std::out_of_range("Out of range."); \
  return value(*(begin()+off)); \
} \
 \
const_mapped_type_reference at(size_type off) const \
{ \
  if (off>data->items) throw std::out_of_range("Out of range."); \
  return value(*(begin()+off)); \
}

#define CSXDefineResize \
void resize(size_type newsize) \
{ \
  if (newsize<data->items) \
  { \
    size_type sz = data->items-newsize; \
    for(size_type i=0;i<sz;i++) \
    { \
      pop_back(); \
    } \
 \
    return; \
  } \
  /* Make it grow. */ \
  CSCreateXItems(value_type,newsize-data->items,push_back) \
} \
 \
void resize(size_type newsize, const value_type &val) \
{ \
  if (newsize<data->items) \
  { \
    resize(newsize); \
    return; \
  } \
  /* Make it grow. */ \
  CSCreateXItemsVal(value_type,val,newsize-data->items,push_back) \
}

#define CSXDefinePushFront \
void push_front(const value_type &val) \
{ \
  data->scan_index[X] = -1; \
 \
  unsigned int newLevel = data->GenerateRandomLevel(); \
 \
  if (newLevel > data->level) \
  { \
    for (unsigned int j=0; j<data->getListCount(); j++) \
    { \
      for (unsigned int i=data->level+1;i<=newLevel;i++) \
      { \
        if (j<data->getIndexCount()) data->head->skip(j,i) = data->items+1; \
        data->head->forward(j,i) = data->tail; \
        data->tail->backward(j,i) = data->head; \
        data->update[j][i].first = -1; \
        data->update[j][i].second = data->head; \
      } \
      if (j<data->getIndexCount()) data->scan_index[j] = -1; \
    } \
    data->level = newLevel; \
    data->head->level = newLevel; \
    data->tail->level = newLevel; \
  } \
 \
  node_type *cursor = data->Alloc(newLevel, val); \
 \
  node_type *back; \
 \
  /* Put new node into place on first level. */ \
  unsigned int i=0; \
  for(;i<=newLevel;i++) \
  { \
    back = data->head->forward(X,i); \
 \
    /* Insert our node. */ \
    cursor->backward(X,i) = data->head; \
    cursor->skip(X,i) = data->head->skip(X,i); \
    cursor->forward(X,i) = back; \
    data->head->skip(X,i) = 1; \
    data->head->forward(X,i) = cursor; \
    back->backward(X,i) = cursor; \
  } \
  for(;i<=data->level;i++) \
  { \
    data->head->skip(X,i)++; \
  } \
 \
  data->items++; \
  for(unsigned int i=0;i<data->getListCount();i++) \
  { \
    if (i!=X) data->Containers[i]->insert(cursor); \
  } \
}

#define CSXDefinePushBackBidi \
void push_back(const value_type &val) \
{ \
  unsigned int newLevel = data->GenerateRandomLevel(); \
 \
  if (newLevel > data->level) \
  { \
    for (unsigned int j=0; j<data->getListCount(); j++) \
    { \
      for (unsigned int i=data->level+1;i<=newLevel;i++) \
      { \
        if (j<data->getIndexCount()) data->head->skip(j,i) = data->items+1; \
        data->head->forward(j,i) = data->tail; \
        data->tail->backward(j,i) = data->head; \
        data->update[j][i].first = -1; \
        data->update[j][i].second = data->head; \
      } \
      data->scan_index[j] = -1; \
/*      if (j<data->IndexCount) data->scan_index[j] = -1; */ \
    } \
    data->level = newLevel; \
    data->head->level = newLevel; \
    data->tail->level = newLevel; \
  } \
 \
  node_type *cursor = data->Alloc(newLevel, val); \
 \
  data->items++; \
  for(unsigned int i=0;i<data->getListCount();i++) \
  { \
    data->Containers[i]->insert(cursor); \
  } \
}

#define CSXDefineInsertBidiIdxNode \
void insert(node_type *node) \
{ \
  /* Put new node into place on first level. */ \
  unsigned int i=0; \
  for(;i<=node->level;i++) \
  { \
    /* Insert our node. */ \
    node->forward(X,i) = data->tail; \
    node->skip(X,i) = 1; \
    node->backward(X,i) = data->tail->backward(X,i); \
    data->tail->backward(X,i)->forward(X,i) = node; \
    data->tail->backward(X,i) = node; \
  } \
  for(;i<=data->level;i++) \
  { \
    data->tail->backward(X,i)->skip(X,i)++; \
  } \
  data->scan_index[X] = -1; \
}

#define CSXDefineInsertVal \
iterator insert(const value_type &val) \
{ \
  unsigned int newLevel = data->GenerateRandomLevel(); \
 \
  if (newLevel > data->level) \
  { \
    for (unsigned int j=0; j<data->getListCount(); j++) \
    { \
      for (unsigned int i=data->level+1;i<=newLevel;i++) \
      { \
        if (j<data->getIndexCount()) data->head->skip(j,i) = data->items+1; \
        data->head->forward(j,i) = data->tail; \
        data->tail->backward(j,i) = data->head; \
        data->update[j][i].second = data->head; \
      } \
      if (j<data->getIndexCount()) data->scan_index[j] = -1; \
    } \
    data->level = newLevel; \
    data->head->level = newLevel; \
    data->tail->level = newLevel; \
  } \
 \
  node_type *cursor = data->Alloc(newLevel, val); \
 \
  data->items++; \
  for(unsigned int i=0;i<data->getListCount();i++) \
  { \
    data->Containers[i]->insert(cursor); \
  } \
  return CSINDEX(iterator(data,cursor,data->scan_index[X]),iterator(data,cursor)); \
}

/* insertx */
#define CSXDefineInsertBidiNodeKey \
void insert(node_type *node) \
{ \
  scan_key(key(node->object)); \
  /* Put new node into place on first level. */ \
  unsigned int i=0; \
  for(;i<=node->level;i++) \
  { \
    /* Insert our node. */ \
    node->backward(X,i) = data->update[X][i].second; \
    node->forward(X,i) = data->update[X][i].second->forward(X,i); \
CSINDEX(node->skip(X,i) = data->update[X][i].second->skip(X,i)-(data->scan_index[X]-data->update[X][i].first-1),); \
    node->forward(X,i)->backward(X,i) = node; \
    data->update[X][i].second->forward(X,i) = node; \
CSINDEX(data->update[X][i].second->skip(X,i) = data->scan_index[X]-data->update[X][i].first,); \
  } \
CSINDEX(for(;i<=data->level;i++) \
  { \
    data->update[X][i].second->skip(X,i)++; \
  },) \
}

// This should be updated to advance update[] to the newly inserted value.
// That way, we can keep inserting just before 'where'.
#define CSXDefineInsertITVal \
iterator insert(const iterator &where, const value_type& val) \
{ \
  unsigned int newLevel = data->GenerateRandomLevel(); \
 \
  if (newLevel > data->level) \
  { \
    for (unsigned int j=0; j<data->getListCount(); j++) \
    { \
      for (unsigned int i=data->level+1;i<=newLevel;i++) \
      { \
        if (j<data->getIndexCount()) data->head->skip(j,i) = data->items+1; \
        data->head->forward(j,i) = data->tail; \
        data->tail->backward(j,i) = data->head; \
        data->update[j][i].second = data->head; \
      } \
      if (j<data->getIndexCount()) data->scan_index[j] = -1; \
    } \
    data->level = newLevel; \
    data->head->level = newLevel; \
    data->tail->level = newLevel; \
  } \
 \
  node_type *cursor = data->Alloc(newLevel, val); \
 \
  if (data->scan_index[X]!=where.Findex) scan(where.Findex); \
 \
  unsigned int i = 0; \
  for (; i<=newLevel; i++) \
  { \
    cursor->backward(X,i) = data->update[X][i].second; \
    cursor->forward(X,i) = data->update[X][i].second->forward(X,i); \
    cursor->skip(X,i) = data->update[X][i].second->skip(X,i)-(where.Findex-data->update[X][i].first-1); \
    cursor->forward(X,i)->backward(X,i) = cursor; \
    data->update[X][i].second->forward(X,i) = cursor; \
    data->update[X][i].second->skip(X,i) = where.Findex-data->update[X][i].first; \
  } \
  for(;i<=data->level;i++) \
  { \
    data->update[X][i].second->skip(X,i)++; \
  } \
  data->items++; \
 \
  /* Add node. */ \
  for(unsigned int i=0;i<data->getListCount();i++) \
  { \
    if (i!=X) data->Containers[i]->insert(cursor); \
  } \
 \
  where.Findex++; \
  return iterator(data,cursor,where.Findex-1); \
}

#define CSXXInsertITSizeVal \
  for(size_type i=0;i<count;++i) \
  { \
    insert(where, val); \
  }

#define CSXDefineInsertITSizeVal \
void insert(const iterator &where, size_type count, const T& val) \
{ \
  CSXXInsertITSizeVal \
} \
 \
void insert(const iterator &where, int count, const T& val) \
{ \
  CSXXInsertITSizeVal \
}

#define CSXDefineInsertITITIT \
template<class InIt> void insert(const iterator &where, InIt first, InIt last) \
{ \
  if (first==last) return; \
 \
  for(InIt i=first;i!=last;++i) \
  { \
    insert(where, *i); \
  } \
}

// Swaps nodes, but should be fixed to re-insert nodes in sequence.
#define CSXDefineReverse \
void reverse() \
{ \
  iterator s = begin(); \
  iterator e = end()-1; \
  while(e>s) \
  { \
    /* Swap the nodes. */ \
    swap(s,e); \
    std::swap(s,e); \
    ++s; \
    --e; \
  } \
}

#define CSXDefineUnique \
void unique() \
{ \
  iterator i = begin(); \
 \
  size_type limit = data->items; \
  if (limit==0) return; \
  for(size_type j=0;j<limit-1;j++) \
  { \
    if (*i==*(i+1)) \
    { \
      erase(i+1); \
    } \
    else \
    { \
      ++i; \
    } \
  } \
}

#define CSXDefineUniquePr \
template<class Pr2> void unique(Pr2 pred) \
{ \
  iterator i = begin(); \
 \
  size_type limit = data->items; \
  if (limit==0) return; \
  for(size_type j=0;j<limit-1;j++) \
  { \
    if (pred(*i,*(i+1))) \
    { \
      erase(i+1); \
    } \
    else \
    { \
      ++i; \
    } \
  } \
}

#define CSXDefineSort \
void sort() \
{ \
  CS::sort<X,container_type>(begin(),end()); \
}

#define CSXDefineSortPr \
template<class Pr3> void sort(Pr3 pred) \
{ \
  CS::sort<X,container_type,Pr3>(begin(),end(),pred); \
}

#define CSXDefineStableSort \
void stable_sort() \
{ \
  CS::stable_sort<X,container_type>(begin(),end()); \
}

#define CSXDefineStableSortPr \
template<class Pr3> void stable_sort(Pr3 pred) \
{ \
  CS::stable_sort<X,container_type,Pr3>(begin(),end(),pred); \
}

// Elements of rigth are put back into current container before 'where'.
#define CSXDefineSplice \
void splice(const iterator &where, tmp_container_type& right) \
{ \
  if (right.level>data->maxLevel) \
    throw level_exception(); \
 \
  if (data->scan_index[X]!=where.Findex) scan(where.Findex); \
 \
  /* Add list into here. */ \
  /* Levels should be the same, so this should never execute, but it does no harm. */ \
  for(unsigned int i=data->level+1;i<=right.level;i++) \
  { \
    data->head->level = i; \
    data->head->skip(X,i) = data->items+1; \
    data->head->forward(X,i) = data->tail; \
    data->tail->backward(X,i) = data->head; \
    data->tail->level = i; \
    data->update[X][i].first = -1; \
    data->update[X][i].second = data->head; \
  } \
 \
  unsigned int i=0; \
  for(;i<=right.level;i++) \
  { \
    if (right.tail->backward(X,i)==right.head) break; \
 \
    right.tail->backward(X,i)->forward(X,i) = data->update[X][i].second->forward(X,i); \
    right.tail->backward(X,i)->skip(X,i) = data->update[X][i].first+data->update[X][i].second->skip(X,i)-where.Findex; \
    data->update[X][i].second->forward(X,i)->backward(X,i) = right.tail->backward(X,i); \
 \
    data->update[X][i].second->forward(X,i) = right.head->forward(X,i); \
    data->update[X][i].second->forward(X,i)->backward(X,i) = data->update[X][i].second; \
    data->update[X][i].second->skip(X,i) = where.Findex-data->update[X][i].first+right.head->forward(X,i)-1; \
  } \
  for(;i<level;i++) \
  { \
    data->update[X][i].second->skip(i)+=right.size(); \
  } \
 \
  data->items+=right.size(); \
  where.Findex+=right.size(); \
 \
  right.head->forward(X,0) = right.tail; \
  right.tail->backward(X,0) = right.head; \
  right.head->skip(X,0) = 1; \
  right.head->level = 0; \
  right.tail->level = 0; \
  right.level = 0; \
  right.items = 0; \
}

}

#endif
