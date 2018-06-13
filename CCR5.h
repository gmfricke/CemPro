#ifndef CCR5_H
#define CCR5_H
#include "World.h"
#include "Protein.h"

// Forward declaration
class CD4;

class CCR5: public Protein
{
 public:
  CCR5( World* world, int init_x, int init_y, int init_z );
  void move();
  void draw();
  void update();

  // CD4 Hetrodimer
  void bindCD4( CD4* partner );
  void unbindCD4();
  bool isUnboundToCD4();
  CD4* getBoundCD4();
  CD4* getAnAdjacentUnboundCD4(int x, int y, int z);;

  // Homodimer
  void bindCCR5( CCR5* partner );
  void unbindCCR5();
  bool isUnboundToCCR5();
  CCR5* getBoundCCR5();
  CCR5* getAnAdjacentUnboundCCR5(int x, int y, int z);

 private:
  
  CCR5* bound_ccr5;
  CD4* bound_cd4;
};
#endif
