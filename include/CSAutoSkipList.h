/*
   Description: Header file for AutoSkipList and MultiAutoSkipList
                Double linked skiplists that act like a set and multiset.
                Containers support logarithmic time random access.

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

#ifndef CSAutoSkipListH
#define CSAutoSkipListH

#include <functional>
#include <iterator>
#include "CSSkipListTools.h"
#include "CSIterators.h"

namespace CS
{
#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) a
#define CSINDEX(a,b) a
#define CSKEY(a,b) a
#define CSLEVEL(a,b) a

template <class T, class Pr = std::less<T>, class R = RNG >
class AutoSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef AutoSkipList<T,Pr,R> container_type;
  typedef BidiIdxIterator<container_type> T0;
  typedef ConstBidiIdxIterator<container_type> T1;
  friend class BidiIdxIterator<container_type>;
  friend class ConstBidiIdxIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef BidiIdxNode<value_type> node_type;
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
  mutable size_type scan_index;
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const T &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanVal
  CSDefineScanKey
  CSDefineScanIndex
  CSDefineScanNode
  CSDefineScanIterator
public:

  CheckSkipNodes

  AutoSkipList() { CSInitDefault; }
  explicit AutoSkipList(size_type maxNodes) { CSInitMaxNodes; }
  AutoSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  AutoSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> AutoSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AutoSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AutoSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit AutoSkipList(const key_compare& comp) : Compare(comp) { CSInitDefault; }
  template<class InIt> AutoSkipList(InIt first, InIt last, const key_compare& comp) : Compare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AutoSkipList(InIt first, InIt last, const key_compare& comp, double probability, unsigned int maxLevel) : Compare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> AutoSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : Compare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~AutoSkipList() { clear(); }
  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackBidi
  CSDefinePopFront
  CSDefinePopBackBidi
  CSDefineAssignITIT(insert)

  CSDefineInsertVal
  iterator insert(const iterator &where, const value_type& val) { return insert(val).first; } // Don't use this.  Calls insert(const value_type& type);
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); }

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseKey
  CSDefineEraseIndex

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut
  CSDefineOperatorArrayIndex

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
bool operator==(const AutoSkipList<T,Pr,R> &left, const AutoSkipList<T,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class T, class Pr, class R>
bool operator<(const AutoSkipList<T,Pr,R> &left, const AutoSkipList<T,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

#define csarg1 template <class T, class Pr, class R>
#define csarg2 AutoSkipList<T,Pr,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSUNIQUE
#define CSUNIQUE(a,b) b


template <class T, class Pr = std::less<T>, class R = RNG >
class MultiAutoSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef MultiAutoSkipList<T,Pr,R> container_type;
  typedef BidiIdxIterator<container_type> T0;
  typedef ConstBidiIdxIterator<container_type> T1;
  friend class BidiIdxIterator<container_type>;
  friend class ConstBidiIdxIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef BidiIdxNode<value_type> node_type;
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
  mutable size_type scan_index;
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const T &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanVal
  CSDefineScanKey
  CSDefineScanIndex
  CSDefineScanNode
  CSDefineScanIterator
public:

  CheckSkipNodes

  MultiAutoSkipList() { CSInitDefault; }
  explicit MultiAutoSkipList(size_type maxNodes) { CSInitMaxNodes; }
  MultiAutoSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  MultiAutoSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),insert); }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  explicit MultiAutoSkipList(const key_compare& comp) : Compare(comp) { CSInitDefault; }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last, const key_compare& comp) : Compare(comp) { CSInitDefault; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last, const key_compare& comp, double probability, unsigned int maxLevel) : Compare(comp) { CSInitPM; CSCopyITIT(InIt, first,last,insert); }
  template<class InIt> MultiAutoSkipList(InIt first, InIt last, const key_compare& comp, size_type maxNodes) : Compare(comp) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,insert); }
  ~MultiAutoSkipList() { clear(); }
  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackBidi
  CSDefinePopFront
  CSDefinePopBackBidi
  CSDefineAssignITIT(insert)

  CSDefineInsertVal
  iterator insert(const iterator &where, const value_type& val) { return insert(val); } // Don't use this.  Calls insert(const value_type& type);
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); }

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseKey
  CSDefineEraseIndex

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); }
  CSDefineEraseIf
  CSDefineDestroyIf

  CSDefineCut
  CSDefineOperatorArrayIndex

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
bool operator<(const MultiAutoSkipList<T,Pr,R> &left, const MultiAutoSkipList<T,Pr,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),left.value_comp());
}

template <class T, class Pr, class R>
bool operator==(const MultiAutoSkipList<T,Pr,R> &left, const MultiAutoSkipList<T,Pr,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

#define csarg1 template <class T, class Pr, class R>
#define csarg2 MultiAutoSkipList<T,Pr,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSLEVEL
#undef CSKEY
#undef CSINDEX
#undef CSUNIQUE
#undef CSBIDI
}


#endif
