#ifndef CD4_H
#define CD4_H
#include "World.h"
#include "Protein.h"
#include "CCR5.h"

class CCR5;

class CD4: public Protein
{
 public:
  CD4( World* world, int init_x, int init_y, int init_z );
  void move();
  void update();
  void draw();

  // Homodimer
  void bindCD4( CD4* partner );
  void unbindCD4();
  bool isUnboundToCD4();
  CD4* getBoundCD4();
  CD4* getAnAdjacentUnboundCD4(int x, int y, int z);

  // CCR5 Hetrodimer
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
