///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/toolbar.h>
#include "image.h"
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define MAIN_FORM 1000
#define SWITCH_0 1001
#define SWITCH_1 1002
#define SWITCH_2 1003
#define IMAGE_PANEL 1004

///////////////////////////////////////////////////////////////////////////////
/// Class main_form
///////////////////////////////////////////////////////////////////////////////
class main_form : public wxFrame
{
	private:

	protected:
		wxMenuBar* m_menubar1;
		wxMenu* menu_file;
		wxMenu* menu_export;
		wxMenu* menu_dump;
		wxMenu* menu_view;
		wxMenu* menu_switch;
		wxMenu* menu_tools;
		wxToolBar* m_toolBar1;

		// Virtual event handlers, overide them in your derived class
		virtual void menu_load_rom( wxCommandEvent& event ) { event.Skip(); }
		virtual void menu_dump_all( wxCommandEvent& event ) { event.Skip(); }
		virtual void menu_about( wxCommandEvent& event ) { event.Skip(); }
		virtual void menu_exit( wxCommandEvent& event ) { event.Skip(); }
		virtual void switch_select( wxCommandEvent& event ) { event.Skip(); }
		virtual void combo_level( wxCommandEvent& event ) { event.Skip(); }
		virtual void canvas_click( wxMouseEvent& event ) { event.Skip(); }


	public:
		wxChoice* combo1;
		wxScrolledWindow* main_scroll;
		wxImagePanel* image_panel;

		main_form( wxWindow* parent, wxWindowID id = MAIN_FORM, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~main_form();

};

