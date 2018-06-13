#pragma once

class World;

class Vertex
{
public:

	Vertex(World* w);
	~Vertex(void);

	void translate( int move_x, int move_y );

	void set_xy( int new_x, int new_y );
	
	int get_x(){ return x; }

	int get_y(){ return y; }

	void print(void);

private:
	int x;
	int y;
	World* the_world;

};


