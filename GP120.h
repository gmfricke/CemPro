#include "World.h"
#include "Protein.h"

class GP120: public Protein
{
 public:
  GP120( World* world, int init_x, int init_y, int init_z );
  void move();
  void draw();
};
