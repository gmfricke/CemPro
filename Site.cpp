// Site.cpp: implementation of the Site class.
//
//////////////////////////////////////////////////////////////////////

#include "Site.h"
#include "World.h"
#include <typeinfo>
#include <stdlib.h>
#include <iostream>
#include "CD4.h"
#include "CCR5.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Site::Site()
{
	contents = NULL;
}

Site::~Site()
{
	delete contents;
}

void Site::update()
{
	if ( contents != NULL )
	{
		contents->update();
	}
}

void Site::draw()
{
	if (contents != NULL )
	{
		contents->draw();
	}
}

void Site::set_contents(Entity *c)
{
	contents = c;
}


int Site::get_adjacency( void )
	{
		int my_adjacency_count = 0;
		for ( int i = -1; i <= 1; i++ )
		{
			for ( int j = -1; j <= 1; j++ )
			{
				int new_x = x_pos + i, new_y = y_pos + j, new_z = z_pos;

				the_world->wrap_location( new_x, new_y, new_z );

				if ( the_world->sites[new_x][new_y][new_z].contents != NULL )
				{
					if ( the_world->sites[new_x][new_y][new_z].contents != this->contents )
					{
						my_adjacency_count++;
					}
				}
			}
		}

		return my_adjacency_count; 
	}

	void Site::set_coords( int x, int y, int z)
	{
		x_pos = x;
		y_pos = y;
		z_pos = z;
	}
