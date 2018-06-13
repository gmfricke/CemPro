// Protein.h: interface for the Protein class.

#if !defined(AFX_AGENT_H__E5F060F3_D0FF_4D5C_9A7C_89E34D18A90D__INCLUDED_)
#define AFX_AGENT_H__E5F060F3_D0FF_4D5C_9A7C_89E34D18A90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Entity.h"

#include <cmath>

class World;
class Cluster;

class Protein : public Entity
{

public:

	Protein( World* world, int init_x, int init_y, int init_z );
	Protein(); // For inheritance

	void update();
	void draw();
	bool move();
	float get_correlation( int d, int target_membrane );
	void update_cluster_membership();

	virtual ~Protein();

	int get_x();
	int get_y();
	int get_z();

	void set_x( int x );
	void set_y( int y );
	void set_z( int z );
	
	Entity_type get_type();
	void sync_cluster();

	Cluster* cluster;
	long unsigned int cluster_id;

	int id;
	bool updated;
	Protein* partner;

protected:
	void wrap_location( int& x, int& y, int& z );
	

	float sum(float* array, int length);


	World* the_world;
	int x_pos, y_pos, z_pos;
	Entity_type type;

	void sync_cluster_helper(int x_offset, int y_offset);
public:
	float get_adjacency(void);
	void link_partner(Protein* p);
	bool move_dimer(void);
	bool break_dimer(void);

	bool moved;
};

#endif // !defined(AFX_AGENT_H__E5F060F3_D0FF_4D5C_9A7C_89E34D18A90D__INCLUDED_)
