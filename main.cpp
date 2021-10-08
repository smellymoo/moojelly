//#define __NDEBUG__

#include <wx/config.h>
#include "main.h"
#include "sml.h"
#include "courses.h"
//#include "emu.h"

IMPLEMENT_APP(GUI)

//using namespace std;

Bitmap level_bitmap(256*16, 48*16, false);
wxConfig *config;

bool GUI::OnInit() {
	wxPNGHandler *handler = new wxPNGHandler;
	wxImage::AddHandler(handler);

	config = new wxConfig("Moojelly");
	FORM *frame = new FORM((wxFrame*) NULL, -1, _T("Moojelly"));
	frame->Show(true);

	wxString str;
	if (config->Read("last_ROM", &str)) frame->load_rom(str);

	return true;
}


void FORM::combo_level( wxCommandEvent& event ) {
	ROM::set_level(combo1->GetSelection());
	ROM::render_level(level_bitmap);
	image_panel->paintNow();

	event.Skip();
}


void FORM::menu_load_rom( wxCommandEvent& event ) {
	wxFileDialog openFileDialog(this, _("Open XYZ file"), "", "", "Gameboy ROM (*.gb)|*.gb", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;

	FORM::load_rom(openFileDialog.GetPath());
	config->Write("last_ROM", openFileDialog.GetPath());
	config->Flush();
	event.Skip();
}


void FORM::switch_select( wxCommandEvent& event ) {
	switch(event.GetId()) {
		case SWITCH_1: ROM::switch_state = 1; break;
		case SWITCH_2: ROM::switch_state = 2; break;
		default: ROM::switch_state = 0;
	}

	ROM::render_level(level_bitmap);
	image_panel->paintNow();
}


void FORM::menu_dump_all( wxCommandEvent& event ) {
	for (int l = 0; l < LEVELS; ++l) {
		ROM::set_level(l);
		ROM::dump_map();
		ROM::dump_blocks();
		ROM::dump_level();
	}

	std::cout << "done.\n";
	event.Skip();

	//ROM::emu_test();
	//ROM::test_sprite_lookup();
}


void FORM::menu_about( wxCommandEvent& event ) {
	wxLaunchDefaultBrowser("http://www.smellymoo.com");
	event.Skip();
}


void FORM::menu_exit( wxCommandEvent& event ) {
	exit(1);
	event.Skip();
}


void FORM::load_rom(wxString filename) {
	if (ROM::load_rom(filename.ToStdString()) ) {
		wxArrayString list(LEVELS, LEVEL_NAMES);
		combo1->Clear();
		combo1->Append(list);
		combo1->SetSelection(0);
		combo1->SendSelectionChangedEvent(wxEVT_CHOICE);

		image_panel->set_buffer((byte*) level_bitmap.RGB, level_bitmap.width, level_bitmap.height);
	}
}

void FORM::canvas_click( wxMouseEvent& event ) {
	//ROM::test_fill(event.m_x/16, event.m_y/16);
}
