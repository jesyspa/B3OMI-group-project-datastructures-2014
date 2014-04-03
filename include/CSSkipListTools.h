/*
   Description: Header file for MACROS and other tools used in skiplists.

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
#ifndef CSSkipListToolsH
#define CSSkipListToolsH

#include <exception>
#include <stddef.h>
#include <math.h>
#include <iterator>
#include "CSRNG.h"

namespace CS
{

template<class K, class T>
struct XAccessSelf
{
  const K& operator()(const T &item) { return item; }
};

template <class T>
class TWrap
{
public:
  T value;
  TWrap() {}
  TWrap(const T &val): value(val) {}
  operator T() const { return value; }
  bool operator<(const TWrap &right) { return value<right.value; }
  bool operator<(const T &right) { return value<right; }
  bool operator==(const TWrap &right) { return value==right.value; }
  bool operator==(const T &right) { return value==right; }
};

class level_exception : public std::exception
{
public:
  const char *msg;
  level_exception() : std::exception(), msg("Incompatible levels") {}
  virtual const char *what() const throw() {return msg;}
};

class container_exception : public std::exception
{
public:
  const char *msg;
  container_exception() : std::exception(), msg("Iterators not from same container.") {}
  virtual const char *what() const throw() {return msg;}
};

// These are the four possible node types.
//
// ForwardNode
// BidiNode
// ForwardIdxNode
// BidiIdxNode

// Clears forward pointers.
#define CSClearNodesForward \
  { \
    for(unsigned int i=0;i<level+1;i++) \
    { \
      forward(i) = NULL; \
    } \
  }

// Clears forward and index pointers.
#define CSClearNodesForwardIdx \
  { \
    for(unsigned int i=0;i<level+1;i++) \
    { \
      skip(i) = 0; \
      forward(i) = NULL; \
    } \
  }

// Clears forward and backward pointers.
#define CSClearNodesBidi \
  { \
    for(unsigned int i=0;i<level+1;i++) \
    { \
      forward(i) = NULL; \
      backward(i) = NULL; \
    } \
  }

// Clears forward, backward and index pointers.
#define CSClearNodesBidiIdx \
  { \
    for(unsigned int i=0;i<level+1;i++) \
    { \
      skip(i) = 0; \
      forward(i) = NULL; \
      backward(i) = NULL; \
    } \
  }

#if 1
#define NCSClearNodesBidiIdx(N,S) \
  { \
  }

#else
#define NCSClearNodesBidiIdx(N,S) \
  { \
    for(unsigned int j=0;j<N;j++) \
    { \
      for(unsigned int i=0;i<level+1;i++) \
      { \
        if (j<S) skip(j,i) = 0; \
        forward(j,i) = NULL; \
        backward(j,i) = NULL; \
      } \
    } \
  }
#endif

// Contains forward pointers only.
template <class T>
class ForwardTinyNode
{
public:
  typedef size_t size_type;
  typedef ForwardTinyNode<T>* ptr_type;

  T object; //!< Object associated with the key.
  ptr_type pointers[1];
  ForwardTinyNode<T>*& forward(unsigned int level) {return pointers[level];}
  ForwardTinyNode<T>* forward(unsigned int level) const {return pointers[level];}
  ForwardTinyNode(unsigned int level, const T &obj) : object(obj) CSClearNodesForward
  explicit ForwardTinyNode(unsigned int level) CSClearNodesForward
};

// Contains forward pointers only.
template <class T>
class ForwardNode
{
public:
  typedef size_t size_type;
  typedef ForwardNode<T>* ptr_type;

  T object; //!< Object associated with the key.
  unsigned int level; //!< how many forward and backward pointers there are.
  ptr_type pointers[1];
  ForwardNode<T>*& forward(unsigned int level) {return pointers[level];}
  ForwardNode<T>* forward(unsigned int level) const {return pointers[level];}
  ForwardNode(unsigned int level, const T &obj) : level(level), object(obj) CSClearNodesForward
  explicit ForwardNode(unsigned int level) : level(level) CSClearNodesForward
};


// Contains forward and index pointers only.
template <class T>
class ForwardIdxTinyNode
{
public:
  typedef size_t size_type;
  struct Pointers
  {
    size_type skip;
    ForwardIdxTinyNode<T> *forward;
  };
  typedef Pointers ptr_type;

  T object; //!< Object associated with the key.
  ptr_type pointers[1];
  ForwardIdxTinyNode<T>*& forward(unsigned int level) {return pointers[level].forward;}
  size_type& skip(unsigned int level) {return pointers[level].skip;}
  ForwardIdxTinyNode<T>* forward(unsigned int level) const {return pointers[level].forward;}
  size_type skip(unsigned int level) const {return pointers[level].skip;}
  ForwardIdxTinyNode(unsigned int level, const T &obj) : object(obj) CSClearNodesForwardIdx
  explicit ForwardIdxTinyNode(unsigned int level) CSClearNodesForwardIdx
};

// Contains forward and index pointers only.
template <class T>
class ForwardIdxNode
{
public:
  typedef size_t size_type;
  struct Pointers
  {
    size_type skip;
    ForwardIdxNode<T> *forward;
  };
  typedef Pointers ptr_type;

  T object; //!< Object associated with the key.
  unsigned int level; //!< how many forward and backward pointers there are.
  ptr_type pointers[1];
  ForwardIdxNode<T>*& forward(unsigned int level) {return pointers[level].forward;}
  size_type& skip(unsigned int level) {return pointers[level].skip;}
  ForwardIdxNode<T>* forward(unsigned int level) const {return pointers[level].forward;}
  size_type skip(unsigned int level) const {return pointers[level].skip;}
  ForwardIdxNode(unsigned int level, const T &obj) : level(level), object(obj) CSClearNodesForwardIdx
  explicit ForwardIdxNode(unsigned int level) : level(level) CSClearNodesForwardIdx
};

// Contains forward and backward pointers only.
template <class T>
class BidiNode
{
public:
  typedef size_t size_type;
  struct Pointers
  {
    BidiNode<T> *forward;
    BidiNode<T> *backward;
  };
  typedef Pointers ptr_type;

  T object; //!< Object associated with the key.
  unsigned int level; //!< how many forward and backward pointers there are.
  ptr_type pointers[1];
  BidiNode<T>*& forward(unsigned int level) {return pointers[level].forward;}
  BidiNode<T>*& backward(unsigned int level) {return pointers[level].backward;}
  BidiNode<T>* forward(unsigned int level) const {return pointers[level].forward;}
  BidiNode<T>* backward(unsigned int level) const {return pointers[level].backward;}
  BidiNode(unsigned int level, const T &obj) : level(level), object(obj) CSClearNodesBidi
  explicit BidiNode(unsigned int level) : level(level) CSClearNodesBidi
};

// Contains forward, backward and index pointers.
template <class T>
class BidiIdxNode
{
public:
  typedef size_t size_type;
  struct Pointers
  {
    size_type skip;
    BidiIdxNode<T> *forward;
    BidiIdxNode<T> *backward;
  };
  typedef Pointers ptr_type;

  T object; //!< Object associated with the key.
  unsigned int level; //!< how many forward and backward pointers there are.
  ptr_type pointers[1];
  BidiIdxNode<T>*& forward(unsigned int level) {return pointers[level].forward;}
  BidiIdxNode<T>*& backward(unsigned int level) {return pointers[level].backward;}
  size_type& skip(unsigned int level) {return pointers[level].skip;}
  BidiIdxNode<T>* forward(unsigned int level) const {return pointers[level].forward;}
  BidiIdxNode<T>* backward(unsigned int level) const {return pointers[level].backward;}
  size_type skip(unsigned int level) const {return pointers[level].skip;}
  BidiIdxNode(unsigned int level, const T &obj) : level(level), object(obj) CSClearNodesBidiIdx
  BidiIdxNode(unsigned int level) : level(level) CSClearNodesBidiIdx
};

// Allocates node
// level is number of pointer levels.
// obj is the entity to copy into the node.
// T is the type of the node (ForwardNode, ForwardIdxNode, BidiNode and BidiIdxNode).
#define CSAlloc2(level, obj, T) \
{ \
  char *ptr = new char[sizeof(T)+level*(sizeof(typename T::ptr_type))]; \
  T *item = new (ptr) T(level,obj); \
  return item; \
}

// Allocates dummy node.
// level is number of pointer levels.
// T is the type of the node (ForwardNode, ForwardIdxNode, BidiNode and BidiIdxNode).
#define CSAlloc(level, T) \
{ \
  char *ptr = new char[sizeof(T)+level*(sizeof(typename T::ptr_type))]; \
  T *item = new (ptr) T(level); \
  return item; \
}

// Free a node.
#define CSFree(item, Tp) \
{ \
  item->Tp::~Tp(); \
  char *ptr = reinterpret_cast<char*>(item); \
  delete[] ptr; \
}

// Defines operators == and != for iterators.
// Works for all iterators.
// Doesn't require index to be valid.
#define CSDefineIteratorEQ(it) \
    bool operator==(const it& other) const \
    { \
      return(node == other.node); \
    } \
 \
    bool operator!=(const it& other) const \
    { \
      return !(node == other.node); \
    }

// Defines preincrement operator++
// Works for node types with index pointers.
// Index is incremented, but isn't required to be valid.
//   In that case, index will remain invalid.
#define CSDefineIteratorInc(it) \
    it& operator++() \
    { \
      node_type* node1; \
      if ((node != NULL)&&((node1 = node->forward(0)) != NULL)) \
      { \
        node = node1; \
CSINDEX(Findex++,); \
      } \
      return *this; \
    } \
 \
 CSDefineIteratorIncPost(it)

// Defines postincrement ++ operator.
// Works for all iterators.
// Automatically included by previous ++ definitions.
// Index is incremented, but isn't required to be valid.
//   In that case, index will remain invalid.
// DO NOT USE MACRO DIRECTLY EXCEPT IN OTHER MACROS DEFINING ++  !!!
#define CSDefineIteratorIncPost(it) \
    it operator++(int) \
    { \
      it tmp(*this); \
      ++(*this); \
      return tmp; \
    }

// Defines preincrement operator--
// Works for node types with index poniters.
// Index is decremented, but isn't required to be valid.
//   In that case, index will remain invalid.
// DO NOT USE MACRO DIRECTLY EXCEPT IN OTHER MACROS DEFINING ++  !!!
#define CSDefineIteratorDecBidi(it) \
    it& operator--() \
    { \
      node_type* node1; \
      node_type* node2; \
      if ((node != NULL)&&((node1 = node->backward(0)) != NULL)&&((node2 = node1->backward(0)) != NULL)) \
      { \
        node = node1; \
CSINDEX(Findex--,); \
      } \
      return *this; \
    } \
 \
 CSDefineIteratorDecPost(it)

#define CSDefineIteratorDecForwardIdx(it) \
    it& operator--() \
    { \
      *this = container->begin()+(Findex-1); \
      return *this; \
    } \
 CSDefineIteratorDecPost(it)


#define CSDefineIteratorDecForward(it) \
  it& operator--() \
  { \
    container_type::value_compare ValueComp = container->value_comp(); \
 \
    node_type *cursor = container->head; \
 \
    for(int i=container->level;i>=0;i--) \
    { \
      node_type *node1 = cursor->forward(i); \
      while ((node1!=container->tail)&&((node==container->tail)||(ValueComp(node1->object,node->object)))) \
      { \
        cursor = node1; \
        node1 = node1->forward(i); \
      } \
    } \
 \
CSUNIQUE(,while (cursor->forward(0) != node) cursor = cursor->forward(0);) \
 \
    if (cursor==container->head) \
    { \
      cursor = cursor->forward(0); \
    } \
 \
    node = cursor; \
    return *this; \
  } \
 CSDefineIteratorDecPost(it)

// Defines postdecrement -- operator.
// Works for all iterators.
// Automatically included by previous -- definitions.
// Index is decremented, but isn't required to be valid.
//   In that case, index will remain invalid.
// DO NOT USE MACRO DIRECTLY EXCEPT IN OTHER MACROS DEFINING --  !!!
#define CSDefineIteratorDecPost(it) \
    it operator--(int) \
    { \
      it tmp(*this); \
      --(*this); \
      return tmp; \
    }

// Defines operator* and operator->
// Works for all non-const iterators.
// Doesn't require index to be valid.
#define CSDefineIteratorAccess \
    reference operator*() const \
    { \
      return node->object; \
    } \
 \
    pointer operator->() const \
    { \
      return &node->object; \
    }

// Defines default comparison operators >, <= and >=
// operator<() is defined separatly.
// Works for all iterators.
// Index validity requirement depends on operator<().
//   If the container is an indexed only container, then index is required to be valid.
#define CSDefineIteratorDefaultCompare(it) \
    bool operator<=(const it &other) const \
    { \
      return !(other<*this); \
    } \
 \
    bool operator>(const it &other) const \
    { \
      return (other<*this); \
    } \
 \
    bool operator>=(const it &other) const \
    { \
      return !(*this<other); \
    }


// Defines operator<() for all node types.
// If the node does not have a key (IndexedSkipList), then compare index.
// If the node does have a key, then the following possibilities exist.
// If the key is unique, then compare by key.
// Otherwise, check if the node has an index (Auto type skiplist).
// If it does have an index, then compare index.
// Otherwise, compare by key, but will take O(k) operations where k is the amount
// of keys that are identical.
#define CSDefineIteratorLTNodeCompare(it) \
  CSKEY( \
  CSUNIQUE(CSDefineIteratorLTNodeKeyCompare(it), \
           CSINDEX(CSDefineIteratorLTNodeIndexCompare(it), \
           CSDefineIteratorLTNodeMultiKeyCompare(it))), \
  CSDefineIteratorLTNodeIndexCompare(it)) \

#define CSDefineIteratorLTNodeKeyCompare(it) \
    bool operator<(const it &other) const \
    { \
      bool a = (node->forward(0)==NULL); \
      bool b = (other.node->forward(0)==NULL); \
      if (a||b) \
      { \
        if (a) return false; \
        return true; \
      } \
 \
      return container->value_comp()(node->object,other.node->object); \
    }

#define CSDefineIteratorLTNodeIndexCompare(it) \
    bool operator<(const it &other) const \
    { \
      return (Findex<other.Findex); \
    }

// Compares two iterators via nodes.
// Supports comparison by multikey.
#define CSDefineIteratorLTNodeMultiKeyCompare(it) \
    bool operator<(const it &other) const \
    { \
      bool a = (node->forward(0)==NULL); \
      bool b = (other.node->forward(0)==NULL); \
      if (a||b) \
      { \
        if (a) return false; \
        return true; \
      } \
 \
      if (container->value_comp()(node->object,other.node->object)) return true; \
      if (container->value_comp()(other.node->object,node->object)) return false; \
 \
      /* keys are equal.  Scan until *this reaches other or until *this<node. */ \
      node_type *node1 = node->forward(0); \
      node_type *node2 = node1->forward(0); \
      for(;;) \
      { \
        if (other.node==node1) return true; /* reached other node. */ \
        if (node2==NULL) return false; /* reached end(). */ \
        if (container->value_comp()(node->object,node1->object)) return false; /* reached non-equal element */ \
        node1 = node2; \
        node2 = node2->forward(0); \
      } \
    }

#define CSDefineIteratorRefreshBidi \
    size_type refresh() \
    { \
      if ((node==NULL)||(node->backward(0)==NULL)) \
      { \
        Findex = -1; \
        return Findex; \
      } \
      Findex = 0; \
      node_type* node1=node->backward(0); \
 \
      unsigned int level = 0; \
      while(node1->backward(level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        node1 = node1->backward(level); \
        Findex+=node1->skip(level); \
      } \
      return Findex; \
    }

#define CSDefineIteratorRefreshIdx \
    size_type refresh() \
    { \
      /* Reset the index */ \
      if (node==NULL) \
      { \
        Findex = -1; \
        return Findex; \
      } \
 \
      node_type *node1 = node; \
      size_type cnt = 0; \
      while(node1!=container->tail) \
      { \
        cnt+=node1->skip(node1->level); \
        node1 = node1->forward(node1->level); \
      } \
 \
      Findex = container->size()-cnt; \
      return Findex; \
    }


#define CSDefineIteratorPlusMinus(T) \
    T operator+(difference_type off) const \
    { \
      T tmp(*this); \
      tmp+=off; \
      return tmp; \
    } \
 \
    T operator-(difference_type off) const \
    { \
      T tmp(*this); \
      tmp-=off; \
      return tmp; \
    }

#define CSDefineIteratorDiff(it) \
    difference_type operator-(const it &other) const \
    { \
      return Findex-other.Findex; \
    } \

#define CSDefineIteratorArray(it) \
    mapped_type_reference operator[](difference_type off) const \
    { \
      it tmp(*this); \
      tmp+=off; \
      return container->value(*tmp); \
    }

#define CSDefineConstIteratorArray(it) \
    const_mapped_type_reference operator[](difference_type off) const \
    { \
      it tmp(*this); \
      tmp+=off; \
      return container->value(*tmp); \
    }

#define CSDefineIteratorIndex \
    size_type getIndex() const \
    { \
      return Findex; \
    }

// Requires container to be defined.
#define CSDefineIteratorForwardArbitraryAccess(it) \
    it& operator+=(difference_type off) \
    { \
      if (off==0) return *this; \
 CSLEVEL(if (off<0) return operator-=(-off),); \
      if (off==1) return operator++(); \
      /* This will use head which has more levels and */ \
      /* will advance much quicker. */ \
CSLEVEL( \
      if ((off>1)&&(Findex==0)) \
      { \
        node = container->head; \
        off++; \
        Findex--; \
      } \
      /* Go forward. */ \
      size_type adv = 0; \
      unsigned int level = node->level; \
      node_type* node1=node; \
      /* Try going up in levels if possible. */ \
      while(node1->forward(level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        if (adv+node1->skip(level)>off) break; /* Start going down in levels. */ \
        adv+=node1->skip(level); \
        node1 = node1->forward(level); \
      } \
      /* Try going down in levels. */ \
      while(node1->forward(level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->skip(level)>off)) \
        { \
          level--; \
        } \
        if (adv+node1->skip(level)>off) break; /* DONE! */ \
        adv+=node1->skip(level); \
        node1 = node1->forward(level); \
      } \
      /* We're at the correct node. */ \
      node = node1; \
      Findex+=adv; \
      \
, \
      \
      difference_type newIndex = Findex+off; \
      if (newIndex<0) \
      { \
        Findex = 0; \
        node = container->head->forward(0); \
        return *this; \
      } \
      if (newIndex>=container->size()) \
      { \
        Findex = container->size(); \
        node = container->tail; \
        return *this; \
      } \
 \
      Findex = newIndex; \
      node = container->head; \
      difference_type adv = -1; \
      node_type* node1=node; \
      unsigned int level = container->level; \
      while(node1->forward(level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->skip(level)>newIndex)) \
        { \
          level--; \
        } \
        if (adv+node1->skip(level)>newIndex) break; /* DONE! */ \
        adv+=node1->skip(level); \
        node1 = node1->forward(level); \
      } \
      /* We're at the correct node. */ \
      node = node1;) \
      return *this; \
    }

// requires backward()
#define CSDefineIteratorReverseArbitraryAccess(it) \
    it& operator-=(difference_type off) \
    { \
      if (off==0) return *this; \
      if (off<0) return operator+=(-off); \
      if (off==1) return operator--(); \
 \
      /* Go backward. */ \
      size_type adv = 0; \
      unsigned int level = node->level; \
      node_type* node1=node; \
      /* Try going up in levels if possible.*/ \
      while(node1->backward(level)!=NULL) \
      { \
        if (node1->level>level) \
        { \
          level = node1->level; \
        } \
        if (adv+node1->backward(level)->skip(level)>off) break; /* Start going down in levels. */ \
        adv+=node1->backward(level)->skip(level); \
        node1 = node1->backward(level); \
      } \
      /* Try going down in levels. */ \
      while(node1->backward(level)!=NULL) \
      { \
        while ((level!=0)&&(adv+node1->backward(level)->skip(level)>off)) \
        { \
          level--; \
        } \
        if (adv+node1->backward(level)->skip(level)>off) break; /* DONE! */ \
        adv+=node1->backward(level)->skip(level); \
        node1 = node1->backward(level); \
      } \
      /* Check that we're not at the dummy node. */ \
      if (node1->backward(0)==NULL) \
      { \
        adv--; \
        node1 = node1->forward(0); \
      } \
      /* We're at the correct node. */ \
      node = node1; \
      Findex-=adv; \
      return *this; \
    }

#define CSDefineIteratorReverseArbitraryAccessIdx(it) \
    it& operator-=(difference_type off) \
    { \
      if (off==0) return *this; \
CSLEVEL(if (off<0) return operator+=(-off); \
      *this = container->begin()+(Findex-off); \
      return *this; \
      , \
 \
      return operator+=(-off);) \
 \
    }


#define CSDefineBeginEnd \
iterator begin() \
{ \
  return CSINDEX(iterator(this,head->forward(0),0),iterator(this,head->forward(0))); \
} \
 \
iterator end() \
{ \
  return CSINDEX(iterator(this,tail,items),iterator(this,tail)); \
} \
 \
const_iterator begin() const \
{ \
  return CSINDEX(const_iterator(this,head->forward(0),0),const_iterator(this,head->forward(0))); \
} \
 \
const_iterator end() const \
{ \
  return CSINDEX(const_iterator(this,tail,items),const_iterator(this,tail)); \
}

#define CSDefineRBeginEnd \
reverse_iterator rbegin() \
{ \
  return reverse_iterator(end()); \
} \
 \
reverse_iterator rend() \
{ \
  return reverse_iterator(begin()); \
} \
 \
const_reverse_iterator rbegin() const \
{ \
  return const_reverse_iterator(end()); \
} \
 \
const_reverse_iterator rend() const \
{ \
  return const_reverse_iterator(begin()); \
}

#define CSDefineGenerateRandomLevel \
unsigned int GenerateRandomLevel() \
{ \
  unsigned int newLevel = 0; \
  while ((newLevel<maxLevel)&&(rng.drand()<probability)) \
  { \
    newLevel++; \
  } \
  return newLevel; \
}

#define CSDefineSize \
size_type size() const \
{ \
  return items; \
}

#define CSDefineEmpty \
bool empty() const \
{ \
  return (items==0); \
}

#define CSDefineFront \
reference front() \
{ \
  return head->forward(0)->object; \
} \
 \
const_reference front() const \
{ \
  return head->forward(0)->object; \
}

#define CSDefineBackBidi \
reference back() \
{ \
  return tail->backward(0)->object; \
} \
 \
const_reference back() const \
{ \
  return tail->backward(0)->object; \
}

#define CSDefineBackForward \
reference back() \
{ \
CSINDEX(if (scan_index==items) return update[0].second->object,); \
CSINDEX(if ((scan_index==items-1)&&(scan_index!=-1)) return update[0].second->forward(0)->object,); \
 \
CSINDEX(scan(items-1),scan(tail)); \
 \
return CSINDEX(update[0].second->forward(0)->object,update[0].second->object); \
} \
 \
const_reference back() const \
{ \
CSINDEX(if (scan_index==items) return update[0].second->object,); \
CSINDEX(if ((scan_index==items-1)&&(scan_index!=-1)) return update[0].second->forward(0)->object,); \
 \
CSINDEX(scan(items-1),scan(tail)); \
 \
  return CSINDEX(update[0].second->forward(0)->object,update[0].second->object); \
}

// Defines other comparison operators for containers based on operator<()
// and operator==()
#define CSDefineCompOps(H,V) \
H \
bool operator<=(const V &left, const V &right) \
{ \
  return !(right < left); \
} \
 \
H \
bool operator>(const V &left, const V &right) \
{ \
  return (right < left); \
} \
 \
H \
bool operator>=(const V &left, const V &right) \
{ \
  return !(left < right); \
} \
 \
H \
bool operator!=(const V &left, const V &right) \
{ \
  return !(left==right); \
}

#define CSDefineKeyCompare(T) \
key_compare key_comp() const { return T; }

#define CSDefineValueCompare(T) \
value_compare value_comp() const { return T; }

#define CSDefineMaxSize \
size_type max_size() const \
{ \
  size_type a = -1; \
  double da = (double)a; \
  da+=1.0; \
  size_type n = sizeof(node_type); \
  double dn = (double)n; \
  double c = probability; \
  double total = 0.0; \
  for(unsigned int i=1;i<=maxLevel;i++) \
  { \
    total+=c; \
    c*=probability; \
  } \
  n = sizeof(typename node_type::ptr_type); \
  dn+=((double)n)*total; \
  da/=dn; \
  return (size_type)da; \
}

#define CSDefineClear \
void clear() \
{ \
  node_type *t1,*t2; \
  t1 = head->forward(0); \
  while((t1)&&(t1!=tail)) \
  { \
    t2 = t1->forward(0); \
    Free(t1); \
    t1 = t2; \
  } \
 \
CSINDEX(head->skip(0) = 1,); \
  head->forward(0) = tail; \
CSBIDI(tail->backward(0) = head,); \
 \
  level = 0; \
  items = 0; \
CSLEVEL(head->level = 0;,) \
CSLEVEL(tail->level = 0;,) \
 \
CSINDEX(scan_index = -1,); \
}

#define CSDefineDestroy \
void destroy() \
{ \
  node_type *t1,*t2; \
  t1 = head->forward(0); \
  while((t1)&&(t1!=tail)) \
  { \
    t2 = t1->forward(0); \
    delete value(t1->object); \
    Free(t1); \
    t1 = t2; \
  } \
 \
CSINDEX(head->skip(0) = 1,); \
  head->forward(0) = tail; \
CSBIDI(tail->backward(0) = head,); \
 \
  level = 0; \
  items = 0; \
CSLEVEL(head->level = 0;,) \
CSLEVEL(tail->level = 0;,) \
 \
CSINDEX(scan_index = -1,); \
}

#define CSDefineFind \
iterator find(const key_type& keyval) \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos += cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos += cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  if ((cursor==tail)||(KeyComp(keyval,key(cursor->object)))) \
  { \
    /* Match not found. */ \
    return end(); \
  } \
 \
  return CSINDEX(iterator(this,cursor,pos),iterator(this,cursor)); \
} \
 \
const_iterator find(const key_type& keyval) const \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos += cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos += cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  if ((cursor==tail)||(KeyComp(keyval,key(cursor->object)))) \
  { \
    /* Match not found. */ \
    return end(); \
  } \
 \
  return CSINDEX(const_iterator(this,cursor,pos),const_iterator(this,cursor)); \
}

#define CSDefineCount \
size_type count(const key_type& keyval) const \
{ \
  const_iterator i = lower_bound(keyval); \
  const_iterator j = upper_bound(keyval); \
CSINDEX(return j-i;, \
  size_type cnt = 0; \
  for(;i!=j;++i,cnt++); \
  return cnt;) \
}

#define CSDefineLowerBound \
iterator lower_bound(const key_type& keyval) \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos+=cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  return CSINDEX(iterator(this,cursor,pos),iterator(this,cursor)); \
} \
 \
const_iterator lower_bound(const key_type& keyval) const \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(KeyComp(key(node1->object),keyval))) \
    { \
CSINDEX(pos+=cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  return CSINDEX(const_iterator(this,cursor,pos),const_iterator(this,cursor)); \
}

#define CSDefineUpperBound \
iterator upper_bound(const key_type& keyval) \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(!KeyComp(keyval,key(node1->object)))) \
    { \
CSINDEX(pos += cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  return CSINDEX(iterator(this,cursor,pos),iterator(this,cursor)); \
} \
 \
const_iterator upper_bound(const key_type& keyval) const \
{ \
  node_type *cursor = head; \
  key_compare KeyComp = key_comp(); \
CSINDEX(size_type pos = -1,); \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(!KeyComp(keyval,key(node1->object)))) \
    { \
CSINDEX(pos += cursor->skip(i),); \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
  } \
 \
CSINDEX(pos+=cursor->skip(0),); \
  cursor = cursor->forward(0); \
 \
  return CSINDEX(const_iterator(this,cursor,pos),const_iterator(this,cursor)); \
}

#define CSDefineEqualRange \
ipair equal_range(const key_type& keyval) \
{ \
  return ipair(lower_bound(keyval),upper_bound(keyval)); \
} \
 \
const_ipair equal_range(const key_type& keyval) const \
{ \
  return const_ipair(lower_bound(keyval),upper_bound(keyval)); \
}

#define CSDefinePopFront \
void pop_front() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
CSINDEX(scan_index = -1,); \
 \
  node_type *cursor = head->forward(0); \
 \
  unsigned int i=0; \
CSLEVEL( \
  for (; i<=cursor->level; i++) \
  { \
CSINDEX(head->skip(i) = cursor->skip(i),); \
    head->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = head,); \
  }, \
  while((i<=level)&&(head->forward(i)==cursor)) \
  { \
CSINDEX(head->skip(i) = cursor->skip(i),); \
    head->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = head,); \
    i++; \
  }) \
CSINDEX(for(;i<=level;i++) \
  { \
    head->skip(i)--; \
  },) \
 \
  Free(cursor); \
  items--; \
  adjust_levels(); \
} \
 \
void destroy_front() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
CSINDEX(scan_index = -1,); \
 \
  node_type *cursor = head->forward(0); \
 \
  unsigned int i=0; \
CSLEVEL( \
  for (; i<=cursor->level; i++) \
  { \
CSINDEX(head->skip(i) = cursor->skip(i),); \
    head->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = head,); \
  }, \
  while((i<=level)&&(head->forward(i)==cursor)) \
  { \
CSINDEX(head->skip(i) = cursor->skip(i),); \
    head->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = head,); \
    i++; \
  }) \
CSINDEX(for(;i<=level;i++) \
  { \
    head->skip(i)--; \
  },) \
 \
  delete value(cursor->object); \
  Free(cursor); \
  items--; \
  adjust_levels(); \
}

#define CSDefinePopBackBidi \
void pop_back() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
CSINDEX(scan_index = -1,); \
 \
  node_type *cursor = tail->backward(0); \
 \
  unsigned int i=0; \
  for (; (i<=level)&&(tail->backward(i) == cursor); i++) \
  { \
    cursor->backward(i)->forward(i) = tail; \
    tail->backward(i) = cursor->backward(i); \
  } \
CSINDEX(for(;i<=level;i++) \
  { \
    tail->backward(i)->skip(i)--; \
  },) \
 \
  Free(cursor); \
  items--; \
  adjust_levels(); \
} \
 \
void destroy_back() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
CSINDEX(scan_index = -1;,) \
 \
  node_type *cursor = tail->backward(0); \
 \
  unsigned int i=0; \
  for (; (i<=level)&&(tail->backward(i) == cursor); i++) \
  { \
    cursor->backward(i)->forward(i) = tail; \
    tail->backward(i) = cursor->backward(i); \
  } \
CSINDEX(for(;i<=level;i++) \
  { \
    tail->backward(i)->skip(i)--; \
  },) \
 \
  delete value(cursor->object); \
  Free(cursor); \
  items--; \
  adjust_levels(); \
}

#define CSDefinePopBackForwardIdx \
void pop_back() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  if (scan_index!=items-1) scan(items-1); \
 \
  /* Remove it. */ \
  node_type *node = update[0].second->forward(0); \
 \
  int i=0; \
CSLEVEL(for(;i<=node->level;i++) \
  { \
    update[i].second->forward(i)=tail; \
  }, \
  while((i<=level)&&(update[i].second->forward(i)==node)) \
  { \
    update[i].second->forward(i) = tail; \
    i++; \
  }) \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)--; \
  } \
 \
  /* This speeds up successive pop_back()'s */ \
  if (update[level].first>=scan_index-1) \
  { \
    scan_index=-1; \
  } \
  else \
  { \
    for(i=level-1;i>=0;i--) \
    { \
      if (update[i].first>=scan_index-1) \
      { \
        size_type tmp_total; \
        size_type total = update[i+1].first; \
        node_type *cursor = update[i+1].second; \
        while ((tmp_total=total+cursor->skip(i))<scan_index-1) \
        { \
          total=tmp_total; \
          cursor = cursor->forward(i); \
        } \
 \
        update[i].first = total; \
        update[i].second = cursor; \
      } \
    } \
    scan_index--; \
  } \
 \
  Free(node); \
  items--; \
  adjust_levels(); \
} \
 \
void destroy_back() \
{ \
  if (items==0) return;  /* Can't delete anything if nothing's there. */ \
 \
  if (scan_index!=items-1) scan(items-1); \
 \
  /* Remove it. */ \
  node_type *node = update[0].second->forward(0); \
 \
  int i=0; \
CSLEVEL(for(;i<=node->level;i++) \
  { \
    update[i].second->forward(i)=tail; \
  }, \
  while((i<=level)&&(update[i].second->forward(i)==node)) \
  { \
    update[i].second->forward(i) = tail; \
    i++; \
  }) \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)--; \
  } \
 \
  /* This speeds up successive pop_back()'s */ \
  if (update[level].first>=scan_index-1) \
  { \
    scan_index=-1; \
  } \
  else \
  { \
    for(i=level-1;i>=0;i--) \
    { \
      if (update[i].first>=scan_index-1) \
      { \
        size_type tmp_total; \
        size_type total = update[i+1].first; \
        node_type *cursor = update[i+1].second; \
        while ((tmp_total=total+cursor->skip(i))<scan_index-1) \
        { \
          total=tmp_total; \
          cursor = cursor->forward(i); \
        } \
 \
        update[i].first = total; \
        update[i].second = cursor; \
      } \
    } \
    scan_index--; \
  } \
 \
  delete value(node->object); \
  Free(node); \
  items--; \
  adjust_levels(); \
} \


#define CSDefinePopBackForward \
void pop_back() \
{ \
  if (items==0) return; \
 \
  node_type *cursor = head; \
  node_type *node = NULL; \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(node1->forward(0)!=tail)) \
    { \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
    if (node1!=tail) \
    { \
      if (node1->forward(0)==tail) \
      { \
        node = node1; \
        cursor->forward(i) = tail; \
      } \
      else \
      { \
        cursor = cursor->forward(i); \
      } \
    } \
  } \
 \
  Free(node); \
  items--; \
  adjust_levels(); \
} \
 \
void destroy_back() \
{ \
  if (items==0) return; \
 \
  node_type *cursor = head; \
  node_type *node = NULL; \
 \
  for(int i=level;i>=0;i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&(node1->forward(0)!=tail)) \
    { \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
    if (node1!=tail) \
    { \
      if (node1->forward(0)==tail) \
      { \
        node = node1; \
        cursor->forward(i) = tail; \
      } \
      else \
      { \
        cursor = cursor->forward(i); \
      } \
    } \
  } \
 \
  delete value(node->object); \
  Free(node); \
  items--; \
  adjust_levels(); \
}

#define CSDefineAdjustLevels \
void adjust_levels() \
{ \
  while ((level>0)&&(head->forward(level)==tail)) \
	{ \
     level--; \
  } \
CSLEVEL(if (level!=head->level) \
  { \
    head->level = level; \
    tail->level = level; \
  },) \
}

#define CSDXDefineAdjustLevels \
void adjust_levels() \
{ \
  while ((level>0)&&(head->forward(0,level)==tail)) \
	{ \
     level--; \
  } \
  if (level!=head->level) \
  { \
    head->level = level; \
    tail->level = level; \
  } \
}

#define CSDefineAssignITIT(op) \
template<class InIt> void assign(InIt first, InIt last) \
{ \
  clear(); \
  for(InIt i=first;i!=last;++i) op(*i); \
}

#define CSInitDefault \
Init(0.25, (unsigned int)ceil(log(100000.0)/log(1.0/0.25))-1)

#define CSInitMaxNodes \
Init(0.25, (unsigned int)ceil(log((double)maxNodes)/log(1.0/0.25))-1)

#define CSInitPM \
Init(probability, maxLevel)

#define CSInitProb \
  Init(prob.probability, prob.maxLevel)

#define CSCopyITIT(InIt, first, last, op) \
for(InIt i=first;i!=last;++i) op(*i)

#define CSCreateXItems(type,count,op) \
 type t = type(); \
 CSCreateXItemsVal(type,t,count,op)

#define CSCreateXItemsVal(type,val,count,op) \
  size_type ct = (count); \
  for(size_type i=0;i<ct;i++) op(val);

#define CSInitCore(xProbability, xMaxLevel) \
CSINDEX(scan_index = -1,); \
  this->probability = xProbability; \
  this->maxLevel = xMaxLevel; \
  update = new std::pair<size_type,node_type*>[xMaxLevel+1]; \
  level = 0; \
  items = 0; \
 \
  head = Alloc(xMaxLevel); \
  tail = Alloc(xMaxLevel); \
 \
  for (unsigned int i=0; i<=xMaxLevel; i++) \
  { \
CSINDEX(head->skip(i) = 1; \
        tail->skip(i) = 0;,) \
    head->forward(i) = tail; \
    tail->forward(i) = NULL; \
CSBIDI(tail->backward(i) = head; \
       head->backward(i) = NULL;,) \
  } \
CSLEVEL(head->level = 0;,) \
CSLEVEL(tail->level = 0;,)


#define CSDefineInit \
void Init(double xProbability,size_type xMaxLevel) \
{ \
  CSInitCore(xProbability, xMaxLevel) \
}

#define CSSwapCore \
  std::swap(maxLevel,right.maxLevel); \
  std::swap(level,right.level); \
  std::swap(head,right.head); \
  std::swap(tail,right.tail); \
  std::swap(probability,right.probability); \
  std::swap(items,right.items); \
  std::swap(update,right.update); \
CSINDEX(std::swap(scan_index, right.scan_index);,)

#define CSDefineOperatorEqual \
container_type& operator=(const container_type &source) \
{ \
  if (this==&source) return *this; \
  clear(); \
 \
CSINDEX(scan_index = -1,); \
  probability = source.probability; \
  level = CSLEVEL(source.level,1); \
  items = source.items; \
 \
  if (maxLevel<source.maxLevel) \
  { \
    maxLevel = source.maxLevel; \
    delete[] update; \
    Free(head); \
    Free(tail); \
 \
    update = new std::pair<size_type,node_type*>[maxLevel+1]; \
 \
    head = Alloc(maxLevel); \
    tail = Alloc(maxLevel); \
    for(unsigned int i=0;i<maxLevel;i++) \
    { \
      tail->forward(i) = NULL; \
CSBIDI(head->backward(i) = NULL;,) \
CSINDEX(tail->skip(i) = 0;,) \
    } \
  } \
 \
CSLEVEL(head->level = level;,) \
CSLEVEL(tail->level = level;,) \
CSLEVEL(,size_type item_count = 0;) \
 \
  /* Copy it. */ \
  for(unsigned int i=0;i<=level;i++) \
  { \
CSINDEX(head->skip(i) = CSLEVEL(source.head->skip(i),1),); \
    head->forward(i) = tail; \
CSBIDI(tail->backward(i) = head,); \
    update[i].second = head; \
  } \
  for(const_iterator i=source.begin();i!=source.end();++i) \
  { \
    unsigned int itemLevel = CSLEVEL(i.node->level,GenerateRandomLevel()); \
CSLEVEL(, \
    if (itemLevel>level) \
    { \
      for(unsigned int k=level+1;k<=itemLevel;k++) \
      { \
        head->forward(k) = tail; \
CSINDEX(head->skip(k) = item_count+1;,) \
CSBIDI(tail->backward(k) = head;,) \
        update[k].second = head; \
      } \
      level = itemLevel; \
    } \
) \
    node_type *item = Alloc(itemLevel,*i); \
    unsigned int j = 0; \
    for(;j<=itemLevel;j++) \
    { \
CSINDEX(item->skip(j) = CSLEVEL(i.node->skip(j),1),); \
      item->forward(j) = update[j].second->forward(j); \
CSBIDI(item->backward(j) = update[j].second,); \
CSBIDI(item->forward(j)->backward(j) = item,); \
      update[j].second->forward(j) = item; \
      update[j].second = item; \
    } \
CSLEVEL(,CSINDEX(for(;j<=level;j++) \
    { \
      update[j].second->skip(j)++; \
    },)) \
    CSLEVEL(,item_count++;) \
  } \
 \
  return *this; \
}

#define CSDefineEraseIf \
template<class Pr1> void erase_if(Pr1 pred) \
{ \
  for(iterator i=begin();i!=end();) \
  { \
    if (pred(*i)) \
    { \
      i = erase(i); \
    } \
    else \
    { \
      ++i; \
    } \
  } \
}

#define CSDefineDestroyIf \
template<class Pr4> void destroy_if(Pr4 pred) \
{ \
  for(iterator i=begin();i!=end();) \
  { \
    if (pred(*i)) \
    { \
      i = destroy(i); \
    } \
    else \
    { \
      ++i; \
    } \
  } \
}

#define CSDefineInsertVal \
CSUNIQUE(slpair,iterator) insert(const value_type& val) \
{ \
  value_compare ValueComp = value_comp(); \
 \
  unsigned int newLevel = GenerateRandomLevel(); \
 \
  scan_val(val); \
 \
CSUNIQUE(if ((update[0].second->forward(0)!=tail)&&(!ValueComp(val,update[0].second->forward(0)->object))),) \
CSUNIQUE({,) \
CSUNIQUE(  return slpair(CSINDEX(iterator(this,update[0].second->forward(0),scan_index),iterator(this,update[0].second->forward(0))),false),); \
CSUNIQUE(},) \
 \
  node_type *cursor = Alloc(newLevel, val); \
 \
  if (newLevel > level) \
  { \
    for (unsigned int i=level+1;i<=newLevel;i++) \
    { \
CSINDEX(head->skip(i) = items+1,); \
      head->forward(i) = tail; \
CSBIDI(tail->backward(i) = head,); \
CSINDEX(update[i].first = -1,); \
      update[i].second = head; \
    } \
    level = newLevel; \
    head->level = newLevel; \
    tail->level = newLevel; \
  } \
 \
  unsigned int i = 0; \
  for (; i<=newLevel; i++) \
  { \
CSBIDI(cursor->backward(i) = update[i].second,); \
    cursor->forward(i) = update[i].second->forward(i); \
CSINDEX(cursor->skip(i) = update[i].second->skip(i)-(scan_index-update[i].first-1),); \
CSBIDI(cursor->forward(i)->backward(i) = cursor,); \
    update[i].second->forward(i) = cursor; \
CSINDEX(update[i].second->skip(i) = scan_index-update[i].first,); \
  } \
CSINDEX(for(;i<=level;i++) \
  { \
    update[i].second->skip(i)++; \
  },) \
  items++; \
  return CSUNIQUE(slpair(CSINDEX(iterator(this,cursor,scan_index),iterator(this,cursor)),true),CSINDEX(iterator(this,cursor,scan_index),iterator(this,cursor))); \
}

#define CSDefineScanNode \
void scan(const node_type *nodex) const \
{ \
  value_compare ValueComp = value_comp(); \
 \
  node_type *node = head; \
 \
CSINDEX(size_type pos = -1,); \
 \
  for (int i=level; i>=0; i--) \
  { \
    node_type *node1 = node->forward(i); \
    while ((node1!=tail)&&((nodex==tail)||(ValueComp(node1->object,nodex->object)))) \
    { \
CSINDEX(pos+=node->skip(i),); \
      node = node1; \
      node1 = node1->forward(i); \
    } \
 \
CSUNIQUE(, node1 = node->forward(0); \
    while(node1!=nodex) \
    { \
      if (node1->level>=i) node=node1; \
      node1=node1->forward(0); \
    }) \
CSINDEX(update[i].first = pos,); \
    update[i].second = node; \
  } \
 \
CSINDEX(scan_index = update[0].first+1,); \
}

// Working on this.
// Must be Bidi, have level.
// TODO: Check if scan_index is valid.  Also check if next node is valid for non-indexed containers.
#define CSDXDefineScanNode \
void scan(node_type *nodex, unsigned int x) const \
{ \
  /* Check if update array already valid. */ \
  if ((scan_index[x]!=-1)&&(update[x][0].second->forward(x,0)==nodex)) return; \
  difference_type pos = 0; \
 \
  unsigned int i=0; \
  /* Set update array up to top level of node */ \
  for(;i<nodex->level;i++) \
  { \
    node_type *node1 = nodex->backward(x,i); \
    if (x<S) {update[x][i].first = pos-node1->skip(x,i);} \
    update[x][i].second = node1; \
  } \
 \
  /* Set update array beyond level of node up to max level of container */ \
  for (; i<level; i++) \
  { \
    while (nodex->level<i) \
    { \
      node_type *node1 = nodex->backward(x,i); \
      nodex = node1; \
      if (x<S) {pos -= node1->skip(x,i);} \
    } \
    if (x<S) {update[x][i].first = pos;} \
    update[x][i].second = nodex; \
  } \
 \
 \
  /* If container is indexed, set the update array forward indexes. */ \
  if (x<S) \
  { \
    pos = -pos; \
    for(unsigned int i=0;i<level;i++) \
    { \
      update[x][i].first += (pos-1); \
    } \
 \
    scan_index[x] = pos; \
  } \
  /* If container is not indexed, then just set that it's valid. */ \
  else \
  { \
    scan_index[x] = 1; \
  } \
 \
} \
\
void scanAll(node_type *nodex) const \
{ \
  for(unsigned int i=0;i<N;i++) \
  { \
    scan(nodex,i); \
  } \
}

#define CSDefineScanVal \
void scan_val(const value_type &val) const \
{ \
  value_compare ValueComp = value_comp(); \
 \
  node_type *node = head; \
 \
CSINDEX(difference_type pos = -1,); \
 \
  for (int i=level; i>=0; i--) \
  { \
    node_type *node1 = node->forward(i); \
    while ((node1!=tail)&&(ValueComp(node1->object,val))) \
    { \
CSINDEX(pos+=node->skip(i),); \
      node = node1; \
      node1 = node1->forward(i); \
    } \
 \
CSINDEX(update[i].first = pos,); \
    update[i].second = node; \
  } \
 \
CSINDEX(scan_index = update[0].first+1,); \
}

#define CSDefineScanKey \
void scan_key(const key_type &val) const \
{\
  key_compare KeyComp = key_comp(); \
 \
  node_type *node = head; \
 \
CSINDEX(difference_type pos = -1,); \
 \
  for (int i=level; i>=0; i--) \
  { \
    node_type *node1 = node->forward(i); \
    while ((node1!=tail)&&(KeyComp(key(node1->object),val))) \
    { \
CSINDEX(pos+=node->skip(i),); \
      node = node1; \
      node1 = node1->forward(i); \
    } \
 \
CSINDEX(update[i].first = pos,); \
    update[i].second = node; \
  } \
 \
CSINDEX(scan_index = update[0].first+1,); \
}

#define CSDefineScanIndex \
void scan(size_type index) const \
{ \
  if (index>items) index = items; \
  if (scan_index==index) return; \
 \
  node_type *cursor = head; \
 \
  size_type total = -1; \
  size_type tmp_total; \
 \
  for (int i=level; i>=0; i--) \
  { \
    while ((tmp_total=total+cursor->skip(i))<index) \
    { \
      total=tmp_total; \
      cursor = cursor->forward(i); \
    } \
 \
		update[i].first = total; \
    update[i].second = cursor; \
  } \
 \
  scan_index = index; \
}

#define CSDefineScanIterator \
void scan(const iterator &where) const \
{ \
CSINDEX(scan(where.Findex),scan(where.node)); \
}

#define CSDefineErase \
iterator erase(const iterator &where) \
{ \
  if CSINDEX((where.Findex>=items),((where.node==tail)||(where.node==head))) return end(); \
 \
CSINDEX(if (scan_index!=where.Findex) scan(where.Findex),scan(where));  \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) return end();  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
 if (cursor!=where.node) return end(); \
 \
  unsigned int i=0; \
  for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
  { \
CSINDEX(update[i].second->skip(i) += cursor->skip(i)-1,); \
    update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
  } \
CSINDEX(for(;i<=level;i++) \
  { \
    update[i].second->skip(i)--; \
  },) \
 \
  node_type *cursor2 = cursor->forward(0); \
  Free(cursor); \
  items--; \
  adjust_levels(); \
 \
  return CSINDEX(iterator(this,cursor2,where.Findex),iterator(this,cursor2)); \
} \
 \
iterator destroy(const iterator &where) \
{ \
  mapped_type tmp = value(where.node->object); \
  iterator i = erase(where); \
  delete tmp; \
  return i; \
}
// Erases a node (XData).
// No need for update array.  Keeps it synchronized.
#define CSDXDefineEraseNode \
void erase(node_type *node) \
{ \
  for(unsigned int j=0;j<N;j++) \
  { \
    if ((scan_index[j]!=-1)&&(update[j][0].second->forward(j,0)==node)) \
    { \
      unsigned int i=0; \
      for (; (i<=level)&&(update[j][i].second->forward(j,i) == node); i++) \
      { \
        if (j<S) update[j][i].second->skip(j,i) += node->skip(j,i)-1; \
        update[j][i].second->forward(j,i) = node->forward(j,i); \
        node->forward(j,i)->backward(j,i) = node->backward(j,i); \
      } \
      if (j<S) \
      { \
        for(;i<=level;i++) \
        { \
          update[j][i].second->skip(j,i)--; \
        } \
      } \
    } \
    else \
    { \
      unsigned int i=0; \
      for(;i<=node->level;i++) \
      { \
        if (j<S) node->backward(j,i)->skip(j,i) += node->skip(j,i)-1; \
        node->backward(j,i)->forward(j,i) = node->forward(j,i); \
        node->forward(j,i)->backward(j,i) = node->backward(j,i); \
      } \
      if (j<S) \
      { \
        node_type *p = node->backward(j,node->level); \
        for(;i<=level;i++) \
        { \
          while(p->level<i) p = p->backward(j,p->level); \
          p->skip(j,i)--; \
        } \
      } \
      scan_index[j] = -1; \
    } \
  } \
  Free(node); \
  items--; \
  adjust_levels(); \
}

#define CSDefineCut \
void cut(const iterator &first, const iterator &last, container_type& right) \
{ \
  if (first==last) return; \
 \
  if (level>right.maxLevel) \
    throw level_exception(); \
 \
CSINDEX(,value_compare ValueComp = value_comp()); \
 \
CSINDEX(difference_type diff = last.Findex-first.Findex,); \
 \
  right.clear(); \
 \
CSINDEX(if (scan_index!=first.Findex) scan(first.Findex),scan(first)); \
 \
  node_type *cursor = update[level].second; \
 \
CSINDEX(size_type total = update[level].first,); \
CSINDEX(size_type tmp_total,); \
  right.level = level; \
CSLEVEL(right.head->level = level;,) \
CSLEVEL(right.tail->level = level;,) \
  for (int i=level; i>=0; i--) \
  { \
CSINDEX( \
    if (update[i].first>total) \
    { \
      cursor = update[i].second; \
      total = update[i].first; \
    }, \
    if ((cursor==head)||((cursor!=tail)&&(update[i].second!=head)&&(ValueComp(cursor->object,update[i].second->object)))) \
    { \
      cursor = update[i].second; \
    }) \
CSINDEX(,node_type *node1 = cursor->forward(i)); \
    while CSINDEX(((tmp_total=total+cursor->skip(i))<last.Findex), \
     ((node1!=tail)&&((last.node==tail)||(ValueComp(node1->object,last.node->object)))))\
    { \
CSINDEX(total=tmp_total,); \
      cursor = CSINDEX(cursor->forward(i),node1); \
CSINDEX(,node1 = node1->forward(i)); \
    } \
 \
CSINDEX(,node1 = cursor->forward(0); \
    while(node1!=last.node) \
    { \
      if (node1->level>=i) cursor = node1; \
      node1 = node1->forward(0); \
    }) \
 \
    /* Create the link */ \
    if (update[i].second==cursor) \
    { \
CSINDEX(cursor->skip(i) -= diff,); \
      right.head->forward(i) = right.tail; \
CSBIDI(right.tail->backward(i) = right.head,); \
CSINDEX(right.head->skip(i) = diff+1,); \
    } \
    else \
    { \
      right.head->forward(i) = update[i].second->forward(i); \
CSBIDI(right.head->forward(i)->backward(i) = right.head,); \
CSINDEX(right.head->skip(i) = update[i].second->skip(i)-(first.Findex-update[i].first)+1,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(update[i].second->forward(i)->backward(i) = update[i].second,); \
CSINDEX(update[i].second->skip(i) = first.Findex-update[i].first+cursor->skip(i)-(last.Findex-total),); \
      cursor->forward(i) = right.tail; \
CSBIDI(right.tail->backward(i) = cursor,); \
CSINDEX(cursor->skip(i) = (last.Findex-total),); \
    } \
  } \
 \
CSINDEX(right.items = diff,); \
 \
CSINDEX(, \
  difference_type diff = 0; \
  cursor=right.head->forward(0); \
  while (cursor!=right.tail) {cursor=cursor->forward(0);diff++;} \
  right.items = diff; \
  ) \
 \
  right.adjust_levels(); \
 \
  items-=diff; \
  adjust_levels(); \
 \
CSINDEX(last.Findex = first.Findex,); \
}

#define CSERASE(del_fun) \
  value_compare ValueComp = value_comp(); \
 \
  scan(first); \
 \
  node_type *cursor = update[level].second; \
 \
  tmp.level = level; \
  tmp.head->level = level; \
  tmp.tail->level = level; \
  for (int i=level; i>=0; i--) \
  { \
    node_type *node1 = cursor->forward(i); \
    while ((node1!=tail)&&((last.node==tail)||(ValueComp(node1->object,last.node->object))))\
    { \
      cursor = node1; \
      node1 = node1->forward(i); \
    } \
 \
    /* Create the link */ \
    if (update[i].second==cursor) \
    { \
      tmp.head->forward(i) = tmp.tail; \
CSBIDI(tmp.tail->backward(i) = tmp.head,); \
    } \
    else \
    { \
      tmp.head->forward(i) = update[i].second->forward(i); \
CSBIDI(tmp.head->forward(i)->backward(i) = tmp.head,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(update[i].second->forward(i)->backward(i) = update[i].second,); \
      cursor->forward(i) = tmp.tail; \
CSBIDI(tmp.tail->backward(i) = cursor,); \
    } \
  } \
 \
  difference_type diff = 0; \
  cursor=tmp.head->forward(0); \
  while (cursor!=tmp.tail) \
  { \
    node_type *tmp2 = cursor->forward(0); \
    del_fun; \
    Free(cursor); \
    cursor=tmp2; \
    diff++; \
  } \
  tmp.level = 0; \
  tmp.head->forward(0) = tmp.tail; \
CSBIDI(tmp.tail->backward(0) = tmp.head,); \
  tmp.head->level = 0; \
  tmp.tail->level = 0; \
 \
  items-=diff; \


#define CSDefineEraseITIT \
iterator erase(const iterator &first, const iterator &last) \
{ \
  if (first==last) return last; \
 \
  container_type tmp(probability,maxLevel); \
 \
CSINDEX(cut(first,last,tmp); \
 \
  tmp.clear();, \
/*  CSERASE(;) */ \
cut(first,last,tmp); \
 \
  tmp.clear();) \
 \
  adjust_levels(); \
 \
CSINDEX(last.Findex = first.Findex,); \
  return last; \
} \
 \
iterator destroy(const iterator &first, const iterator &last) \
{ \
  if (first==last) return last; \
 \
  container_type tmp(probability,maxLevel); \
 \
CSINDEX(cut(first,last,tmp); \
 \
  tmp.destroy();, \
/*  CSERASE(delete value(cursor->object)) */ \
cut(first,last,tmp); \
 \
  tmp.destroy();) \
 \
  adjust_levels(); \
 \
CSINDEX(last.Findex = first.Findex,); \
  return last; \
}

#define CSDefineEraseKey \
size_type erase(const key_type &keyval) \
{ \
  scan_key(keyval); \
 \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) return 0;  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  size_type cnt = 0; \
  do \
  { \
    unsigned int i=0; \
    for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
    { \
CSINDEX(update[i].second->skip(i) += cursor->skip(i)-1,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
    } \
CSINDEX(for(;i<=level;i++) \
    { \
      update[i].second->skip(i)--; \
    },) \
    node_type *cursor3 = cursor->forward(0); \
    Free(cursor); \
    cursor = cursor3; \
    items--; \
    cnt++; \
  } while ((cursor!=tail)&&(!key_comp()(keyval,key(cursor->object)))&& \
      (!key_comp()(key(cursor->object),keyval))); \
 \
  adjust_levels(); \
 \
  return cnt; \
} \
 \
size_type destroy(const key_type &keyval) \
{ \
  scan_key(keyval); \
 \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) return 0;  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
    return 0; \
 \
  size_type cnt = 0; \
  do \
  { \
    unsigned int i=0; \
    for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
    { \
CSINDEX(update[i].second->skip(i) += cursor->skip(i)-1,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
    } \
CSINDEX(for(;i<=level;i++) \
    { \
      update[i].second->skip(i)--; \
    },) \
    node_type *cursor3 = cursor->forward(0); \
    delete value(cursor->object); \
    Free(cursor); \
    cursor = cursor3; \
    items--; \
    cnt++; \
  } while ((cursor!=tail)&&(!key_comp()(keyval,key(cursor->object)))&& \
      (!key_comp()(key(cursor->object),keyval))); \
 \
  adjust_levels(); \
 \
  return cnt; \
} \
 \
size_type erase(const key_type &keyval, iterator &next) \
{ \
  scan_key(keyval); \
 \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) { next = end(); return 0; }  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
  { \
    next = end(); \
    return 0; \
  } \
 \
  size_type cnt = 0; \
  do \
  { \
    unsigned int i=0; \
    for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
    { \
CSINDEX(update[i].second->skip(i) += cursor->skip(i)-1,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
    } \
CSINDEX(for(;i<=level;i++) \
    { \
      update[i].second->skip(i)--; \
    },) \
    node_type *cursor3 = cursor->forward(0); \
    Free(cursor); \
    cursor = cursor3; \
    items--; \
    cnt++; \
  } while ((cursor!=tail)&&(!key_comp()(keyval,key(cursor->object)))&& \
      (!key_comp()(key(cursor->object),keyval))); \
 \
  adjust_levels(); \
 \
  next = CSINDEX(iterator(this,update[0].second->forward(0),scan_index),iterator(this,update[0].second->forward(0))); \
  return cnt; \
} \
 \
size_type destroy(const key_type &keyval, iterator &next) \
{ \
  scan_key(keyval); \
 \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) { next = end(); return 0;}  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  if ((key_comp()(keyval,key(cursor->object)))|| \
      (key_comp()(key(cursor->object),keyval))) \
  { \
    next = end(); \
    return 0; \
  } \
 \
  size_type cnt = 0; \
  do \
  { \
    unsigned int i=0; \
    for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
    { \
CSINDEX(update[i].second->skip(i) += cursor->skip(i)-1,); \
      update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
    } \
CSINDEX(for(;i<=level;i++) \
    { \
      update[i].second->skip(i)--; \
    },) \
    node_type *cursor3 = cursor->forward(0); \
    delete value(cursor->object); \
    Free(cursor); \
    cursor = cursor3; \
    items--; \
    cnt++; \
  } while ((cursor!=tail)&&(!key_comp()(keyval,key(cursor->object)))&& \
      (!key_comp()(key(cursor->object),keyval))); \
 \
  adjust_levels(); \
 \
  next = CSINDEX(iterator(this,update[0].second->forward(0),scan_index),iterator(this,update[0].second->forward(0))); \
  return cnt; \
}


#define CSDefineEraseIndex \
iterator erase_index(size_type index) \
{ \
  if (index>=items) return end(); \
 \
  if (scan_index!=index) scan(index);  \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) return end();  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  unsigned int i=0; \
  for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
  { \
    update[i].second->skip(i) += cursor->skip(i)-1; \
    update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
  } \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)--; \
  } \
 \
  node_type *cursor2 = cursor->forward(0); \
  Free(cursor); \
  items--; \
  adjust_levels(); \
 \
  return iterator(this,cursor2,index); \
} \
 \
iterator destroy_index(size_type index) \
{ \
  if (index>=items) return end(); \
 \
  if (scan_index!=index) scan(index);  \
  if ((update[0].second==tail)||(update[0].second->forward(0)==tail)) return end();  /* Error */ \
 \
  node_type *cursor = update[0].second->forward(0); \
 \
  unsigned int i=0; \
  for (; (i<=level)&&(update[i].second->forward(i) == cursor); i++) \
  { \
    update[i].second->skip(i) += cursor->skip(i)-1; \
    update[i].second->forward(i) = cursor->forward(i); \
CSBIDI(cursor->forward(i)->backward(i) = cursor->backward(i),); \
  } \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)--; \
  } \
 \
  node_type *cursor2 = cursor->forward(0); \
  delete value(cursor->object); \
  Free(cursor); \
  items--; \
  adjust_levels(); \
 \
  return iterator(this,cursor2,index); \
}

#define CSDefineOperatorArrayIndex \
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
  if (off>items) throw std::out_of_range("Out of range."); \
  return value(*(begin()+off)); \
} \
 \
const_mapped_type_reference at(size_type off) const \
{ \
  if (off>items) throw std::out_of_range("Out of range."); \
  return value(*(begin()+off)); \
}

#define CSDefineOperatorArrayMap \
mapped_type_reference operator[](const key_type& key) \
{ \
  return insert(value_type(key, mapped_type())).first->second; \
} \
 \
const_mapped_type_reference operator[](const key_type& key) const \
{ \
  return (*find(key)).second; \
}

#define CSDefineOperatorArrayMapAccess \
mapped_type_reference operator[](const key_type& key) \
{ \
  return *find(key); \
} \
 \
const_mapped_type_reference operator[](const key_type& key) const \
{ \
  return *find(key); \
}

#define CSDefineOperatorArrayMap2 \
mapped_type_reference operator()(const key_type& key) \
{ \
  return insert(value_type(key, mapped_type())).first->second; \
} \
 \
const_mapped_type_reference operator()(const key_type& key) const \
{ \
  return (*find(key)).second; \
}

#define CSDefineOperatorArrayMap2Access \
mapped_type_reference operator()(const key_type& key) \
{ \
  return *find(key); \
} \
 \
const_mapped_type_reference operator()(const key_type& key) const \
{ \
  return *find(key); \
}

#define CSDefineOperatorArrayMap3 \
mapped_type_reference operator()(const size_type& key) \
{ \
  return insert(value_type(key, mapped_type())).first->second; \
} \
 \
const_mapped_type_reference operator()(const size_type& key) const \
{ \
  return (*find(key)).second; \
}

#define CSDefineOperatorArrayMap3Access \
mapped_type_reference operator()(const size_type& key) \
{ \
  return *find(key); \
} \
 \
const_mapped_type_reference operator()(const size_type& key) const \
{ \
  return *find(key); \
}

#define CSDefineResize \
void resize(size_type newsize) \
{ \
  if (newsize<items) \
  { \
    container_type tmp(probability,maxLevel); \
    cut(begin()+newsize,end(),tmp); \
    tmp.clear(); \
 \
    return; \
  } \
  /* Make it grow. */ \
  CSCreateXItems(value_type,newsize-items,push_back) \
} \
 \
void resize(size_type newsize, const value_type &val) \
{ \
  if (newsize<items) \
  { \
    resize(newsize); \
    return; \
  } \
  /* Make it grow. */ \
  CSCreateXItemsVal(value_type,val,newsize-items,push_back) \
}

#define CSDefinePushFront \
void push_front(const value_type &val) \
{ \
  scan_index = -1; \
 \
  unsigned int newLevel = GenerateRandomLevel(); \
 \
  if (newLevel > level) \
  { \
    for (unsigned int i=level+1;i<=newLevel;i++) \
    { \
      head->skip(i) = items+1; \
      head->forward(i) = tail; \
CSBIDI(tail->backward(i) = head;,) \
    } \
    level = newLevel; \
    head->level = newLevel; \
    tail->level = newLevel; \
  } \
 \
  node_type *cursor = Alloc(newLevel, val); \
 \
  node_type *back; \
 \
  /* Put new node into place on first level. */ \
  unsigned int i=0; \
  for(;i<=newLevel;i++) \
  { \
    back = head->forward(i); \
 \
    /* Insert our node. */ \
CSBIDI(cursor->backward(i) = head;,) \
    cursor->skip(i) = head->skip(i); \
    cursor->forward(i) = back; \
    head->skip(i) = 1; \
    head->forward(i) = cursor; \
CSBIDI(back->backward(i) = cursor;,) \
  } \
  for(;i<=level;i++) \
  { \
    head->skip(i)++; \
  } \
 \
  items++; \
}

#define CSDefinePushBackBidi \
void push_back(const value_type &val) \
{ \
  scan_index = -1; \
 \
  unsigned int newLevel = GenerateRandomLevel(); \
 \
  if (newLevel > level) \
  { \
    for (unsigned int i=level+1;i<=newLevel;i++) \
    { \
      head->skip(i) = items+1; \
      head->forward(i) = tail; \
      tail->backward(i) = head; \
    } \
    level = newLevel; \
    head->level = newLevel; \
    tail->level = newLevel; \
  } \
 \
  node_type *cursor = Alloc(newLevel, val); \
 \
  /* Put new node into place on first level. */ \
  unsigned int i=0; \
  for(;i<=newLevel;i++) \
  { \
    /* Insert our node. */ \
    cursor->forward(i) = tail; \
    cursor->skip(i) = 1; \
    cursor->backward(i) = tail->backward(i); \
    tail->backward(i)->forward(i) = cursor; \
    tail->backward(i) = cursor; \
  } \
  for(;i<=level;i++) \
  { \
    tail->backward(i)->skip(i)++; \
  } \
 \
  items++; \
}

#define CSDefinePushBackForward \
void push_back(const value_type &val) \
{ \
  unsigned int newLevel = GenerateRandomLevel(); \
 \
  node_type *cursor = Alloc(newLevel, val); \
 \
  if ((scan_index==items-1)&&(scan_index!=-1)) \
  { \
    /* Advance update array by one. */ \
    node_type *c = update[0].second->forward(0); \
    size_type pos = update[0].first+1; \
CSLEVEL( \
    for(unsigned int i=0;i<=c->level;i++) \
    { \
      update[i].first=pos; \
      update[i].second = c; \
    }, \
    unsigned int i=0; \
    while(update[i].second->forward(i)==c) \
    { \
      update[i].first = pos; \
      update[i].second = c; \
      ++i; \
    }) \
    scan_index++; \
  } \
  else if (scan_index!=items) scan(items); \
 \
  unsigned int oldLevel = level; \
 \
  if (newLevel>level) \
  { \
    for(unsigned int i=level+1;i<=newLevel;i++) \
    { \
      head->skip(i) = items+1; \
      head->forward(i) = tail; \
      update[i].first = -1; \
      update[i].second = head; \
    } \
CSLEVEL(head->level = newLevel;,) \
CSLEVEL(tail->level = newLevel;,) \
    level = newLevel; \
  } \
 \
  unsigned int i=0; \
  for(;i<=newLevel;i++) \
  { \
    cursor->skip(i) = 1; \
    cursor->forward(i) = tail; \
    update[i].second->forward(i) = cursor; \
    update[i].second = cursor; \
    update[i].first = items; \
  } \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)++; \
  } \
 \
  items++; \
  scan_index++; \
}

#define CSDefineInsertITVal \
iterator insert(const iterator &where, const value_type& val) \
{ \
  unsigned int newLevel = GenerateRandomLevel(); \
 \
  node_type *cursor = Alloc(newLevel, val); \
 \
  if (scan_index!=where.Findex) scan(where.Findex); \
 \
  if (newLevel > level) \
  { \
    for (unsigned int i=level+1;i<=newLevel;i++) \
    { \
      head->skip(i) = items+1; \
      head->forward(i) = tail; \
CSBIDI(tail->backward(i) = head;,) \
      update[i].first = -1; \
      update[i].second = head; \
    } \
    level = newLevel; \
CSLEVEL(head->level = newLevel;,) \
CSLEVEL(tail->level = newLevel;,) \
  } \
 \
  unsigned int i = 0; \
  for (; i<=newLevel; i++) \
  { \
CSBIDI(cursor->backward(i) = update[i].second;,) \
    cursor->forward(i) = update[i].second->forward(i); \
    cursor->skip(i) = update[i].second->skip(i)-(where.Findex-update[i].first-1); \
CSBIDI(cursor->forward(i)->backward(i) = cursor;,) \
    update[i].second->forward(i) = cursor; \
    update[i].second->skip(i) = where.Findex-update[i].first; \
  } \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)++; \
  } \
  items++; \
  where.Findex++; \
  return iterator(this,cursor,where.Findex-1); \
}

// Used in CSIndexedSkipList
#define CSXInsertITSizeVal \
  container_type tmp((double)probability,(size_type)maxLevel); \
 \
  for(size_type i=0;i<count;++i) \
  { \
    tmp.push_back(val); \
  } \
 \
  splice(where,tmp);


#define CSDefineInsertITSizeVal \
void insert(const iterator &where, size_type count, const T& val) \
{ \
  CSXInsertITSizeVal \
} \
 \
void insert(const iterator &where, int count, const T& val) \
{ \
  CSXInsertITSizeVal \
}

#define CSDefineInsertITITIT \
template<class InIt> void insert(const iterator &where, InIt first, InIt last) \
{ \
  if (first==last) return; \
 \
  if (scan_index!=where.Findex) scan(where.Findex); \
 \
  size_type elements = 0; \
  for(InIt i=first;i!=last;++i,elements++) \
  { \
    unsigned int newLevel = GenerateRandomLevel(); \
 \
    if (newLevel > level) \
    { \
      for (unsigned int i=level+1;i<=newLevel;i++) \
      { \
        head->skip(i) = items+1+elements; \
        head->forward(i) = tail; \
CSBIDI( tail->backward(i) = head;,) \
        update[i].first = -1; \
        update[i].second = head; \
      } \
      level = newLevel; \
CSLEVEL(head->level = newLevel;,) \
CSLEVEL(tail->level = newLevel;,) \
    } \
 \
    node_type *node = Alloc(newLevel,*i); \
 \
    unsigned int j=0; \
    for (; j<=newLevel; j++) \
    { \
CSBIDI(node->backward(j) = update[j].second;,) \
      node->forward(j) = update[j].second->forward(j); \
      node->skip(j) = update[j].second->skip(j)-(where.Findex-update[j].first-1); \
CSBIDI(node->forward(j)->backward(j) = node;,) \
      update[j].second->forward(j) = node; \
      update[j].second->skip(j) = where.Findex-update[j].first; \
      update[j].first+=update[j].second->skip(j); \
      update[j].second = node; \
    } \
    for(;j<=level;j++) \
    { \
      update[j].second->skip(j)++; \
    } \
    where.Findex++; \
    scan_index++; \
  } \
 \
  items+=elements; \
}

#if 0
/* This is slow with single linked skiplists. */
#define CSDefineReverse \
void reverse() \
{ \
  iterator s = begin(); \
  iterator e = end()-1; \
  while(e>s) \
  { \
    /* Swap the nodes. */ \
    iter_swap(s,e); \
    ++s; \
    --e; \
  } \
}
#else
#define CSDefineReverse \
void reverse() \
{ \
  container_type c(probability,maxLevel); \
 \
  /* move nodes to temporary list */ \
  c.head->forward(0) = head->forward(0); \
 \
  if (scan_index!=items) scan(items); \
  node_type *cursor = update[0].second; \
  cursor->forward(0) = c.tail; \
  scan_index = -1; \
 \
  /* Clear current list */ \
  for(unsigned int i=0;i<=level;i++) \
  { \
    head->forward(i) = tail; \
    head->skip(i) = 1; \
CSBIDI(tail->backward(i) = head;,) \
  } \
 \
  /* take nodes and push them to the front. */ \
  while((cursor=c.head->forward(0))!=c.tail) \
  { \
    c.head->forward(0) = cursor->forward(0); \
    /* Add it to the front of the list. */ \
    /* Put new node into place on first level. */ \
    unsigned int i=0; \
    for(;i<=cursor->level;i++) \
    { \
      node_type *back = head->forward(i); \
 \
      /* Insert our node. */ \
CSBIDI(cursor->backward(i) = head;,) \
      cursor->skip(i) = head->skip(i); \
      cursor->forward(i) = back; \
      head->skip(i) = 1; \
      head->forward(i) = cursor; \
CSBIDI(back->backward(i) = cursor;,) \
    } \
    for(;i<=level;i++) \
    { \
      head->skip(i)++; \
    } \
  } \
  c.head->forward(0) = c.tail; \
}
#endif

#define CSDefineReverseForward \
void reverse() \
{ \
  container_type c(probability,maxLevel); \
 \
  if (scan_index!=items) scan(items); \
  /* move nodes to temporary list */ \
  for(unsigned int i=0;i<=level;i++) \
  { \
    size_type skip = head->skip(i); \
    node_type *next = head->forward(i); \
    c.head->forward(i) = next; \
    c.head->skip(i) = skip; \
    update[i].second->forward(i) = c.tail; \
    update[i].second = next; \
    update[i].first = skip; \
    head->forward(i) = tail; \
  } \
 \
  scan_index = -1; \
  for(unsigned int i=0;i<items;i++) \
  { \
    /* Insert update[i].second to front of list. */ \
    node_type *cursor = update[0].second; \
    size_type skip = update[0].first; \
    update[0].first = cursor->skip(0); \
    update[0].second = cursor->forward(0); \
    cursor->forward(0) = head->forward(0); \
    cursor->skip(0) = skip; \
CSBIDI(cursor->forward(0)->backward(0) = cursor;,) \
    unsigned int j = 1; \
    while((j<=level)&&(update[j].second==cursor)) \
    { \
      skip = update[j].first; \
      update[j].first = cursor->skip(j); \
      update[j].second = cursor->forward(j); \
      cursor->forward(j) = head->forward(j); \
      cursor->skip(j) = skip; \
CSBIDI(cursor->forward(j)->backward(j) = cursor;,) \
      j++; \
    } \
  } \
  for(unsigned int i=0;i<level;i++) \
  { \
    head->skip(i) = update[0].first; \
CSBIDI(head->forward(i)->backward(i) = head;,) \
  } \
  c.head->forward(0) = c.tail; \
  c.level = 1; \
}

#define CSDefineUnique \
void unique() \
{ \
  iterator i = begin(); \
 \
  size_type limit = items; \
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

#define CSDefineUniquePr \
template<class Pr2> void unique(Pr2 pred) \
{ \
  iterator i = begin(); \
 \
  size_type limit = items; \
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

// Fix this to move nodes.
#define CSDefineSort \
void sort() \
{ \
  std::sort(begin(),end()); \
}

#define CSDefineSortPr \
template<class Pr3> void sort(Pr3 pred) \
{ \
  std::sort(begin(),end(),pred); \
}

#define CSDefineStableSort \
void stable_sort() \
{ \
  std::stable_sort(begin(),end()); \
}

#define CSDefineStableSortPr \
template<class Pr3> void stable_sort(Pr3 pred) \
{ \
  std::stable_sort(begin(),end(),pred); \
}

#define CSDefineEraseValue \
void erase_value(const value_type& val) \
{ \
  for(iterator i=begin();i!=end();) \
  { \
    if (*i==val) \
    { \
      i = erase(i); \
    } \
    else \
    { \
      ++i; \
    } \
  } \
}

#define CSDefineSplice \
void splice(const iterator &where, container_type& right) \
{ \
  if (&right == this) return; \
  if (right.size()==0) return; \
 \
  if (right.level>maxLevel) \
    throw level_exception(); \
 \
  scan(where); \
CSBIDI(,if (right.scan_index!=right.items) right.scan(right.items);) \
 \
  /* Add list into here. */ \
  for(unsigned int i=level+1;i<=right.level;i++) \
  { \
CSLEVEL(head->level = i;,) \
    head->skip(i) = items+1; \
    head->forward(i) = tail; \
CSBIDI(tail->backward(i) = head;,) \
CSLEVEL(tail->level = i;,) \
    update[i].first = -1; \
    update[i].second = head; \
  } \
 \
  unsigned int i=0; \
  for(;i<=right.level;i++) \
  { \
    if (right.head->forward(i) == right.tail) break; \
 \
    node_type *node; \
CSBIDI(node = right.tail->backward(i), \
       node = right.update[i].second); \
 \
    node->forward(i) = update[i].second->forward(i); \
    node->skip(i) += update[i].first+update[i].second->skip(i)-where.Findex; \
CSBIDI(update[i].second->forward(i)->backward(i) = right.tail->backward(i);,) \
 \
    update[i].second->forward(i) = right.head->forward(i); \
CSBIDI(update[i].second->forward(i)->backward(i) = update[i].second;,) \
    update[i].second->skip(i) = where.Findex-update[i].first+right.head->skip(i)-1; \
  } \
  for(;i<=level;i++) \
  { \
    update[i].second->skip(i)+=right.size(); \
  } \
 \
  items+=right.size(); \
  where.Findex+=right.size(); \
 \
  right.head->forward(0) = right.tail; \
CSBIDI(right.tail->backward(0) = right.head;,) \
  right.head->skip(0) = 1; \
CSLEVEL(right.head->level = 0;,) \
CSLEVEL(right.tail->level = 0;,) \
  right.level = 0; \
  right.items = 0; \
}

// Removes the single item at first and puts it before where
#define CSDefineSpliceIT \
void splice(const iterator &where, container_type& right, iterator first) \
{ \
  if (right.size()==0) return; \
 \
  if (right.level>maxLevel) \
    throw level_exception(); \
 \
  bool b = false; \
 \
  if ((this==&right)&&(first<where)) \
  { \
    b = true; \
  } \
 \
  container_type tmp(right.probability,right.maxLevel); \
  iterator j = first; \
  j++; \
 \
  right.cut(first,j,tmp); \
 \
  if (b) where.Findex--; \
 \
  splice(where,tmp); \
}

#define CSDefineSpliceITIT \
void splice(const iterator &where, container_type& right, iterator first, iterator last) \
{ \
  if (first==last) return; \
  if (first+1==last) \
  { \
    splice(where,right,first); \
    return; \
  } \
 \
  if (right.size()==0) return; \
 \
  if (right.level>maxLevel) \
    throw level_exception(); \
 \
  bool b = false; \
 \
  if ((this==&right)&&(first<where)) \
  { \
    b = true; \
  } \
 \
  container_type tmp(right.probability,right.maxLevel); \
 \
  right.cut(first,last,tmp); \
 \
  size_type sz = right.size(); \
  if (b) where.Findex-=sz; \
 \
  splice(where,tmp); \
}

#define CheckSkipNodes \
bool CheckSkipCount() \
{ \
  std::pair<size_type,node_type*> *upd = new std::pair<size_type,node_type*>[level+1]; \
  for(unsigned int i=0;i<=level;i++) \
  { \
CSINDEX(upd[i].first = -1 + head->skip(i);,) \
    upd[i].second = head->forward(i); \
CSBIDI(   if (head->forward(i)->backward(i) != head) \
          { \
            delete[] upd; \
            return false; \
          },) \
  } \
CSINDEX(if (upd[0].first!=0) \
  { \
    delete[] upd; \
    return false; \
  },) \
 \
  for(size_type i=0;i<=items;i++) \
  { \
    for(unsigned int j=1;j<=level;j++) \
    { \
      if (upd[j].second==upd[0].second) \
      { \
CSINDEX(if (upd[j].first!=upd[0].first) \
        { \
          delete[] upd; \
          return false; \
        },) \
        if (upd[j].second!=tail) \
        { \
CSBIDI(   if (upd[j].second->forward(j)->backward(j) != upd[j].second) \
          { \
            delete[] upd; \
            return false; \
          },) \
CSINDEX(  upd[j].first += upd[j].second->skip(j);,) \
          upd[j].second = upd[j].second->forward(j); \
        } \
      } \
      else \
      { \
        break; \
      } \
    } \
    if (upd[0].second!=tail) \
    { \
CSINDEX(if (upd[0].second->skip(0)!=1) \
      { \
        delete[] upd; \
        return false; \
      } \
      upd[0].first++;,) \
      upd[0].second = upd[0].second->forward(0); \
    } \
  } \
  for(unsigned int i=0;i<=level;i++) \
  { \
CSINDEX(if (upd[i].first != items) \
    { \
      delete[] upd; \
      return false; \
    },) \
    if (upd[i].second != tail) \
    { \
      delete[] upd; \
      return false; \
    } \
  } \
  delete[] upd; \
  return true; \
}

#define XCheckSkipNodes2 \
  bool CheckSkipCount() \
  { \
    return data->CheckSkipCount(); \
  }

#define DCheckSkipNodes \
bool CheckSkipCount() \
{ \
  std::pair<size_type,node_type*> *upd = new std::pair<size_type,node_type*>[level+1]; \
  unsigned int limit = getListCount(); \
  for(unsigned int k=0;k<limit;k++) \
  { \
    bool bIndexed = (k<getIndexCount()); \
    for(unsigned int i=0;i<=level;i++) \
    { \
      if (bIndexed) upd[i].first = -1 + head->skip(k,i); \
      upd[i].second = head->forward(k,i); \
      if (head->forward(k,i)->backward(k,i) != head) \
      { \
        delete[] upd; \
        return false; \
      } \
    } \
    if ((bIndexed)&&(upd[0].first!=0)) \
    { \
      delete[] upd; \
      return false; \
    } \
   \
    for(size_type i=0;i<=items;i++) \
    { \
      for(unsigned int j=1;j<=level;j++) \
      { \
        if (upd[j].second==upd[0].second) \
        { \
          if ((bIndexed)&&(upd[j].first!=upd[0].first)) \
          { \
            delete[] upd; \
            return false; \
          } \
          if (upd[j].second!=tail) \
          { \
            if (upd[j].second->forward(k,j)->backward(k,j) != upd[j].second) \
            { \
              delete[] upd; \
              return false; \
            } \
            if (bIndexed) upd[j].first += upd[j].second->skip(k,j); \
            upd[j].second = upd[j].second->forward(k,j); \
          } \
        } \
        else \
        { \
          break; \
        } \
      } \
      if (upd[0].second!=tail) \
      { \
        if (bIndexed) \
        { \
          if (upd[0].second->skip(k,0)!=1) \
          { \
            delete[] upd; \
            return false; \
          } \
          upd[0].first++; \
        } \
        upd[0].second = upd[0].second->forward(k,0); \
      } \
    } \
    for(unsigned int i=0;i<=level;i++) \
    { \
      if ((bIndexed)&&(upd[i].first != items)) \
      { \
        delete[] upd; \
        return false; \
      } \
      if (upd[i].second != tail) \
      { \
        delete[] upd; \
        return false; \
      } \
    } \
  } \
  delete[] upd; \
  return true; \
}

}

#endif

