// protein.cpp: implementation of the Protein class.
//
//////////////////////////////////////////////////////////////////////
#include <climits> // For INT_MAX

#include <iostream>

#include "Protein.h"
#include "World.h"
#include "CharString.h"
#include "Cluster.h"

#include <ctime>
#include <sys/timeb.h>
#include <cmath>

#include "glut.h"

//#include <cstdlib>
#include <cstdio>
#include "Protein.h"

using namespace std;

//static long int agent_id = 0;
static long unsigned int static_cluster_id = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int argmax( float array[], int size )
{
	int max_index = 0;

	for ( int i = 0; i < size; i++ )
	{
		if ( array[i] > array[ max_index ] )
		{
			max_index = i;
		}
	}

	return max_index;
}

// The original agent
Protein::Protein( World* world, int init_x, int init_y, int init_z ) 
: the_world(world), x_pos( init_x ), y_pos( init_y ), z_pos( init_z )
{
	partner = NULL;
	id = the_world->num_proteins++;
	cluster = new Cluster(the_world, static_cluster_id++);
	the_world->clusters.insert( cluster );
	cluster->add( this );
	sync_cluster();	

	//update();
	updated = false;
}

// For inheritance
Protein::Protein()
{
}

Protein::~Protein()
{
	// Nothing to do
}

float Protein::get_correlation( int d, int target_membrane )
{
	float cor = 0.0;
	int new_x = 0, new_y = 0, new_z = 0;

	// x positive case
	new_x = x_pos + d;
	new_y = y_pos;
	new_z = target_membrane; 
	// target_membrane allows cross_correlation (ie look at
	//the other membrane to calculate the auto correlation)

	wrap_location( new_x, new_y, new_z );

	if (the_world->sites[new_x][new_y][new_z].contents != NULL )
	{
		cor += 1.0;
	}

	// x negative case
	new_x = x_pos - d;
	new_y = y_pos;
	new_z = target_membrane;

	wrap_location( new_x, new_y, new_z );

	if ( the_world->sites[new_x][new_y][new_z].contents != NULL )
	{
		cor += 1.0;
	}

	// y positive case
	new_x = x_pos;
	new_y = y_pos + d;
	new_z = target_membrane;

	wrap_location( new_x, new_y, new_z );

	if ( the_world->sites[new_x][new_y][new_z].contents != NULL )
	{
		cor += 1.0;
	}

	// y negative case
	new_x = x_pos;
	new_y = y_pos - d;
	new_z = target_membrane;

	wrap_location( new_x, new_y, new_z );

//	if ( the_world->current_sites[new_x][new_y][new_z].contents->get_type() == PROTEIN )
	if ( the_world->sites[new_x][new_y][new_z].contents != NULL )	
	{
		cor += 1.0;
	}

	return cor/4.0;
}

// Proteins should be able to move anywhere on the map in a singe decision - then 
// they actually move one square towards it. Have them choos desinations not
// just directions
bool Protein::move()
{
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

	// dont overwrite other proteins - need to SWAP positions!
	if ( the_world->sites[x][y][z].contents != NULL )
	{
		return false;
	}

	// look at the energy state of the position being considered
	// roll a di to see if we move given the change in energy
	// ...

	// look at the positions adjacent to the postion being considered (except us)
	int target_adjacency_count = (int)(the_world->sites[x][y][z].get_adjacency() - 1);

	// my adjacency count
	int	my_adjacency_count = (int)(get_adjacency());
	
	/*
	for ( int i = -1; i <= 1; i++ )
	{
		for ( int j = -1; j <= 1; j++ )
		{
			int new_x = x_pos + i, new_y = y_pos + j, new_z = 0;

			wrap_location( new_x, new_y, new_z );

			if ( the_world->sites[new_x][new_y][new_z].contents != NULL )
			{
				if ( the_world->sites[new_x][new_y][new_z].contents != this )
				{
					my_adjacency_count++;
				}
			}
		}
	}
	*/

	// Return if move would increase free energy

	//cout << "my energy: " << my_adjacency_count << " target energy: " << target_adjacency_count << endl;


	float delta_energy= 0.0;
	int my_z_count = 0;
	int target_z_count = 0;

	if ( z_pos == 0 )
	{
		if ( the_world->sites[x_pos][y_pos][1].contents != NULL )
		{
			my_z_count = 1;
		}

		if ( the_world->sites[x][y][1].contents != NULL )
		{
			target_z_count = 1;
		}

		delta_energy = (my_adjacency_count*epsilon_0 + my_z_count*cross_epsilon) - (target_adjacency_count*epsilon_0 + target_z_count*cross_epsilon);
	}
	else
	{
		if ( the_world->sites[x_pos][y_pos][0].contents != NULL )
		{
			my_z_count = 1;
		}

		if ( the_world->sites[x][y][0].contents != NULL )
		{
			target_z_count = 1;
		}

		delta_energy = (my_adjacency_count*epsilon_1 + my_z_count*cross_epsilon) - (target_adjacency_count*epsilon_1 + target_z_count*cross_epsilon);
	}

	float probability_of_move = 0.0;
	float r = 0.0;

	if ( delta_energy > 0 ) // i.e. delta_energy is positive
	{
		probability_of_move = exp( -delta_energy );

		r = rand()/static_cast<float>(RAND_MAX); // 0.01 to 1 random value
		if (  r > probability_of_move) 
		{
			//cout << "Probability of movement: " << probability_of_move << " r=" << r << endl;
			return false;
		}
		else
		{
			//cout << "moving: " << r << " <= " << probability_of_move << endl;
		}
	}

	//cout << "Successful move: " << r << " <= " << probability_of_move << endl;

	the_world->sites[x][y][z].contents = this;
	the_world->sites[x_pos][y_pos][z_pos].contents = NULL;

	x_pos = x;
	y_pos = y;
	z_pos = z;

	moved = true;

	return moved;
}

void Protein::update()
{
	moved = false;

	// See if there is a partner
	z_pos == 1 ? partner = static_cast<Protein*>(the_world->sites[x_pos][y_pos][0].contents) : partner = static_cast<Protein*>(the_world->sites[x_pos][y_pos][1].contents);
	
	
	//for( int i = 0; i < 5 && !move(); i++ );
	if ( move() )
	{
		// At every move generate a new cluster and make all reachable
		// clusters point their cluster pointer to the new cluster. Add the new 
		// cluster to the world cluster list
		cluster->remove( this );
		if ( the_world->clusters.get( the_world->clusters.find( cluster ))->get_size() <= 0 ) 
		{
			the_world->clusters.erase( the_world->clusters.find( cluster ) );
		}

		cluster = new Cluster( the_world, static_cluster_id++ );
		cluster->add( this );
		
		the_world->clusters.insert( cluster );
		sync_cluster();

		moved = true;
		
	}
	else if ( partner != NULL && partner->moved == false)
	{
		if( move_dimer() )
		{
			moved = true;
			partner->moved = true;
			//cout << "Moved dimer" << endl;
		}
	}

	partner = NULL;
}
 
void Protein::draw()
{
  return;
  

	//*******************************
// my adjacency count
	/*
	int my_adjacency_count = 0;
	for ( int i = -1; i <= 1; i++ )
	{
		for ( int j = -1; j <= 1; j++ )
		{
			int new_x = x_pos + i, new_y = y_pos + j, new_z = 0;

			wrap_location( new_x, new_y, new_z );

			if ( the_world->sites[new_x][new_y][new_z].contents != NULL )
			{
				if ( !(new_x == x_pos && new_y == y_pos && new_z == z_pos) )
				{
					my_adjacency_count++;
				}
			}
			else
			{
				//my_adjacency_count++;
			}
		}
	}
	*/

	//*******************************

	if (z_pos == 0 ) glColor3f(1.0, 0.0, 0.0);
	else glColor3f(0.0, 0.0, 1.0);

	
	if ( z_pos == 1 && the_world->sites[x_pos][y_pos][0].contents != NULL ) glColor3f(0.0, 0.0, 0.0);
	else if ( z_pos == 0 && the_world->sites[x_pos][y_pos][1].contents != NULL ) glColor3f(0.0, 0.0, 0.0);
	

	/*
	if ( z_pos == 0 )
	{
		
		if ( cluster->get_size() > 1 )
		{
			glColor3f(1.0, 0.0, 1.0);	
		}
		else
		{
			glColor3f(1.0, 1.0, 0.0);
		}
		
		
		if ( the_world->sites[x_pos][y_pos][1].contents != NULL )
		{
			glColor3f(1.0, 1.0, 1.0);
		}
	}

	if ( z_pos == 1 )
	{
		
		
		if ( cluster->get_size() > 1 )
		{
			glColor3f(0.0, 1.0, 1.0);	
		}
		else
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		

		if ( the_world->sites[x_pos][y_pos][0].contents != NULL )
		{
			glColor3f(1.0, 1.0, 1.0);
		}

	}
*/
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

	//glLoadIdentity();

	// Draw cluster affiliation
/*
	glColor3f(0.0, 0.0, 0.0);
	char label3[20];
	itoa( id, label3, 10 );
	glRasterPos3f(x-width_scale/4, y, z);
		
	for( int i = 0; i < strlen(label3); i++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, label3[i] );
	}
*/
	glColor3f(1.0, 1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
}

void Protein::set_x( int x ) { x_pos = x; }

void Protein::set_y( int y ) { y_pos = y; }

void Protein::set_z( int z ) { z_pos = z; }

int Protein::get_z() { return z_pos; }

int Protein::get_x() { return x_pos; }

int Protein::get_y() { return y_pos; }

Entity_type Protein::get_type()
{
	return type;
}
// returns the sum of an array of integers
float Protein::sum(float* array, int length)
{
	float sum = 0;

	for ( int i = 0; i < length; i++ )
	{
		sum += array[ i ];
	}

	return sum;
}
	void Protein::wrap_location( int& x, int& y, int& z )
	{
		int height = the_world->world_height;
		int width = the_world->world_width;
		int depth = the_world->world_depth;

		// Wrap in the x direction
		if ( x < 0 )
		{
			x = width + x;
		}
		else if ( x == width )
		{
			x = 0;
		}
		else if ( x > width )
		{
			x = x - width;
		}

		// Wrap in the y direction
		if ( y < 0 )
		{
			y = height + y;
		}
		else if ( y == height )
		{
			y = 0;
		}
		else if ( y > height )
		{
			y = y - height;
		}

		// Wrap in the z direction
		if ( z < 0 )
		{
			z = depth + x;
		}
		else if ( z == depth )
		{
			z = 0;
		}
		else if ( z > depth )
		{
			z = z - depth;
		}

	}

	void Protein::sync_cluster()
	{
		
		sync_cluster_helper(0,1);
		sync_cluster_helper(1,0);
		sync_cluster_helper(0,-1);
		sync_cluster_helper(-1,0);
		
	}
	
	void Protein::sync_cluster_helper(int x_offset, int y_offset)
	{
		int x_new = 0, y_new = 0, z_new = 0;

		x_new = x_pos + x_offset; y_new = y_pos + y_offset, z_new = z_pos;
		wrap_location( x_new, y_new, z_new );

		Protein* adjacent_protein = static_cast<Protein*>(the_world->sites[x_new][y_new][z_new].contents);

		if ( adjacent_protein != NULL )
		{
			if ( adjacent_protein->cluster != cluster )
				//if ( static_cast<Protein*>(the_world->current_sites[x_new][y_new][z_new].contents)->cluster_id != cluster_id )
			{
				adjacent_protein->cluster->remove(adjacent_protein);
				if ( adjacent_protein->cluster->get_size() <= 0 )
				{
					the_world->clusters.erase( the_world->clusters.find( adjacent_protein->cluster ) );
				}
				cluster->add( adjacent_protein );
				adjacent_protein->sync_cluster();
			}
		}

	}

	float Protein::get_adjacency(void)
	{
		return the_world->sites[x_pos][y_pos][z_pos].get_adjacency();
	}


	void Protein::link_partner(Protein* p)
	{
		partner = p;
		p->partner = this;
	}

	bool Protein::move_dimer(void)
	{
		// DEBUGGING
		//int n_proteins = the_world->count_proteins();


		// 1. Pick a random direction for the dimer
		// 2. Evaluate the change in energy for each membrane configuration
		// 3. Sum the energy changes and use in the movement probability exponential
		// 4. Move the proteins to the new locations or return

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

		int this_x = x_pos + x_offset, this_y = y_pos + y_offset, this_z = z_pos;
		int partner_x = partner->x_pos + x_offset, partner_y = partner->y_pos + y_offset, partner_z = partner->z_pos;

		wrap_location( this_x, this_y, this_z );
		wrap_location( partner_x, partner_y, partner_z );

		// Three cases:
		// 1) protein at both target sites
		// 2) protein at only one of the target sites
		// 3) no proteins at either target site
	
		// PROTEIN AT BOTH TARGET SITES - SWAP WITH OTHER DIMER, NO CHANGE
		if ( the_world->sites[this_x][this_y][this_z].contents != NULL
			&& the_world->sites[partner_x][partner_y][partner_z].contents != NULL)
		{
			return true;
		}
		
	// COMMON TO ALL REMAINING CASES - CALCULATE PROB. OF MOVE
			float this_target_adjacency = the_world->sites[this_x][this_y][this_z].get_adjacency() - 1;
			float partner_target_adjacency = the_world->sites[partner_x][partner_y][partner_z].get_adjacency() - 1;

			float this_adjacency = the_world->sites[x_pos][y_pos][z_pos].get_adjacency();
			float partner_adjacency = the_world->sites[partner->get_x()][partner->get_y()][partner->get_z()].get_adjacency();

			float this_delta = this_adjacency - this_target_adjacency;
			float partner_delta = partner_adjacency - partner_target_adjacency;

			//probability of moving
			
			float delta = 0.0;
			
			if (0 == this->z_pos)
			{
				delta = this_delta*epsilon_0 + partner_delta*epsilon_1;
			}
			else
			{
				delta = this_delta*epsilon_1 + partner_delta*epsilon_0;
			}

			float probability_of_move = 0;
	
			if ( delta > 0 ) // i.e. delta_energy is positive
			{
				probability_of_move = exp( -delta );

				float r = rand()/static_cast<float>(RAND_MAX);
				
				// FAILURE TO MOVE
				if (  r > probability_of_move) 
				{
				    //cout << "Probability of movement: " << probability_of_move << " r=" << r << endl;
					return false;
				}
			}

			// MOVE

			// Three cases:
			// PROTEIN AT ONLY ONE OF THE TARGET SITES
			// Case 1: The protein is at the target site for the partner
			
			if ( the_world->sites[this_x][this_y][this_z].contents == NULL
				&& the_world->sites[partner_x][partner_y][partner_z].contents != NULL )
			{
				// handle swap and coord update here
				Protein* temp;

				// Swap the partner with the target
				//temp = static_cast<Protein*>(the_world->sites[partner_x][partner_y][partner_z].contents);
				//the_world->sites[partner_x][partner_y][partner_z].contents = partner;
				//the_world->sites[partner->get_x()][partner->get_y()][partner->get_z()].contents = temp;

				// Clear "this"'s old site
				the_world->sites[x_pos][y_pos][z_pos].contents = NULL;

				this->set_x( this_x );
				this->set_y( this_y );
				this->set_z( this_z );

				// DEBUG
				//if ( n_proteins != the_world->count_proteins() )
				//{
				//	cout << "Error in 1" << endl;
					//exit(1);
				//}

			}
			else if ( the_world->sites[this_x][this_y][this_z].contents != NULL
				&& the_world->sites[partner_x][partner_y][partner_z].contents == NULL)
			{
				
				// handle swap and coord update here
				Protein* temp;

				// Swap "this" with the target
				//temp = static_cast<Protein*>(the_world->sites[this_x][this_y][this_z].contents);
				//the_world->sites[this_x][this_y][this_z].contents = this;
				//the_world->sites[this->get_x()][this->get_y()][this->get_z()].contents = temp;

				// Clear partner's old site
				the_world->sites[partner->get_x()][partner->get_y()][partner->get_z()].contents = NULL;

				partner->set_x( partner_x );
				partner->set_y( partner_y );
				partner->set_z( partner_z );

				// DEBUG
				//if ( n_proteins != the_world->count_proteins() )
				//{
				//	cout << "Error in 2" << n_proteins << " != " << the_world->count_proteins() << endl;
				//}
			}
			else if ( the_world->sites[this_x][this_y][this_z].contents == NULL
				&& the_world->sites[partner_x][partner_y][partner_z].contents == NULL )
			{
				// Clear "this" old site
				the_world->sites[this->get_x()][this->get_y()][this->get_z()].contents = NULL;

				this->set_x( this_x );
				this->set_y( this_y );
				this->set_z( this_z );

				// Clear partner's old site
				the_world->sites[partner->get_x()][partner->get_y()][partner->get_z()].contents = NULL;

				partner->set_x( partner_x );
				partner->set_y( partner_y );
				partner->set_z( partner_z );

				// DEBUG
				//if ( n_proteins != the_world->count_proteins() )
				//{
				//	cout << "Error in 3: " << n_proteins << " != " << the_world->count_proteins() << endl;
				//	
				//}
			}
			else
			{
				//cout << "ERROR: Impossible condition found in Protein::dimer_move()." << endl;
				return false;
			}
	
			//place proteins in the world
			the_world->sites[get_x()][get_y()][get_z()].contents = this;
			the_world->sites[partner->get_x()][partner->get_y()][partner->get_z()].contents = partner;

			return true;
}
