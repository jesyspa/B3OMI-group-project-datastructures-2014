/*
   Description: Header file for XIndexedSkipList.

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

#ifndef CSXIndexedSkipListH
#define CSXIndexedSkipListH

#include "CSCompositeSkipList.h"

namespace CS
{

#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) a
#define CSKEY(a,b) b
#define CSLEVEL(a,b) a

template <unsigned int X, class CT>
class XIndexedSkipList : public XList<typename CT::value_type,typename CT::node_type>
{
public:
  typedef typename CT::size_type size_type;
  typedef typename CT::difference_type difference_type;
  typedef XIndexedSkipList<X,CT> container_type;

  typedef XIterator0<X,container_type> T0;
  typedef XIterator1<X,container_type> T1;

  friend class XIterator0<X,container_type>;
  friend class XIterator1<X,container_type>;

  typedef T0 iterator;
  typedef T1 const_iterator;

  typedef typename CT::value_type value_type;
  typedef value_type T;
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

  typedef XTmpContainer<size_type, node_type> tmp_container_type;

private:
  data_type *data;
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
  size_type getItems() const {return data->items;}
  void setItems(const size_type sz) {data->items = sz;}
  std::pair<size_type,node_type*>* getUpdate() const {return data->update[X];}
  size_type getScanIndex() const {return data->scan_index[X];}
  void setScanIndex(const size_type sz) { data->scan_index[X] = sz;}

  typedef std::pair<size_type,node_type*>* pair_t;

  // Cannot use copy constructor
  XIndexedSkipList(const XIndexedSkipList &other ) { }

public:
  XCheckSkipNodes2

  // Good constructors
  XIndexedSkipList() : XList<value_type,node_type>() { }
  ~XIndexedSkipList() { data=NULL; }

  void setData(void *data)
  {
    this->data = reinterpret_cast<data_type*>(data);
  }

  data_type* getData() { return this->data; }

  CSXDefineOperatorEqual

  CSXDefineBeginEnd
  CSDefineRBeginEnd
  CSXDefineSize
  CSXDefineEmpty
  CSXDefineFront
  CSXDefineBackBidi
  CSXDefinePopFront
  CSXDefinePopBackBidi
  CSDefineAssignITIT(push_back)

  void assign(size_type count, const T& val) { clear(); resize(count,val); }

  // Adds node to end of list.
  CSXDefineInsertBidiIdxNode

  iterator insert(const T &val)
  {
    push_back(val);
    return end()-1;
  }

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

  CSXDefineErase
  CSXDefineEraseITIT
  CSXDefineEraseIndex

  CSXDefineClear
  CSXDefineDestroy

  // Not implemented yet.
//  void swap(container_type& right) { CSSwapCore }
  mapped_type_reference value(reference value) const {return value;}
  const_mapped_type_reference value(const_reference value) const {return value;}

  CSXDefineOperatorArrayIndex

  CSXDefineMaxSize

  CSXDefineResize
  CSXDefinePushFront
  CSXDefinePushBackBidi // Creates node and then calls insert on all lists.

  CSXDefineInsertITVal
  CSXDefineInsertITSizeVal // SLOW!  Inserts one at a time.
  CSXDefineInsertITITIT

  CSXDefineReverse
  CSXDefineUnique
  CSXDefineUniquePr
  CSXDefineSort
  CSXDefineSortPr
  CSXDefineStableSort
  CSXDefineStableSortPr
  CSDefineEraseValue
  CSDefineEraseIf
  CSDefineDestroyIf

  // Need to define move operations.
  // Use these functions if you want to move items around without affecting
  // the other containers.
  // It is MUCH faster than erase and insert's.
  // 1. Move one item to another location.
  // 2. Move a range of items to another location.
  // 3. Swap two items.
  // 4. Swap two ranges.

  // All iterators continue to point to the same elements in their new locations.
  // 'last' iterators point to one beyond the range, so these iterators will no
  // longer specify the end of the range after a move or swap.  But they will
  // all remain valid for use.

  // Moves source element to position immediately before dest.
  // All iterators between source and dest will be invalidated.
  // but can be revalidated with refresh.
  void move(const iterator &dest, const iterator &source)
  {
    if ((dest==source)||(dest-source==1)) return;
    bool adjust = (source<dest);

    cut(source,*data->tmp_container);
    if (adjust) dest.Findex--;
    splice(dest,*data->tmp_container);
    source.Findex = dest.Findex-1;
  }

  // Moves [first,last) range to immediately before dest.
  // After call [first,dest) is new range.
  // 'last' still points to same element.
  // All other iterators after lowest index of 'dest', 'first' and 'last' are invalidated,
  // but can be revalidated with refresh.
  void move(const iterator &dest, const iterator &first, const iterator &last)
  {
    if ((dest>=first)&&(dest<=last)) return;
    if ((first==last)||(last==dest)) return;
    if (last-first==1) { move(dest,first); return; }

    bool adjust = (dest>last);
    size_type diff = last-first;

    cut(first,last,*data->tmp_container);
    if (adjust) { dest.Findex-=diff; } else { last.Findex+=diff; }
    splice(dest,*data->tmp_container);
    first.Findex = dest.Findex-diff;
  }

  // Swap the position of the elements.
  // TODO: Optimization if left.node->level==right.node->level, just swap the pointers and set scan_index=-1.
  // Iterators keep pointing to same element, but in new location.
  // All other iterators remain valid.
  void swap(const iterator &left, const iterator &right)
  {
    if (left==right) return;
    if (left>right) { swap(right,left); return; }
    if (right-left == 1) { move(left,right); return; }

    size_type tmp_left = left.Findex;
    size_type tmp_right = right.Findex;

    iterator a = right+1;

    // Move right to left.
    cut(right,*data->tmp_container);
    splice(left,*data->tmp_container);

    // Move left to original left (a).
    cut(left,*data->tmp_container);
    a.Findex--;
    splice(a,*data->tmp_container);

    // Swap original indexes.
    left.Findex = tmp_right;
    right.Findex = tmp_left;
  }

  // Swap the position of the ranges [first1,last1) with [first2,last2)
  // New ranges will be [first2,last1) and [first1,last2) unless last1==first2 or last2==first1
  // Remember that iterators always point to same elements.  Only their index changes.
  // If last1==first2,
  // New ranges will be [first2,first1) and [first1,last2)
  // If last2==first1,
  // Now ranges will be [first1,first2) and [first2,last1)
  // If both ranges have the same number of elements, all iterators outside of
  // both ranges remain valid.  Iterators pointing to 'last1' or 'last2' also remain valid.
  // For inequal size ranges, all other iterators after lowest index of
  // 'first1', 'last1', 'first2' and 'last2' are invalidated,
  // but can be revalidated with refresh.
  void swap(const iterator &first1, const iterator &last1, const iterator &first2, const iterator &last2)
  {
    if (((first1>=first2)&&(first1<=last2))||((last1>first2)&&(last1<=last2))) return;
    if (first1>first2) { swap(first2,last2,first1,last1); return; }

    if (last1==first2) { move(first1,first2,last2); return; }

    // Calc new locations.
    size_type diff1 = last1-first1;
    size_type diff2 = last2-first2;

    size_type loc_first1 = first1.Findex;
    size_type loc_last1 = last1.Findex-diff1+diff2;
    size_type loc_first2 = first2.Findex-diff1+diff2;
    size_type loc_last2 = last2.Findex;

    cut(first2,last2,*data->tmp_container);
    splice(first1,*data->tmp_container);

    last1.Findex+=diff2;
    last2.Findex+=diff2;

    cut(first1,last1,*data->tmp_container);
    last2.Findex-=diff1;
    splice(last2,*data->tmp_container);

    // Reset valid indexes.
    first2.Findex = loc_first1;
    last1.Findex = loc_last1;
    first1.Findex = loc_first2;
    last2.Findex = loc_last2;
  }

  // These are internal functions for moving elements around.
void cut(const iterator &first, const iterator &last, tmp_container_type& right)
{
  if (first==last) return;
  if (last-first==1) { cut(first, right); last.Findex--; return; }

  unsigned int cur_level = data->level;

  if (cur_level>right.maxLevel)
    throw level_exception();

  difference_type diff = last.Findex-first.Findex;

  if (data->scan_index[X]!=first.Findex) scan(first.Findex);

  std::pair<size_type,node_type*> *upd = data->update[X];
  node_type *cursor = upd[cur_level].second;

  size_type total = upd[cur_level].first;
  size_type tmp_total;
  right.level = cur_level;
  right.head->level = cur_level;
  right.tail->level = cur_level;
  for (int i=cur_level; i>=0; i--)
  {
    while ((tmp_total=total+cursor->skip(X,i))<last.Findex)
    {
      total=tmp_total;
      cursor = cursor->forward(X,i);
    }

    /* Create the link */
    if (upd[i].second==cursor)
    {
      cursor->skip(X,i) -= diff;
      right.head->forward(X,i) = right.tail;
      right.tail->backward(X,i) = right.head;
      right.head->skip(X,i) = diff+1;
    }
    else
    {
      node_type *t;
      node_type* &t2 = upd[i].second;
      t = (right.head->forward(X,i) = t2->forward(X,i));
      t->backward(X,i) = right.head;
      size_type n = t2->skip(X,i)-(first.Findex-upd[i].first)+1;
      right.head->skip(X,i) = n;
      t = (t2->forward(X,i) = cursor->forward(X,i));
      t->backward(X,i) = t2;
      t2->skip(X,i) = first.Findex-upd[i].first+cursor->skip(X,i)-(last.Findex-total);
      cursor->forward(X,i) = right.tail;
      right.tail->backward(X,i) = cursor;
      cursor->skip(X,i) = last.Findex-total;
    }
  }

  last.Findex = first.Findex;
  right.items = diff;
  data->items -= diff;
}

// Removes a single item
void cut(const iterator &where, tmp_container_type& right)
{
  if (where==end()) return; // Not really necessary.  We're not here to hold their hands.

  unsigned int cur_level = data->level;

  if (cur_level>right.maxLevel)
    throw level_exception();

  if (data->scan_index[X]!=where.Findex) scan(where.Findex);

  std::pair<size_type,node_type*> *upd = data->update[X];
  node_type *cursor = upd[0].second->forward(X,0);

  // We need to remove cursor and put it into right.
  node_type *rhead = right.head;
  node_type *rtail = right.tail;
  right.level = cur_level;
  rhead->level = cur_level;
  rtail->level = cur_level;
  unsigned int limit = cursor->level;
  int i=cur_level;
  for(; i>limit; i--)
  {
    rhead->forward(X,i) = rtail;
    rtail->backward(X,i) = rhead;
    rhead->skip(X,i) = 2;
    upd[i].second->skip(X,i)--;
  }
  for(;i>=0;i--)
  {
    // Put cursor in 'right'
    rhead->forward(X,i) = cursor;
    rhead->skip(X,i) = 1;
    rtail->backward(X,i) = cursor;

    // Update original list.
    node_type* &cf = cursor->forward(X,i);
    node_type *updt = upd[i].second;
    updt->forward(X,i) = cf;
    updt->skip(X,i) += cursor->skip(X,i)-1;
    cf->backward(X,i) = updt;

    // Update cursor
    cf = rtail;
    cursor->skip(X,i) = 1;
    cursor->backward(X,i) = rhead;
  }

  right.items = 1;
  data->items--;
}

void splice(const iterator &where, tmp_container_type& right)
{
  if (right.level>data->maxLevel)
    throw level_exception();

  if (data->scan_index[X]!=where.Findex) scan(where.Findex);
  std::pair<size_type,node_type*> *upd = data->update[X];

  /* Add list into here. */
  /* right.level should equal data->level */
  int i=0;
  unsigned int limit = right.level;

  for(;i<=limit;i++)
  {
    node_type *t;
    if ((t=right.tail->backward(X,i))==right.head) break;

    node_type* &t1=upd[i].second;
    node_type* &t2=t1->forward(X,i);
    t->forward(X,i) = t2;
    size_type &v = t->skip(X,i);
    v += upd[i].first+t1->skip(X,i)-where.Findex;
    t2->backward(X,i) = t;

    t2 = right.head->forward(X,i);
    t2->backward(X,i) = upd[i].second;
    t1->skip(X,i) = where.Findex-upd[i].first+right.head->skip(X,i)-1;
  }
  size_type sz = right.size();
  for(;i<=limit;i++)
  {
    upd[i].second->skip(X,i)+=sz;
  }

  data->items += sz;
  where.Findex+=sz;

  right.head->forward(X,0) = right.tail;
  right.tail->backward(X,0) = right.head;
  right.head->skip(X,0) = 1;
  right.head->level = 0;
  right.tail->level = 0;
  right.level = 0;
  right.items = 0;
  data->scan_index[X] = -1; // This might still be good.
}

// These compilers incorrectly pick the wrong method.
// So the methods below are used to override this incorrect selection.
#ifdef __BORLANDC__
#if /*(__BORLANDC__ >= 0x0580) && */ (__BORLANDC__ <= 0x582)
  void assign(size_type count, size_type val) { clear(); resize(count,val); }
  void assign(size_type count, int val) { clear(); resize(count,val); }

  void insert(const iterator &where, unsigned int count, unsigned int val)
  {
    CSXXInsertITSizeVal
  }

  void insert(const iterator &where, int count, unsigned int val)
  {
    CSXXInsertITSizeVal
  }

  void insert(const iterator &where, unsigned int count, int val)
  {
    CSXXInsertITSizeVal
  }

  void insert(const iterator &where, int count, int val)
  {
    CSXXInsertITSizeVal
  }

#endif
#endif


};

template <unsigned int X, class CT>
bool operator==(const XIndexedSkipList<X,CT> &left, const XIndexedSkipList<X,CT> &right)
{
  return ((left.size() == right.size()) &&
          (std::equal(left.begin(), left.end(), right.begin())));

}

template <unsigned int X, class CT>
bool operator<(const XIndexedSkipList<X,CT> &left, const XIndexedSkipList<X,CT> &right)
{
  return lexicographical_compare(left.begin(),left.end(),right.begin(),right.end());
}

#define csarg1 template<unsigned int X, class CT>
#define csarg2 XIndexedSkipList<X,CT>
CSDefineCompOps(csarg1, csarg2)
#undef csarg1
#undef csarg2

#undef CSBIDI
#undef CSLEVEL
#undef CSKEY
#undef CSUNIQUE
#undef CSINDEX

}

#endif
