// List.h: interface for the List class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIST_H__068EA3DE_26E0_4AD8_B965_92AFDEFB4806__INCLUDED_)
#define AFX_LIST_H__068EA3DE_26E0_4AD8_B965_92AFDEFB4806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Node.h"

class Entity;

template<typename T>
class List
{

 public:

	List()
	{
		size = 0;
		head = 0;
	}

// Put a data item into the linked list (at the head)
void insert( T item )
{
  Node<T>* temp = head;

  head = new Node<T>( item );

  head->setnext( temp );

  size++;
}

// Return the nth element in the linked list

T get( int n )
{

  Node<T>* temp = head;

  for ( int i = 0; i < n; i++ )
    {
      temp = temp->getnext();
    } 

  return temp->getdata();

}

// Delete the nth element from the linked list

void erase( int n )
{

  Node<T>* temp = head;

  Node<T>* to_erase = NULL;

 size--;

  // Delete the head

  if ( n == 0 )

    { 

      to_erase = head;

      head = head->getnext();

    }

  else // Delete something down the list

    {

      for ( int i = 0; i < n-1; i++ )

      {

        //cout << "Looked at node:" << temp->getdata();

        temp = temp->getnext();

      } 

      

      to_erase = temp->getnext();

      //cout << "Going to erase: " << to_erase->getdata();

      

      temp->setnext( to_erase->getnext() );

      //cout << "Set temp->next = " << to_erase->getnext() << endl;

    }

 
	to_erase->erase();
	delete to_erase;

}

int get_size()
{
	return size;
}
 

void erase_all()
{
	Node<T>* temp = head;
	Node<T>* previous = NULL;

	while( temp != NULL )
    {
      previous = temp;

      temp = temp->getnext();

      delete previous;
    } 
}


int find( T e )
{
	Node<T>* temp = head;

	for ( int i = 0; i < size; i++ )
    {
		if ( temp->getdata() == e )
		{
			return i;
		}

		temp = temp->getnext();
    } 

	return -1;
}

~List() 
{
	Node<T>* temp = head;
	Node<T>* previous = NULL;

	while( temp != NULL )
    {
      previous = temp;

      temp = temp->getnext();

      delete previous;
    } 
}

int size;

	Node<T>* head; 

};


#endif // !defined(AFX_LIST_H__068EA3DE_26E0_4AD8_B965_92AFDEFB4806__INCLUDED_)
