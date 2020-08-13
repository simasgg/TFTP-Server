#pragma warning(disable:4819)
#include "cMain.h"

using namespace std;

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnButtonClicked)
	EVT_PAINT(cMain::render)
	EVT_BUTTON(10002, exitProgram)
	EVT_DIRPICKER_CHANGED(10007, chooseFolder)
wxEND_EVENT_TABLE()


cMain::cMain() : wxFrame(nullptr, wxID_ANY, "TFTPServer", wxPoint(500, 300), wxSize(850, 600))
{
	SetBackgroundColour(*wxLIGHT_GREY);
	wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);
	wxFont font2(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
	wxFont font3(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_LIGHT, false);
	wxFont font4(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_BOLD, false, wxEmptyString, wxFONTENCODING_DEFAULT);

	//start server
	m_btn1 = new wxButton(this, 10001, "Start server", wxPoint(340, 100), wxSize(80, 60));

	//ipaddress
	m_stat2 = new wxStaticText(this, wxID_ANY, "IP Address", wxPoint(50, 100));
	m_txt2 = new wxTextCtrl(this, wxID_ANY, "127.0.0.1", wxPoint(140, 100), wxSize(100, 20));
	m_stat2->SetFont(font);
	m_txt2->SetBackgroundColour(*wxLIGHT_GREY);
	m_txt2->SetStyle(0, -1, m_txt2->GetDefaultStyle());

	//port
	m_stat4 = new wxStaticText(this, wxID_ANY, "Port: ", wxPoint(50, 130));
	m_txt3 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(140, 130), wxSize(50, 20));
	m_stat4->SetFont(font);
	m_txt3->SetBackgroundColour(*wxLIGHT_GREY);
	m_txt3->SetStyle(0, -1, m_txt3->GetDefaultStyle());

	//choose current folder
	wxDirPickerCtrl* dirPickerCtrl = new wxDirPickerCtrl(this, 10007,
		"Default Folder", wxDirSelectorPromptStr,
		wxPoint(30, 500), wxSize(90, 30), wxDIRP_CHANGE_DIR);
	m_stat5 = new wxStaticText(this, wxID_ANY, "Default folder", wxPoint(30, 530), wxSize(90, 27));

	//quit
	m_quit = new wxButton(this, 10002, "Quit", wxPoint(700, 500), wxSize(90, 27));

	//screen + info
	m_stat2 = new wxStaticText(this, wxID_ANY, "TFTP Server", wxPoint(500, 100));
	m_stat2->SetFont(font4);
	m_stat1 = new wxStaticText(this, wxID_ANY, "Steps:\n"
		">Enter server IP address\n"
		">Enter port number (optional)\n"
		">Choose Server location (optional)\n"
		">Press Start Server\n", wxPoint(510, 200));
	m_stat1->SetFont(font3);

	//Main TFTP code
	myServer = new server();
}

cMain::~cMain()
{
	delete myServer;
}

void cMain::exitProgram(wxCommandEvent& evt) {
	Destroy();
}

void cMain::chooseFolder(wxFileDirPickerEvent& evt)
{
	flag = true;
	if (m_stat5)
	{
		m_stat5->SetLabel(evt.GetPath());
		wxString value = evt.GetPath();
		filename = string(value.mb_str(wxConvUTF8));
	}
}

void cMain::render(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
	//ipadd and port
	dc.DrawRoundedRectangle(30, 80, 250, 100, 10);
	dc.SetBackgroundMode(wxSOLID);
	dc.DrawText(wxT("Server"), 130, 70);

}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	//ipaddress
	wxString value = m_txt2->GetValue();
	string str = string(value.mb_str(wxConvUTF8));
	
	//port
	wxString valueport = m_txt3->GetValue();
	string myport = string(valueport.mb_str(wxConvUTF8));
	m_txt2->Enable(false);
	m_txt3->Enable(false);

	//create list box
	wxFont font3(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);
	m_list1 = new wxListBox(this, wxID_ANY, wxPoint(30, 230), wxSize(390, 270));
	m_list1->SetBackgroundColour(*wxLIGHT_GREY);

	//Main program
	myServer->Init(m_list1);
	myServer->setFilename(filename);
	myServer->start(str, myport, m_btn1);
	myServer->EndProgram();
	evt.Skip();
}


void cMain::printList(string str) {
	m_list1->AppendString(str);
	m_list1->SetSelection(m_list1->GetCount() - 1);
	wxYield();
}
