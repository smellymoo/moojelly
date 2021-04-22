#pragma once

#include <wx/wx.h>
#include <wx/rawbmp.h>

class wxImagePanel : public wxPanel {   
    public:
    wxImage raw_image;
    bool ready = false;

    wxImagePanel(wxWindow* parent);
    void set_buffer(unsigned char* data, int width, int height);
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void render(wxDC& dc);
    wxSize DoGetBestClientSize() const; 

    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */

    DECLARE_EVENT_TABLE()
};