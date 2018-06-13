// This is the CemPro driver
//#include "stdafx.h" 
#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include "glut.h"

#include "World.h"
#include "CharString.h"
#include "Rectangle.h"

using namespace std;

void init();
void display();
void model();

bool no_display = false;
bool move_clusters = false;
bool rectangular_mixing = false;
int end_time = 1000;
float density = 0.05;
float epsilon_0 = 0.0;
float epsilon_1 = 0.0;
float cross_epsilon = 0.0;

init_distribution_type init_layout = SINGLE_CLUSTER;

World* the_world;
fstream* outfile;
int alpha_count = 0, population_count = 0;

bool display_correlation = false;
bool display_proteins = false;
int arg_position( char** command_line, int command_line_size, char* test_string );
void parse_command_line( char** command_line, int argc );

char* intToCharStr( int number, int buff_size );

int main( int argc, char* argv[] )
{

	srand( time(NULL) );

	parse_command_line( argv, argc );

	outfile = new fstream("cempro.log", ios::out|ios::app|ios::ate );
	
	// Create the world
	the_world = new World( init_layout, epsilon_0, epsilon_1, cross_epsilon, density, end_time, 100, 100 );
	the_world->rectangular_mixing = rectangular_mixing;
	the_world->move_clusters = move_clusters;

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize(640,480);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Cell Membrane Model");
	init();
	glutDisplayFunc( display );
	glutIdleFunc( model );
	glutMainLoop();



	return 0;
}

void init()
{
	glClearColor( 1.0,1.0,1.0,1.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);
}

void display()
{
	if (no_display) return;


	int width = the_world->world_width;
	int height = the_world->world_height;
	int depth = the_world->world_depth;
	
	// Clear all pixels
	glClear( GL_COLOR_BUFFER_BIT );

// Draw a white grid
/*
glBegin( GL_LINES );

for ( float i = -1.0; i <= 1.1; i += 0.1)
{
		glVertex2f(i, 1.0);
		glVertex2f(i, -1.0);		
}

// 2/world_height = increment
for ( float j = -1.0; j <= 1.1; j += 0.1)
{
		glVertex2f(1.0, j);
		glVertex2f(-1.0, j);		
}
	glEnd();
*/

	for ( int i = 0; i < 100000; i ++ )
	{
	}

	if ( display_proteins )
	{
		for ( int k = 0; k < width; k++ )
		{
			for ( int m = 0; m < height; m++ )
			{
				for ( int n = 0; n < depth; n++ )
					{
						the_world->sites[k][m][n].draw();
					}
			}
		}
		if ( the_world->r1 != NULL )
		{
			the_world->r1->draw(0);
			the_world->r2->draw(1);
			//cout << "Print: "; the_world->r1->print(); cout << endl;
		}
	}
	else if( display_correlation )
	{
		correlation_type correlate = CROSS;

		glColor3f(1.0, 1.0, 1.0);
	
		glBegin(GL_LINES);

		glVertex2f(0.03, 0.03 );
		glVertex2f(0.97, 0.03 );

		glVertex2f(0.03, 0.03 );
		glVertex2f(0.03, 0.97 );
	
		glEnd();

		CharString label = "Distance";
		glRasterPos2f(0.5, 0.0);

		for( int i=0; i < label.length; i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label[i] );
		}	

		CharString label2 = "0.0";
		glRasterPos2f(0.02, 0.0);
		
		for( int i = 0; i < label2.length; i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label2[i] );
		}

		char label3[20];
		//itoa( width/2, label3, 10 );
		
		

		glRasterPos2f(0.97, 0.0);
		
		for( int i = 0; i < strlen(label3); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label3[i] );
		}

		char* label4 = "1.0";
		glRasterPos2f(0.0, 0.97);
		
		for( int i = 0; i < strlen(label4); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label4[i] );
		}

		char* label5 = "0.5";
		glRasterPos2f(0.0, 0.5);
		
		for( int i = 0; i < strlen(label5); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label5[i] );
		}

		// KEY
		char* label6 = "Membrane 0";
		glRasterPos2f(0.7, 0.95);
		glColor3f(1.0, 1.0, 1.0);

		for( int i = 0; i < strlen(label6); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label6[i] );
		}

		glColor3f(0.0, 1.0, 1.0);
		glBegin(GL_LINE_STRIP);
		glVertex2f( 0.82, 0.96 );
		glVertex2f( 0.85, 0.96 );
		glEnd();

		char* label7 = "Membrane 1";
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(0.7, 0.9);
		
		for( int i = 0; i < strlen(label7); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label7[i] );
		}

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		glVertex2f( 0.82, 0.91 );
		glVertex2f( 0.85, 0.91 );
		glEnd();

		char* label8 = "Cross";
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(0.7, 0.85);
		
		for( int i = 0; i < strlen(label8); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, label8[i] );
		}

		glColor3f(1.0, 0.0, 1.0);
		glBegin(GL_LINE_STRIP);
		glVertex2f( 0.82, 0.86);
		glVertex2f( 0.85, 0.86);
		glEnd();

		// CROSS CORRELATION
		glColor3f(1.0, 0.0, 1.0);

			glBegin(GL_LINE_STRIP);

			for( int d = 0; d <= width/2; d++ )
			{
				glVertex2f( 0.03 + d/((width/2)*1.0), 0.03 + the_world->get_correlation( d, CROSS )/(the_world->num_proteins) );
				
				//cout << "Correlation: d = " << d << ": " << the_world->get_correlation( d, correlate ) << "/" << the_world->num_proteins << endl;
			}
	
			glEnd();

			// MEMBRANE_0 CORRELATION
			glColor3f(0.0, 1.0, 1.0);

			glBegin(GL_LINE_STRIP);

			for( int d = 0; d <= width/2; d++ )
			{
				glVertex2f( 0.03 + d/((width/2)*1.0), 0.03 + the_world->get_correlation( d, MEMBRANE_0 )/(the_world->num_proteins) );
				
			}
	
			glEnd();

			// MEMBRANE_1 CORRELATION
			glColor3f(1.0, 0.0, 0.0);

			glBegin(GL_LINE_STRIP);

			for( int d = 0; d <= width/2; d++ )
			{
				glVertex2f( 0.03 + d/((width/2)*1.0), 0.03 + the_world->get_correlation( d, MEMBRANE_1 )/(the_world->num_proteins) );
				
			}
	
			glEnd();

	}

	//glFlush();
	glutSwapBuffers();

}

void model()
{
	//cout << the_world->clock << endl;
	//Sleep(1000);
	the_world->update();
	//cout << "Proteins: " << the_world->num_proteins << endl;
	glutPostRedisplay();
}

// return the index into the command line of the test string or -1 if test_string
// not present in command_line.
int arg_position( char** command_line, int command_line_size, char* test_string )
{
	for ( int i = 0; i < command_line_size; i++ )
	{
		if ( strcmp( command_line[i], test_string ) == 0 )
		{
			return i;
		}
	}
	return -1;
}

// Parse the command line
void parse_command_line( char** argv, int argc )
{
	char* syntax = "cempro -show [proteins|correlation|nothing] -epsilon_0 decimal_value -epsilon_1 decimal_value -epsilon_cross decimal_value -init_layout [random|separated] [-mix] [-density decimal_value] -end_time integer_value"; 
	
	int move_clusters = arg_position( argv, argc, "-move_clusters" );
	int show_pos = arg_position( argv, argc, "-show" );

	int epsilon_0_pos = arg_position( argv, argc, "-epsilon_0" );
	int epsilon_1_pos = arg_position( argv, argc, "-epsilon_1" );
	int epsilon_cross_pos = arg_position( argv, argc, "-epsilon_cross" );
	
	int init_layout_pos = arg_position( argv, argc, "-init_layout" );
	int mix_pos = arg_position( argv, argc, "-mix" );
	int density_pos = arg_position( argv, argc, "-density" );
	int end_time_pos = arg_position( argv, argc, "-end_time" );
	
	if ( density_pos != -1 )
	{
		density = atof( argv[density_pos + 1] );
	}

	if ( show_pos == -1 || epsilon_0_pos == -1 || epsilon_1_pos == -1 || epsilon_cross_pos == -1)
	{
		cout << "Recieved: "; 
		for ( int i = 0; i < argc; i++ )
		{
			cout << argv[i] << " ";
		}
		cout << endl;

		cout << "(1) Command line should be: " << syntax << endl;
		exit(1);
	}

	if ( move_clusters != -1 )
	{
		move_clusters = true;
	}

	// Get display choice
	if ( strcmp( argv[show_pos + 1], "proteins") == 0 )
	{
		display_proteins = true;
	}
	else if ( strcmp( argv[show_pos + 1], "correlation" ) == 0 )
	{
		display_correlation = true;
	}
	else if ( strcmp( argv[show_pos + 1], "nothing" ) == 0 )
	{
		no_display = true;
	}
	else
	{
		cout << "Recieved: "; 
		for ( int i = 0; i < argc; i++ )
		{
			cout << argv[i] << " ";
		}
		cout << endl;

		cout << "(2) Command line should be: ";
		cout << syntax << endl;
		exit(1);
	}

	if ( end_time_pos != -1 )
	{
		end_time = atoi( argv[end_time_pos + 1] );
	}
	else
	{
		cout << "Recieved: "; 
		for ( int i = 0; i < argc; i++ )
		{
			cout << argv[i] << " ";
		}
		cout << endl;

		cout << "(2) Command line should be: ";
		cout << syntax << endl;
		exit(1);
	}

	// Get epsilon values
	cross_epsilon = atof( argv[epsilon_cross_pos + 1] );
    epsilon_0 = atof( argv[epsilon_0_pos + 1] );
    epsilon_1 = atof( argv[epsilon_1_pos + 1] );

	// Mix the proteins?
	if ( mix_pos != -1 )
	{
		rectangular_mixing = true;
	}
}

char* intToCharStr( int number, int buff_size )
{
  ostringstream sin;
  sin << number;
  
  string str = sin.str();
  str.resize(buff_size); // Make sure we dont overflow the c_str buffer
  
  char* cstr = new char [buff_size];
  strcpy (cstr, str.c_str()); // Make a copy of the chars in the string object
  return cstr;
}
