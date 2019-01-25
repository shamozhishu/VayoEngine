#include "stdafx.h"
#include "MsgVisitor.h"
#include "TessgridApp.h"

void CMsgVisitor::OnResetCamera()
{
	OrbitCamera* pCam = TessgridApp::getSingleton().getOrbitCamera();
	if (pCam)
	{
		pCam->lookAt(Vector3df(0, 0, 100), Vector3df::Origin, Vector3df::YAxis);
		pCam->resetArcball();
	}
}

void CMsgVisitor::OnChangeBgColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF clr = dlg.GetColor();
		Root::getSingleton().getMainDevice()->setBgClearColor(Colour(255, GetRValue(clr), GetGValue(clr), GetBValue(clr))._clr);
	}
}
