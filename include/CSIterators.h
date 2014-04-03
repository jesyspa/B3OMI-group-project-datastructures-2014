/*
   Description: Header file for iterators.

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

#ifndef CSIteratorsH
#define CSIteratorsH

#include "CSSkipListTools.h"
#include "CSProperty.h"

namespace CS
{

class multikey_tag {};       // MultiKeyedSkipList
class uniquekey_tag {};      // KeyedSkipList
class multi_tag {};          // IndexedSkipList
class unique_tag {};         // Cannot happen

#define CSDefineIteratorDecForward2(it) \
  it& operator--() \
  { \
    return dec(tag()); \
  } \
 \
  CSDefineIteratorDecPost(it) \
 \
  it& dec(const multikey_tag &t) \
  { \
    typename container_type::value_compare ValueComp = container->value_comp(); \
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
    while (cursor->forward(0) != node) cursor = cursor->forward(0); \
 \
    if (cursor==container->head) \
    { \
      cursor = cursor->forward(0); \
    } \
 \
    node = cursor; \
    return *this; \
  } \
 \
  it& dec(const multi_tag &t) \
  { \
    typename container_type::value_compare ValueComp = container->value_comp(); \
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
    while (cursor->forward(0) != node) cursor = cursor->forward(0); \
 \
    if (cursor==container->head) \
    { \
      cursor = cursor->forward(0); \
    } \
 \
    node = cursor; \
    return *this; \
  } \
 \
  it& dec(const uniquekey_tag &t) \
  { \
    typename container_type::value_compare ValueComp = container->value_comp(); \
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
    if (cursor==container->head) \
    { \
      cursor = cursor->forward(0); \
    } \
 \
    node = cursor; \
    return *this; \
  } \
 \
  it& dec(const unique_tag &t) \
  { \
    typename container_type::value_compare ValueComp = container->value_comp(); \
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
    if (cursor==container->head) \
    { \
      cursor = cursor->forward(0); \
    } \
 \
    node = cursor; \
    return *this; \
  } \


#define CSDefineIteratorLTNodeCompare2(it) \
  bool operator<(const it &other) const \
  { \
    return less(other, tag()); \
  } \
 \
  bool less(const it &other, const multikey_tag& t) const \
  { \
    CSINDEX( \
    return (Findex<other.Findex);, \
  \
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
    }) \
  } \
 \
  bool less(const it &other, const uniquekey_tag& t) const \
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
  } \
 \
  bool less(const it &other, const multi_tag& t) const \
  { \
CSINDEX(return (Findex<other.Findex);,) \
  } \
 \
  bool less(const it &other, const unique_tag& t) const \
  { \
    /* Can't happen */ \
CSINDEX(return (Findex<other.Findex);,) \
  }

#ifdef __BORLANDC__
#define FRIENDCLASS
#else
#define FRIENDCLASS class
#endif

#define CSLEVEL(a,b) a
#define CSBIDI(a,b) a
#define CSINDEX(a,b) a

// BidiIdxIterator
template<class container_type>
class ConstBidiIdxIterator;

template<class container_type>
class BidiIdxIterator : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef BidiIdxIterator<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  BidiIdxIterator() : container(NULL), node(NULL), Findex(-1) InitProp(index) { }
  BidiIdxIterator(const T0 &t0) : container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) { }
  BidiIdxIterator(const T1 &t1) : container(t1.container), node(const_cast<node_type*>(t1.node)), Findex(t1.Findex) InitProp(index) { }
  BidiIdxIterator(container_type *container, node_type* p) : container(container), node(p) InitProp(index) { refresh(); }
  BidiIdxIterator(container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T0)
  CSDefineIteratorDecBidi(T0)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T0)
  CSDefineIteratorForwardArbitraryAccess(T0)
  CSDefineIteratorReverseArbitraryAccess(T0)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshBidi
  CSDefineIteratorArray(T0)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

template<class container_type>
class ConstBidiIdxIterator : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef ConstBidiIdxIterator<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  const container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  ConstBidiIdxIterator() : container(NULL), node(NULL), Findex(-1) InitProp(index) { }
  ConstBidiIdxIterator(const T1 &t1): container(t1.container), node(t1.node), Findex(t1.Findex) InitProp(index) { }
  ConstBidiIdxIterator(const T0 &t0): container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) { }
  ConstBidiIdxIterator(const container_type *container, node_type* p) : container(container), node(p) InitProp(index) { refresh(); }
  ConstBidiIdxIterator(const container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T1)
  CSDefineIteratorDecBidi(T1)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T1)
  CSDefineIteratorForwardArbitraryAccess(T1)
  CSDefineIteratorReverseArbitraryAccess(T1)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshBidi
  CSDefineConstIteratorArray(T1)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

#undef CSBIDI
#undef CSINDEX
#define CSBIDI(a,b) b
#define CSINDEX(a,b) a

// Forward Idx Iterator
template<class container_type>
class ConstForwardIdxIterator;

template<class container_type>
class ForwardIdxIterator : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef ForwardIdxIterator<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  ForwardIdxIterator() : container(NULL), node(NULL), Findex(-1) InitProp(index) {  }
  ForwardIdxIterator(const T0 &t0) : container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) {  }
  ForwardIdxIterator(const T1 &t1) : container(t1.container), node(const_cast<node_type*>(t1.node)), Findex(t1.Findex) InitProp(index) {  }
  ForwardIdxIterator(container_type *container, node_type* p) : container(container), node(p) InitProp(index) {  refresh(); }
  ForwardIdxIterator(container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) {  }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T0)
  CSDefineIteratorDecForwardIdx(T0)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T0)
  CSDefineIteratorForwardArbitraryAccess(T0)
  CSDefineIteratorReverseArbitraryAccessIdx(T0)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshIdx
  CSDefineIteratorArray(T0)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

template<class container_type>
class ConstForwardIdxIterator : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef ConstForwardIdxIterator<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  const container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  ConstForwardIdxIterator() : container(NULL), node(NULL), Findex(-1) InitProp(index) {  }
  ConstForwardIdxIterator(const T1 &t1): container(t1.container), node(t1.node), Findex(t1.Findex) InitProp(index) {  }
  ConstForwardIdxIterator(const T0 &t0): container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) {  }
  ConstForwardIdxIterator(const container_type *container, node_type* p) : container(container), node(p) InitProp(index) {  refresh(); }
  ConstForwardIdxIterator(const container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) {  }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T1)
  CSDefineIteratorDecForwardIdx(T1)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T1)
  CSDefineIteratorForwardArbitraryAccess(T1)
  CSDefineIteratorReverseArbitraryAccessIdx(T1)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshIdx
  CSDefineConstIteratorArray(T1)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

#undef CSBIDI
#undef CSINDEX
#define CSBIDI(a,b) a
#define CSINDEX(a,b) b

// Bidi Iterator
template<class container_type>
class ConstBidiIterator;

template<class container_type>
class BidiIterator : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
public:
  container_type *container;
  node_type* node;
public:
  BidiIterator() : container(NULL), node(NULL) {}
  BidiIterator(const T0 &t0) : container(t0.container), node(t0.node) { }
  BidiIterator(const T1 &t1) : container(t1.container), node(const_cast<node_type*>(t1.node)) { }
  BidiIterator(container_type *container, node_type* p) : container(container), node(p) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T0)
  CSDefineIteratorDecBidi(T0)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
};

template<class container_type>
class ConstBidiIterator : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
public:
  const container_type *container;
  node_type* node;
public:
  ConstBidiIterator() : container(NULL), node(NULL) {}
  ConstBidiIterator(const T1 &t1): container(t1.container), node(t1.node) {}
  ConstBidiIterator(const T0 &t0): container(t0.container), node(t0.node) {}
  ConstBidiIterator(const container_type *container, node_type* p) : container(container), node(p) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T1)
  CSDefineIteratorDecBidi(T1)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
};

#undef CSBIDI
#undef CSINDEX
#define CSBIDI(a,b) b
#define CSINDEX(a,b) b

// Forward Iterator
template<class container_type>
class ConstForwardIterator;

template<class container_type>
class ForwardIterator : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  container_type *container;
  node_type* node;
public:
  ForwardIterator() : container(NULL), node(NULL) {}
  ForwardIterator(const T0 &t0) : container(t0.container), node(t0.node) { }
  ForwardIterator(const T1 &t1) : container(t1.container), node(const_cast<node_type*>(t1.node)) { }
  ForwardIterator(container_type *container, node_type* p) : container(container), node(p) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T0)
  CSDefineIteratorDecForward2(T0)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
};

template<class container_type>
class ConstForwardIterator : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
public:
  const container_type *container;
  node_type* node;
public:
  ConstForwardIterator() : container(NULL), node(NULL) {}
  ConstForwardIterator(const T1 &t1): container(t1.container), node(t1.node) {}
  ConstForwardIterator(const T0 &t0): container(t0.container), node(t0.node) {}
  ConstForwardIterator(const container_type *container, node_type* p) : container(container), node(p) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T1)
  CSDefineIteratorDecForward2(T1)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
};
#undef CSBIDI
#undef CSINDEX
#undef CSLEVEL
#define CSLEVEL(a,b) b
#define CSBIDI(a,b) b
#define CSINDEX(a,b) a

// Forward Idx Iterator
template<class container_type>
class ConstForwardIdxIteratorNL;

template<class container_type>
class ForwardIdxIteratorNL : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, typename container_type::pointer, typename container_type::reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef ForwardIdxIteratorNL<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  ForwardIdxIteratorNL() : container(NULL), node(NULL), Findex(-1) InitProp(index) {}
  ForwardIdxIteratorNL(const T0 &t0) : container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) { }
  ForwardIdxIteratorNL(const T1 &t1) : container(t1.container), node(const_cast<node_type*>(t1.node)), Findex(t1.Findex) InitProp(index) { }
  ForwardIdxIteratorNL(container_type *container, node_type* p) : container(container), node(p) InitProp(index) { refresh(); }
  ForwardIdxIteratorNL(container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T0)
  CSDefineIteratorDecForwardIdx(T0)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T0)
  CSDefineIteratorForwardArbitraryAccess(T0)
  CSDefineIteratorReverseArbitraryAccessIdx(T0)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshIdx
  CSDefineIteratorArray(T0)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

template<class container_type>
class ConstForwardIdxIteratorNL : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::tag tag;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef ConstForwardIdxIteratorNL<container_type> self_type;

// This is public because container_type cannot be made a friend.
// Nested iterator classes could solve this, but Borland does cannot
// do iterator_traits on nested classes within partially specialized containers.
// WARNING:  DO NOT WRITE TO THE FOLLOWING MEMBERS!!!
// Use getIndex() or the 'index' property to obtain the index.
public:
  const container_type *container;
  mutable size_type Findex;
  node_type* node;
public:
  ConstForwardIdxIteratorNL() : container(NULL), node(NULL), Findex(-1) InitProp(index) {}
  ConstForwardIdxIteratorNL(const T1 &t1): container(t1.container), node(t1.node), Findex(t1.Findex) InitProp(index) { }
  ConstForwardIdxIteratorNL(const T0 &t0): container(t0.container), node(t0.node), Findex(t0.Findex) InitProp(index) {  }
  ConstForwardIdxIteratorNL(const container_type *container, node_type* p) : container(container), node(p) InitProp(index) { refresh(); }
  ConstForwardIdxIteratorNL(const container_type *container, node_type* p, size_type index) : container(container), node(p), Findex(index) InitProp(index) { }

  CSDefineIteratorEQ(T0)
  CSDefineIteratorEQ(T1)
  CSDefineIteratorInc(T1)
  CSDefineIteratorDecForwardIdx(T1)
  CSDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSDefineIteratorLTNodeCompare2(T0)
  CSDefineIteratorLTNodeCompare2(T1)
  CSDefineIteratorPlusMinus(T1)
  CSDefineIteratorForwardArbitraryAccess(T1)
  CSDefineIteratorReverseArbitraryAccessIdx(T1)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSDefineIteratorRefreshIdx
  CSDefineConstIteratorArray(T1)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

#undef CSBIDI
#undef CSINDEX
#undef CSLEVEL
}
#endif
