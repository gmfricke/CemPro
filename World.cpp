// World.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

//#include "pthread.h"
#include "World.h"
#include <iostream>
#include <ctime>
#include "Cluster.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "Rectangle.h"
#include <cmath>

#include "CCR5.h"
#include "CD4.h"
#include "GP120.h"

//#include "random.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

World::World( init_distribution_type init_distribution, float eps_0, float eps_1, float cross_eps, float density, int et, int x_size, int y_size ):
num_singletons(0), num_proteins(0), clock(0), r1(NULL), r2(NULL), end_time(et)
{
	init_distribution_type init_distribution_0 = RANDOM;
	init_distribution_type init_distribution_1 = RANDOM;

	epsilon_0 = eps_0;
	epsilon_1 = eps_1;
	cross_epsilon = cross_eps;

	//final_epsilon = eps;

	//cout << "Epsilon: "<<eps<< endl;
	rectangular_mixing = false;
	move_clusters = false;

	world_height = y_size;
	world_width = x_size;
	world_depth = 2;

	int space = world_height*world_width;
	
	int num_proteins_to_place = (int)(density*space);

	sites = new Site**[ world_width ];
	
	for ( int i = 0; i < world_width; i++ )
	{
		sites[ i ] = new Site*[ world_height ];
		
		for ( int k = 0; k < world_height; k++ )
		{
			sites[i][ k ] = new Site[ world_depth ];
			
			// Give the sites a pointer to the world
			for ( int j = 0; j < world_depth; j++ )
			{
				sites[i][ k ][j].set_world( this );
				sites[i][k][j].set_coords(i, k, j);
			}
		}
	}	

	// Create cluster data structure
	proteins = new Protein*[num_proteins_to_place];


	// Place proteins in layer 0
	if ( init_distribution_0 == SINGLE_CLUSTER )
	{
		// Place proteins in one quadrant
		int protein_count = 0;
		num_proteins = 0;

		for ( int i = (int)(world_width/4.0); i < world_width*(3.0/4.0) && num_proteins < num_proteins_to_place; i++ )
		{
		  for ( int j = (int)(world_width/4.0); j < world_height*(3.0/4.0) && num_proteins < num_proteins_to_place; j++ )
			{
				int x_pos = i;
				int y_pos = j;
				int z_pos = 0;
				wrap_location( x_pos, y_pos, z_pos );
				sites[x_pos][y_pos][z_pos].contents = new Protein( this, x_pos, y_pos, z_pos );
			}
		}
	}
	else if ( init_distribution_0 == RANDOM )
	{
		int protein_count = 0;
		num_proteins = 0;
	
		while ( num_proteins < num_proteins_to_place ) 
		{
		  // Place a CCR5
		  int x_pos = rand() % world_width;
		  int y_pos = rand() % world_height;
		  int z_pos = 0;		
		  
		  wrap_location( x_pos, y_pos, z_pos );
		  
		  if ( sites[x_pos][y_pos][z_pos].contents == NULL )
		    {
		      sites[x_pos][y_pos][z_pos].contents = new CCR5( this, x_pos, y_pos, z_pos );
		      layer_0_count++;
		    }
		  
		  // Place a CD4
		  
		  x_pos = rand() % world_width;
		  y_pos = rand() % world_height;
		  z_pos = 0;		
		  
		  wrap_location( x_pos, y_pos, z_pos );
		  
		  if ( sites[x_pos][y_pos][z_pos].contents == NULL )
		    {
		      sites[x_pos][y_pos][z_pos].contents = new CD4( this, x_pos, y_pos, z_pos );
		      layer_0_count++;
		    }
		}
	}
	else
	{
		cout << "Unknown initialization" << endl;
	}



	// Place proteins in layer 1
	if ( init_distribution_1 == SINGLE_CLUSTER )
	{
		// Place proteins in one quadrant
		num_proteins = 0; // TEMP SOLUTION
		int protein_count = 0;
		for ( int i = (int)(world_width/4.0); i < world_width*(3.0/4.0) && num_proteins < num_proteins_to_place; i++ )
		{
		  for ( int j = (int)(world_width/4.0); j < world_height*(3.0/4.0) && num_proteins < num_proteins_to_place; j++ )
			{
				int x_pos = i;
				int y_pos = j;
				int z_pos = 1;
				//wrap_location( x_pos, y_pos, z_pos );
				sites[x_pos][y_pos][z_pos].contents = new Protein( this, x_pos, y_pos, z_pos );
			}
		}
	}
	else if ( init_distribution_1 == RANDOM )
	{
		int protein_count = 0;
		num_proteins = 0;
		while ( num_proteins < num_proteins_to_place ) 
		{
			int x_pos = rand() % world_width;
			int y_pos = rand() % world_height;
			int z_pos = 1;		

			wrap_location( x_pos, y_pos, z_pos );

			if ( sites[x_pos][y_pos][z_pos].contents == NULL )
			{
				sites[x_pos][y_pos][z_pos].contents = new GP120( this, x_pos, y_pos, z_pos );
				layer_1_count++;
			}
		}
	}
	else
	{
		cout << "Unknown initialization" << endl;
	}

	cout << "Placed " << layer_0_count << " proteins in membrane 0" << endl;
	cout << "Placed " << layer_1_count << " proteins in membrane 1" << endl;
}

World::~World()
{
	for ( int i = 0; i < world_height; i++ )
	{
		delete [] sites[i];
	}

	delete [] sites;
}

void World::reset( void )
{

}

void World::update()
{
	//epsilon += final_epsilon/end_time;
	//cout << "epsilon: " << epsilon << endl;

	// Update sites
	if (clock < end_time ) 
		for ( int i = 0; i < world_width; i++ )
		{
			for ( int j = 0; j < world_height; j++ )
			{
				for ( int k = 0; k < world_depth; k++ )
				{
					if ( sites[i][j][k].contents != NULL )
					{
					  sites[i][j][k].update();
					}
				}
			}
		}

	int clustered_proteins_sum = 0;

	// swap random squares
	if (rectangular_mixing) mix();

	//cout << "Proteins in clusters: " << clustered_proteins_sum << endl;

	// Find the size of the largest cluster
	int max = 0;
	for ( int i = 0; i < clusters.get_size(); i++ )
	{
		if ( clusters.get(i)->get_size() > max )
		{
			max = clusters.get(i)->get_size();
		}		
	}

	// Count proteins and singletons
	//int protein_count = count_proteins();

	//cout << "protein count: " << protein_count << endl;
	//cout << "num_singletons: " << num_singletons << endl;

	//if ( clock > 15000 ) exit(0);

	running_singleton_sum[clock%500] = num_singletons;
	
	/*
	if ( clock % 100 == 0 && clock > 10000 ) 
	{
		cout << clock << " ";
		cout << "Singletons: " << num_singletons/(1.0*protein_count) << " " << protein_count << " ";
		cout << " Clusters: " << clusters.get_size();
		cout << " Largest cluster: " << max << endl;
	}

	//int random_index = rand() % num_proteins;
	
	//if ( protein_count < num_proteins ) Sleep(1000000);
	*/

	if ( clock > end_time )
	{
		//for ( int i = 0; i < 25; i++ )
		//{
		//	cout << get_correlation( i, CROSS ) << ",";
		//}
		//cout << endl;

		//cout << cross_epsilon << "\t" << epsilon_1 << "\t" << phase() << endl;
		//cout << "Cross membrane dimers: " << cross_dimer_fraction() << endl;
		//count_proteins();

		//exit(0);
	}

	//cout << cross_dimer_fraction() << endl;
	

	if (clock % 500 == 0 ) cout << "Clock: " << clock << endl;
	
	// Update the clock
	clock++;

}

void World::wrap_location( int& x, int& y, int& z )
{
	int height = world_height;
	int width = world_width;
	int depth = world_depth;

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



float World::get_correlation( int d, correlation_type correlate )
{

	// Correlation type can be MEMBRANE_0, MEMBRANE_1, or CROSS

	float cor = 0.0;

	int this_membrane = 0;
	int target_membrane = 0;

	if ( CROSS == correlate )
	{
		this_membrane = 0;
		target_membrane = 1;
	}
	else if ( MEMBRANE_0 == correlate )
	{
		this_membrane = 0;
		target_membrane = 0;
	}
	else if ( MEMBRANE_1 == correlate )
	{
		this_membrane = 1;
		target_membrane = 1;
	}
	else
	{
		cout << "Error: Unknown correlation type in get_correlation( int d )" << endl;
	}

	for ( int i = 0; i < world_width; i++ )
	{
		for ( int j = 0; j < world_height; j++ )
		{
			if ( sites[i][j][this_membrane].contents != NULL )
			{
				cor += sites[i][j][this_membrane].contents->get_correlation(d, target_membrane);
			}
		}
	}

	return cor;
}
float World::get_singleton_running_average(void)
{
	float sum = 0.0;
	for ( int i = 0; i < 500; i++ )
	{
		sum += running_singleton_sum[i]/(1.0*num_proteins);
	}
	return sum/500.0;
}

void World::set_epsilon(float e)
{
	epsilon_0 = epsilon_1 = cross_epsilon = e;
}

void World::mix()
{

	/*
	// 1. Choose the size and position of the rectangle by randomly picking two 
	// sets of x,y coordinates

	if (r1) delete r1;
	if (r2) delete r2;
	
	int x_length = (rand() % (world_width/4)); //4) + 2;
	int y_length = (rand() % (world_height/4)); //4) + 2;

	r1 = new My_Rectangle( this, x_length, y_length );
	r2 = new My_Rectangle( this, x_length, y_length );

	int rand_xloc = rand() % (world_width);
	int rand_yloc = rand() % (world_width);

	r1->upper_right->set_xy( rand_xloc, rand_yloc );
	r1->upper_left->set_xy( rand_xloc + x_length, rand_yloc );
	r1->lower_right->set_xy( rand_xloc, rand_yloc + y_length );
	r1->lower_left->set_xy( rand_xloc + x_length, rand_yloc + y_length );

	// Translate the rectangle
	//int r1_xtranslation = rand() % world_width;
	//int r1_ytranslation = (rand() % 3) - 1; //rand() % world_width;
	//r1->translate( r1_xtranslation, r1_ytranslation );

	// Position r2
	r2->lower_left->set_xy( r1->lower_left->get_x(), r1->lower_left->get_y() ); 
	r2->lower_right->set_xy( r1->lower_right->get_x(), r1->lower_right->get_y() ); 
	r2->upper_left->set_xy( r1->upper_left->get_x(), r1->upper_left->get_y() ); 
	r2->upper_right->set_xy( r1->upper_right->get_x(), r1->upper_right->get_y() ); 
	

	// add x or y length to clear the first rectangle so that
	// there is no overlap
	int r2_xtranslation = 0;
	int r2_ytranslation = 0;

	
	if (rand() % 2 == 0)
	{
		r2_xtranslation += x_length + (rand() % (world_width - 2*x_length));
		r2_ytranslation = rand() % (world_height - y_length);
	}
	else
	{
		r2_ytranslation += y_length + (rand() % (world_height - 2*y_length));
		r2_xtranslation = rand() % (world_width - x_length);
	}
	

	

	//cout << "Rect1: ";
	//r1->print();
	//cout << endl;

	r2->translate( r2_xtranslation, r2_ytranslation ); 

	//cout << "Rect2: ";
	//r2->print();
	//cout << endl;
	//cout << endl;

//	r1->draw();
	//r2->draw();

	// 2.1 Get the energy of the rectangles
	float r1_initial_energy = get_rectangle_energy( r1 );
	float r2_initial_energy = get_rectangle_energy( r2 );

	// 3. Perform the swap
	// a. Copy proteins in rectangle 2 to a temp location
	// Create the temp space
	swap_rectangles( r1, r2 );
	
	// 4. Calculate the change in energy along the borders of the two rectangles
	// just the borders or the energy of the proteins adjacent to the rectangle?
	// don't forget to do both rectangles
	
	float r1_new_energy = get_rectangle_energy( r1 );
	float r2_new_energy = get_rectangle_energy( r2 );

	// 5. If the total energy has increased possibly reverse the swap using the same
	// rules as for protein motion
	// Swap again if energy increases

	float initial_energy = r1_initial_energy + r2_initial_energy;
	float new_energy = r1_new_energy + r2_new_energy;

	// Normalize the delta in energy by the number of proteins (?)
	int num_border_proteins = r1->count_border_proteins()+r2->count_border_proteins();

	float delta_energy = 0.0;
	float probability_of_move = 0.0;


	if ( num_border_proteins != 0 )
	{
		delta_energy = (initial_energy - new_energy)/num_border_proteins;
		probability_of_move = exp( -epsilon*delta_energy );
	}

	if ( delta_energy >= 0 )
	{
		//cout << "Prob: " << probability_of_move << endl;

		if ( (rand() % 1000)/1000.0 > probability_of_move )
		{
			swap_rectangles( r1, r2 );
			//cout << "******MOVE REJECTED*****" << endl << endl;
			//exit(0);		
		}
		else
		{
			
			
			//cout << "MOVE ACCEPTED" << endl << endl;
		}
	}
	else
	{
		/*
		cout << clock << " | Move Accepted: Delta Negative: " << delta_energy << endl;
		cout << "R1 Energy: " << get_rectangle_energy( r1 ) << endl;
		r1->print(); cout << endl;
		cout << "R1 x length: " << r1->x_length << ", " << r1->upper_left->get_x() - r1->upper_right->get_y() << endl;
		cout << "R1 y length: " << r1->y_length << ", " << r1->lower_right->get_y() - r1->upper_right->get_y() << endl;
		cout << "R1 num proteins" << r2->count_border_proteins() << endl;
		cout << "R2 Energy: " << get_rectangle_energy( r2 ) << endl;
		r2->print(); cout << endl;
		cout << "R2 x length: " << r2->x_length << ", " << r2->upper_left->get_x() - r2->upper_right->get_y() << endl;
		cout << "R2 y length: " << r2->y_length << ", " << r2->lower_right->get_y() - r2->upper_right->get_y() << endl;
		cout << "R2 num proteins" << r2->count_border_proteins() << endl;
		cout << endl;
		Sleep(15000);
	}

		*/

}

void World::swap_rectangles( My_Rectangle* r1, My_Rectangle* r2 )
{
	int x_length = r1->x_length;
	int y_length = r1->y_length;

	Entity*** temp = new Entity**[ x_length ];
	for ( int i = 0; i < x_length; i++ )
	{
		temp[i] = new Entity*[ y_length ];
	}
	
	// copy the proteins in rectangle 2 into the temp space
	for ( int i = 0; i < x_length; i++ )
		for ( int j = 0; j < y_length; j++ )
		{
			int r2_x_coord = r2->upper_right->get_x() + i;
			int r2_y_coord = r2->upper_right->get_y() + j;
			int z_coord = 0;
			wrap_location( r2_x_coord, r2_y_coord, z_coord );
			temp[i][j] = sites[ r2_x_coord ][ r2_y_coord ][z_coord].contents;
		}
	
	// b. Copy proteins in rectangle 1 to rectangle 2

	for ( int i = 0; i < x_length; i++ )
	{
		for ( int j = 0; j < y_length; j++ )
		{
			//cout << "(" << i << ", " << j << ") ";
			int r1_x_coord = r1->upper_right->get_x() + i;
			int r1_y_coord = r1->upper_right->get_y() + j;
			int r2_x_coord = r2->upper_right->get_x() + i;
			int r2_y_coord = r2->upper_right->get_y() + j;
			
			int z_coord = 0;
			wrap_location( r1_x_coord, r1_y_coord, z_coord );
			wrap_location( r2_x_coord, r2_y_coord, z_coord );
			sites[ r2_x_coord ][ r2_y_coord ][ z_coord ].contents = sites[ r1_x_coord ][ r1_y_coord ][ z_coord ].contents ;
			
			if (sites[ r2_x_coord ][ r2_y_coord ][ z_coord ].contents != NULL )
			{
				static_cast<Protein*>(sites[ r2_x_coord ][ r2_y_coord ][ z_coord ].contents)->set_x( r2_x_coord );
				static_cast<Protein*>(sites[ r2_x_coord ][ r2_y_coord ][ z_coord ].contents)->set_y( r2_y_coord );
			}
		}
		//cout << endl;
	}
	// c. Copy proteins in temp to rectangle 1
	for ( int i = 0; i < x_length; i++ )
		for ( int j = 0; j < y_length; j++ )
		{	
			int r1_x_coord = r1->upper_right->get_x() + i;
			int r1_y_coord = r1->upper_right->get_y() + j;
			int z_coord = 0;
			wrap_location( r1_x_coord, r1_y_coord, z_coord );
			sites[ r1_x_coord ][ r1_y_coord ][ z_coord ].contents = temp[i][j];

			if ( temp[i][j] != NULL )
			{
				static_cast<Protein*>(temp[i][j])->set_x( r1_x_coord );
				static_cast<Protein*>(temp[i][j])->set_y( r1_y_coord );
			}
		}

	// delete the temp space
	for ( int i = 0; i < x_length; i++ )
	{
		delete [] temp[i];
	}	
	delete [] temp;
}

float World::get_rectangle_energy( My_Rectangle* rect )
{
	float top_energy = 0;
	float bottom_energy = 0;
	float left_energy = 0;
	float right_energy = 0;

	int x_length = rect->x_length;
	int y_length = rect->y_length;
	int num_proteins = 0;

	// top row
	for ( int i = 0; i < x_length ; i++ )
	{
		int x_coord = rect->upper_right->get_x() + i;
		int y_coord = rect->upper_right->get_y();
		int z_coord = 0;
		wrap_location( x_coord, y_coord, z_coord );

		if ( sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{			
			num_proteins++;
			top_energy += sites[ x_coord ][ y_coord ][ z_coord ].get_adjacency();
		}
	}

	// bottom row
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = rect->lower_right->get_x() + i;
		int y_coord = rect->lower_right->get_y();
		int z_coord = 0;
		wrap_location( x_coord, y_coord, z_coord );

		if ( sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
			bottom_energy += sites[ x_coord ][ y_coord ][ z_coord ].get_adjacency();
		}
	}

	// left column
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = rect->upper_left->get_x();
		int y_coord = rect->upper_left->get_y() + i;
		int z_coord = 0;
		wrap_location( x_coord, y_coord, z_coord );

		if ( sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
			left_energy += sites[ x_coord ][ y_coord ][ z_coord ].get_adjacency();
		}
	}

	// right column
	for ( int i = 0; i < x_length; i++ )
	{
		int x_coord = rect->upper_right->get_x();
		int y_coord = rect->upper_right->get_y() + i;
		int z_coord = 0;
		wrap_location( x_coord, y_coord, z_coord );

		if ( sites[ x_coord ][ y_coord ][ z_coord ].contents != NULL )
		{
			num_proteins++;
			right_energy += sites[ x_coord ][ y_coord ][ z_coord ].get_adjacency();
		}
	}

	//cout << "top energy: " << top_energy << endl;
	//cout << "bottom energy: " << bottom_energy << endl;
	//cout << "left energy: " << left_energy << endl;
	//cout << "right energy: " << right_energy << endl;
	//cout << "num_proteins: " << num_proteins << endl;
	return top_energy + bottom_energy + left_energy + right_energy;
}

void World::set_density(double d)
{
	density = d;
}

int World::count_proteins(void)
{
	int protein_count = 0;
	int dimers = 0;

	for ( int i = 0; i < world_width; i++ )
	{
		for ( int j = 0; j < world_height; j++ )
		{
			for ( int k = 0; k < world_depth; k++ )
			{
				if ( sites[i][j][k].contents != NULL )
				{
					protein_count++;
					if ( sites[i][j][k].contents != NULL && k == 1 )
					{
						if ( sites[i][j][0].contents != NULL )
						{
							dimers++;
						}
					}
				}
			}
		}
	}
	
	if (clock == 5000) 
	{
		cout << "Dimers: " << dimers << endl;
		exit(0);
	}

	return protein_count;
}

float World::phase(void)
{

	int num_sites = world_height*world_width*world_depth;

	float* densities = new float[num_sites]; // don't need to initialize densities
	// because I set the value explicitly using get_adjacency()

	int m = 0; // index into densities
	float total = 0.0;

	// Iterate through all sites and get local density and put into an array
	for ( int i = 0; i < world_width; i++ )
	{
		for ( int j = 0; j < world_height; j++ )
		{
			for ( int k = 0; k < world_depth; k++ )
			{
				// The adjaceny count is the local density
				densities[m] = sites[i][j][0].get_adjacency();

				// Add one if the middle site of the nine is occupied
				if ( sites[i][j][0].contents != NULL )
				{
					densities[m]++;
				}

				total += densities[m];
				m++;
			}
		}
	}


	// Take the standard deviation of the array of densities. The lower
	// the standard deviation the more ordered the system. The higher it is
	// the more random

    float mean = total/num_sites;
	
	float sum_deltas = 0.0;

	// Calculate x - x_bar (i.e. densities[x] - mean)
	for ( int i = 0; i < num_sites; i++ )
	{
		float delta = densities[i] - mean;
		sum_deltas += delta*delta;
	}

	float stdev = sqrt( sum_deltas/(num_sites - 1) );

	delete densities;
	
	return stdev/9.0; // every protein is seen 9 times so divide that out
}

float World::cross_dimer_fraction(void)
{

	int side_length = world_width;
	int dimer_count = 0;
	int protein_count = 0;

	for ( int i = 0; i < side_length; i++ )
	{
		for ( int j = 0; j < side_length; j++ )
		{
			if ( sites[i][j][0].contents != NULL )
			{				
				protein_count++;
				if ( sites[i][j][0].contents->get_adjacency() != 0 )
				{
					dimer_count++;
				}
			}
		}
	}

	return dimer_count/(protein_count*1.0);
}
