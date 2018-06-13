#include "Cluster.h"
#include "Protein.h"

#include <cstdlib>
#include <iostream>

using namespace std;

#ifndef NULL
#define NULL 0
#endif

Cluster::Cluster( World* w, int cluster_id )
: the_world(w), id( cluster_id )
{
}

Cluster::~Cluster(void)
{
}

bool Cluster::add( Protein* m )
{
	if ( members.find(m) != -1 )
	{
		cout << "Warning: attempt to add duplicate protein to cluster" << endl;
		return false;
	}

	members.insert( m );
	m->cluster = this;
	return true;
}

bool Cluster::remove( Protein* m )
{
	int m_indx = members.find( m );
	
	if (m_indx == -1)
	{
		cout << "Warning: attempt to remove non-member protein from cluster" << endl;
		return false;
	}
	else
	{
		members.erase( m_indx );
	}

	//m->cluster = NULL;

	return true;
}

bool Cluster::merge( Protein* current_protein )
{
	//cout << "Current protein: " << current_protein->get_x() << " " << current_protein->get_y() << endl;
	merge_helper(0,1,current_protein);
	merge_helper(1,0,current_protein);
	merge_helper(0,-1,current_protein);
	merge_helper(-1,0,current_protein);

	return false;
}

int Cluster::get_size()
{
	return members.get_size();
}

// Moves all the members of a cluster in the same direction
// by definition of cluster there will be no obsticals (i.e.
// adjacent non-member proteins)
void Cluster::move()
{	
	//cout << " <<< Begin cluster motion >>>" <<  endl;

	merge( members.get(0) );

	bool cluster_adjacent = false;

	//updating the frame must be done here (not by protein update)
	int move_direction = rand() % 5;
	int x_offset = 0;
	int y_offset = 0;
	int old_z = 0;

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

	// Remove the cluster from sites so there is no self intersection
	for ( int i = 0; i < get_size(); i++ )
	{
		the_world->sites[members.get(i)->get_x()][members.get(i)->get_y()][members.get(i)->get_z()].contents = NULL;
	}

	// Update coordinates for all the member proteins
	for ( int i = 0; i < get_size(); i++ )
	{
		int new_x = members.get(i)->get_x() + x_offset;
	    int new_y = members.get(i)->get_y() + y_offset;
		int new_z = 0;

		the_world->wrap_location( new_x, new_y, new_z );

		members.get(i)->set_x( new_x );
		members.get(i)->set_y( new_y );
		members.get(i)->set_z( new_z );
	}

	// Write the cluster back to sites in the new location
	for ( int i = 0; i < get_size(); i++ )
	{
		Protein* new_loc = static_cast<Protein*>(the_world->sites[members.get(i)->get_x()][members.get(i)->get_y()][members.get(i)->get_z()].contents);
		if ( new_loc != NULL )
		{
			//cout << "Error!" << endl;
			//if ( new_loc->cluster != this ) cout << "other" << endl;
		}
		the_world->sites[members.get(i)->get_x()][members.get(i)->get_y()][members.get(i)->get_z()].contents = members.get(i);
	}

	// sync clusters (absorb any proteins now adjacent)
	merge( members.get(0) );

	//cout << " <<< End cluster motion >>>" <<  endl << endl;

}


void Cluster::merge_helper(int x_offset, int y_offset, Protein* current_protein)
{
		
		int x_new = current_protein->get_x() + x_offset,
			y_new = current_protein->get_y() + y_offset, 
			z_new = current_protein->get_z();

		the_world->wrap_location( x_new, y_new, z_new );

		//cout << "Examining " << x_new << " " << y_new << "... ";

		Protein* adjacent_protein = static_cast<Protein*>(the_world->sites[x_new][y_new][z_new].contents);
		
		if ( adjacent_protein != NULL )
		{
			if ( adjacent_protein->cluster != this )
			{
				adjacent_protein->cluster->remove(adjacent_protein);
				if ( adjacent_protein->cluster->get_size() <= 0 )
				{
					the_world->clusters.erase( the_world->clusters.find( adjacent_protein->cluster ) );
				}
				
				int old_size = get_size();
				add( adjacent_protein );
				//cout << "Added protein " << adjacent_protein->id << ", ";
				//cout << "Cluster grew from " << old_size << " to " << get_size() << endl; 
				merge( adjacent_protein );
			}
			else
			{
				//cout << " already in this cluster" << endl;
			}
		}
		else
		{
			//cout << "empty" << endl;
		}

}
