
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h>
#endif
#include <amanita/aApplication.h>
#include <amanita/gui/aWindow.h>
#include <amanita/gui/aMenu.h>
#include <amanita/gui/aStatus.h>
#include <amanita/gui/aNotebook.h>
#include <amanita/gui/aButton.h>
#include <amanita/gui/aChoice.h>
#include <amanita/gui/aList.h>




#ifdef USE_GTK
static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	aWindow *wnd = (aWindow *)data;
	return !wnd->close();
}
#endif



#ifdef USE_WIN32
LRESULT CALLBACK AmanitaMainWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	aWindow *wnd = 0;
	if(msg==WM_CREATE) {
//debug_output("AmanitaMainWndProc(WM_CREATE)\n");
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		wnd = (aWindow *)cs->lpCreateParams;
		wnd->component = (aComponent)hwnd;
		SetWindowLongPtr(hwnd,GWL_USERDATA,(LONG_PTR)wnd);
		wnd->create(wnd,0);
		wnd->createAll(0,true);
		wnd->show();
		return 0;
	} else {
		wnd = (aWindow *)GetWindowLongPtr(hwnd,GWL_USERDATA);
		if(wnd) switch(msg) {
			case WM_COMMAND:
				if(aWindow::handleEvent(wnd,msg,wparam,lparam)) return 0;
				break;
			case WM_NOTIFY:
				if(aWindow::handleEvent(wnd,msg,wparam,lparam)) return 0;
				break;
			case WM_DRAWITEM:
				if(aWindow::handleEvent(wnd,msg,wparam,lparam)) return 1;
				break;
			case WM_ERASEBKGND:
				if(!(wnd->style&aWINDOW_DIALOG)) return 1;
				break;
			case WM_DESTROY:
				wnd->close();
				return 0;
			case WM_SIZE:
				if(wparam!=SIZE_MINIMIZED)
					wnd->makeLayout(0,0,LOWORD(lparam),HIWORD(lparam));
				return 0;
		}
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
#endif



aObject_Inheritance(aWindow,aContainer)

aWindow::aWindow(aApplication *a,widget_event_handler weh) : aContainer(weh,aWIDGET_WINDOW) {
	app = a;
	window = 0;
	menu = 0;
	status = 0;
}

aWindow::~aWindow() {
debug_output("aWindow::~aWindow()\n");
	if(window) delete window;
	window = 0;
	if(menu) delete menu;
	menu = 0;
	if(status) delete status;
	status = 0;
}


void aWindow::open(aWindow *p) {
	const char *nm = text;
	if(!nm) nm = app->getApplicationName();
	if(p) p->openWindow(this);
#ifdef USE_GTK
	wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if(p) {
		parent = p;
		gtk_window_set_transient_for(GTK_WINDOW(wnd),GTK_WINDOW(p->wnd));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(wnd),true);
		if((style&aWINDOW_MODAL))
			gtk_window_set_modal(GTK_WINDOW(wnd),true);
	}
	gtk_window_set_title(GTK_WINDOW(wnd),nm);
	gtk_window_set_default_size(GTK_WINDOW(wnd),width,height);
	if(!(style&aWINDOW_RESIZABLE)) {
		gtk_widget_set_size_request(wnd,width,height);
		gtk_window_set_resizable(GTK_WINDOW(wnd),false);
	}
	if((style&aWINDOW_CENTER))
		gtk_window_set_position(GTK_WINDOW(wnd),GTK_WIN_POS_CENTER);
	if((style&aWINDOW_DIALOG)) {
	}
	g_signal_connect(G_OBJECT(wnd),"delete_event",G_CALLBACK(delete_event_callback),this);
	create(this,0);
	createAll(0,true);
	gtk_widget_show_all(wnd);
#endif
#ifdef USE_WIN32
	HWND hwnd;
	MSG msg;
	DWORD ex = WS_EX_CLIENTEDGE;
	DWORD st = WS_OVERLAPPEDWINDOW;
	LPCTSTR cl = aWINDOW_CLASS;
	int x = CW_USEDEFAULT,y = CW_USEDEFAULT;
	if(!(class_registers&aWINDOW_CLASS_REGISTER)) {
		WNDCLASSEX wndclassx = {
			sizeof(WNDCLASSEX),
			CS_HREDRAW|CS_VREDRAW,
			AmanitaMainWndProc,
			0,0,hMainInstance,
			LoadIcon(0,IDI_APPLICATION),
			LoadCursor(0,IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE+1),
			0,cl,LoadIcon(0,IDI_APPLICATION)
		};
		RegisterClassEx(&wndclassx);
		class_registers |= aWINDOW_CLASS_REGISTER;
	}
	if((style&aWINDOW_CENTER)) {
		RECT rc;
		GetWindowRect(GetDesktopWindow(),&rc);
		x = (rc.right-width)/2,y = (rc.bottom-height)/2;
	}
	if(p) parent = p;
	if((style&aWINDOW_DIALOG)) ex = WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE;
	if(!(style&aWINDOW_RESIZABLE))
		st = (WS_OVERLAPPEDWINDOW|WS_SYSMENU)& ~(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME);
#ifdef USE_WCHAR
	int len = strlen(nm)+1;
	wchar_t tnm[len];
	char2w(tnm,nm,len);
#else
	char *tnm = nm;
#endif
	hwnd = CreateWindowEx(ex,cl,tnm,st,
		x,y,width,height,(HWND)(parent? parent->component : 0),0,hMainInstance,this);
	if((style&aWINDOW_MODAL) && parent) {
		EnableWindow((HWND)parent->component,false);
		SetFocus((HWND)parent->component);
		SetFocus(hwnd);
	}

	if(!hwnd) MessageBox(NULL,_T("Window Creation Failed!"),_T("Error!"),MB_ICONEXCLAMATION|MB_OK);
#endif
}

bool aWindow::close() {
	if(parent) {
		if(component) {
#ifdef USE_GTK
			gtk_widget_destroy(GTK_WIDGET(component));
#endif
#ifdef USE_WIN32
			if((style&aWINDOW_MODAL) && parent) {
				EnableWindow((HWND)parent->component,true);
				SetFocus((HWND)parent->component);
			}
#endif
			component = 0;
		}
		((aWindow *)parent)->closeWindow(this);
	} else app->destroy();
	return true;
}

void aWindow::openWindow(aWindow *wnd) {
debug_output("aWindow::openWindow(wnd: %p)\n",wnd);
	if(window) wnd->next = window;
	window = wnd;
}

void aWindow::closeWindow(aWindow *wnd) {
debug_output("aWindow::closeWindow(wnd: %p)\n",wnd);
	if(window==wnd) window = (aWindow *)wnd->next;
	else {
		for(aWidget *w1=window,*w2 =w1->next; w2; w1=w2,w2=w2->next)
			if(w2==wnd) {
				w1->next = w2->next;
				break;
			}
	}
	delete wnd;
}

void aWindow::setMenu(aMenu *m) {
	menu = m;
	menu->parent = this;
}

void aWindow::setStatus(aStatus *sb) {
	status = sb;
	status->parent = this;
}

void aWindow::updateStatus(int n,const char *format, ...) {
	if(status) {
		char str[129];
		if(n<0 || n>=status->ncells) return;
		if(format && *format) {
			va_list args;
			va_start(args,format);
			vsnprintf(str,128,format,args);
			va_end(args);
//debug_output("setStatus(\"%s\")\n",str);
		} else *str = '\0';
		status->update(n,str);
	}
}

void aWindow::create(aWindow *wnd,uint32_t st) {
	// Window is actually already created in open().
}

void aWindow::createAll(aComponent p,bool n) {
#ifdef USE_GTK
	component = gtk_vbox_new(FALSE,0);
	if(menu || status) {
		if(menu) {
			menu->create(this,0);
			gtk_box_pack_start(GTK_BOX(component),GTK_WIDGET(menu->component),FALSE,FALSE,0);
		}
		if(status) {
			status->create(this,0);
			gtk_box_pack_end(GTK_BOX(component),GTK_WIDGET(status->component),FALSE,FALSE,0);
		}
	}

debug_output("aWindow::createAll(component: %p)\n",component);
	if(child) {
		child->create(this,0);
		child->createAll(0,true);
	}
	gtk_box_pack_start(GTK_BOX(component),GTK_WIDGET(child->component),TRUE,TRUE,0);
	gtk_container_add(GTK_CONTAINER(wnd),component);
#endif

#ifdef USE_WIN32
	if(menu) menu->create(this,0);
	if(status) status->create(this,0);

debug_output("aWindow::createAll(component: %p)\n",component);
	if(child) {
		child->create(this,0);
		child->createAll(component,true);
	}
#endif
}

#ifdef USE_WIN32
bool aWindow::handleEvent(aWindow *wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	switch(msg) {
		case WM_COMMAND:
//debug_output("WidgetProc(WM_COMMAND)\n");
			return wnd->command(wparam,lparam);
		case WM_NOTIFY:
//debug_output("WidgetProc(WM_NOTIFY)\n");
			return wnd->notify((LPNMHDR)lparam);
		case WM_DRAWITEM:
//debug_output("WidgetProc(WM_DRAWITEM)\n");
			return wnd->drawItem((LPDRAWITEMSTRUCT)lparam);
	}
	return false;
}

bool aWindow::command(WPARAM wparam,LPARAM lparam) {
	int t = (LOWORD(wparam))>>9;
//debug_output("aWindow::command(type: %d, id: %d)\n",t,LOWORD(wparam)&0x1ff);
	switch(t) {
		case aWIDGET_MENU:
			widget_event_handler weh;
			if(menu && (weh=menu->getEventHandler())) {
				aMenuItem *mi = menu->getItem(LOWORD(wparam)&0x1ff);
				weh(menu,aMENU_EVENT_ACTION,mi->id,(intptr_t)mi->name,(intptr_t)mi->data);
				return true;
			}
			break;
		case aWIDGET_BUTTON:
		case aWIDGET_CHECKBOX:
		case aWIDGET_RADIOBUTTON:
			if(HIWORD(wparam)==BN_CLICKED) {
				aButton *b = (aButton *)getWidget((aComponent)lparam);
				widget_event_handler weh = b->getEventHandler();
				if(weh) {
					if(b->isCheckBox() || b->isRadioButton())
						weh(b,aBUTTON_TOGGLED,b->isChecked(),0,0);
					else weh(b,aBUTTON_CLICKED,0,0,0);
					return true;
				}
			}
			break;
		case aWIDGET_COMBOBOX:
		case aWIDGET_COMBOBOX_ENTRY:
			if(HIWORD(wparam)==CBN_SELENDOK) {
				aChoice *ch = (aChoice *)getWidget((aComponent)lparam);
				widget_event_handler weh = ch->getEventHandler();
				if(weh) {
					weh(ch,aCHOICE_CHANGED,0,0,0);
					return true;
				}
			}
			break;
		case aWIDGET_LISTBOX:
			if(HIWORD(wparam)==LBN_SELCHANGE) {
				aChoice *ch = (aChoice *)getWidget((aComponent)lparam);
				widget_event_handler weh = ch->getEventHandler();
				if(weh) {
					weh(ch,aCHOICE_CHANGED,0,0,0);
					return true;
				}
			}
			break;
	}
	return false;
}

bool aWindow::notify(LPNMHDR nmhdr) {
	int t = nmhdr->idFrom>>9;
	switch(t) {
		case aWIDGET_NOTEBOOK:
			if(nmhdr->code==TCN_SELCHANGE) {
				aNotebook *nb = (aNotebook *)getWidget((aComponent)nmhdr->hwndFrom);
				nb->tabEvent();
				return true;
			}
			break;
		case aWIDGET_LISTVIEW:
			if(nmhdr->code==NM_CLICK || nmhdr->code==LVN_ITEMACTIVATE) {
				aList *l = (aList *)getWidget((aComponent)nmhdr->hwndFrom);
				LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)nmhdr;
				widget_event_handler weh = l->getEventHandler();
				if(weh) {
					weh(l,nmhdr->code==NM_CLICK? aLIST_CHANGED : aLIST_ACTIVATE,(intptr_t)nmia->iItem,0,0);
					return true;
				}
				return true;
			}
			break;
	}
	return false;
}

bool aWindow::drawItem(LPDRAWITEMSTRUCT dis) {
	aWidget *w = getWidget((aComponent)dis->hwndItem);
//debug_output("WM_DRAWITEM hwnd: %p, w: %p\n",dis->hwndItem,w);
	if(w) {
		w->drawItem(dis);
		return true;
	}
	return false;
}


void aWindow::makeLayout(int x,int y,int w,int h) {
	if(status) {
		RECT r;
//debug_output("aWindow::makeLayout(status - w: %d, h: %d)\n",w,h);
		SendMessage((HWND)status->getComponent(),WM_SIZE,0,0);
		GetClientRect((HWND)status->getComponent(),&r);
		h -= r.bottom-r.top;
	}
	h -= y;
//debug_output("aWindow::makeLayout(child: %p, component: %p, x: %d, y: %d, w: %d, h: %d)\n",child,child->getContainer(),x,y,w,h);
	aContainer::makeLayout(x,y,w,h);
//debug_output("aWindow::makeLayout(move children)\n");
	if(child)
		for(aWidget *w=child; w; w=w->next) w->move();
}
#endif




