// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "lab.h"
#include "ChildView.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double GR_PI = 3.1415926535897932384626433832795;

const GLdouble RED[3] = {0.8, 0.0, 0.0};
const GLdouble BLU[3] = {0.0, 0.0, 0.8};

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
    m_camera.Set(20, 10, 50, 0, 0, 0, 0, 1, 0);

    m_spinangle = 0.;
    m_spintimer = 0;

    m_mode = ID_LABSTUFF_BOX;

    SetDoubleBuffer(true);

    m_wood.LoadFile("plank01.bmp");
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,COpenGLWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_COMMAND(ID_FILE_SAVEBMPFILE, OnFileSavebmpfile)
	ON_COMMAND(ID_LABSTUFF_SPIN, OnLabstuffSpin)
	ON_UPDATE_COMMAND_UI(ID_LABSTUFF_SPIN, OnUpdateLabstuffSpin)
	ON_WM_TIMER()
	ON_COMMAND(ID_LABSTUFF_BOX, OnLabstuffBox)
	ON_UPDATE_COMMAND_UI(ID_LABSTUFF_BOX, OnUpdateLabstuffBox)
	ON_COMMAND(ID_LABSTUFF_SPHERE, OnLabstuffSphere)
	ON_UPDATE_COMMAND_UI(ID_LABSTUFF_SPHERE, OnUpdateLabstuffSphere)
	ON_COMMAND(ID_LABSTUFF_TORUS, OnLabstuffTorus)
	ON_UPDATE_COMMAND_UI(ID_LABSTUFF_TORUS, OnUpdateLabstuffTorus)
	//}}AFX_MSG_MAP
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!COpenGLWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}




void CChildView::OnGLDraw(CDC *pDC)
{
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
   // Set up the camera
   //

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // Determine the screen size so we can determine the aspect ratio
   int width, height;
   GetSize(width, height);
   GLdouble aspectratio = GLdouble(width) / GLdouble(height);

   // Set the camera parameters
   gluPerspective(15.,           // Vertical field of view in degrees.
                  aspectratio,   // The aspect ratio.
                  10.,           // Near clipping
                  200.);         // Far clipping

   // Set the camera location
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   m_camera.gluLookAt();

   //
   // Some standard parameters
   //

   // Enable depth test
   glEnable(GL_DEPTH_TEST);

   // Cull backfacing polygons
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);

   // Enable lighting
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   GLfloat lightpos[] = {.5, 1, 1, 0.};
   glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

   glPushMatrix();

   // Note that I'm texture mapping most of the following
   // objects.  So, I'm setting the color to white.  The
   // actual color will be the object color multiplied 
   // by the texture color.  If I set a color it, it will
   // tint the texture.  Maybe that's what you'll want 
   // sometimes?

   GLfloat white[] = {1.f, 1.f, 1.f, 1.0f};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
   glMaterialfv(GL_FRONT, GL_SPECULAR, white);
   GLfloat shininess[] = {100};
   glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

   //
   // Switch for the three lab examples
   //

   switch(m_mode)
   {
   case ID_LABSTUFF_BOX:
      glPushMatrix();
      glRotated(m_spinangle / 2, 0, 1, .1);
      glTranslated(-2.5, -2.5, -2.5);
      Box(5, 5, 5);
      glPopMatrix();
      break;

   case ID_LABSTUFF_SPHERE:
      glRotated(m_spinangle / 3, 0, 1, 0);
      Sphere(5.);
      break;

   case ID_LABSTUFF_TORUS:
      // This rotation spins the tori
      glRotated(m_spinangle / 3, 0, 1, 0);

      glPushMatrix();

      // The flips them down 45 degress so they look nicer.
      glRotated(45., 1, 0, 0);

      // First torus is left 2.5 inches
      glPushMatrix();
      glTranslated(-2.5, 0, 0);

      Torus(5, 1, 50, 20);

      glPopMatrix();

      // Second torus is right 2.5 inches and rotated
      // around the X axis to right angles with the first
      // torus.
      glPushMatrix();

      glTranslated(2.5, 0, 0);
      glRotated(90., 1, 0, 0);

      Torus(5, 1, 50, 20);

      glPopMatrix();

      glPopMatrix();
      break;
   }

   glPopMatrix();

   glFlush();
}

double Normal3dv(double *v)
{
   return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void CChildView::OnFileSavebmpfile() 
{
   OnSaveImage();	
}


void CChildView::OnLabstuffSpin() 
{
   if(m_spintimer)
   {
      KillTimer(m_spintimer);
      m_spintimer = 0;
   }
   else
   {
      m_spintimer = SetTimer(1, 50, NULL);
   }
}

void CChildView::OnUpdateLabstuffSpin(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_spintimer != 0);	
}

void CChildView::OnTimer(UINT nIDEvent) 
{
	m_spinangle += 10;
   Invalidate();

	COpenGLWnd ::OnTimer(nIDEvent);
}

//
//        Name : CChildView::Box()
// Description : Draw an arbitrary size box. p_x, p_y, and 
//               p_z are the height of the box. We'll use this is a 
//               common primitive.
//      Origin : The back corner is at 0, 0, 0, and the box 
//               is entirely in the positive octant.
//

void CChildView::Box(GLdouble p_x, GLdouble p_y, GLdouble p_z)
{
   GLdouble a[] = {0., 0., p_z};
   GLdouble b[] = {p_x, 0., p_z};
   GLdouble c[] = {p_x, p_y, p_z};
   GLdouble d[] = {0., p_y, p_z};
   GLdouble e[] = {0., 0., 0.};
   GLdouble f[] = {p_x, 0., 0.};
   GLdouble g[] = {p_x, p_y, 0.};
   GLdouble h[] = {0., p_y, 0.};

   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D, m_wood.TexName());

   // Front
   glBegin(GL_QUADS);
      glNormal3d(0, 0, 1);
      glTexCoord2f(0, 0);
      glVertex3dv(a);
      glTexCoord2f(1, 0);
      glVertex3dv(b);
      glTexCoord2f(1, 1);
      glVertex3dv(c);
      glTexCoord2f(0, 1);
      glVertex3dv(d);
   glEnd();

   glDisable(GL_TEXTURE_2D);

   // Right
   glBegin(GL_QUADS);
      glNormal3d(1, 0, 0);
      glVertex3dv(c);
      glVertex3dv(b);
      glVertex3dv(f);
      glVertex3dv(g);
   glEnd();

   // Back
   glBegin(GL_QUADS);
      glNormal3d(0, 0, -1);
      glVertex3dv(h);
      glVertex3dv(g);
      glVertex3dv(f);
      glVertex3dv(e);
   glEnd();

   // Left
   glBegin(GL_QUADS);
      glNormal3d(-1, 0, 0);
      glVertex3dv(d);
      glVertex3dv(h);
      glVertex3dv(e);
      glVertex3dv(a);
   glEnd();

   // Top
   glBegin(GL_QUADS);
      glNormal3d(0, 1, 0);
      glVertex3dv(d);
      glVertex3dv(c);
      glVertex3dv(g);
      glVertex3dv(h);
   glEnd();

   // Bottom
   glBegin(GL_QUADS);
      glNormal3d(0, -1, 0);
      glVertex3dv(e);
      glVertex3dv(f);
      glVertex3dv(b);
      glVertex3dv(a);
   glEnd();

}



void CChildView::Sphere(double p_radius)
{
   GLdouble a[] = {1, 0, 0};
   GLdouble b[] = {0, 0, -1};
   GLdouble c[] = {-1, 0, 0};
   GLdouble d[] = {0, 0, 1};
   GLdouble e[] = {0, 1, 0};
   GLdouble f[] = {0, -1, 0};

   int recurse = 5;

   SphereFace(recurse, p_radius, d, a, e);
   SphereFace(recurse, p_radius, a, b, e);
   SphereFace(recurse, p_radius, b, c, e);
   SphereFace(recurse, p_radius, c, d, e);
   SphereFace(recurse, p_radius, a, d, f);
   SphereFace(recurse, p_radius, b, a, f);
   SphereFace(recurse, p_radius, c, b, f);
   SphereFace(recurse, p_radius, d, c, f);
}

inline void Normalize(GLdouble *v)
{
   GLdouble len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
   v[0] /= len;
   v[1] /= len;
   v[2] /= len;
}

//
// Name :         CChildView::SphereFace()
// Description :  Draw a single facet of the sphere.  If p_recurse > 1,
//                triangulate that facet and recurse.
//

void CChildView::SphereFace(int p_recurse, double p_radius, GLdouble *a, GLdouble *b, GLdouble *c)
{
   if(p_recurse > 1)
   {
      // Compute vectors halfway between the passed vectors
      GLdouble d[3] = {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
      GLdouble e[3] = {b[0] + c[0], b[1] + c[1], b[2] + c[2]};
      GLdouble f[3] = {c[0] + a[0], c[1] + a[1], c[2] + a[2]};

      Normalize(d);
      Normalize(e);
      Normalize(f);

      SphereFace(p_recurse-1, p_radius, a, d, f);
      SphereFace(p_recurse-1, p_radius, d, b, e);
      SphereFace(p_recurse-1, p_radius, f, e, c);
      SphereFace(p_recurse-1, p_radius, f, d, e);
   }

   glBegin(GL_TRIANGLES);
      glNormal3dv(a);
      glVertex3d(a[0] * p_radius, a[1] * p_radius, a[2] * p_radius);

      glNormal3dv(b);
      glVertex3d(b[0] * p_radius, b[1] * p_radius, b[2] * p_radius);

      glNormal3dv(c);
      glVertex3d(c[0] * p_radius, c[1] * p_radius, c[2] * p_radius);
   glEnd();
}




//
// Name :         CChildView::Torus(double r)
// Description :  Render a Torus of large radius p_r1 and small radius p_r2
//                p_steps1 is the number of steps we do for the large
//                radius and p_steps2 is the number of steps for the
//                small radius.  There will be p_steps1 * p_steps2 
//                quadrilaterals in the torus.
//

void CChildView::Torus(double p_r1, double p_r2, int p_steps1, int p_steps2)
{
   // How large are the angular steps in radians
   const double step1r = 2. * GR_PI / p_steps1;
   const double step2r = 2. * GR_PI / p_steps2;

   // We build the torus in slices that go from a1a to a1b
   double a1a = 0;
   double a1b = step1r;

   for(int s=0;  s<p_steps1;  s++, a1a = a1b, a1b += step1r)
   {
      // We build a slice out of quadrilaterals that range from 
      // angles a2a to a2b.

      double a2a = 0;
      double a2b = step2r;

      for(int s2=0;  s2<p_steps2;  s2++, a2a = a2b,  a2b += step2r)
      {
         // We need to know the corners
         double n[3], v[3];

         glBegin(GL_QUADS);
            TorusVertex(a1a, p_r1, a2a, p_r2, v, n);
            glNormal3dv(n);

            glVertex3dv(v);

            TorusVertex(a1b, p_r1, a2a, p_r2, v, n);
            glNormal3dv(n);

            glVertex3dv(v);

            TorusVertex(a1b, p_r1, a2b, p_r2, v, n);
            glNormal3dv(n);

            glVertex3dv(v);

            TorusVertex(a1a, p_r1, a2b, p_r2, v, n);
            glNormal3dv(n);

            glVertex3dv(v);

         glEnd();
      }
   }
}

//
// Name :         CChildView::TorusVertex()
// Description :  Compute the x,y,z coordinates and surface normal for a 
//                torus vertex.  
// Parameters :   a1 - The angle relative to the center of the torus
//                r1 - Radius of the entire torus
//                a2 - The angle relative to the center of the torus slice
//                r2 - Radius of the torus ring cross-section
//                v - Returns vertex
//                n - Returns surface normal
//

void CChildView::TorusVertex(double a1, double r1, double a2, double r2, 
                             double *v, double *n)
{
   // Some sines and cosines we'll need.
   double ca1 = cos(a1);
   double sa1 = sin(a1);
   double ca2 = cos(a2);
   double sa2 = sin(a2);

   // What is the center of the slice we are on.
   double centerx = r1 * ca1;
   double centerz = -r1 * sa1;    // Note, y is zero

   // Compute the surface normal
   n[0] = ca2 * ca1;          // x
   n[1] = sa2;                // y
   n[2] = -ca2 * sa1;         // z

   // And the vertex
   v[0] = centerx + r2 * n[0];
   v[1] = r2 * n[1];
   v[2] = centerz + r2 * n[2];
}

void CChildView::OnLabstuffBox() 
{
   m_mode = ID_LABSTUFF_BOX;
   Invalidate();
}

void CChildView::OnUpdateLabstuffBox(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_mode == ID_LABSTUFF_BOX);
}

void CChildView::OnLabstuffSphere() 
{
   m_mode = ID_LABSTUFF_SPHERE;
   Invalidate();
}

void CChildView::OnUpdateLabstuffSphere(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_mode == ID_LABSTUFF_SPHERE);
}


void CChildView::OnLabstuffTorus() 
{
   m_mode = ID_LABSTUFF_TORUS;
   Invalidate();
}

void CChildView::OnUpdateLabstuffTorus(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_mode == ID_LABSTUFF_TORUS);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_camera.MouseDown(point.x, point.y);

    COpenGLWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    if(nFlags & MK_LBUTTON)
    {
        m_camera.MouseMove(point.x, point.y);
        Invalidate();
    }

    COpenGLWnd::OnMouseMove(nFlags, point);
}
