#pragma once

#include "World.h"
#include "List.h"

class Protein;

class Cluster
{
public:
	Cluster( World* w, int cluster_id );
	~Cluster(void);
	bool add( Protein* m );
	bool remove( Protein* m );
	bool merge( Protein* current_protein );
	int get_size();
	void move();

	int id;
	List<Protein*> members;
	World* the_world;
private:
	void merge_helper(int x_offset, int y_offset, Protein* current_protein);
};
