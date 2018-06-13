#pragma once

#include "Vertex.h"

class World;

class My_Rectangle
{
public:
	My_Rectangle(World* world, int x_len, int y_len );
	~My_Rectangle(void);

	void translate( int x_move, int y_move )
	{
		upper_left->translate( x_move, y_move );
		upper_right->translate( x_move, y_move );
		lower_left->translate( x_move, y_move );
		lower_right->translate( x_move, y_move );
	}

	Vertex* upper_left;
	Vertex* upper_right;
	Vertex* lower_left;
	Vertex* lower_right;

	int x_length;
	int y_length;

	void draw( int color );
	void print(void);
	void get_proteins(void);

	int num_border_proteins;

private:
	World* the_world;

public:
	int count_border_proteins(void);
};
