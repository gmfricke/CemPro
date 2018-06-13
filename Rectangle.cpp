#include "Rectangle.h"
#include "World.h"

#include <iostream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

My_Rectangle::My_Rectangle( World* world, int x_len, int y_len ) : x_length( x_len ), y_length( y_len )
, num_border_proteins(0)
{
	// Count border proteins


	the_world = world;

	upper_right = new Vertex( world );
	upper_left = new Vertex( world );
	lower_right = new Vertex( world );
	lower_left = new Vertex( world );
}

My_Rectangle::~My_Rectangle(void)
{
	delete upper_right;
	delete lower_right;
	delete upper_left;
	delete lower_left;
}

void My_Rectangle::draw(int color)
{
	if ( color == 0 )
	{
		glColor3f(1.0, 0.0, 0.0);
	}
	else
	{
		glColor3f(0.0, 1.0, 1.0);
	}
	// draw the object

	float depth_scale = 2.0/the_world->world_depth;
	float height_scale = 2.0/the_world->world_height;
	float width_scale = 2.0/the_world->world_width;

	int x_pos = upper_left->get_x();
	int y_pos = upper_left->get_y();
	int z_pos = 0;

	float upper_left_x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
	float upper_left_y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
	float upper_left_z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;

	x_pos = upper_right->get_x();
	y_pos = upper_right->get_y();

	float upper_right_x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
	float upper_right_y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
	float upper_right_z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;

	x_pos = lower_right->get_x();
	y_pos = lower_right->get_y();
	
	float lower_right_x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
	float lower_right_y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
	float lower_right_z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;

	x_pos = lower_left->get_x();
	y_pos = lower_left->get_y();

	float lower_left_x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
	float lower_left_y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
	float lower_left_z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;

	//cout << "Draw: ";
	//cout << "[(" << upper_right_x << ", " << upper_right_y << ")" 
	//	 << ", " << "(" << upper_left_x << ", " << upper_left_y << ")"
	//     << ", " << "(" << lower_left_x << ", " << lower_left_y << ")"
	//	 << ", " << "(" << lower_right_x << ", " << lower_right_y << ")]" << endl;

	if ( upper_left_y < lower_left_y ) return;
	if ( upper_right_x < upper_left_y ) return;

	glBegin( GL_POLYGON );
	glVertex3f(upper_left_x, upper_left_y, z_pos );
	glVertex3f(upper_right_x, upper_right_y, z_pos );
	glVertex3f(lower_right_x, lower_right_y, z_pos );
	glVertex3f(lower_left_x, lower_left_y, z_pos );
	glEnd();
}

void My_Rectangle::print(void)
{
	cout << "["; 
	upper_left->print();
	cout << ", ";
	upper_right->print();
	cout << ", ";
	lower_left->print();
	cout << ", ";
	lower_right->print();
	cout << "]";
}

void My_Rectangle::get_proteins(void)
{}
	

int My_Rectangle::count_border_proteins(void)
{
	int num_proteins = 0;

		// top row
	for ( int i = 0; i < x_length ; i++ )
	{
		int x_coord = upper_right->get_x() + i;
		int y_coord = upper_right->get_y();
		int z_coord = 0;
		the_world->wrap_location( x_coord, y_coord, z_coord );

		if ( the_world->sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
		}
	}

	// bottom row
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = lower_right->get_x() + i;
		int y_coord = lower_right->get_y();
		int z_coord = 0;
		the_world->wrap_location( x_coord, y_coord, z_coord );

		if ( the_world->sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
		}
	}

	// left column
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = upper_left->get_x();
		int y_coord = upper_left->get_y() + i;
		int z_coord = 0;
		the_world->wrap_location( x_coord, y_coord, z_coord );

		if ( the_world->sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
		}
	}

	// right column
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = upper_right->get_x();
		int y_coord = upper_right->get_y() + i;
		int z_coord = 0;
		the_world->wrap_location( x_coord, y_coord, z_coord );
		
		if ( the_world->sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
		}
	}

	return num_proteins;
}
