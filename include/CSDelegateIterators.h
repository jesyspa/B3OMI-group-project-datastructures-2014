/*
   Description: Header file for delegate container iterators.

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

#ifndef CSDelegateIteratorsH
#define CSDelegateIteratorsH

#include <algorithm>
#include <functional>
#include <utility>
#include <iterator>
#include "CSSkipListTools.h"
#include "CSCompositeTools.h"
#include "CSProperty.h"

namespace CS
{

template<unsigned int X, class container_type>
class XIterator0;

template<unsigned int X, class container_type>
class XIterator1;

template<unsigned int X, class container_type>
class YIterator0;

template<unsigned int X, class container_type>
class YIterator1;

#define CSBIDI(a,b) a
//#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) a
//#define CSKEY(a,b) b
#define CSLEVEL(a,b) a

// iterators after a deletion or insertion are invalidated
// (and only for iterators following the inserted or deleted location), but
// can be revalidated by calling refresh() on the iterator.
// Iterator preceding the inserted or deleted location remain valid.
template<unsigned int X, class container_type>
class XIterator0 : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type>
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::data_type data_type;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef XIterator0<X,container_type> self_type;

//private:

// This is public because we can't make friends of template arguments.
public:
  data_type *data;
  mutable size_type Findex;
  node_type *FNode;
public:
  XIterator0() : FNode(NULL) InitProp(index) {}
  XIterator0(const T0 &t0) : data(t0.data), FNode(t0.FNode), Findex(t0.Findex) InitProp(index) { }
  XIterator0(const T1 &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)), Findex(t1.Findex) InitProp(index) { }
  XIterator0(data_type *data, node_type* p) : data(data), FNode(p) InitProp(index) { refresh(); }
  XIterator0(data_type *data, node_type* p, size_type index) : data(data), FNode(p), Findex(index) InitProp(index) { }

  // Conversion constructors
  template<unsigned int nX, class ncontainer_type>
  XIterator0(const XIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator0(const XIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator0(const YIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator0(const YIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)) { refresh(); }

  T0& operator=(const T0 &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    this->Findex = t0.Findex;
    return *this;
  }

  T0& operator=(const T1 &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    this->Findex = t1.Findex;
    return *this;
  }

  // Define assignment operations for other iterators
  // These all refresh the index because other lists don't have the same ordering.
  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const XIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const XIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const YIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const YIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    refresh();
    return *this;
  }

  node_type* getNode() const { return FNode; }
  void setNode(node_type *n) { FNode = n; }

  container_type* getContainer() const { return dynamic_cast<container_type*>(data->Containers[X]); }

  CSXDefineIteratorEQ(T0)
  CSXDefineIteratorEQ(T1)

  CSXDefineIteratorInc(T0)
  CSXDefineIteratorDecBidi(T0)
  CSXDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSXDefineIteratorLTNodeCompare(T0)
  CSXDefineIteratorLTNodeCompare(T1)
  CSDefineIteratorPlusMinus(T0)
  CSXDefineIteratorForwardArbitraryAccess(T0)
  CSXDefineIteratorReverseArbitraryAccess(T0)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSXDefineIteratorRefreshBidi
  CSXDefineIteratorArray(T0)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

template<unsigned int X, class container_type>
class XIterator1 : public std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference >
{
public:
  typedef std::iterator<std::random_access_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::data_type data_type;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

  typedef XIterator1<X,container_type> self_type;

//private:

// This is public because we can't make friends of template arguments.
public:
  data_type *data;
  mutable size_type Findex;
  node_type *FNode;
public:
  XIterator1() : FNode(NULL) InitProp(index) {}
  XIterator1(const T1 &t1): data(t1.data), FNode(t1.FNode), Findex(t1.Findex) InitProp(index) { }
  XIterator1(const T0 &t0): data(t0.data), FNode(t0.FNode), Findex(t0.Findex) InitProp(index) { }
  XIterator1(data_type *data, node_type* p) : data(data), FNode(p) InitProp(index) { refresh(); }
  XIterator1(data_type *data, node_type* p, size_type index) : data(data), FNode(p), Findex(index) InitProp(index) { }

  // Conversion constructors
  template<unsigned int nX, class ncontainer_type>
  XIterator1(const XIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator1(const XIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(t1.FNode) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator1(const YIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) { refresh(); }
  template<unsigned int nX, class ncontainer_type>
  XIterator1(const YIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(t1.FNode) { refresh(); }

  T1& operator=(const T0 &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    this->Findex = t0.Findex;
    return *this;
  }

  T1& operator=(const T1 &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    this->Findex = t1.Findex;
    return *this;
  }

  // Define assignment operations for other iterators
  // These all refresh the index because other lists don't have the same ordering.
  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const XIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const XIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const YIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    refresh();
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const YIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    refresh();
    return *this;
  }

  node_type* getNode() const { return FNode; }
  void setNode(node_type *n) { FNode = n; }

  container_type* getContainer() const { return dynamic_cast<container_type*>(data->Containers[X]); }

  CSXDefineIteratorEQ(T0)
  CSXDefineIteratorEQ(T1)
  CSXDefineIteratorInc(T1)
  CSXDefineIteratorDecBidi(T1)
  CSXDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSXDefineIteratorLTNodeCompare(T0)
  CSXDefineIteratorLTNodeCompare(T1)
  CSDefineIteratorPlusMinus(T1)
  CSXDefineIteratorForwardArbitraryAccess(T1)
  CSXDefineIteratorReverseArbitraryAccess(T1)
  CSDefineIteratorDiff(T0)
  CSDefineIteratorDiff(T1)
  CSXDefineIteratorRefreshBidi
  CSXDefineIteratorArray(T1)
  CSDefineIteratorIndex

  PROPERTY_TYPE_GET(size_type,getIndex,index)
};

#undef CSBIDI
#undef CSLEVEL
//#undef CSKEY
//#undef CSUNIQUE
#undef CSINDEX

#define CSBIDI(a,b) a
//#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) b
//#define CSKEY(a,b) a
#define CSLEVEL(a,b) a

// Bidirectional iterators for XMultiSkipList
template<unsigned int X, class container_type>
class YIterator0 : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::data_type data_type;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

//private:

// This is public because we can't make friends of template arguments.
public:
  data_type *data;
  node_type *FNode;
public:
  YIterator0() {}
  YIterator0(const T0 &t0) : data(t0.data), FNode(t0.FNode) { }
  YIterator0(const T1 &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)) { }
  YIterator0(data_type *data, node_type* p) : data(data), FNode(p) { }

  // Conversion constructors
  template<unsigned int nX, class ncontainer_type>
  YIterator0(const XIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator0(const XIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator0(const YIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator0(const YIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(const_cast<node_type*>(t1.FNode)) {}

  T0& operator=(const T0 &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  T0& operator=(const T1 &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    return *this;
  }

  // Define assignment operations for other iterators
  // These all refresh the index because other lists don't have the same ordering.
  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const XIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const XIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const YIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T0& operator=(const YIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = const_cast<node_type*>(t1.FNode);
    return *this;
  }

  node_type* getNode() const { return FNode; }
  void setNode(node_type *n) { FNode = n; }

  container_type* getContainer() const { return dynamic_cast<container_type*>(data->Containers[X]); }

  CSXDefineIteratorEQ(T0)
  CSXDefineIteratorEQ(T1)
  CSXDefineIteratorInc(T0)
  CSXDefineIteratorDecBidi(T0)
  CSXDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSXDefineIteratorLTNodeCompare(T0)
  CSXDefineIteratorLTNodeCompare(T1)
};

template<unsigned int X, class container_type>
class YIterator1 : public std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference>
{
public:
  typedef std::iterator<std::bidirectional_iterator_tag, typename container_type::value_type, typename container_type::difference_type, const typename container_type::pointer, typename container_type::const_reference> baseclass;
  typedef typename baseclass::iterator_category iterator_category;
  typedef typename baseclass::value_type value_type;
  typedef typename baseclass::difference_type difference_type;
  typedef typename baseclass::pointer pointer;
  typedef typename baseclass::reference reference;

  typedef typename container_type::data_type data_type;
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::mapped_type mapped_type;
  typedef typename container_type::const_mapped_type const_mapped_type;
  typedef typename container_type::mapped_type_reference mapped_type_reference;
  typedef typename container_type::const_mapped_type_reference const_mapped_type_reference;

  typedef typename container_type::T0 T0;
  typedef typename container_type::T1 T1;

//private:

// This is public because we can't make friends of template arguments.
public:
  data_type *data;
  node_type *FNode;
public:
  YIterator1() {}
  YIterator1(const T1 &t1): data(t1.data), FNode(t1.FNode) { }
  YIterator1(const T0 &t0): data(t0.data), FNode(t0.FNode) { }
  YIterator1(data_type *data, node_type* p) : data(data), FNode(p) { }

  // Conversion constructors
  template<unsigned int nX, class ncontainer_type>
  YIterator1(const XIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator1(const XIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(t1.FNode) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator1(const YIterator0<nX,ncontainer_type> &t0) : data(t0.data), FNode(t0.FNode) {}
  template<unsigned int nX, class ncontainer_type>
  YIterator1(const YIterator1<nX,ncontainer_type> &t1) : data(t1.data), FNode(t1.FNode) {}

  T1& operator=(const T0 &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  T1& operator=(const T1 &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    return *this;
  }

  // Define assignment operations for other iterators
  // These all refresh the index because other lists don't have the same ordering.
  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const XIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const XIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const YIterator0<nX,ncontainer_type> &t0)
  {
    this->data = t0.data;
    this->FNode = t0.FNode;
    return *this;
  }

  template<unsigned int nX, class ncontainer_type>
  T1& operator=(const YIterator1<nX,ncontainer_type> &t1)
  {
    this->data = t1.data;
    this->FNode = t1.FNode;
    return *this;
  }

  node_type* getNode() const { return FNode; }
  void setNode(node_type *n) { FNode = n; }

  container_type* getContainer() const { return dynamic_cast<container_type*>(data->Containers[X]); }

  CSXDefineIteratorEQ(T0)
  CSXDefineIteratorEQ(T1)
  CSXDefineIteratorInc(T1)
  CSXDefineIteratorDecBidi(T1)
  CSXDefineIteratorAccess
  CSDefineIteratorDefaultCompare(T0)
  CSDefineIteratorDefaultCompare(T1)
  CSXDefineIteratorLTNodeCompare(T0)
  CSXDefineIteratorLTNodeCompare(T1)
};

#undef CSBIDI
#undef CSLEVEL
//#undef CSKEY
//#undef CSUNIQUE
#undef CSINDEX

}

#endif
