#pragma once
#pragma warning(disable:4819)
#include "wx/wx.h"
#include "cMain.h"


class cApp : public wxApp
{
public:
	cApp();
	~cApp();
private:
	cMain* m_frame1 = nullptr;
public:
	virtual bool OnInit();
};

