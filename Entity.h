// entity.h: interface for the entity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__9808721F_ADAE_46D7_975D_DAAC1F46341E__INCLUDED_)
#define AFX_ENTITY_H__9808721F_ADAE_46D7_975D_DAAC1F46341E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum Entity_type { PROTEIN };
enum Action_type { MOVE };

class World;

class Entity  
{

public:
	Entity();
	Entity( World* world );
	virtual ~Entity();
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual Entity_type get_type() = 0;
	virtual float get_correlation(int d, int target_membrane) = 0;
	virtual void sync_cluster() = 0;
	virtual float get_adjacency() = 0;
	
private: 

};

#endif // !defined(AFX_ENTITY_H__9808721F_ADAE_46D7_975D_DAAC1F46341E__INCLUDED_)
