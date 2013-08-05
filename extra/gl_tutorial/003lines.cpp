
#include <GL/gl.h>
#include <GL/glut.h>

#define X 100 
#define Z 100

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{

   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (0.0, 1.0, 1.0);

	static GLfloat vdata[4][3] = {    
   	{100, 300, 0}, {300, 300, 0}, {100, 100, 100}, {300, 0.0, 100},    
	};

	int i;



	glBegin(GL_TRIANGLES);    
	   	glVertex3fv(&vdata[0][0]); 
	   	glVertex3fv(&vdata[1][0]); 
	   	glVertex3fv(&vdata[2][0]); 
	glEnd();

   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (1000, 750); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutMainLoop();
   return 0;
}



