/*
   Description: Header file for IndexedSkipList, IndexedSSkipList and
                IndexedTinySkiplist.
                Skiplists that act like a vector with logn time operations.
                IndexedSkipList is a double linked list.
                IndexedSSkipList is a single linked list.
                IndexedTinySkipList is a single linked list with smaller
                memory footprint (no level count in nodes).

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
#ifndef CSIndexedSkipListH
#define CSIndexedSkipListH

#include <functional>
#include <utility>
#include <algorithm>
#include <iterator>
#include "CSSkipListTools.h"
#include "CSIterators.h"

namespace CS
{
#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) a
#define CSKEY(a,b) b
#define CSLEVEL(a,b) a

template <class T, class R = RNG>
class IndexedSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef IndexedSkipList<T,R> container_type;
  typedef BidiIdxIterator<container_type> T0;
  typedef ConstBidiIdxIterator<container_type> T1;
  friend class BidiIdxIterator<container_type>;
  friend class ConstBidiIdxIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef value_type mapped_type;
  typedef value_type& mapped_type_reference;
  typedef const value_type const_mapped_type;
  typedef const value_type& const_mapped_type_reference;
  typedef BidiIdxNode<value_type> node_type;
  typedef T0 iterator;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const T& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  R rng;
  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable size_type scan_index;
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanIndex
  CSDefineScanIterator

public:

  struct Prob
  {
    double probability;
    unsigned int maxLevel;
    explicit Prob(size_type maxNodes) : probability(0.25), maxLevel((int)ceil(log((double)maxNodes)/log(1.0/0.25))-1) {}
    Prob(double probability, unsigned int maxLevel) : probability(probability), maxLevel(maxLevel) {}
  };

  CheckSkipNodes

  // Good constructors
  IndexedSkipList() { CSInitDefault; }
  IndexedSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  IndexedSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),push_back); }
  template<class InIt> IndexedSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,push_back); }
  explicit IndexedSkipList(size_type count) { CSInitDefault; CSCreateXItems(value_type,count,push_back) }
  IndexedSkipList(size_type count, const T& val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSkipList(size_type count, const T& val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSkipList(size_type count, const T& val, size_type maxNodes) { CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }

  // Used with Prob
  explicit IndexedSkipList(const Prob &prob) { CSInitProb; }
  IndexedSkipList(size_type count, const Prob &prob) { CSInitProb; CSCreateXItems(value_type,count,push_back) }
  IndexedSkipList(size_type count, const T& val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  template<class InIt> IndexedSkipList(InIt first, InIt last, const Prob &prob) { CSInitProb; CSCopyITIT(InIt, first,last,push_back); }

  ~IndexedSkipList() { clear(); }

  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackBidi
  CSDefinePopFront
  CSDefinePopBackBidi
  CSDefineAssignITIT(push_back)

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseIndex

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore }
  CSDefineEraseIf
  CSDefineDestroyIf
  mapped_type& value(value_type& value) const {return value;}
  const_mapped_type& value(const value_type& value) const {return value;}

  CSDefineCut

  CSDefineOperatorArrayIndex

  CSDefineMaxSize

  void assign(size_type count, const T& val) { clear(); resize(count,val); }

  CSDefineResize
  CSDefinePushFront
  CSDefinePushBackBidi

  CSDefineInsertITVal
  CSDefineInsertITSizeVal
  CSDefineInsertITITIT

  CSDefineReverse
  CSDefineUnique
  CSDefineUniquePr
  CSDefineSort
  CSDefineSortPr
  CSDefineStableSort
  CSDefineStableSortPr
  CSDefineEraseValue

  CSDefineSplice
  CSDefineSpliceIT
  CSDefineSpliceITIT

// These compilers incorrectly pick the wrong method.
// So the methods below are used to override this incorrect selection.
#ifdef __BORLANDC__
#if /* (__BORLANDC__ >= 0x0580) && */ (__BORLANDC__ <= 0x582)
  IndexedSkipList(size_type count, size_type val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSkipList(size_type count, size_type val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSkipList(size_type count, size_type val, size_type maxNodes) {CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSkipList(size_type count, size_type val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  void assign(size_type count, size_type val) { clear(); resize(count,val); }
  void assign(size_type count, int val) { clear(); resize(count,val); }

  void insert(const iterator &where, unsigned int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, unsigned int count, int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, int val)
  {
    CSXInsertITSizeVal
  }

#endif
#endif

};

template <class T, class R>
bool operator==(const IndexedSkipList<T,R> &left, const IndexedSkipList<T,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class T, class R>
bool operator<(const IndexedSkipList<T,R> &left, const IndexedSkipList<T,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template<class T, class R>
#define csarg2 IndexedSkipList<T,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSBIDI
#define CSBIDI(a,b) b

template <class T, class R = RNG>
class IndexedSSkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef IndexedSSkipList<T,R> container_type;
  typedef ForwardIdxIterator<container_type> T0;
  typedef ConstForwardIdxIterator<container_type> T1;
  friend class ForwardIdxIterator<container_type>;
  friend class ConstForwardIdxIterator<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef value_type mapped_type;
  typedef value_type& mapped_type_reference;
  typedef const value_type const_mapped_type;
  typedef const value_type& const_mapped_type_reference;
  typedef ForwardIdxNode<value_type> node_type;
  typedef T0 iterator;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const T& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  R rng;
  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable size_type scan_index;
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanIndex
  CSDefineScanIterator

public:

  struct Prob
  {
    double probability;
    unsigned int maxLevel;
    explicit Prob(size_type maxNodes) : probability(0.25), maxLevel((int)ceil(log((double)maxNodes)/log(1.0/0.25))-1) {}
    Prob(double probability, unsigned int maxLevel) : probability(probability), maxLevel(maxLevel) {}
  };

  CheckSkipNodes

  // Good constructors
  IndexedSSkipList() { CSInitDefault; }
  IndexedSSkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  IndexedSSkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, source.begin(),source.end(),push_back); }
  template<class InIt> IndexedSSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedSSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedSSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,push_back); }
  explicit IndexedSSkipList(size_type count) { CSInitDefault; CSCreateXItems(value_type,count,push_back) }
  IndexedSSkipList(size_type count, const T& val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSSkipList(size_type count, const T& val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSSkipList(size_type count, const T& val, size_type maxNodes) { CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }

  // Used with Prob
  explicit IndexedSSkipList(const Prob &prob) { CSInitProb; }
  IndexedSSkipList(size_type count, const Prob &prob) { CSInitProb; CSCreateXItems(value_type,count,push_back) }
  IndexedSSkipList(size_type count, const T& val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  template<class InIt> IndexedSSkipList(InIt first, InIt last, const Prob &prob) { CSInitProb; CSCopyITIT(InIt, first,last,push_back); }

  ~IndexedSSkipList() { clear(); }

  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackForward
  CSDefinePopFront
  CSDefinePopBackForwardIdx
  CSDefineAssignITIT(push_back)

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseIndex

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore }
  CSDefineEraseIf
  CSDefineDestroyIf
  mapped_type& value(value_type& value) const {return value;}
  const_mapped_type& value(const value_type& value) const {return value;}

  CSDefineCut
  CSDefineOperatorArrayIndex

  CSDefineMaxSize

  void assign(size_type count, const T& val) { clear(); resize(count,val); }

  CSDefineResize
  CSDefinePushFront
  CSDefinePushBackForward
  CSDefineInsertITVal
  CSDefineInsertITSizeVal
  CSDefineInsertITITIT

  CSDefineReverse
  CSDefineUnique
  CSDefineUniquePr
  CSDefineSort
  CSDefineSortPr
  CSDefineStableSort
  CSDefineStableSortPr
  CSDefineEraseValue

  CSDefineSplice
  CSDefineSpliceIT
  CSDefineSpliceITIT

// These compilers incorrectly pick the wrong method.
// So the methods below are used to override this incorrect selection.
#ifdef __BORLANDC__
#if /*(__BORLANDC__ >= 0x0580) &&*/ (__BORLANDC__ <= 0x582)
  IndexedSSkipList(size_type count, size_type val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSSkipList(size_type count, size_type val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSSkipList(size_type count, size_type val, size_type maxNodes) {CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedSSkipList(size_type count, size_type val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  void assign(size_type count, size_type val) { clear(); resize(count,val); }
  void assign(size_type count, int val) { clear(); resize(count,val); }

  void insert(const iterator &where, unsigned int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, unsigned int count, int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, int val)
  {
    CSXInsertITSizeVal
  }

#endif
#endif

};

template <class T, class R>
bool operator==(const IndexedSSkipList<T,R> &left, const IndexedSSkipList<T,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class T, class R>
bool operator<(const IndexedSSkipList<T,R> &left, const IndexedSSkipList<T,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template<class T, class R>
#define csarg2 IndexedSSkipList<T,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2


#undef CSLEVEL
#define CSLEVEL(a,b) b

template <class T, class R = RNG>
class IndexedTinySkipList
{
public:
  typedef CSUNIQUE(CSKEY(uniquekey_tag,unique_tag),CSKEY(multikey_tag,multi_tag)) tag;
  typedef IndexedTinySkipList<T,R> container_type;
  typedef ForwardIdxIteratorNL<container_type> T0;
  typedef ConstForwardIdxIteratorNL<container_type> T1;
  friend class ForwardIdxIteratorNL<container_type>;
  friend class ConstForwardIdxIteratorNL<container_type>;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef value_type mapped_type;
  typedef value_type& mapped_type_reference;
  typedef const value_type const_mapped_type;
  typedef const value_type& const_mapped_type_reference;
  typedef ForwardIdxTinyNode<value_type> node_type;
  typedef T0 iterator;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const T& const_reference;
  typedef T1 const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  R rng;
  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.
  mutable size_type scan_index;
  mutable std::pair<size_type,node_type*> *update;
  CSDefineInit
  node_type* Alloc(unsigned int level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item,node_type)
  CSDefineGenerateRandomLevel
  CSDefineAdjustLevels
  CSDefineScanIndex
  CSDefineScanIterator

public:

  struct Prob
  {
    double probability;
    unsigned int maxLevel;
    explicit Prob(size_type maxNodes) : probability(0.25), maxLevel((int)ceil(log((double)maxNodes)/log(1.0/0.25))-1) {}
    Prob(double probability, unsigned int maxLevel) : probability(probability), maxLevel(maxLevel) {}
  };

  CheckSkipNodes

  // Good constructors
  IndexedTinySkipList() { CSInitDefault; }
  IndexedTinySkipList(double probability, unsigned int maxLevel) { CSInitPM; }
  IndexedTinySkipList(const container_type &source) { CSInitCore(source.probability, source.maxLevel)  CSCopyITIT(const_iterator, source.begin(),source.end(),push_back); }
  template<class InIt> IndexedTinySkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedTinySkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,push_back); }
  template<class InIt> IndexedTinySkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,push_back); }
  explicit IndexedTinySkipList(size_type count) { CSInitDefault; CSCreateXItems(value_type,count,push_back) }
  IndexedTinySkipList(size_type count, const T& val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedTinySkipList(size_type count, const T& val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedTinySkipList(size_type count, const T& val, size_type maxNodes) { CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }

  // Used with Prob
  explicit IndexedTinySkipList(const Prob &prob) { CSInitProb; }
  IndexedTinySkipList(size_type count, const Prob &prob) { CSInitProb; CSCreateXItems(value_type,count,push_back) }
  IndexedTinySkipList(size_type count, const T& val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  template<class InIt> IndexedTinySkipList(InIt first, InIt last, const Prob &prob) { CSInitProb; CSCopyITIT(InIt, first,last,push_back); }

  ~IndexedTinySkipList() { clear(); }

  CSDefineOperatorEqual

  CSDefineBeginEnd
  CSDefineRBeginEnd
  CSDefineSize
  CSDefineEmpty
  CSDefineFront
  CSDefineBackForward
  CSDefinePopFront
  CSDefinePopBackForwardIdx
  CSDefineAssignITIT(push_back)

  CSDefineErase
  CSDefineEraseITIT
  CSDefineEraseIndex

  CSDefineClear
  CSDefineDestroy
  void swap(container_type& right) { CSSwapCore }
  CSDefineEraseIf
  CSDefineDestroyIf
  mapped_type& value(value_type& value) const {return value;}
  const_mapped_type& value(const value_type& value) const {return value;}

  CSDefineCut
  CSDefineOperatorArrayIndex

  CSDefineMaxSize

  void assign(size_type count, const T& val) { clear(); resize(count,val); }

  CSDefineResize
  CSDefinePushFront
  CSDefinePushBackForward

  CSDefineInsertITVal
  CSDefineInsertITSizeVal
  CSDefineInsertITITIT

  CSDefineReverseForward
  CSDefineUnique
  CSDefineUniquePr
  CSDefineSort
  CSDefineSortPr
  CSDefineStableSort
  CSDefineStableSortPr
  CSDefineEraseValue

  CSDefineSplice
  CSDefineSpliceIT
  CSDefineSpliceITIT

// These compilers incorrectly pick the wrong method.
// So the methods below are used to override this incorrect selection.
#ifdef __BORLANDC__
#if /*(__BORLANDC__ >= 0x0580) &&*/ (__BORLANDC__ <= 0x582)
  IndexedTinySkipList(size_type count, size_type val) { CSInitDefault; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedTinySkipList(size_type count, size_type val, double probability, unsigned int maxLevel) { CSInitPM; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedTinySkipList(size_type count, size_type val, size_type maxNodes) {CSInitMaxNodes; CSCreateXItemsVal(value_type,val,count,push_back) }
  IndexedTinySkipList(size_type count, size_type val, const Prob &prob) { CSInitProb; CSCreateXItemsVal(value_type,val,count,push_back) }
  void assign(size_type count, size_type val) { clear(); resize(count,val); }
  void assign(size_type count, int val) { clear(); resize(count,val); }

  void insert(const iterator &where, unsigned int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, unsigned int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, unsigned int count, int val)
  {
    CSXInsertITSizeVal
  }

  void insert(const iterator &where, int count, int val)
  {
    CSXInsertITSizeVal
  }

#endif
#endif

};

template <class T, class R>
bool operator==(const IndexedTinySkipList<T,R> &left, const IndexedTinySkipList<T,R> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <class T, class R>
bool operator<(const IndexedTinySkipList<T,R> &left, const IndexedTinySkipList<T,R> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template<class T, class R>
#define csarg2 IndexedTinySkipList<T,R>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSLEVEL
#undef CSKEY
#undef CSBIDI
#undef CSUNIQUE
#undef CSINDEX

}

#endif

