///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui.h"

///////////////////////////////////////////////////////////////////////////

main_form::main_form( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 800,600 ), wxDefaultSize );

	m_menubar1 = new wxMenuBar( 0 );
	menu_file = new wxMenu();
	wxMenuItem* menu_load;
	menu_load = new wxMenuItem( menu_file, wxID_ANY, wxString( wxT("&Load ROM") ) , wxEmptyString, wxITEM_NORMAL );
	menu_file->Append( menu_load );

	wxMenuItem* menu_save;
	menu_save = new wxMenuItem( menu_file, wxID_ANY, wxString( wxT("&Patch ROM") ) , wxEmptyString, wxITEM_NORMAL );
	menu_file->Append( menu_save );
	menu_save->Enable( false );

	menu_file->AppendSeparator();

	wxMenuItem* menu_import;
	menu_import = new wxMenuItem( menu_file, wxID_ANY, wxString( wxT("I&mport") ) , wxEmptyString, wxITEM_NORMAL );
	menu_file->Append( menu_import );
	menu_import->Enable( false );

	menu_export = new wxMenu();
	wxMenuItem* menu_exportItem = new wxMenuItem( menu_file, wxID_ANY, wxT("E&xport"), wxEmptyString, wxITEM_NORMAL, menu_export );
	wxMenuItem* menu_export_level;
	menu_export_level = new wxMenuItem( menu_export, wxID_ANY, wxString( wxT("Current level") ) , wxEmptyString, wxITEM_NORMAL );
	menu_export->Append( menu_export_level );
	menu_export_level->Enable( false );

	menu_file->Append( menu_exportItem );

	menu_dump = new wxMenu();
	wxMenuItem* menu_dumpItem = new wxMenuItem( menu_file, wxID_ANY, wxT("&Dump"), wxEmptyString, wxITEM_NORMAL, menu_dump );
	wxMenuItem* menu_dump_all;
	menu_dump_all = new wxMenuItem( menu_dump, wxID_ANY, wxString( wxT("Everything") ) , wxEmptyString, wxITEM_NORMAL );
	menu_dump->Append( menu_dump_all );

	menu_file->Append( menu_dumpItem );

	menu_file->AppendSeparator();

	wxMenuItem* menu_about;
	menu_about = new wxMenuItem( menu_file, wxID_ANY, wxString( wxT("A&bout") ) , wxEmptyString, wxITEM_NORMAL );
	menu_file->Append( menu_about );

	wxMenuItem* menu_exit;
	menu_exit = new wxMenuItem( menu_file, wxID_ANY, wxString( wxT("E&xit") ) , wxEmptyString, wxITEM_NORMAL );
	menu_file->Append( menu_exit );

	m_menubar1->Append( menu_file, wxT("&File") );

	menu_view = new wxMenu();
	menu_switch = new wxMenu();
	wxMenuItem* menu_switchItem = new wxMenuItem( menu_view, wxID_ANY, wxT("Switch"), wxEmptyString, wxITEM_NORMAL, menu_switch );
	wxMenuItem* menu_switch_none;
	menu_switch_none = new wxMenuItem( menu_switch, SWITCH_0, wxString( wxT("none") ) , wxEmptyString, wxITEM_RADIO );
	menu_switch->Append( menu_switch_none );
	menu_switch_none->Check( true );

	wxMenuItem* menu_switch_a;
	menu_switch_a = new wxMenuItem( menu_switch, SWITCH_1, wxString( wxT("A") ) , wxEmptyString, wxITEM_RADIO );
	menu_switch->Append( menu_switch_a );

	wxMenuItem* menu_switch_b;
	menu_switch_b = new wxMenuItem( menu_switch, SWITCH_2, wxString( wxT("B") ) , wxEmptyString, wxITEM_RADIO );
	menu_switch->Append( menu_switch_b );

	menu_view->Append( menu_switchItem );

	wxMenuItem* menu_sectors;
	menu_sectors = new wxMenuItem( menu_view, wxID_ANY, wxString( wxT("&Sectors") ) , wxEmptyString, wxITEM_CHECK );
	menu_view->Append( menu_sectors );
	menu_sectors->Check( true );

	wxMenuItem* menu_colorise;
	menu_colorise = new wxMenuItem( menu_view, wxID_ANY, wxString( wxT("&Colorise") ) , wxEmptyString, wxITEM_CHECK );
	menu_view->Append( menu_colorise );
	menu_colorise->Check( true );

	wxMenuItem* menu_sprites;
	menu_sprites = new wxMenuItem( menu_view, wxID_ANY, wxString( wxT("&Sprites") ) , wxEmptyString, wxITEM_CHECK );
	menu_view->Append( menu_sprites );
	menu_sprites->Check( true );

	m_menubar1->Append( menu_view, wxT("&View") );

	menu_tools = new wxMenu();
	wxMenuItem* menu_overworld;
	menu_overworld = new wxMenuItem( menu_tools, wxID_ANY, wxString( wxT("Over&world Editor") ) , wxEmptyString, wxITEM_NORMAL );
	menu_tools->Append( menu_overworld );
	menu_overworld->Enable( false );

	wxMenuItem* menu_warp;
	menu_warp = new wxMenuItem( menu_tools, wxID_ANY, wxString( wxT("&Warp Editor") ) , wxEmptyString, wxITEM_NORMAL );
	menu_tools->Append( menu_warp );
	menu_warp->Enable( false );

	wxMenuItem* menu_credits;
	menu_credits = new wxMenuItem( menu_tools, wxID_ANY, wxString( wxT("Over&world Editor") ) , wxEmptyString, wxITEM_NORMAL );
	menu_tools->Append( menu_credits );
	menu_credits->Enable( false );

	m_menubar1->Append( menu_tools, wxT("&Tools") );

	this->SetMenuBar( m_menubar1 );

	wxBoxSizer* form_sizer;
	form_sizer = new wxBoxSizer( wxVERTICAL );

	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	wxString combo1Choices[] = { wxT("levels") };
	int combo1NChoices = sizeof( combo1Choices ) / sizeof( wxString );
	combo1 = new wxChoice( m_toolBar1, wxID_ANY, wxDefaultPosition, wxDefaultSize, combo1NChoices, combo1Choices, 0 );
	combo1->SetSelection( 0 );
	m_toolBar1->AddControl( combo1 );
	m_toolBar1->Realize();

	form_sizer->Add( m_toolBar1, 0, wxTOP|wxEXPAND, 5 );

	main_scroll = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE|wxHSCROLL|wxVSCROLL );
	main_scroll->SetScrollRate( 16, 16 );
	main_scroll->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* scroll_sizer;
	scroll_sizer = new wxBoxSizer( wxVERTICAL );

	image_panel = new wxImagePanel(main_scroll);
	image_panel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	scroll_sizer->Add( image_panel, 1, wxALL|wxEXPAND, 0 );


	main_scroll->SetSizer( scroll_sizer );
	main_scroll->Layout();
	scroll_sizer->Fit( main_scroll );
	form_sizer->Add( main_scroll, 1, wxEXPAND | wxALL, 6 );


	this->SetSizer( form_sizer );
	this->Layout();
	form_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	menu_file->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::menu_load_rom ), this, menu_load->GetId());
	menu_dump->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::menu_dump_all ), this, menu_dump_all->GetId());
	menu_file->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::menu_about ), this, menu_about->GetId());
	menu_file->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::menu_exit ), this, menu_exit->GetId());
	menu_switch->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::switch_select ), this, menu_switch_none->GetId());
	menu_switch->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::switch_select ), this, menu_switch_a->GetId());
	menu_switch->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( main_form::switch_select ), this, menu_switch_b->GetId());
	combo1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( main_form::combo_level ), NULL, this );
	image_panel->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( main_form::canvas_click ), NULL, this );
}

main_form::~main_form()
{
	// Disconnect Events
	combo1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( main_form::combo_level ), NULL, this );
	image_panel->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( main_form::canvas_click ), NULL, this );

}
