#include "image.h"

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)

    /*
     EVT_MOTION(wxImagePanel::mouseMoved)
     EVT_LEFT_DOWN(wxImagePanel::mouseDown)
     EVT_LEFT_UP(wxImagePanel::mouseReleased)
     EVT_RIGHT_DOWN(wxImagePanel::rightClick)
     EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
     EVT_KEY_DOWN(wxImagePanel::keyPressed)
     EVT_KEY_UP(wxImagePanel::keyReleased)
     EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
     */

    EVT_PAINT(wxImagePanel::paintEvent)

END_EVENT_TABLE()


wxImagePanel::wxImagePanel(wxWindow* parent) : wxPanel(parent) {}


void wxImagePanel::set_buffer(unsigned char* data, int width, int height) {
	raw_image = wxImage(width, height, data, true);
	ready = true;
	
	this->GetParent()->SetVirtualSize(raw_image.GetSize());
	InvalidateBestSize();
	paintNow();
}


wxSize wxImagePanel::DoGetBestClientSize() const {
	if (ready) return raw_image.GetSize();
	else return wxSize(0,0);
}


void wxImagePanel::paintEvent(wxPaintEvent & evt) { wxPaintDC dc(this); render(dc); }
void wxImagePanel::paintNow() { wxClientDC dc(this); render(dc); }

void wxImagePanel::render(wxDC&  dc) {	
	if (ready) {
		wxBitmap bitmap(raw_image);
		dc.DrawBitmap(bitmap, 0, 0, false);
	} 
	else dc.Clear();
}

