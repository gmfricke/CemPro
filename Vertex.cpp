#include "Vertex.h"
#include "World.h"

#include <iostream>

using namespace std;

Vertex::Vertex(World* world):x(0),y(0),the_world(world)
{
}

Vertex::~Vertex(void)
{
}

void Vertex::translate( int move_x, int move_y )
{
	x += move_x;
	y += move_y;
	int z = 0;

	the_world->wrap_location( x, y, z );
}

void Vertex::print(void)
{
	cout << "(" << x << ", " << y << ")";
}

void Vertex::set_xy( int new_x, int new_y )
{
	int z = 0;
	the_world->wrap_location( new_x, new_y, z );
	x = new_x;
	y = new_y;
}

