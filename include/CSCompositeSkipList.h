/*
   Description: Header file for XBidiCompositeContainer,
                XIndexedSkipList, XMultiSkipList, XMultiAutoSkipList.
                XBidiCompositeContainer is a base class for custom containers
                that can order the same elements in many difference ways.
                The other containers are delegate containers used to control
                each ordering.

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

#ifndef CSCompositeSkipListH
#define CSCompositeSkipListH

#include <algorithm>
#include <functional>
#include <utility>
#include <iterator>
#include "CSSkipListTools.h"
#include "CSCompositeTools.h"
#include "CSProperty.h"
#include "CSDelegateIterators.h"

namespace CS
{
// Some prototypes for sorting.
template<unsigned int X, class container_type, class Pr>
void sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last, const Pr pred);

template<unsigned int X, class container_type, class Pr>
void stable_sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last, const Pr pred);

template<unsigned int X, class container_type>
void sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last);

template<unsigned int X, class container_type>
void stable_sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last);

// Base type for delegate containers.
template <class T, class NT>
class XList
{
public:
  // The following three methods should only be used by derived classes.
  // However, since we can have many different derived classes, protected won't allow access, so they are public.
  // Need two prototypes for updating a node connection and removing a node connection.
  virtual void insert(NT *node)=0;
//  virtual void insert(NT *ref, NT *node)=0; // Inserts node after ref.
  virtual void erase(NT *node)=0;

  virtual void setData(void *data)=0;
  virtual void clear()=0;
  // Return value is a custom iterator, so cannot make it virtual.
  // virtual void insert(const T &val)=0;
};

// Temporary container for cutting and splicing.
template<class size_type, class node_type>
class XTmpContainer
{
private:
  XTmpContainer() {} // Cannot create default list.
public:
  typedef node_type n_type;
  unsigned int maxLevel; //!< Maximum number of levels for forward pointers possible.
  unsigned int level;    //!< The maximum number of levels for forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end nodes.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.

  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item, n_type)

  XTmpContainer(double probability, unsigned int maxLevel) :
    probability(probability), maxLevel(maxLevel), level(0), items(0)
  {
    head = Alloc(maxLevel);
    tail = Alloc(maxLevel);

    head->level = 0;
    tail->level = 0;
  }

  ~XTmpContainer()
  {
    Free(head);
    Free(tail);
  }

  CSDefineSize
  CSDefineEmpty

};

template<class size_type, class difference_type, class node_type, class value_type, unsigned int N, unsigned int S, class R>
class XData
{
public:
  typedef XData<size_type,difference_type,node_type,value_type,N,S,R> data_type;
  typedef node_type n_type;

  R rng;

  unsigned int maxLevel; //!< Maximum number of forward pointers possible.
  unsigned int level;    //!< The maximum number of forward pointers on any given container currently in use.
  node_type *head,*tail; //!< Start and end containers.
  double probability; //!< Probability to go to the next level.
  size_type items; //!< Number of items in the list.

  // Cache
  mutable size_type scan_index[N];
  mutable std::pair<size_type,node_type*> *update[N];

  // Containers using this data.
  XList<value_type,node_type>* Containers[N];

  // Temporary container for cut and splice.
  typedef XTmpContainer<size_type, node_type> tmp_container_type;
  tmp_container_type *tmp_container;

  // Call this after setting the Containers[] array.
  void InitData()
  {
    for(int i=0;i<N;i++)
    {
      Containers[i]->setData(this);
    }
  }

  unsigned int getListCount() const { return N; }
  unsigned int getIndexCount() const { return S; }

  node_type* Alloc(unsigned int level, const value_type &obj) CSAlloc2(level,obj,node_type)
  node_type* Alloc(unsigned int level) CSAlloc(level,node_type)
  void Free(node_type *item) CSFree(item, n_type)
  CSDefineGenerateRandomLevel
  CSDXDefineAdjustLevels
  CSDXDefineScanNode

  CSDXDefineEraseNode

  DCheckSkipNodes

  XData() { tmp_container = NULL; }

  ~XData()
  {
    Free(head);
    Free(tail);
    for(int i=0;i<N;i++)
    {
      delete update[i];
    }
    delete tmp_container;
  }
};


// Node type for composite skiplist
template <class T, unsigned int N=1, unsigned int S=0>
class XBidiNode
{
public:
  typedef XBidiNode<T,N,S> data_type;
  typedef size_t size_type;
  struct Pointers
  {
    size_type skip[S];
    XBidiNode<T,N,S> *forward[N];
    XBidiNode<T,N,S> *backward[N];
  };
  typedef Pointers ptr_type;

  T object; //!< Object associated with the key.
  unsigned int level; //!< how many forward and backward pointers there are.
  ptr_type pointers[1];
  XBidiNode<T,N,S>*& forward(unsigned int ptr_type, unsigned int level) {return pointers[level].forward[ptr_type];}
  XBidiNode<T,N,S>*& backward(unsigned int ptr_type, unsigned int level) {return pointers[level].backward[ptr_type];}
  size_type& skip(unsigned int ptr_type, unsigned int level) {return pointers[level].skip[ptr_type];}
  XBidiNode<T,N,S>* forward(unsigned int ptr_type, unsigned int level) const {return pointers[level].forward[ptr_type];}
  XBidiNode<T,N,S>* backward(unsigned int ptr_type, unsigned int level) const {return pointers[level].backward[ptr_type];}
  size_type skip(unsigned int ptr_type, unsigned int level) const {return pointers[level].skip[ptr_type];}
  XBidiNode(unsigned int level, const T obj) : level(level), object(obj) NCSClearNodesBidiIdx(N,S)
  explicit XBidiNode(unsigned int level) : level(level) NCSClearNodesBidiIdx(N,S)

  unsigned int getListCount() const { return N; }
  unsigned int getIndexCount() const { return S; }
};

#define CSBIDI(a,b) a
#define CSUNIQUE(a,b) b
#define CSINDEX(a,b) a
#define CSKEY(a,b) b
#define CSLEVEL(a,b) a

// Now to build the containers.
// We need to define the aggregate container.
template <class T, unsigned int N, unsigned int S=0, class R = RNG>
class XBidiCompositeSkipList
{
public:
  typedef XBidiCompositeSkipList<T,N,S,R> container_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef XBidiNode<value_type,N,S> node_type;
  typedef XData<size_type,difference_type,node_type,value_type,N,S,R> data_type;
  friend class XList<T,node_type>;
protected:
  data_type data;

  CSXDefineInit

public:
  // Good constructors
  XBidiCompositeSkipList() { Containers = data.Containers; CSInitDefault; }
  XBidiCompositeSkipList(double probability, unsigned int maxLevel) { Containers = data.Containers; CSInitPM; }
  XBidiCompositeSkipList(size_type maxNodes) { Containers = data.Containers; CSInitMaxNodes; }

  // These aren't implemented yet.
//  XBidiCompositeSkipList(const container_type &source) { CSXInitCore(source.probability, source.maxLevel) CSCopyITIT(const_iterator, data.Containers[0]->begin(),data.Containers[0]->end(),data.Containers[0].insert); }
//  template<class InIt> XBidiCompositeSkipList(InIt first, InIt last) { CSInitDefault; CSCopyITIT(InIt, first,last,push_back); }
//  template<class InIt> XBidiCompositeSkipList(InIt first, InIt last, double probability, unsigned int maxLevel) { CSInitPM; CSCopyITIT(InIt, first,last,push_back); }
//  template<class InIt> XBidiCompositeSkipList(InIt first, InIt last, size_type maxNodes) { CSInitMaxNodes; CSCopyITIT(InIt, first,last,push_back); }

  virtual ~XBidiCompositeSkipList() { clear(); }

  // These aren't implemented yet.
//  container_type& operator=(const container_type &source){}
//  void swap(container_type& right) {}

  CSXDefineClear2
  CSXDefineDestroy2
  size_type size() const { return data.items; }
  bool empty() const { return (data.items==0); }

  XList<T,node_type>** getContainers() { return data.Containers; }
  typedef XList<T,node_type>** XListArray;
  XListArray Containers;
};


#undef CSBIDI
#undef CSLEVEL
#undef CSKEY
#undef CSUNIQUE
#undef CSINDEX

// pred is usually 'less'
template<unsigned int X, class container_type, class Pr>
CS::XIterator0<X,container_type> scan(const CS::XIterator0<X,container_type> &first, const CS::XIterator0<X,container_type> &last, typename container_type::const_reference value, const Pr &pred)
{
  typedef typename container_type::node_type node_type;
  typedef typename container_type::size_type size_type;
  typedef CS::XIterator0<X,container_type> it;
  if (first==last) return last;
  if (!pred(*first,value)) return first;

  // Start skipping ahead.
  container_type *c = first.getContainer();
  node_type *node = first.getNode();

  // Scan up.
  unsigned int lvl = 0;
  size_type idx = first.getIndex();
  size_type end_idx = last.getIndex();
  for(;;)
  {
    node_type *node1 = node->forward(X,lvl);
    size_type tidx = node->skip(X,lvl);
    if (idx+tidx>=end_idx)
    {
      if (lvl==0) return last;
      lvl--;
      break;
    }
//    if (node1->object<=value)
// <=  (!(b<a))
    if (pred(node1->object,value))
    {
      if (lvl==node->level)
      {
        idx += tidx;
        node = node1;
      }
      else
      {
        lvl++;
      }
    }
    else
    {
      if (lvl==0)
      {
        return it(c->getData(),node1,idx+1);
      }
      lvl--;
      break;
    }
  }

  // Scan down.
  for(;;)
  {
    node_type *node1 = node->forward(X,lvl);
    size_type tidx = node->skip(X,lvl);
    if (idx+tidx>=end_idx)
    {
      if (lvl==0) return last;
      lvl--;
      continue;
    }
//    if (node1->object<=value)
// <=  (!(b<a))
    if (pred(node1->object,value))
    {
      idx += tidx;
      node = node1;
      continue;
    }
    if (lvl==0) break;
    lvl--;
  }
  node = node->forward(X,0);
  idx++;

  return it(c->getData(),node,idx);
}

// Special note that iterators remain pointing to original data.
// So the iterators will be swapped as well.
template<unsigned int X, class container_type>
void iter_swap(CS::XIterator0<X,container_type> left, CS::XIterator0<X,container_type> right)
{
  if (left.getContainer()!=right.getContainer())
  {
    throw container_exception();
  }

  left.getContainer()->swap(left,right);
}

// first will keep pointing at original element.  New location will be last-1.
// last is not modified.
template<unsigned int X, class container_type>
void reverse(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last)
{
  CS::XIterator0<X,container_type> s = first;
  CS::XIterator0<X,container_type> e = last-1;
  container_type *c = first.getContainer();
  while(e>s)
  {
    /* Swap the nodes. */
    c->swap(s,e);
    std::swap(s,e);
    ++s;
    --e;
  }
}

// Sort routines.
// Just using Quicksort and insertion sort.
// Can update this later.
template<unsigned int X, class container_type>
void quick_sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last)
{
  typedef typename container_type::size_type size_type;
  typedef CS::XIterator0<X,container_type> it;
  if( last > first+1 )
  {
    container_type *c = first.getContainer();

    if (last-first < 100)
    {
      insert_sort(first,last);
      return;
    }

    size_type first_index = first.getIndex();

    it left  = first;
    it right = last;
    it r = right-1;

    // Grab pivot.
    // Three elements are used.
    // *left, *(right-1) and *((left+right)/2)
    // The middle value is taken as the pivot.
    it middle = left+((right-left)/2);

    if (*middle>=*left)
    {
      if (*middle<=*r)
      {
        c->swap(left,middle);
        std::swap(left,middle);
      }
      else if (*r>*left)
      {
        c->swap(left,r);
        std::swap(left,r);
      }
    }
    else
    {
      if (*middle>=*r)
      {
        c->swap(left,middle);
        std::swap(left,middle);
      }
      else if (*r<*left)
      {
        c->swap(left,r);
        std::swap(left,r);
      }
    }

    it pivot = left++;

    while( left != right )
    {
      if( ( *left <= *pivot ) )
      {
        ++left;
      }
      else
      {
         while( (left != --right) && ( *pivot <= *right ) );
         c->swap( left, right );
         std::swap(left, right);
      }
    }

    --left;
    first = c->begin()+first_index;
    c->swap(first,left);
    std::swap(first,left);


    sort( first, left );
    sort( right, last );
  }
}

// T is node_type
template <class T, class Pr>
class XDerefT : std::binary_function<T, T, bool>
{
private:
  Pr pred;
  XDerefT() {}
public:
  XDerefT(const Pr &pred) : pred(pred) {}
  bool operator()(const T &a, const T &b) const { return pred(a->object,b->object); }
};

// A quicker insertion sort.
// Uses N extra integers of memory on the stack.
// Only use this for small arrays.
// first can be reused with a refresh(), but is no longer pointing at the beginning of the list.
template<unsigned int X, class container_type, class Pr>
void insert_sort_N(const CS::XIterator0<X,container_type> &first, const CS::XIterator0<X,container_type> &last, const Pr &pred)
{
  typedef typename container_type::node_type node_type;
  typedef typename container_type::data_type data_type;
  typedef typename container_type::value_type value_type;
  typedef typename container_type::value_type difference_type;
  typedef CS::XIterator0<X,container_type> it;

  difference_type sz = last-first;
  if (sz<=1) return;

  container_type *c = first.getContainer();

  // Copy all the nodes in the range into an array.
  node_type* *array = new node_type*[sz];
  int cnt = 0;
  for(it i=first;i!=last;++i,cnt++) array[cnt]=i.getNode();

  // Use insertion sort to sort the nodes.
  XDerefT<node_type*,Pr> xpred(pred);
  for(int i = 1; i < sz; i++)
  {
    node_type *nt = array[i];
    value_type &v = nt->object;
    int j = i - 1;
    // Scan using binary search.
    for (; (j >= 0) && pred(v, array[j]->object); j--)
    {
      array[j+1] = array[j];
    }
    array[j+1] = nt;
  }

  // Remove the range from the original list.
  data_type *data = c->getData();
  typename data_type::tmp_container_type &tmp_container = *data->tmp_container;
  c->cut(first,last,tmp_container);

  // Build new list into temporary list.
  tmp_container.level = 0;
  tmp_container.head->forward(X,0) = tmp_container.tail;
  tmp_container.tail->backward(X,0) = tmp_container.head;

  for(int i=0;i<sz;i++)
  {
    node_type *node = array[i];
    unsigned int old_level = tmp_container.level;
    int j = std::max(node->level,old_level);
    if (j>old_level)
    {
      for(;j>old_level;j--)
      {
        tmp_container.head->forward(X,j) = node;
        tmp_container.head->skip(X,j) = i+1;
        node->forward(X,j) = tmp_container.tail;
        node->skip(X,j) = 1;
        node->backward(X,j) = tmp_container.head;
        tmp_container.tail->backward(X,j) = node;
      }
      tmp_container.level = node->level;
    }
    for(;j>node->level;--j)
    {
      tmp_container.tail->backward(X,j)->skip(X,j)++;
    }
    for(;j>=0;--j)
    {
      node->backward(X,j) = tmp_container.tail->backward(X,j);
      node->backward(X,j)->forward(X,j) = node;
      tmp_container.tail->backward(X,j) = node;
      node->forward(X,j) = tmp_container.tail;
      node->skip(X,j) = 1;
    }
  }

  for(unsigned int i=tmp_container.level+1;i<=data->level;i++)
  {
    tmp_container.head->forward(X,i) = tmp_container.tail;
    tmp_container.head->skip(X,i) = sz;
    tmp_container.tail->backward(X,i) = tmp_container.head;
  }
  tmp_container.level = data->level;
  tmp_container.head->level = data->level;
  tmp_container.tail->level = data->level;
  tmp_container.items = sz;

  // Put new list into original list.
  c->splice(last,tmp_container);

  delete[] array;
}

template<unsigned int X, class container_type, class Pr>
void insert_sort(const CS::XIterator0<X,container_type> &first, const CS::XIterator0<X,container_type> &last, const Pr &pred)
{
  typedef CS::XIterator0<X,container_type> it;
  typedef typename container_type::value_type T;
  typedef typename container_type::difference_type difference_type;
  typedef typename container_type::size_type size_type;

  it i,j,k;
  container_type *c = first.getContainer();

  size_type first_index = first.getIndex();
  difference_type cnt1 = 1;
  for(i = first+1; i < last;cnt1++)
  {
    // Could update this to be logn.
    difference_type cnt = cnt1;
    T &v = *i;
    for (j = i - 1; (cnt > 0) && pred(v,*j); --j,cnt--);
    if ((first_index!=0)||(cnt>0)) ++j;
    k = i+1;
    c->move(j,i);
    i = k;
  }
}

template<unsigned int X, class container_type>
void insert_sort(const CS::XIterator0<X,container_type> &first, const CS::XIterator0<X,container_type> &last)
{
  typedef typename container_type::value_type T;
  insert_sort(first,last,std::less<T>());
}

template <class T, class Pr>
class LessEqualT : std::binary_function<T, T, bool>
{
private:
  Pr pred;
  LessEqualT() {}
public:
  LessEqualT(const Pr &pred) : pred(pred) {}
  bool operator()(const T &a, const T &b) const { return (!pred(b,a)); }
};

// Merge sort for linked list is likely the simplest, fastest, in-place and stable sort you can use.
template<unsigned int X, class container_type, class Pr>
void merge_sort(CS::XIterator0<X,container_type> &first, CS::XIterator0<X,container_type> &last, const Pr &pred)
{
  typedef typename container_type::size_type size_type;
  typedef typename container_type::value_type value_type;
  typedef CS::XIterator0<X,container_type> it;

  if (last-first <= 100)
  {
    insert_sort_N(first,last,pred);
    return;
  }

  container_type *c = first.getContainer();

  it middle = first+(last-first)/2;

  // first and middle will get overwritten by stable_sort().
  // So we need a way to get those iterators back.
  // For first, we set a temporary iterator to point to first-1.
  // When the sort is done, just add 1 and you have the same position as before.
  // If the index was 0, there is no first-1.  So we check for this and use begin() if the index is 0.
  // For middle, we know there is a previous index, so we just go back one and then forward by one after the sort.
  size_type first_index = first.getIndex();
  it tfirst = first;
  if (first_index!=0) tfirst--; // Back up just before the sort.

  merge_sort(first,middle,pred);
  first = (first_index)?tfirst+1:c->begin(); // Go forward again unless index is 0.
  it tmiddle = middle-1; // Back up.
  merge_sort(middle,last,pred);
  middle = tmiddle+1;    // Go forward again.

  // Merge the two lists.
  it i = first;
  LessEqualT<value_type,Pr> pred2(pred);
  for(;((i!=middle)&&(middle!=last));)
  {
    // Could use levels to advance in logn instead of n.
    i = CS::scan(i,middle,*middle,pred);
    if (i==middle) break; // Done
    it j = middle;
    j = CS::scan(++j,last,*i,pred2);
    c->move(i,middle,j);
    middle = j;
  }
}

template<unsigned int X, class container_type>
void merge_sort(CS::XIterator0<X,container_type> &first, CS::XIterator0<X,container_type> &last)
{
  typedef typename container_type::value_type T;
  merge_sort(first,last,std::less<T>());
}

// Merge sort for linked list is likely the simplest, fastest, in-place and stable sort you can use.
template<unsigned int X, class container_type, class Pr>
void sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last, const Pr pred)
{
  merge_sort(first,last,pred);
}

// Merge sort for linked list is likely the simplest, fastest, in-place and stable sort you can use.
template<unsigned int X, class container_type, class Pr>
void stable_sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last, const Pr pred)
{
  merge_sort(first,last,pred);
}

template<unsigned int X, class container_type>
void sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last)
{
  merge_sort(first,last,std::less<typename container_type::value_type>());
}

template<unsigned int X, class container_type>
void stable_sort(CS::XIterator0<X,container_type> first, CS::XIterator0<X,container_type> last)
{
  merge_sort(first,last,std::less<typename container_type::value_type>());
}

// < (a<b)
// >= is (!(a<b))
// <= is (!(b<a))
// > is (b<a)
// == is (!((a<b)||(b<a)))
// != is ((a<b)||(b<a))

}
#endif

