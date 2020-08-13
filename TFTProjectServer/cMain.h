#pragma once
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#include "server.h"
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <thread>
#include <ctime>
#include "wx/wx.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include <wx/wfstream.h>
#include <wx/filepicker.h>


class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	wxButton *m_btn1 = nullptr;
	wxButton* m_btn2 = nullptr;
	wxButton* m_quit = nullptr;
	wxTextCtrl *m_txt1 = nullptr;
	wxTextCtrl *m_txt2 = nullptr;
	wxTextCtrl* m_txt3 = nullptr;
	wxStaticText *m_stat1 = nullptr;
	wxStaticText* m_stat2 = nullptr;
	wxStaticText* m_stat4 = nullptr;
	wxStaticText* m_stat5 = nullptr;
	wxListBox *m_list1 = nullptr;
	wxStaticText* m_stat3 = nullptr;
	std::string filename;
	bool flag = false;
	server* myServer;


	void render(wxPaintEvent& evt);
	void OnButtonClicked(wxCommandEvent &evt);
	void exitProgram(wxCommandEvent& evt);
	void chooseFolder(wxFileDirPickerEvent& evt);
	void printList(std::string str);

	wxDECLARE_EVENT_TABLE();
};

