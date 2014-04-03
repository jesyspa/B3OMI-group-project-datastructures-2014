/*
   Description: Header file for XMultiSkipList.

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

#ifndef CSXMultiSkipListH
#define CSXMultiSkipListH

#include "CSCompositeSkipList.h"

namespace CS
{
#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) b
#define CSKEY(a,b) a
#define CSLEVEL(a,b) a

template <unsigned int X, class CT, class Pr = std::less<typename CT::value_type> >
class XMultiSkipList : public XList<typename CT::value_type, typename CT::node_type>
{
public:
  typedef typename CT::size_type size_type;
  typedef typename CT::difference_type difference_type;
  typedef XMultiSkipList<X,CT,Pr> container_type;

  typedef YIterator0<X,container_type> T0;
  typedef YIterator1<X,container_type> T1;

  friend class YIterator0<X,container_type>;
  friend class YIterator1<X,container_type>;

  typedef T0 iterator;
  typedef T1 const_iterator;

  typedef typename CT::value_type T;
  typedef T value_type;
  typedef value_type key_type;
  typedef value_type mapped_type;
  typedef value_type& mapped_type_reference;
  typedef const value_type const_mapped_type;
  typedef const value_type& const_mapped_type_reference;
  typedef typename CT::node_type node_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef typename CT::data_type data_type;

  typedef std::pair<iterator, bool> slpair;
  typedef std::pair<iterator, iterator> ipair;
  typedef std::pair<const_iterator, const_iterator> const_ipair;
  typedef Pr key_compare;
  typedef Pr value_compare;

private:
  data_type *data;
  Pr Compare;
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
  XMultiSkipList(const XMultiSkipList &other ) { }

public:
  XCheckSkipNodes2

  XMultiSkipList() : XList<value_type,node_type>() { }
  explicit XMultiSkipList(const key_compare& comp) : XList<value_type,node_type>(), Compare(comp) { }
  ~XMultiSkipList() { data=NULL; }

  void setData(void *data)
  {
    this->data = reinterpret_cast<data_type*>(data);
  }

  data_type* getData() { return this->data; }

  CSXDefineOperatorEqual // #

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
  template<class InIt> void insert(InIt first, InIt last) { CSCopyITIT(InIt, first, last, insert); } // UPDATE ME!!!  Is good???

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

  CSXDefineClear      // #
  CSXDefineDestroy    // #
  // Not implemented yet.
//  void swap(container_type& right) { CSSwapCore std::swap(Compare, right.Compare); } // UPDATE ME!!!
  CSDefineEraseIf    // #
  CSDefineDestroyIf
  CSXDefineUnique      // #
  CSXDefineUniquePr    // #

  CSDefineKeyCompare(Compare)   // #
  CSDefineValueCompare(Compare) // #
  CSXDefineMaxSize               // #

  const key_type& key(const_reference value) const {return value;} // #
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}
  CSXDefineFind       // #
  CSDefineCount       // #
  CSXDefineLowerBound // #
  CSXDefineUpperBound // #
  CSDefineEqualRange  // #
};

template <unsigned int X, class CT, class Pr>
bool operator==(const XMultiSkipList<X,CT,Pr> &left, const XMultiSkipList<X,CT,Pr> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <unsigned int X, class CT, class Pr>
bool operator<(const XMultiSkipList<X,CT,Pr> &left, const XMultiSkipList<X,CT,Pr> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template<unsigned int X, class CT, class Pr>
#define csarg2 XMultiSkipList<X,CT,Pr>
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
