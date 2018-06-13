#if defined(_WIN32) || defined(WIN32) 
#pragma once
#endif

#ifndef CHARSTRING_H
#define CHARSTRING_H

#include <iostream>
#include <iomanip>
#include <string.h>
#include <assert.h>

using namespace std;

class CharString
{
	//friend ostream& operator<<{ ostream& output, const CharString& string )
	//{
	//	output << string.sPtr;
	//	return output;
	//}

	//friend istream &operator>>( istream &input; CharString& string )
	//{
	//	char temp[100];
	//	input >> setw( 100 ) >> temp; //Force 100 char max
	//	string = temp;
	//	return input;
	//} 

public:

	CharString( const char* char_array = "" ) : length( strlen( char_array ) ) // Default Constructor
	{
		set_string( char_array );
	}
	
	CharString( const CharString& right ) : length( right.length )  // Copy Constructor
	{
		set_string( right.sPtr );
	}

	const CharString& operator=( const CharString& right )
	{
		if ( &right != this )
		{
			if (sPtr != NULL) delete [] sPtr;
			length = right.length;
			set_string( right.sPtr );
		}

		return *this;
	}
	
	const CharString& operator=( const char*& right )
	{
		//delete [] sPtr;
		length = strlen( right );

		set_string( right );
		return *this;
	}

	const CharString& operator+=( const CharString& right )
	{
		char* tempPtr = sPtr;
		length += right.length;
		sPtr = new char[ length + 1 ];
		assert( sPtr != NULL );
		strcpy( sPtr, tempPtr );
		strcat( sPtr, right.sPtr );
		delete [] tempPtr;
		return *this;
	}

	// this is too expensive (define an add n chars function)
	const CharString& operator+=( const char right )
	{
		//char* tempChar = new char( right );
		length += 1;
		char* tempPtr = sPtr;
		sPtr = new char[ length + 1 ];
		assert( sPtr != NULL );
		strcpy( sPtr, tempPtr );
		strcat( sPtr, &right );
		delete [] tempPtr;
		return *this;
	}

	bool operator!() const
	{
		return ( length == 0 ); 
	}

	bool operator==( const CharString& right ) const
	{
		return strcmp( sPtr, right.sPtr ) == 0;
	}

	bool operator!=( const CharString& right ) const
	{
		return !( *this == right );
	}
	
	//Return char as an lvalue
	char& operator[]( const int subscript )
	{
		assert( subscript >= 0 && subscript <= length - 1);
		return sPtr[ subscript ];
	}

	// Return char as rvalue
	const char& operator[]( const int subscript ) const
	{ 
		assert( subscript >= 0 && subscript <= length - 1);
		return sPtr[ subscript ];
	}

	int get_length() const
	{
		return length;
	}

	~CharString(void)
	{
		//if (sPtr != NULL) delete [] sPtr;
	}

	void set_string( const char* char_array )
	{
		sPtr = new char[ length+1 ];
		assert( sPtr != NULL );
		strcpy( sPtr, char_array );
	}

	int length;
	char* sPtr;

private: 



};


#endif
