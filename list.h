/*! \file list.h
	\brief Linkattu lista
	
	Created: 10.4.2015 15:02:17
	Author: Kremmen
	Listaluokka ottaa void pointereita ja linkitt‰‰ niist‰ 2-suuntaisen listan jota voi navigoida edes-takaisin
	
 */ 
#include "stdint.h"

#ifndef LIST_H_
#define LIST_H_

class ListItem {
	public:
	ListItem( void *actualItem );
	~ListItem() { delete item; };
	void setNext(ListItem *next) { nextItem = next; }
	void setPrev(ListItem *prev) { prevItem = prev; }
	ListItem *getNext() { return nextItem; }
	ListItem *getPrev() { return prevItem; }
	void *getItem() { return item; }
	private:
	void *item;
	ListItem *nextItem;
	ListItem *prevItem;
};

class LinkedList {
	public:
	LinkedList();
	~LinkedList();
	void insertFirst( void *Item );
	void insertLast( void *Item );
	void insertCurr( void *Item );
	void deleteCurr();
	void deleteAll();
	void *getFirst();
	void *getLast();
	void *getNext();
	void *getPrev();
	uint16_t length() { return numItems; }
	private:
	ListItem *head;
	ListItem *tail;
	ListItem *curr;
	uint16_t numItems =0;
};



#endif /* LIST_H_ */