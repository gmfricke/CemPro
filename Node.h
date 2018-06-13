// Node.h: interface for the Node class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODE_H__B52B92E2_E44A_4418_B41E_048153FFFEFF__INCLUDED_)
#define AFX_NODE_H__B52B92E2_E44A_4418_B41E_048153FFFEFF__INCLUDED_

#ifndef NULL
#define NULL 0
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Entity;

template < typename T >
class Node
{

 public:


Node(); // Default Constructor

Node( T d )
{
  data = d;
}

T getdata()
{
  return data;
}

void setnext( Node<T>* n )
{
  next = n;
}

~Node()
{

}

Node<T>* getnext()
{
  return next;
}

void erase()
{
	//delete data;
}

 private:

  T data;

  Node<T>* next;
};


#endif // !defined(AFX_NODE_H__B52B92E2_E44A_4418_B41E_048153FFFEFF__INCLUDED_)
