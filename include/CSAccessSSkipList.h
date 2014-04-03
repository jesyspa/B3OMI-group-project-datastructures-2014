/*
   Description: Header file for AccessSSkipList and MultiAccessSSkipList
                Single linked skiplists that act like a map and multimap,
                but where the key is part of the value.

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
#ifndef CSAccessSSkipListH
#define CSAccessSSkipListH

#include <utility>
#include <iterator>
#include <functional>
#include "CSSkipListTools.h"
#include "CSIterators.h"

namespace CS
{
#define CSBIDI(a,b) b
#define CSUNIQUE(a,b) a
#define CSINDEX(a,b) b
#define CSKEY(a,b) a
#define CSLEVEL(a,b) b

template <class K, class T, class A=XAccessSelf<K,T>, class Pr = std::less<K>, class R = RNG >
class AccessSSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef AccessSSkipList<K,T,A,Pr,R> container_type;
  typedef ForwardIterator<container_type> T0;
  typedef ConstForwardIterator<container_type> T1;
  friend class ForwardIterator<container_type>;
  friend class ConstForwardIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef K key_type;
  typedef T value_type;
  typedef ForwardNode<value_type> node_type;
  typedef T0 iterator;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef T data_type;
  typedef T mapped_type;
  typedef T& mapped_type_reference;
  typedef const T const_mapped_type;
  typedef const T& const_mapped_type_reference;
  typedef const value_type& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;

  class value_compare
    : public std::binary_function<value_type, value_type, bool>
  {
  friend class AccessSSkipList<K,T,A,Pr,R>;
  public:
    bool operator()(const value_type& left, const value_type& right) const
      {return (comp(a(left), a(right))); }
  protected:
    value_compare(const key_compare &pr) : comp(pr) {}
    key_compare comp;
    A a;
  };

private:
  R rng;
  A a;
  key_compare KeyCompare;
  value_compare ValueCompare;
  size_type maxLevel; //!< Maximum number of forward pointers possible.
  size_type level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(size_type level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(size_type level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanKey
  CSDefineScanVal
  CSDefineScanIterator
  CSDefineScanNode
public:

  CheckSkipNodes

  AccessSSkipList() : ValueCompare(Pr()) { CSInitDefault; }
  explicit AccessSSkipList(size_type maxNodes) : ValueCompare(Pr()) { CSInitMaxNodes; }
  AccessSSkipList(double probability, size_type maxLevel) : ValueCompare(Pr()) { CSInitPM; }
  AccessSSkipList(const container_type &source) : ValueCompare(source.ValueCompare), KeyCompare(source.KeyCompare) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> AccessSSkipList(InIt first, InIt last) : ValueCompare(Pr()) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AccessSSkipList(InIt first, InIt last, double probability, size_type maxLevel) : ValueCompare(Pr()) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AccessSSkipList(InIt first, InIt last, size_type maxNodes) : ValueCompare(Pr()) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit AccessSSkipList(const key_compare& comp) : ValueCompare(comp), KeyCompare(comp) { CSInitDefault; }
  template<class InIt> AccessSSkipList(InIt first, InIt last, const key_compare& comp) : ValueCompare(comp), KeyCompare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AccessSSkipList(InIt first, InIt last, const key_compare& comp, double probability, size_type maxLevel) : ValueCompare(comp), KeyCompare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AccessSSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : ValueCompare(comp), KeyCompare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~AccessSSkipList() { clear(); }
  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackForward
  CSDefinePopFront
  CSDefinePopBackForward
  CSDefineAssignITIT(insert)

  CSDefineInsertVal
  iterator insert(const iterator &where, const value_type& val) { return insert(val).first; } // Don't use this.  Calls insert(const value_type& type);
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); }

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseKey

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore std::swap(ValueCompare, right.ValueCompare); std::swap(KeyCompare, right.KeyCompare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut
  CSDefineOperatorArrayMapAccess
  CSDefineOperatorArrayMap2Access

  CSDefineKeyCompare(KeyCompare)
  CSDefineValueCompare(ValueCompare)
  CSDefineMaxSize

  const key_type& key(const reference value) const { return a(value); }
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSDefineFind
  CSDefineCount
  CSDefineLowerBound
  CSDefineUpperBound
  CSDefineEqualRange
};

template <class K, class T, class A, class Pr, class R>
bool operator==(const AccessSSkipList<K,T,A,Pr,R> &left, const AccessSSkipList<K,T,A,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class K, class T, class A, class Pr, class R>
bool operator<(const AccessSSkipList<K,T,A,Pr,R> &left, const AccessSSkipList<K,T,A,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

#define csarg1 template<class K, class T, class A, class Pr, class R>
#define csarg2 AccessSSkipList<K,T,Pr,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSUNIQUE
#define CSUNIQUE(a,b) b

template <class K, class T, class A=XAccessSelf<K,T>, class Pr = std::less<K>, class R = RNG >
class MultiAccessSSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef MultiAccessSSkipList<K,T,A,Pr,R> container_type;
  typedef ForwardIterator<container_type> T0;
  typedef ConstForwardIterator<container_type> T1;
  friend class ForwardIterator<container_type>;
  friend class ConstForwardIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef K key_type;
  typedef T value_type;
  typedef ForwardNode<value_type> node_type;
  typedef T0 iterator;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef T data_type;
  typedef T mapped_type;
  typedef T& mapped_type_reference;
  typedef const T const_mapped_type;
  typedef const T& const_mapped_type_reference;
  typedef const value_type& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;

  class value_compare
    : public std::binary_function<value_type, value_type, bool>
  {
  friend class MultiAccessSSkipList<K,T,A,Pr,R>;
  public:
    bool operator()(const value_type& left, const value_type& right) const
      {return (comp(a(left), a(right))); }
  protected:
    value_compare(const key_compare &pr) : comp(pr) {}
    key_compare comp;
    A a;
  };

private:
  R rng;
  A a;
  key_compare KeyCompare;
  value_compare ValueCompare;
  size_type maxLevel; //!< Maximum number of forward pointers possible.
  size_type level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(size_type level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(size_type level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanKey
  CSDefineScanVal
  CSDefineScanIterator
  CSDefineScanNode
public:

  CheckSkipNodes

  MultiAccessSSkipList() : ValueCompare(Pr()) { CSInitDefault; }
  explicit MultiAccessSSkipList(size_type maxNodes) : ValueCompare(Pr()) { CSInitMaxNodes; }
  MultiAccessSSkipList(double probability, size_type maxLevel) : ValueCompare(Pr()) { CSInitPM; }
  MultiAccessSSkipList(const container_type &source) : ValueCompare(source.ValueCompare), KeyCompare(source.KeyCompare) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last) : ValueCompare(Pr()) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last, double probability, size_type maxLevel) : ValueCompare(Pr()) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last, size_type maxNodes) : ValueCompare(Pr()) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit MultiAccessSSkipList(const key_compare& comp) : ValueCompare(comp), KeyCompare(comp) { CSInitDefault; }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last, const key_compare& comp) : ValueCompare(comp), KeyCompare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last, const key_compare& comp, double probability, size_type maxLevel) : ValueCompare(comp), KeyCompare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAccessSSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : ValueCompare(comp), KeyCompare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~MultiAccessSSkipList() { clear(); }
  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackForward
  CSDefinePopFront
  CSDefinePopBackForward
  CSDefineAssignITIT(insert)

  CSDefineInsertVal
  iterator insert(const iterator &where, const value_type& val) { return insert(val); } // Don't use this.  Calls insert(const value_type& type);
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); }

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseKey

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore std::swap(ValueCompare, right.ValueCompare); std::swap(KeyCompare, right.KeyCompare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut

  CSDefineKeyCompare(KeyCompare)
  CSDefineValueCompare(ValueCompare)
  CSDefineMaxSize

  const key_type& key(const_reference value) const {return a(value);}
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSDefineFind
  CSDefineCount
  CSDefineLowerBound
  CSDefineUpperBound
  CSDefineEqualRange
};

template <class K, class T, class A, class Pr, class R>
bool operator==(const MultiAccessSSkipList<K,T,A,Pr,R> &left, const MultiAccessSSkipList<K,T,A,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class K, class T, class A, class Pr, class R>
bool operator<(const MultiAccessSSkipList<K,T,A,Pr,R> &left, const MultiAccessSSkipList<K,T,A,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

#define csarg1 template<class K, class T, class A, class Pr, class R>
#define csarg2 MultiAccessSSkipList<K,T,Pr,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSKEY
#undef CSINDEX
#undef CSUNIQUE
#undef CSBIDI
#undef CSLEVEL
}

#endif

