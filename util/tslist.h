/**
 ******************************************************************************
 * @file    tslist.h
 * @author  minafe
 * @version V1.0.0
 * @date    07-October-2011
 * @brief   tslist (thread safe linked list)
 *          management of a list of items, thread safe
 ******************************************************************************
 ******************************************************************************
 *
 * @verbatim
 *
 *  How to use this list
 *
 *  example 1:
 *
 *  tsList<int> lstTest;
 *  tsList<int>::iterator it;
 *
 *  lstTest.push(1);
 *  lstTest.push(3);
 *  lstTest.push(5);
 *  lstTest.push(2);
 *  lstTest.push(7);
 *
 *  for (it = lstTest.getIterator(); it->end() == false; it->incr()) {
 *	  std::cout << it->getData() <<  "\n";
 *  }
 *  delete it;
 *
 *
 *
 *  example 2:
 *
 *  tsList<int> lstTest;
 *
 *  lstTest.push(1);
 *  lstTest.push(3);
 *  lstTest.push(5);
 *  lstTest.push(2);
 *  lstTest.push(7);
 *  int a;
 *
 *  tsList<int>::iterator it;
 *
 *  tsList<int>::iterator it2 = lstTest.getIterator();
 *
 *  lstTest.insert(it2,10);
 *
 *
 *  lstTest.remove(2);
 *
 *
 *  delete it2;
 *
 *  std::cout << "con for" <<  "\n";
 *  for (it2 = lstTest.getIterator(); it2->end() == false; it2->incr()) {
 *	  a = it2->getData();
 *	  std::cout << a <<  "\n";
 *  }
 *  delete it2;
 *
 *  std::cout << "con do while" <<  "\n";
 *
 *  it = lstTest.getIterator();
 *
 *  do {
 *     a = it->getData();
 *     std::cout << a <<  "\n";
 *  } while (it->incr() == true);
 *
 *  delete it;
 *
 *
 *  example 3
 *
 *  tsList<int> lstTest;
 *
 *  lstTest.push(1);
 *  lstTest.push(3);
 *  lstTest.push(5);
 *  lstTest.push(2);
 *  lstTest.push(7);
 *
 *
 *  tsList<int>::iterator it = lstTest.getIterator();
 *
 *
 *
 *
 *  if (lstTest.remove(1) == false) {
 *  	std::cout << "not possible to remove" <<  "\n";
 *  } else {
 *  	std::cout << "removed" <<  "\n";
 *  }
 *
 *  it->incr();
 *
 *  if (lstTest.remove(1) == false) {
 *  	std::cout << "not possible to remove" <<  "\n";
 *  } else {
 *  	std::cout << "removed" <<  "\n";
 *  }
 *
 *  delete it;
 *
 *  possible pitfalls:
 *
 *  - forget to delete the iterator
 *
 *
 * @endverbatim
 *
 *
 ******************************************************************************
 *
 * @attention
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

//#define DEBUG_ENABLE

#ifndef tsList_H_
#define tsList_H_

#include "thread"
#include "iostream"

enum iterFlags {
	iterFlags_clear = 0,
	iterFlags_isend = 1,
	iterFlags_invalid = 2
};


template <class T>
class tsList;

template <class Tp>
struct _List_node {
  Tp data;
  _List_node *next;
  _List_node *prev;
  bool invalid;
  long nIterator; // iterators that point to this
};

template <class Tp>



class _List_iterator {
public:
  _List_node<Tp>* data;
  tsList<Tp>* list;
  iterFlags flag;
  _List_iterator *next;
  _List_iterator *prev;
  pthread_mutex_t* m_mutex;
  void init(pthread_mutex_t* m, tsList<Tp>* l) {
	  m_mutex = m;
	  list = l;
  }

  /**
    * @brief  get the item pointed by the iterator
    * @param  none
    * @retval the item
    */
  Tp getData() {
	  Tp d;
	  pthread_mutex_lock(m_mutex);
	  if (data != NULL) {
		  d = data->data;
	  } else {
		  d = NULL;
	  }
	  pthread_mutex_unlock(m_mutex);
	  return d;
  }
  _List_iterator() {
	  flag = iterFlags_invalid;
  }
  ~_List_iterator() {
#ifdef DEBUG_ENABLE
	  if (flag == iterFlags_isend || flag == iterFlags_clear){}
	  else{
		  std::cout << "invalid flag iterator...." << std::endl;
	  }
#endif
	  list->removeIterator(this);
	  flag = iterFlags_invalid;
  }
  _List_iterator<Tp>&  operator++()
  {
    	  list->incrementIterator(this);
    	  return *this;
  }

  _List_iterator<Tp>  operator++(int)
  {
  	  list->incrementIterator(this);
  	  return *this;
  }
  /**
    * @brief  to check if the iterator is at the end of the list. It return true only if the iterator has been increment after it has reached the end of the list.
    * @param  none
    * @retval true = end of the list, false is not at the end of the list
    */
  bool end()
  {
	  return list->endIterator(this);
  }
  /**
    * @brief  move the iterator at the begin of the list
    * @param  none
    * @retval none
    */
  void setBegin() {
	  list->beginIterator(this);
  }
  /**
    * @brief increment the iterator to the next item in the list
    * @param  none
    * @retval true if the iterator has been incremented, false if not
    */
  bool incr()
  {
	  return list->incrementIterator(this);
  }
  /**
    * @brief  to check if the iterator is at the end of the list. This method return true also if the iterator has not been incremented after it has reached the end of the list
    * @param  none
    * @retval true = end of the list, false is not at the end of the list
    */
  bool isLastElement()
  {
      return list->isLastElementIterator(this);
  }
};



template <class T>
class tsList {
public:
	typedef _List_iterator<T>* iterator;
    tsList () {
    	pthread_mutex_init(&m_mutex, NULL);
    	first = NULL;
    	last = NULL;
    	firstIterator = NULL;
    	lastIterator = NULL;
    	empty = true;
    	size = 0;
    	invalid = false;
    }
    void eraseAll();
    T pop ();
    T popFront ();
    T begin ();
    T getLast ();
    bool push(T data);
    iterator getIterator();
    T getValueIterator(iterator it);
    bool deleteIterator(iterator it);
    bool removeIterator(iterator it);
    bool incrementIterator(iterator it);
    bool insert(iterator it,T data);
    bool getEmpty();
    bool erase(iterator it);
    bool remove(T t);
    bool endIterator(iterator it);
    bool beginIterator(iterator it);
    unsigned int getSize();
    bool invalid;
    bool isLastElementIterator(iterator it);


private:

    _List_node<T> *first;
    _List_node<T> *last;

    int size;
    iterator firstIterator;
    iterator lastIterator;
    bool empty;
    pthread_mutex_t m_mutex;


};



/**
  * @brief  get an iterator to iterate on all the items
  * @param  none
  * @retval the iterator
  */
template <class T>  _List_iterator<T>*  tsList<T>::getIterator()
{
	iterator it;
	pthread_mutex_lock(&m_mutex);
	if (firstIterator == NULL){
		lastIterator = new _List_iterator<T>;
		lastIterator->prev = NULL;
		firstIterator = lastIterator;
	}else{
		lastIterator->next = new _List_iterator<T>;
		lastIterator->next->prev = lastIterator;
		lastIterator = lastIterator->next;
	}
	lastIterator->init(&m_mutex, this);
	lastIterator->next = NULL;
	lastIterator->data = first;
	it = lastIterator;
	it->flag = iterFlags_clear;
	if (it->data != NULL){
		it->data->nIterator++;
	}else{
		it->flag = iterFlags_isend;
	}
	pthread_mutex_unlock(&m_mutex);
	return it;
}

/**
  * @brief  get the right (object) pointed by the iterator
  * @param  it: the iterator
  * @retval the right (object)
  */
template <class T>  T tsList<T>::getValueIterator(iterator it)
{
	pthread_mutex_lock(&m_mutex);
	T d = it->data->data;
	pthread_mutex_unlock(&m_mutex);
	return d;
}


/**
  * @brief  to check if the iterator is at the end of the list. It return true only if the iterator has been increment after it has reached the end of the list.
  * @param  it: the iterator
  * @retval true = end of the list, false is not at the end of the list
  */
template <class T>  bool tsList<T>::endIterator(iterator it)
{

	bool ret = false;
	pthread_mutex_lock(&m_mutex);
	if (it->flag & iterFlags_isend) ret = true;
	if (first == NULL) ret = true;
	if (invalid) ret = true;
	pthread_mutex_unlock(&m_mutex);
	return ret;
}


/**
  * @brief  to check if the iterator is at the end of the list. This method return true also if the iterator has not been incremented after it has reached the end of the list
  * @param  it: the iterator
  * @retval true = end of the list, false is not at the end of the list
  */
template <class T>  bool tsList<T>::isLastElementIterator(iterator it)
{

	bool ret = false;
	pthread_mutex_lock(&m_mutex);
	if (it->flag & iterFlags_isend) ret = true;
	if (first == NULL) ret = true;
	if (invalid) ret = true;
	if (it->data == last) ret = true;
	pthread_mutex_unlock(&m_mutex);
	return ret;
}

/**
  * @brief  move the iterator at the begin of the list
  * @param  it: the iterator
  * @retval always false
  */
template <class T>  bool tsList<T>::beginIterator(iterator it)
{

	bool ret = false;
	pthread_mutex_lock(&m_mutex);
	if (it->data != NULL) {
		it->data->nIterator--;
		if (it->data->nIterator < 0) {
			std::cout << "nIterator negative...." << std::endl;
		}
	}
	it->data = first;
	it->flag = (iterFlags)(it->flag & ~iterFlags_isend); // flag for end iterator
	if (it->data != NULL) {
		it->data->nIterator++;
	} else {
		it->flag = iterFlags_isend;
	}
	pthread_mutex_unlock(&m_mutex);
	return ret;
}

/**
  * @brief increment the iterator to the next item in the list
  * @param  it: the iterator
  * @retval true if the iterator has been incremented, false if not
  */
template <class T>  bool tsList<T>::incrementIterator(iterator it)
{

	bool ret = false;
	pthread_mutex_lock(&m_mutex);
	if (it->data != NULL) {
		it->data->nIterator--;
#ifdef DEBUG_ENABLE
		if (it->data->nIterator < 0) {
			std::cout << "nIterator negative...." << std::endl;
		}
#endif
	}

	if (it->data != last) {
		it->data = it->data->next;
		ret = true;
	} else {
		it->flag = (iterFlags)(it->flag | iterFlags_isend); // flag for end iterator
	}
	if (invalid) {
		ret = false;
	}
	if (it->data != NULL) {
		it->data->nIterator++;
	} else {
		it->flag = iterFlags_isend;
	}
	pthread_mutex_unlock(&m_mutex);
	return ret;
}

/**
  * @brief delete an iterator, but not remove it from the list of the iterator
  * @param  it: the iterator
  * @retval always true
  */
template <class T>  bool tsList<T>::deleteIterator(iterator it)
{
	bool ret = true; //removeIterator(it);
	delete it;
	return ret;
}

/**
  * @brief remove the iterator from the list of the iterators
  * @param  it: the iterator
  * @retval always true
  */
template <class T>  bool tsList<T>::removeIterator(iterator it)
{
	pthread_mutex_lock(&m_mutex);
	if (it->data != NULL) {
		it->data->nIterator--;
		if (it->data->nIterator < 0) {
#ifdef DEBUG_ENABLE
			std::cout << "nIterator negative...." << std::endl;
#endif
		}
	}
	if (firstIterator == lastIterator) {
		firstIterator = NULL;
		lastIterator = NULL;
	} else {
		if (it == lastIterator) {
			lastIterator = lastIterator->prev;
			if (lastIterator != NULL){
				lastIterator->next = NULL;
			}
		} else if (it->next != NULL) {
			it->next->prev = it->prev;
		}
		if (it == firstIterator) {
			firstIterator = firstIterator->next;
			if (firstIterator != NULL) firstIterator->prev = NULL;
		} else if (it->prev != NULL) {
			it->prev->next = it->next;
		}


	}
	//delete it;
	pthread_mutex_unlock(&m_mutex);
	return true;
}

/**
  * @brief put a new item in the list
  * @param  data: the item
  * @retval always true
  */
template <class T> bool tsList<T>::push(T data)
{
	pthread_mutex_lock(&m_mutex);
	if (size == 0) {
		first = new _List_node<T>;
		first->prev = NULL;
		first->next = NULL;
		last = first;
	} else {
		last->next = new _List_node<T>;
		last->next->prev = last;
		last = last->next;
		last->next = NULL;
	}
	last->data = data;
	last->invalid = false;
	last->nIterator = 0;
	empty = false;
	size++;
	if (first != NULL && last != NULL) {
#ifdef DEBUG_ENABLE
		if (first->prev != NULL || last->next != NULL) {
			std::cout << "strange....." << std::endl;
		}
#endif
	}
#ifdef DEBUG_ENABLE
	if (size == 0) {
		std::cout << "strange....." << std::endl;
	}
	if (first != last && size == 0 && first != NULL) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first != last && size == 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first == last && size > 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (size > 100){
		//std::cout << "list too big !!!!" << std::endl;
	}
#endif
	pthread_mutex_unlock(&m_mutex);
	return true;
}

/**
  * @brief put a new item in the list, the new item is put before the iterator item
  * @param  it: the iterator
  * @param  data: the item to add
  * @retval always true
  */
template <class T> bool tsList<T>::insert(iterator it,T data)
{
	pthread_mutex_lock(&m_mutex);
	_List_node<T>* newNode = new _List_node<T>;
	newNode->data = data;
	if (size == 0) {
		newNode->next = NULL;
		newNode->prev = NULL;
		first = newNode;
		last = first;
	} else {
		newNode->prev = it->data->prev;
		if (it->data == first) {
			first = newNode;
			first->prev = NULL;
		}
		else it->data->prev->next = newNode;
		it->data->prev = newNode;
		newNode->next = it->data;
	}

	newNode->invalid = false;
	newNode->nIterator = 0;

	empty = false;
	size++;
#ifdef DEBUG_ENABLE
	if (first != NULL && last != NULL) {
		if (first->prev != NULL || last->next != NULL){
			std::cout << "strange....." << std::endl;
		}
	}

	if (first->prev != NULL || last->next != NULL) {
		std::cout << "strange....." << std::endl;
	}
	if (size == 0) {
		std::cout << "strange....." << std::endl;
	}
	if (first != last && size == 0 && first != NULL) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first != last && size == 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first == last && size > 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
#endif
	pthread_mutex_unlock(&m_mutex);
	return true;
}


/**
  * @brief remove an item in the list
  * @param  t: the item to remove
  * @retval true if has been removed
  */
template <class T> bool tsList<T>::remove(T t)
{
	pthread_mutex_lock(&m_mutex);
	_List_node<T> *it = first;
	bool ret =  false;
	while(it != NULL) {
		if (it->data == t && it->nIterator == 0) {

			_List_node<T> *it2 = it;
			if (first == last) {
				first = NULL;
				last = NULL;
			} else {
				if (it != last) {
					it->next->prev = it->prev;
				} else {
					last = it->prev;
					if (last != NULL) last->next = NULL;
				}
				if (it != first) {
					it->prev->next = it->next;
				} else {
					first = it->next;
					if (first != NULL) first->prev = NULL;
				}
			}
			it = it->next;
			if (it2->invalid) {
				std::cout << "strange....." << std::endl;
			}
			it2->invalid = true;
			delete it2;
			it2 = NULL;
			size--;
#ifdef DEBUG_ENABLE
			if (first != NULL && last != NULL){
				if (first->prev != NULL || last->next != NULL){
					std::cout << "strange....." << std::endl;
				}
			}

			if (size < 0) {
				std::cout << "negative size !!!!" << std::endl;
			}
			if (first != last && size == 0 && first != NULL){
				std::cout << "somthing wrong.... !!!!" << std::endl;
			}
			if (first != last && size == 1){
				std::cout << "somthing wrong.... !!!!" << std::endl;
			}
			if (first == last && size > 1){
				std::cout << "somthing wrong.... !!!!" << std::endl;
			}
#endif
			it = NULL;
			ret = true;

		}else{
			it = it->next;
		}
	}
	if (size == 0) {
		empty = true;
	}
	pthread_mutex_unlock(&m_mutex);
	return ret;
}



/**
  * @brief erase the item pointed by the iterator
  * @param  it: the iterator
  * @retval true if the item has been removed
  */
template <class T> bool tsList<T>::erase(iterator it)
{
	pthread_mutex_lock(&m_mutex);
	_List_node<T>* itData = it->data;
    bool ret = true;
    // I can cancel an element only if no iterator are pointed to it, except the one used to cancel it

    if (itData->nIterator <= 1) {
    	itData->nIterator--;
#ifdef DEBUG_ENABLE
    	if (itData->nIterator != 0) {
    		std::cout << "warning, nIterator negative !!!!" << std::endl;
    	}
#endif
    	if (first == last) {
    		first = NULL;
    		last = NULL;
    		it->flag = (iterFlags)(it->flag | iterFlags_isend); // flag for end iterator
    		it->data = NULL;
    	} else {
			if (itData == first) {
				first = itData->next;
				if (first != NULL) first->prev = NULL;
			}
			else itData->prev->next = itData->next;
			if (itData == last) {
				last = itData->prev;
				if (last != NULL) last->next = NULL;
				it->flag = (iterFlags)(it->flag | iterFlags_isend); // flag for end iterator
				it->data = last;
			} else {
				itData->next->prev = itData->prev;
				it->data = itData->next;
			}

    	}

		if (it->data != NULL) it->data->nIterator++;
#ifdef DEBUG_ENABLE
		if (itData->invalid) {
			std::cout << "strange....." << std::endl;
		}
#endif
		itData->invalid = true;
		delete itData;
		itData = NULL;
		size--;
#ifdef DEBUG_ENABLE
		if (first != NULL && last != NULL) {
			if (first->prev != NULL || last->next != NULL) {
				std::cout << "strange....." << std::endl;
			}
		}
		if (size < 0){
			std::cout << "negative size !!!!" << std::endl;
		}
		if (first != last && size == 0 && first != NULL){
			std::cout << "something wrong.... !!!!" << std::endl;
		}
		if (first != last && size == 1){
			std::cout << "something wrong.... !!!!" << std::endl;
		}
		if (first == last && size > 1){
			std::cout << "something wrong.... !!!!" << std::endl;
		}
#endif
		if (size == 0) empty = true;
    } else {
    	ret = false;
    }
	pthread_mutex_unlock(&m_mutex);

	return ret;
}

/**
  * @brief retrieve the first item in the list
  * @param  none
  * @retval the item
  */
template <class T> T tsList<T>::begin()
{
	T p;
	pthread_mutex_lock(&m_mutex);
	if (first) {
		p = first->data;
	} else {
		p = NULL;
	}
	pthread_mutex_unlock(&m_mutex);
	return p;
}

/**
  * @brief retrieve the last item in the list, but it remain in the list
  * @param  none
  * @retval the item
  */
template <class T> T tsList<T>::getLast()
{
	pthread_mutex_lock(&m_mutex);
	T p = last->data;
	pthread_mutex_unlock(&m_mutex);
	return p;
}

/**
  * @brief remove all the items from the list
  * @param  none
  * @retval none
  */
template <class T> void tsList<T>::eraseAll()
{
	while (size > 0) {
		pop();
	}
}

/**
  * @brief get the last item in the list, the item is removed from the list
  * @param  none
  * @retval the item
  */
template <class T> T tsList<T>::pop()
{
	pthread_mutex_lock(&m_mutex);
	T p = last->data;
	if (last->prev != NULL){
		last = last->prev;
#ifdef DEBUG_ENABLE
		if (last->next->invalid) {
			std::cout << "strange in pop....." << std::endl;
		}
#endif
		last->next->invalid = true;
		delete last->next;
		last->next = NULL;
	} else {
#ifdef DEBUG_ENABLE
		if (last->invalid) {
			std::cout << "strange in pop....." << std::endl;
		}
#endif
		last->invalid = true;
		empty = true;
		delete last;
		last = NULL;
		first = NULL;
	}
	size--;
#ifdef DEBUG_ENABLE
	if (first != NULL && last != NULL){
		if (first->prev != NULL || last->next != NULL) {
			std::cout << "strange....." << std::endl;
		}
	}
	if (first != last && size == 0 && first != NULL) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first != last && size == 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first == last && size > 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (size < 0) {
		std::cout << "negative size !!!!" << std::endl;
	}
#endif
	pthread_mutex_unlock(&m_mutex);
	return p;
}


/**
  * @brief get the first item in the list, the item is removed from the list
  * @param  none
  * @retval the item
  */
template <class T> T tsList<T>::popFront()
{
	pthread_mutex_lock(&m_mutex);
	T p = first->data;
	if (first->next != NULL) {
		first = first->next;
		delete first->prev;
		first->prev = NULL;
	} else {
#ifdef DEBUG_ENABLE
		if (last->invalid) {
			std::cout << "strange in popFront....." << std::endl;
		}
		if (last != first) {
			std::cout << "strange in popFront....." << std::endl;
		}
#endif
		last->invalid = true;
		empty = true;
		delete last;
		last = NULL;
		first = NULL;
	}
	size--;
#ifdef DEBUG_ENABLE
	if (first != NULL && last != NULL) {
		if (first->prev != NULL || last->next != NULL) {
			std::cout << "strange....." << std::endl;
		}
	}
	if (first != last && size == 0 && first != NULL) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first != last && size == 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (first == last && size > 1) {
		std::cout << "somthing wrong.... !!!!" << std::endl;
	}
	if (size < 0) {
		std::cout << "negative size !!!!" << std::endl;
	}
#endif
	pthread_mutex_unlock(&m_mutex);
	return p;
}




/**
  * @brief check if the list is empty
  * @param  none
  * @retval true if is empty
  */
template <class T> bool tsList<T>::getEmpty()
{
	if (invalid) {
		return true;
	} else {
		return empty;
	}
}
/**
  * @brief get the size of the list
  * @param  none
  * @retval the size
  */
template <class T> unsigned int tsList<T>::getSize()
{
	return size;
}



#endif


