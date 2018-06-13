#include "GP120.h"
#include "glut.h"

GP120::GP120( World* world, int init_x, int init_y, int init_z ) : Protein( world, init_x, init_y, init_z )
{

}

void GP120::move()
{
}

void GP120::draw()
{
  return;


  glColor3f(1.0, 0.0, 1.0);

  glPolygonMode(GL_FRONT, GL_FILL);

  // draw the object
  float depth_scale = 2.0/the_world->world_depth;
  float height_scale = 2.0/the_world->world_height;
  float width_scale = 2.0/the_world->world_width;
  
  float x = (the_world->world_width/2.0)*width_scale - width_scale/4 - x_pos*width_scale/2;
  float y = (the_world->world_height/2.0)*height_scale - height_scale/4 - y_pos*height_scale/2;
  float z = (the_world->world_depth/2.0)*depth_scale - depth_scale/4 - z_pos*depth_scale/2;
  
  glTranslatef(x, y, z);
  glutSolidCube( width_scale/2.0 );	
  glTranslatef(-x, -y, -z);
  
  glColor3f(1.0, 1.0, 1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
