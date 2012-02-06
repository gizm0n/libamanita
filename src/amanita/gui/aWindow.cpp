
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/aApplication.h>
#include <amanita/gui/aWindow.h>
#include <amanita/gui/aMenu.h>
#include <amanita/gui/aStatusbar.h>




#ifdef __linux__
static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	aWindow *wnd = (aWindow *)data;
	return !wnd->close();
}
#endif



#ifdef WIN32
LRESULT CALLBACK AmanitaMainWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	aWindow *wnd = 0;
	if(msg==WM_CREATE) {
debug_output("AmanitaMainWndProc(WM_CREATE)\n");
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		wnd = (aWindow *)cs->lpCreateParams;
		wnd->container = (aComponent)hwnd;
		SetProp(hwnd,"aWindow",(HANDLE)wnd);
		if(wnd->create()) {
			wnd->show();
			return 0;
		}
	} else {
		wnd = (aWindow *)GetProp(hwnd,"aWindow");
		if(wnd) {
			switch(msg) {
				case WM_DESTROY:
					RemoveProp(hwnd,"aWindow");
					if(!wnd->close()) return 0;
					break;
				case WM_SIZE:
					if(wparam!=SIZE_MINIMIZED)
						wnd->makeLayout(LOWORD(lparam),HIWORD(lparam));
						//MoveWindow((HWND)app->child->getContainer(),0,0,LOWORD(lparam),HIWORD(lparam),true);
					return 0;
//				default:if(wnd->event_handler && wnd->event_handler(wnd,msg,wparam,lparam,0)) return 0;
			}
		}
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
/*
BOOL CALLBACK AmanitaMainDlgProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg==WM_INITDIALOG) {
		aWindow *wnd = (aWindow *)lparam;
		SetProp(hwnd,"aWindow",(HANDLE)wnd);
		wnd->create();
fprintf(stderr,"AmanitaMainDlgProc(msg=%d)\n",msg);
		return true;
	}
	//return Window::handleMessage(hwnd,msg,wParam,lParam);
	return false;
}*/
#endif



aWindow::aWindow(aApplication *a,widget_event_handler weh) : aWidget(weh,WIDGET_WINDOW) {
	app = a;
	menu = 0;
	statusbar = 0;
}

aWindow::~aWindow() {
	if(window) delete window;
	window = 0;
	if(menu) delete menu;
	menu = 0;
	if(statusbar) delete statusbar;
	statusbar = 0;
}


void aWindow::open() {
	const char *nm = text;
	if(!nm) nm = app->getApplicationName();
#ifdef __linux__
	GtkWidget *wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wnd),nm);
	gtk_window_set_default_size(GTK_WINDOW(wnd),width,height);
	gtk_widget_set_size_request(wnd,min_width,min_height);
	gtk_window_set_resizable(GTK_WINDOW(wnd),(format&WINDOW_RESIZABLE)? true : false);
	if((format&WINDOW_CENTER))
		gtk_window_set_position(GTK_WINDOW(wnd),GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(wnd),"delete_event",G_CALLBACK(delete_event_callback),this);
	container = (aComponent)wnd;
	create();
	show();
#endif
#ifdef WIN32
	HWND hwnd;
	MSG msg;
	DWORD ex;
	LPCTSTR cl;
//	if((format&WINDOW_DIALOG)) {
//		dlg_result = DialogBoxParam(hinst,MAKEINTRESOURCE(instance()),hparent,AmanitaMainDlgProc,(LPARAM)this);
//	} else {
		ex = WS_EX_CLIENTEDGE;
		cl = AMANITA_MAIN_WINDOW_CLASS;
		if(!(class_registers&WINDOW_CLASS_WINDOW)) {
			WNDCLASSEX wndclassx = {
				sizeof(WNDCLASSEX),
				CS_HREDRAW|CS_VREDRAW,
				AmanitaMainWndProc,
				0,0,hinst,
				LoadIcon(0,IDI_APPLICATION),
				LoadCursor(0,IDC_ARROW),
				(HBRUSH)(COLOR_WINDOW+1),
				0,cl,LoadIcon(0,IDI_APPLICATION)
			};
			RegisterClassEx(&wndclassx);
			class_registers |= WINDOW_CLASS_WINDOW;
		}
		hwnd = CreateWindowEx(ex,cl,nm,
			WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,width,height,0,0,hinst,this);
//	}

	if(!hwnd) MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
#endif
}

bool aWindow::close() {
#ifdef WIN32
	if((format&WINDOW_DIALOG)) {
//		EndDialog((HWND)container,dlg_result);
	} else {
		
	}
#endif
	if(parent) ((aWindow *)parent)->closeWindow(this);
	else app->destroy();
	return true;
}

void aWindow::setMenu(aMenu *m) {
	menu = m,menu->parent = this;
}

void aWindow::setStatusbar(aStatusbar *sb) {
	statusbar = sb,statusbar->parent = this;
}

aComponent aWindow::create() {
	int lvl = 1;
	aComponent p,h;
	aWidget *w = child;

	component = container;
#ifdef __linux__
	if(menu || statusbar) {
		component = gtk_vbox_new(FALSE,0);
		if(menu) {
			menu->create();
			gtk_box_pack_start(GTK_BOX(component),(GtkWidget *)menu->getContainer(),FALSE,FALSE,0);
		}
		if(statusbar) {
			statusbar->create();
			gtk_box_pack_end(GTK_BOX(component),(GtkWidget *)statusbar->getContainer(),FALSE,FALSE,0);
		}
		gtk_container_add(GTK_CONTAINER(container),component);
	}
#endif

	while(w && w!=this) {
		h = w->create();
debug_output("aWindow::create(w: %p, h: %p, lvl: %d)\n",w,h,lvl);
#ifdef __linux__
		p = w->getParent()->getContainer();
		gtk_container_add(GTK_CONTAINER(p),(GtkWidget *)h);
#endif

		w = w->iterate(lvl);
	}
	return container;
}

#ifdef WIN32
void aWindow::makeLayout(int w,int h) {
	MoveWindow((HWND)child->getContainer(),0,0,w,h,true);
}
#endif




