/*
   Description: Header file for XMultiAutoAccessSkipList.

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

#ifndef CSXMultiAutoAccessSkipListH
#define CSXMultiAutoAccessSkipListH

#include "CSCompositeSkipList.h"

namespace CS
{
// Next up is MultiAutoAccessSkipList with key
#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) a
#define CSKEY(a,b) a
#define CSLEVEL(a,b) a

template <unsigned int X, class K, class CT, class A=XAccessSelf<K,typename CT::value_type>, class Pr = std::less<K> >
class XMultiAutoAccessSkipList : public XList<typename CT::value_type, typename CT::node_type>
{
public:
  typedef typename CT::size_type size_type;
  typedef typename CT::difference_type difference_type;
  typedef XMultiAutoAccessSkipList<X,K,CT,A,Pr> container_type;

  typedef XIterator0<X,container_type> T0;
  typedef XIterator1<X,container_type> T1;

  friend class XIterator0<X,container_type>;
  friend class XIterator1<X,container_type>;

  typedef T0 iterator;
  typedef T1 const_iterator;

  typedef typename CT::value_type T;
  typedef T value_type;
  typedef K key_type;
  typedef value_type mapped_type;
  typedef value_type& mapped_type_reference;
  typedef const value_type const_mapped_type;
  typedef const value_type& const_mapped_type_reference;
  typedef typename CT::node_type node_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const T& const_reference;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef typename CT::data_type data_type;

  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;

  class value_compare
    : public std::binary_function<value_type, value_type, bool>
  {
  friend class XMultiAutoAccessSkipList<X,K,CT,A,Pr>;
  public:
    bool operator()(const value_type& left, const value_type& right) const
      {return (comp(a(left), a(right))); }
  protected:
    value_compare(const key_compare &pr) : comp(pr) {}
    key_compare comp;
    A a;
  };

private:
  A a;
  data_type *data;
  key_compare KeyCompare;
  value_compare ValueCompare;
  // Only scans current container.  scan(index)
  CSXDefineScanIndex
  // Scans all containers.          scanAll(index)
  CSXDefineScanAllIndex
  // Only scans current container.  scan(iter)
  CSXDefineScanIterator
  // Scans all containers.          scanAll(iter)
  CSXDefineScanAllIterator
  // scan(node) and scanAll(node)
  CSXDefineScanNode
  // scan_val(val) and scanAll_val(val)
  CSXDefineScanVal
  // scan_key(val) and scanAll_key(val)
  CSXDefineScanKey

  size_type& getItems() {return data->items;}
  void setItems(const size_type sz) {data->items = sz;}
  std::pair<size_type,node_type*>* getUpdate() {return data->update[X];}
  size_type getScanIndex() const {return data->scan_index[X];}
  void setScanIndex(const size_type sz) { data->scan_index[X] = sz;}

  typedef std::pair<size_type,node_type*>* pair_t;

  // Cannot use copy constructor
  XMultiAutoAccessSkipList(const XMultiAutoAccessSkipList &other ) { }

public:
  XCheckSkipNodes2

  XMultiAutoAccessSkipList() : XList<value_type,node_type>(), ValueCompare(Pr()) { }
  explicit XMultiAutoAccessSkipList(const key_compare& comp) : XList<value_type,node_type>(), KeyCompare(comp), ValueCompare(comp) { }
  ~XMultiAutoAccessSkipList() { data=NULL; }

  void setData(void *data)
  {
    this->data = reinterpret_cast<data_type*>(data);
  }

  data_type* getData() { return this->data; }

  CSXDefineOperatorEqual    // #

  CSXDefineBeginEnd          // #
  CSDefineRBeginEnd          // #
  CSXDefineSize               // #
  CSXDefineEmpty              // #
  CSXDefineFront             // #
  CSXDefineBackBidi          // #
  CSXDefinePopFront          // #
  CSXDefinePopBackBidi       // #
  CSDefineAssignITIT(insert) // #

  CSXDefineInsertBidiNodeKey // #
  CSXDefineInsertVal  // #
  iterator insert(const iterator &where, const value_type& val) { return insert(val); } // Don't use this.  Calls insert(const value_type& type);
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); } // UPDATE ME!!!  IS good???

  void erase(node_type *node)
  {
    if ((data->scan_index[X]!=-1)&&(data->update[X][0].second->forward(X,0)==node))
    {
      int i=0;
      for (; (i<=data->level)&&(data->update[X][i].second->forward(X,i) == node); i++)
      {
CSINDEX(data->update[X][i].second->skip(X,i) += node->skip(X,i)-1;,)
        data->update[X][i].second->forward(X,i) = node->forward(X,i);
        node->forward(X,i)->backward(X,i) = node->backward(X,i);
      }
CSINDEX(for(;i<=data->level;i++)
      {
        data->update[X][i].second->skip(X,i)--;
      },)
    }
    else
    {
      int i=0;
      for(;i<=node->level;i++)
      {
CSINDEX(node->backward(X,i)->skip(X,i) += node->skip(X,i)-1;,)
        node->backward(X,i)->forward(X,i) = node->forward(X,i);
        node->forward(X,i)->backward(X,i) = node->backward(X,i);
      }
CSINDEX(node_type *p = node->backward(X,node->level);
      for(;i<=data->level;i++)
      {
        while(p->level<i) p = p->backward(X,p->level);
        p->skip(X,i)--;
      },)
      data->scan_index[X] = -1;
    }
  }

  CSXDefineErase      // #
  CSXDefineEraseITIT  // #
  CSXDefineEraseKey   // #
  CSXDefineEraseIndex // #

  CSXDefineClear      // #
  CSXDefineDestroy    // #
  // Not implemented yet.
//  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); } // UPDATE ME!!!
  CSDefineEraseIf    // #
  CSDefineDestroyIf
  CSXDefineUnique      // #
  CSXDefineUniquePr    // #

  CSXDefineOperatorArrayIndex // #

  CSDefineKeyCompare(KeyCompare)   // #
  CSDefineValueCompare(ValueCompare) // #
  CSXDefineMaxSize               // #

  const key_type& key(const reference value) const { return a(value); } // #
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSXDefineFind       // #
  CSDefineCount       // #
  CSXDefineLowerBound  // #
  CSXDefineUpperBound  // #
  CSDefineEqualRange  // #
};

template <unsigned int X, class K, class CT, class A, class Pr>
bool operator==(const XMultiAutoAccessSkipList<X,K,CT,A,Pr> &left, const XMultiAutoAccessSkipList<X,K,CT,A,Pr> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <unsigned int X, class K, class CT, class A, class Pr>
bool operator<(const XMultiAutoAccessSkipList<X,K,CT,A,Pr> &left, const XMultiAutoAccessSkipList<X,K,CT,A,Pr> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template <unsigned int X, class K, class CT, class A, class Pr>
#define csarg2 XMultiAutoAccessSkipList<X,K,CT,A,Pr>
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