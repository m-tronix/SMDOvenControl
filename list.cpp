/*
 * list.cpp
 *
 * Created: 10.4.2015 15:03:15
 *  Author: Kremmen
 */

#include "list.h"
#include "Arduino.h"


ListItem::ListItem( void *actualItem ) {
	item = actualItem;
	nextItem = NULL;
	prevItem = NULL;
}



LinkedList::LinkedList() {
	head = NULL;
	tail = NULL;
	curr = NULL;
}

LinkedList::~LinkedList() {
	getFirst();
	while ( curr ) { deleteCurr(); }
}

void LinkedList::insertFirst( void *Item ){
	ListItem *listItem;

	listItem = new ListItem(Item);
	listItem->setNext(head);
	if ( head ) head->setPrev(listItem);
	head = listItem;
	if ( !tail ) tail = listItem;
	curr = listItem;
	numItems++;
}

void LinkedList::insertLast( void *Item ) {
	ListItem *listItem;

	listItem = new ListItem(Item);
	listItem->setPrev(tail);
	if ( tail ) tail->setNext(listItem);
	tail = listItem;
	if ( !head ) head = listItem;
	numItems++;
}

void LinkedList::insertCurr( void *Item ) {
	ListItem *listItem;
	ListItem *tmp;

	listItem = new ListItem(Item);
	if ( curr ) {
		tmp = curr;
		curr = listItem;
		curr->setNext( tmp );
		curr->setPrev( tmp->getPrev() );
		tmp->setPrev( curr );
		tmp = tmp->getPrev();
		tmp->setNext( curr );
	} else {
		head = tail = curr = listItem;
	}
	numItems++;
}

void LinkedList::deleteCurr() {
	ListItem *tmp;

	if ( head == curr ) {
		head = curr->getNext();
		} else {
		tmp = curr->getPrev();
		tmp->setNext( curr->getNext() );
	}
	if ( tail == curr ) {
		tail = curr->getPrev();
	} else {
		tmp = curr->getNext();
		tmp->setPrev( curr->getPrev() );
	}
	tmp = curr;
	curr = curr->getNext();
	if ( !curr ) curr = tail;
	if ( curr ) numItems--;
	delete tmp;
}

void LinkedList::deleteAll() {
	curr = head;
	while ( curr ) deleteCurr();
	numItems = 0;
}

void *LinkedList::getFirst() {
	curr = head;
	if ( curr ) return curr->getItem();
	else return NULL;
}

void *LinkedList::getLast() {
	curr = tail;
	if ( curr ) return curr->getItem();
	else return NULL;
}

void *LinkedList::getNext() {
	if ( curr ) {
		if ( curr->getNext() ) {
			curr = curr->getNext();
			return curr->getItem();
		} else return NULL;
	} else return NULL;
}

void *LinkedList::getPrev() {
	if ( curr ) {
		if ( curr->getPrev() ) {
			curr = curr->getPrev();
			return curr->getItem();
		} else return NULL;
	} else return NULL;
}
