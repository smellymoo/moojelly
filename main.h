#pragma once

#include "wx/wxprec.h"
#include "gui.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class GUI : public wxApp{
    public:
    virtual bool OnInit();
};

DECLARE_APP(GUI)

class FORM : public main_form{
    public:
    using main_form::main_form;
    void menu_load_rom( wxCommandEvent& event ) override;
    void menu_dump_all( wxCommandEvent& event ) override;
    void menu_about( wxCommandEvent& event ) override;
    void menu_exit( wxCommandEvent& event ) override;
    void switch_select( wxCommandEvent& event ) override;
    void combo_level( wxCommandEvent& event ) override;
    void canvas_click( wxMouseEvent& event ) override;
    
    void load_rom(wxString filename);
};