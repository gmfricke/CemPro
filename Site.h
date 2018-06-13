// Site.h: interface for the Site class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SITE_H)
#define SITE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Entity.h"

// Forward declaration
class World;
class CD4;
class CCR5;

class Site  
{
public:
	void set_contents( Entity* c);
	Site();
	void set_world( World* w )
	{
		the_world = w;
	}

	virtual ~Site();
	void set_coords( int x, int y, int z );

	void update();
	void draw();

	int x_pos;
	int y_pos;
	int z_pos;

	int get_adjacency(void);

	CCR5* getAnAdjacentUnboundCCR5();
	CD4* getAnAdjacentUnboundCD4();

	World* the_world;

	Entity* contents;
};

#endif // !defined(SITE_H)
