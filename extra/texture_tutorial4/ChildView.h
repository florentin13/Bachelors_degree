// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__50C8628F_112A_40D0_9AB2_53368988C69B__INCLUDED_)
#define AFX_CHILDVIEW_H__50C8628F_112A_40D0_9AB2_53368988C69B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "graphics/OpenGLWnd.h"
#include "graphics/Texture.h"	// Added by ClassView
#include "graphics/GrCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public COpenGLWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnGLDraw(CDC *pDC);
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnFileSavebmpfile();
	afx_msg void OnLabstuffSpin();
	afx_msg void OnUpdateLabstuffSpin(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLabstuffBox();
	afx_msg void OnUpdateLabstuffBox(CCmdUI* pCmdUI);
	afx_msg void OnLabstuffSphere();
	afx_msg void OnUpdateLabstuffSphere(CCmdUI* pCmdUI);
	afx_msg void OnLabstuffTorus();
	afx_msg void OnUpdateLabstuffTorus(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TorusVertex(double a1, double r1, double a2, double r2,  double *v, double *n);
	void Torus(double p_r1, double p_r2, int p_steps1, int p_steps2);
	CTexture m_wood;
	void OnFirstDraw();
	int m_mode;
	void SphereFace(int p_recurse, double p_radius, GLdouble *a, GLdouble *b, GLdouble *c);
	void Sphere(double p_radius);
	UINT m_spintimer;
	double m_spinangle;

    void Box(GLdouble p_x, GLdouble p_y, GLdouble p_z);

    CGrCamera       m_camera;
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__50C8628F_112A_40D0_9AB2_53368988C69B__INCLUDED_)
