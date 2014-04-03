/*
   Description: Header file for SSkipList and MultiSSkipList
                Single linked skiplists that act like a set and multiset.

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

#ifndef CSSSkipListH
#define CSSSkipListH

#include <functional>
#include <iterator>
#include "CSSkipListTools.h"
#include "CSIterators.h"

namespace CS
{
#define CSBIDI(a,b) b
#define CSUNIQUE(a,b) a
#define CSINDEX(a,b) b
#define CSKEY(a,b) a
#define CSLEVEL(a,b) b

template <class T, class Pr = std::less<T>, class R = RNG >
class SSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef SSkipList<T,Pr,R> container_type;
  typedef ForwardIterator<container_type> T0;
  typedef ConstForwardIterator<container_type> T1;
  friend class ForwardIterator<container_type>;
  friend class ConstForwardIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef ForwardNode<value_type> node_type;
  typedef T key_type;
  typedef T0 iterator;
  typedef T* pointer;
  typedef T& reference;
  typedef T mapped_type;
  typedef T& mapped_type_reference;
  typedef const T const_mapped_type;
  typedef const T& const_mapped_type_reference;
  typedef const T& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;
  typedef Pr value_compare;

private:
  R rng;
  Pr Compare;
  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const T &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanVal
  CSDefineScanKey
  CSDefineScanNode
  CSDefineScanIterator
public:

  CheckSkipNodes

  SSkipList() { CSInitDefault; }
  explicit SSkipList(size_type maxNodes) { CSInitMaxNodes; }
  SSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  SSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> SSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> SSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> SSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit SSkipList(const key_compare& comp) : Compare(comp) { CSInitDefault; }
  template<class InIt> SSkipList(InIt first, InIt last, const key_compare& comp) : Compare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> SSkipList(InIt first, InIt last, const key_compare& comp, double probability, unsigned int maxLevel) : Compare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> SSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : Compare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~SSkipList() { clear(); }
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
  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut

  CSDefineKeyCompare(Compare)
  CSDefineValueCompare(Compare)
  CSDefineMaxSize

  const key_type& key(const_reference value) const {return value;}
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSDefineFind
  CSDefineCount
  CSDefineLowerBound
  CSDefineUpperBound
  CSDefineEqualRange
};

template <class T, class Pr, class R>
bool operator==(const SSkipList<T,Pr,R> &left, const SSkipList<T,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class T, class Pr, class R>
bool operator<(const SSkipList<T,Pr,R> &left, const SSkipList<T,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

#define csarg1 template <class T, class Pr, class R>
#define csarg2 SSkipList<T,Pr,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSUNIQUE
#define CSUNIQUE(a,b) b

template <class T, class Pr = std::less<T>, class R = RNG >
class MultiSSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef MultiSSkipList<T,Pr,R> container_type;
  typedef ForwardIterator<container_type> T0;
  typedef ConstForwardIterator<container_type> T1;
  friend class ForwardIterator<container_type>;
  friend class ConstForwardIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef ForwardNode<value_type> node_type;
  typedef T key_type;
  typedef T0 iterator;
  typedef T* pointer;
  typedef T& reference;
  typedef T mapped_type;
  typedef T& mapped_type_reference;
  typedef const T const_mapped_type;
  typedef const T& const_mapped_type_reference;
  typedef const T& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;
  typedef Pr value_compare;

private:
  R rng;
  Pr Compare;
  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const T &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanVal
  CSDefineScanKey
  CSDefineScanNode
  CSDefineScanIterator
public:

  CheckSkipNodes

  MultiSSkipList() { CSInitDefault; }
  explicit MultiSSkipList(size_type maxNodes) { CSInitMaxNodes; }
  MultiSSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  MultiSSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> MultiSSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiSSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiSSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit MultiSSkipList(const key_compare& comp) : Compare(comp) { CSInitDefault; }
  template<class InIt> MultiSSkipList(InIt first, InIt last, const key_compare& comp) : Compare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiSSkipList(InIt first, InIt last, const key_compare& comp, double probability, unsigned int maxLevel) : Compare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiSSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : Compare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~MultiSSkipList() { clear(); }
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
  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut

  CSDefineKeyCompare(Compare)
  CSDefineValueCompare(Compare)
  CSDefineMaxSize

  const key_type& key(const_reference value) const {return value;}
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSDefineFind
  CSDefineCount
  CSDefineLowerBound
  CSDefineUpperBound
  CSDefineEqualRange
};

template <class T, class Pr, class R>
bool operator<(const MultiSSkipList<T,Pr,R> &left, const MultiSSkipList<T,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

template <class T, class Pr, class R>
bool operator==(const MultiSSkipList<T,Pr,R> &left, const MultiSSkipList<T,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

#define csarg1 template <class T, class Pr, class R>
#define csarg2 MultiSSkipList<T,Pr,R>
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
