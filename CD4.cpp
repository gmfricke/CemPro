#include "CD4.h"
#include "CCR5.h"
#include "glut.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

CD4::CD4( World* world, int init_x, int init_y, int init_z ) : Protein( world, init_x, init_y, init_z )
{

}

void CD4::update()
{
  move();
}

void CD4::move()
{
  cout << "CD4::move() called" << endl;

  // CD4s are attracted to CCR5. Each CD4 can bind two CCR5s and each CCR5 can 
  // bind two CD4s.

	float epsilon_0 = the_world->epsilon_0;
	float epsilon_1 = the_world->epsilon_1;
	float cross_epsilon = the_world->cross_epsilon;

	bool moved;

	int move_direction = rand() % 5;
	int x_offset = 0;
	int y_offset = 0;
	int old_z = z_pos;

	// Choose a random direction to move
	if ( move_direction == 0 )
	{
		x_offset = 0; y_offset = 1;
	}
	else if ( move_direction == 1 )
	{
		x_offset = 0; y_offset = -1;
	}
	else if ( move_direction == 2 )
	{
		x_offset = 1; y_offset = 0;
	}
	else if ( move_direction == 3 )
	{
		x_offset = -1; y_offset = 0;
	}
	else if ( move_direction == 4 )
	{
		x_offset = 0; y_offset = 0;
	}

	int x = x_pos + x_offset, y = y_pos + y_offset, z = old_z;

	wrap_location( x, y, z );

	// dont overwrite other proteins
	if ( the_world->sites[x][y][z].contents != NULL )
	{
		return;
	}

	// look at the energy state of the position being considered
	// roll a di to see if we move given the change in energy
	// ...

	int cd4_bound = 0, ccr5_bound = 0, gp120_bound = 0;

	if ( bound_cd4 ) 
	  {
	    cout << "found existing cd4 bond" << endl;
	    cd4_bound = 1;
	  }
	
	if ( bound_ccr5 ) 
	  {
	    cout << "found existing ccr5 bond" << endl;
	    ccr5_bound = 1;
	  }

	float delta_energy= 0.0;
	int my_z_count = 0;
	int target_z_count = 0;
	
	int target_unbound_ccr5 = 0, target_unbound_cd4 = 0, target_unbound_gp120 = 0;
	
	CD4* unbound_cd4 = getAnAdjacentUnboundCD4(x,y,z);
	if (unbound_cd4) target_unbound_cd4 = 1;

	CCR5* unbound_ccr5 = getAnAdjacentUnboundCCR5(x,y,z);
	if (unbound_ccr5) target_unbound_ccr5 = 1;

	delta_energy = (cd4_bound*epsilon_0 + gp120_bound*cross_epsilon + ccr5_bound*epsilon_1) 
	  - (target_unbound_cd4*epsilon_0 + target_unbound_gp120*cross_epsilon + target_unbound_ccr5*epsilon_1);

	cout << "e1=" << epsilon_0 << "e2=" << epsilon_1 << "ex=" << cross_epsilon << endl;

	float probability_of_move = 0.0;
	float r = 0.0;

	if ( delta_energy > 0 ) // i.e. delta_energy is positive
	{
		probability_of_move = exp( -delta_energy );

		r = rand()/static_cast<float>(RAND_MAX); // 0.01 to 1 random value
		if (  r > probability_of_move) 
		  {
		    cout << "CD4: not moving (delta energy = " << delta_energy << ") " << probability_of_move << " < " << r << endl;
		    return;
		  }
		else
		  {
		    cout << "CD4: moving: (delta energy = " << delta_energy << ") " << probability_of_move << " > " << r << endl;
		  }
	}
	
	the_world->sites[x][y][z].contents = this;
	the_world->sites[x_pos][y_pos][z_pos].contents = NULL;

	x_pos = x;
	y_pos = y;
	z_pos = z;

	// Break old bindings
	unbindCD4();
	unbindCCR5();

	// Form new bindings
	if (unbound_cd4) bindCD4(unbound_cd4);
	if (unbound_ccr5) bindCCR5(unbound_ccr5);
	

	return;

}

void CD4::draw()
{
  // cout << "CD4: draw() called" << endl;

  if (bound_cd4) glColor3f(0.0, 1.0, 1.0);
  else glColor3f(0.0, 0.5, 0.75);

  glPolygonMode(GL_FRONT, GL_FILL);

  // draw the object
  float depth_scale = 2.0/the_world->world_depth;
  float height_scale = 2.0/the_world->world_height;
  float width_scale = 2.0/the_world->world_width;
  
  float x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
  float y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
  float z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;
  
  glTranslatef(x, y, z);
  glutSolidCube( width_scale/2.0 );	
  glTranslatef(-x, -y, -z);
  
  glColor3f(1.0, 1.0, 1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void CD4:: bindCD4( CD4* partner )
{
  if (bound_cd4) bound_cd4->unbindCD4();
  bound_cd4 = partner;
  if (bound_cd4->getBoundCD4() != this) bound_cd4->bindCD4( this );
}

void CD4::unbindCD4()
{ 
  if (!bound_cd4) return;
  CD4* temp = bound_cd4;
  bound_cd4 = NULL;
  temp->unbindCD4();
}

CD4* CD4::getBoundCD4()
{
  return bound_cd4;
}

bool CD4::isUnboundToCCR5()
{
  return bound_ccr5 == NULL;
}

bool CD4::isUnboundToCD4()
{
  return bound_cd4 == NULL;
}

void CD4:: bindCCR5( CCR5* partner )
{
  if (bound_ccr5) bound_ccr5->unbindCD4();
  bound_ccr5 = partner;
  if (bound_ccr5->getBoundCD4() != this) bound_ccr5->bindCD4( this );
}

void CD4::unbindCCR5()
{ 
  if (!bound_ccr5) return;
  CCR5* temp = bound_ccr5;
  bound_ccr5 = NULL;
  temp->unbindCCR5();
}

CCR5* CD4::getBoundCCR5()
{
  return bound_ccr5;
}

CD4* CD4::getAnAdjacentUnboundCD4(int x, int y, int z)
{
  Site this_site = the_world->sites[x][y][z];
  int cd4_count = 0;
  Site* site = NULL;
  CD4* cd4s[8];
  for ( int i = -1; i <= 1; i++ )
    {
      for ( int j = -1; j <= 1; j++ )
	{
	  int new_x = x_pos + i, new_y = y_pos + j, new_z = z_pos;
	  
	  the_world->wrap_location( new_x, new_y, new_z );
	  
	  site = &the_world->sites[new_x][new_y][new_z];
	  if ( site->contents != NULL )
	    {
	      if ( site->contents != this_site.contents ) // skip us
		{ 
		  CD4* cd4 = dynamic_cast<CD4*>(site->contents);
		
		  if ( cd4  ) // cast suceeded so is CD4
		    {
		      if ( cd4->isUnboundToCD4() )
			{
			  cout << "Saw an unbound cd4" << endl;
			  cd4s[cd4_count++] = cd4;
			}
		    }
		  else
		    {
		      cout << "Protein seen is not CD4" << endl;
		    }
		}
	    }
	}
    }

  // choose a random cd4 from the list to return
  
return (cd4_count > 0)? cd4s[rand()%cd4_count] : NULL; 
}

CCR5* CD4::getAnAdjacentUnboundCCR5(int x, int y, int z)
{
  Site this_site = the_world->sites[x][y][z];
  int ccr5_count = 0;
  Site* site = NULL;
  CCR5* ccr5s[8];
  for ( int i = -1; i <= 1; i++ )
    {
      for ( int j = -1; j <= 1; j++ )
	{
	  int new_x = x_pos + i, new_y = y_pos + j, new_z = z_pos;
	  
	  the_world->wrap_location( new_x, new_y, new_z );
	  
	  site = &the_world->sites[new_x][new_y][new_z];
	  if ( site->contents != NULL )
	    {
	      if ( site->contents != this_site.contents ) // skip us
		{ 
		  CCR5* ccr5 = dynamic_cast<CCR5*>(site->contents);
		
		  if ( ccr5  ) // cast suceeded so is CCR5
		    {
		      if ( ccr5->isUnboundToCD4() )
			{
			  cout << "Saw an unbound CCR5" << endl;
			  ccr5s[ccr5_count++] = ccr5;
			}
		    }
		  else
		    {
		      cout << "Protein seen is not CCR5" << endl;
		    }
		}
	    }
	}
    }

  // choose a random cd4 from the list to return
  
return (ccr5_count > 0)? ccr5s[rand()%ccr5_count] : NULL; 
}
