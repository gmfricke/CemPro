// World.h: interface for the World class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLD_H__54940B9C_BC69_4CE2_89BB_9C1764D89B26__INCLUDED_)
#define AFX_WORLD_H__54940B9C_BC69_4CE2_89BB_9C1764D89B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"
#include "Protein.h"
#include "Site.h"

#include <sys/timeb.h>

enum drawing_type {CORRELATION, PROTEINS};
enum init_distribution_type {SINGLE_CLUSTER, RANDOM};
enum correlation_type {MEMBRANE_0, MEMBRANE_1, CROSS};


// Forward declaration
class My_Rectangle;
class Site;

class World  
{
public:

	World( init_distribution_type init_distribution, float eps_0, float eps_1, float cross_eps, float density, int et, int x_size, int y_size );
	virtual ~World();

	void update(void);
	void reset(void);
	void wrap_location( int& x, int& y, int& z );

	float get_correlation( int d, correlation_type correlate );

	Site*** sites;
	int world_height;
	int world_width;
	int world_depth;
	long unsigned int clock;
	Protein** proteins;
	int num_proteins;
	float density;
	int num_singletons;
	int running_singleton_sum[500];

	int layer_1_count;
	int layer_0_count;

	float epsilon_0;
	float epsilon_1;
	float cross_epsilon;

	List<Cluster*> clusters;
	float get_singleton_running_average(void);
	void set_epsilon(float e);

	bool move_clusters;

	My_Rectangle* r1;
	My_Rectangle* r2;

	bool rectangular_mixing;
	int end_time;
	float final_epsilon;

private:
	void swap_rectangles(My_Rectangle* rect1, My_Rectangle* rect2);
	float get_rectangle_energy(My_Rectangle* r);
	void mix(void);
	
public:
	void set_density(double d);
	int count_proteins(void);
	float phase(void);
	float cross_dimer_fraction(void);
};

#endif // !defined(AFX_WORLD_H__54940B9C_BC69_4CE2_89BB_9C1764D89B26__INCLUDED_)
