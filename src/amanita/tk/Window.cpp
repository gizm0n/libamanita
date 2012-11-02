
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
#include <amanita/Resource.h>
#include <amanita/tk/Window.h>
#include <amanita/tk/Menu.h>
#include <amanita/tk/Status.h>
#include <amanita/tk/Notebook.h>
#include <amanita/tk/Button.h>
#include <amanita/tk/Select.h>
#include <amanita/tk/List.h>
#include <amanita/tk/Panel.h>


namespace a {
namespace tk {



#ifdef USE_WIN32
static tchar_t tooltip_string[1025];
#endif


#ifdef USE_GTK
static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	Window *wnd = (Window *)data;
	return !wnd->close();
}
#endif



#ifdef USE_WIN32
LRESULT CALLBACK AmanitaMainWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	Window *wnd = 0;
	if(msg==WM_CREATE) {
//debug_output("AmanitaMainWndProc(WM_CREATE)\n");
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		wnd = (Window *)cs->lpCreateParams;
		wnd->component = (Component)hwnd;
		SetWindowLongPtr(hwnd,GWL_USERDATA,(LONG_PTR)wnd);
		wnd->create(wnd,0);
		wnd->createAll(0,true);
		wnd->show();
		return 0;
	} else {
		wnd = (Window *)GetWindowLongPtr(hwnd,GWL_USERDATA);
		if(wnd) switch(msg) {
			case WM_COMMAND:
				if(Window::handleEvent(wnd,msg,wparam,lparam)) return 0;
				break;
			case WM_NOTIFY:
				if(Window::handleEvent(wnd,msg,wparam,lparam)) return 0;
				break;
			case WM_DRAWITEM:
				if(Window::handleEvent(wnd,msg,wparam,lparam)) return 1;
				break;
			case WM_ERASEBKGND:
				if(!(wnd->style&WINDOW_DIALOG)) return 1;
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



Object_Inheritance(Window,Container)

Window::Window(Application *a,widget_event_handler weh) : Container(weh,WIDGET_WINDOW) {
	app = a;
	window = 0;
	menu = 0;
	status = 0;
	icons = 0;
}

Window::~Window() {
debug_output("Window::~Window()\n");
	if(window) delete window;
	window = 0;
	if(menu) delete menu;
	menu = 0;
	if(status) delete status;
	status = 0;
	clearIcons();
}


void Window::open(Window *p) {
	const char *nm = text;
	if(!nm) nm = app->getApplicationName();
	if(p) p->openWindow(this);
#ifdef USE_GTK
	wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if(p) {
		parent = p;
		gtk_window_set_transient_for(GTK_WINDOW(wnd),GTK_WINDOW(p->wnd));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(wnd),true);
		if((style&WINDOW_MODAL))
			gtk_window_set_modal(GTK_WINDOW(wnd),true);
	}
	gtk_window_set_title(GTK_WINDOW(wnd),nm);
	gtk_window_set_default_size(GTK_WINDOW(wnd),width,height);
	if(!(style&WINDOW_RESIZABLE)) {
		gtk_widget_set_size_request(wnd,width,height);
		gtk_window_set_resizable(GTK_WINDOW(wnd),false);
	}
	if((style&WINDOW_CENTER))
		gtk_window_set_position(GTK_WINDOW(wnd),GTK_WIN_POS_CENTER);
	if((style&WINDOW_DIALOG)) {
	}
	g_signal_connect(G_OBJECT(wnd),"delete_event",G_CALLBACK(delete_event_callback),this);
	create(this,0);
	createAll(0,true);
	if(icons) setIcons(icons);
	gtk_widget_show_all(wnd);
#endif
#ifdef USE_WIN32
	HWND hwnd;
	DWORD ex = WS_EX_CLIENTEDGE;
	DWORD st = WS_OVERLAPPEDWINDOW;
	LPCTSTR cl = WINDOW_CLASS;
	int x = CW_USEDEFAULT,y = CW_USEDEFAULT;
	tchar_t *t = tstrdup(nm);
	if(!(class_registers&WINDOW_CLASS_REGISTER)) {
		HICON hicon = 0,hicon_sm = 0;
		if(icons) {
			WindowIcon *icon,*icon_b = 0,*icon_sm = 0;
			for(icon=icons; icon->size>0; ++icon)
				if(icon->size==16 && icon->id>0) icon_sm = icon;
				else if((!icon_b || icon_b->size<icon->size) && icon->id>0) icon_b = icon;
			if(icon_b) hicon = (HICON)LoadImage(hMainInstance,MAKEINTRESOURCE(icon_b->id),IMAGE_ICON,0,0,LR_SHARED);
			if(icon_sm) hicon_sm = (HICON)LoadImage(hMainInstance,MAKEINTRESOURCE(icon_sm->id),IMAGE_ICON,16,16,LR_SHARED);
debug_output("Window::open(hicon: %p[%d], hicon_sm: %p[%d])\n",hicon,icon_b->id,hicon_sm,icon_sm->id);
			clearIcons();
		}
		if(!hicon) hicon = LoadIcon(0,IDI_APPLICATION);
		if(!hicon_sm) hicon_sm = LoadIcon(0,IDI_APPLICATION);
		WNDCLASSEX wndclassx;
		wndclassx.cbSize				= sizeof(wndclassx);
		wndclassx.style				= CS_HREDRAW|CS_VREDRAW;
		wndclassx.lpfnWndProc		= AmanitaMainWndProc;
		wndclassx.cbClsExtra			= 0;
		wndclassx.cbWndExtra			= 0;
		wndclassx.hInstance			= hMainInstance;
		wndclassx.hIcon				= hicon;
		wndclassx.hIconSm				= hicon_sm;
		wndclassx.hCursor				= LoadCursor(0,IDC_ARROW);
		wndclassx.hbrBackground		= (HBRUSH)(COLOR_BTNFACE+1);
		wndclassx.lpszClassName		= cl;
		wndclassx.lpszMenuName		= 0;
		RegisterClassEx(&wndclassx);
		class_registers |= WINDOW_CLASS_REGISTER;
	}
	if((style&WINDOW_CENTER)) {
		RECT rc;
		GetWindowRect(GetDesktopWindow(),&rc);
		x = (rc.right-width)/2,y = (rc.bottom-height)/2;
	}
	if(p) parent = p;
	if((style&WINDOW_DIALOG)) ex = WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE;
	if(!(style&WINDOW_RESIZABLE))
		st = (WS_OVERLAPPEDWINDOW|WS_SYSMENU)& ~(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME);
	hwnd = CreateWindowEx(ex,cl,t,st,
		x,y,width,height,(HWND)(parent? parent->component : 0),0,hMainInstance,this);
	if((style&WINDOW_MODAL) && parent) {
		EnableWindow((HWND)parent->component,false);
		SetFocus((HWND)parent->component);
		SetFocus(hwnd);
	}
	tfree(t);

	if(!hwnd) MessageBox(0,_T("Window Creation Failed!"),_T("Error!"),MB_ICONEXCLAMATION|MB_OK);
#endif
}

bool Window::close() {
	if(parent) {
		if(component) {
#ifdef USE_GTK
			gtk_widget_destroy(GTK_WIDGET(component));
#endif
#ifdef USE_WIN32
			if((style&WINDOW_MODAL) && parent) {
				EnableWindow((HWND)parent->component,true);
				SetFocus((HWND)parent->component);
			}
#endif
			component = 0;
		}
		((Window *)parent)->closeWindow(this);
	} else app->destroy();
	return true;
}

void Window::openWindow(Window *wnd) {
debug_output("Window::openWindow(wnd: %p)\n",wnd);
	if(window) wnd->next = window;
	window = wnd;
}

void Window::closeWindow(Window *wnd) {
debug_output("Window::closeWindow(wnd: %p)\n",wnd);
	if(window==wnd) window = (Window *)wnd->next;
	else {
		for(Widget *w1=window,*w2 =w1->next; w2; w1=w2,w2=w2->next)
			if(w2==wnd) {
				w1->next = w2->next;
				break;
			}
	}
	delete wnd;
}

void Window::setMenu(Menu *m) {
	menu = m;
	menu->parent = this;
}

void Window::setStatus(Status *sb) {
	status = sb;
	status->parent = this;
}

void Window::updateStatus(int n,const char *format, ...) {
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

void Window::setIcons(const WindowIcon icons[]) {
	int n = 0;
	if(icons) for(const WindowIcon *icon=icons; icon->size>0; ++icon,++n);
	else return;
	if(component) {
		const WindowIcon *icon;
#ifdef USE_GTK
		GError *error = 0;
		GdkPixbuf *pixbuf;
		GList *iconlist = 0;
		for(icon=icons; icon->size>0; ++icon) {
			if(icon->xpm) pixbuf = gdk_pixbuf_new_from_xpm_data(icon->xpm);
			else if(icon->png) pixbuf = gdk_pixbuf_new_from_file(icon->png,&error);
			else pixbuf = 0;
			if(pixbuf) iconlist = g_list_append(iconlist,pixbuf);
		}
		if(!component) gtk_window_set_default_icon_list(iconlist);
		else gtk_window_set_icon_list(GTK_WINDOW(wnd),iconlist);
#endif
#ifdef USE_WIN32
		HICON hicon;
		for(icon=icons; icon->size>0; ++icon) {
			if(icon->size==16 && icon->id>0) {
				hicon = (HICON)LoadImage(hMainInstance,MAKEINTRESOURCE(icon->id),IMAGE_ICON,16,16,LR_SHARED);
				SendMessage((HWND)component,WM_SETICON,ICON_SMALL,(LPARAM)hicon);
			} else if(icon->id>0) {
				hicon = (HICON)LoadImage(hMainInstance,MAKEINTRESOURCE(icon->id),IMAGE_ICON,0,0,LR_SHARED);
				SendMessage((HWND)component,WM_SETICON,ICON_BIG,(LPARAM)hicon);
			}
		}
#endif
		if(this->icons) clearIcons();
	} else {
		WindowIcon *icon;
		this->icons = (WindowIcon *)malloc((n+1)*sizeof(WindowIcon));
		memcpy(this->icons,icons,(n+1)*sizeof(WindowIcon));
		for(icon=this->icons; icon->size>0; ++icon)
			if(icon->png) icon->png = strdup(icon->png);
		
	}
}

void Window::clearIcons() {
	if(icons) {
		WindowIcon *icon;
		for(icon=icons; icon->size>0; ++icon)
			if(icon->png) free((char *)icon->png);
		free(icons);
		icons = 0;
	}
}

void Window::create(Window *wnd,uint32_t st) {
	// Window is actually already created in open().
}

void Window::createAll(Component p,bool n) {
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

debug_output("Window::createAll(component: %p)\n",component);
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

debug_output("Window::createAll(component: %p)\n",component);
	if(child) {
		child->create(this,0);
		child->createAll(component,true);
	}
#endif
}

#ifdef USE_WIN32
bool Window::handleEvent(Window *wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
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

bool Window::command(WPARAM wparam,LPARAM lparam) {
	int t = WIDGET_ID_TYPE(LOWORD(wparam));
//debug_output("Window::command(type: %d, id: %d)\n",t,WIDGET_ID_INDEX(LOWORD(wparam)));
//fflush(stderr);
	switch(t) {
		case WIDGET_MENU:
			widget_event_handler weh;
			if(menu && (weh=menu->getEventHandler())) {
				MenuItem *mi = menu->getItem(WIDGET_ID_INDEX(LOWORD(wparam)));
				weh(menu,MENU_EVENT_ACTION,mi->id,(intptr_t)mi->name,(intptr_t)mi->data);
				return true;
			}
			break;
		case WIDGET_BUTTON:
		case WIDGET_CHECKBOX:
		case WIDGET_RADIOBUTTON:
			if(HIWORD(wparam)==BN_CLICKED) {
				Button *b = (Button *)getWidget((Component)wparam);
				widget_event_handler weh = b->getEventHandler();
				if(weh) {
					if(b->isCheckBox() || b->isRadioButton())
						weh(b,BUTTON_TOGGLED,b->isChecked(),0,0);
					else weh(b,BUTTON_CLICKED,0,0,0);
					return true;
				}
			}
			break;
		case WIDGET_COMBOBOX:
		case WIDGET_COMBOBOX_ENTRY:
			if(HIWORD(wparam)==CBN_SELENDOK) {
				Select *ch = (Select *)getWidget((Component)wparam);
				widget_event_handler weh = ch->getEventHandler();
				if(weh) {
					weh(ch,SELECT_CHANGED,0,0,0);
					return true;
				}
			}
			break;
		case WIDGET_LISTBOX:
			if(HIWORD(wparam)==LBN_SELCHANGE) {
				Select *ch = (Select *)getWidget((Component)wparam);
				widget_event_handler weh = ch->getEventHandler();
				if(weh) {
					weh(ch,SELECT_CHANGED,0,0,0);
					return true;
				}
			}
			break;
		case WIDGET_PANEL:
			if(HIWORD(wparam)==BN_CLICKED) {
				PanelButton *pb = (PanelButton *)getWidget(LOWORD(wparam));
//				Panel *p = (Panel *)getWidget((Component)lparam);
//debug_output("Window::command(pb: %p, id: %x, lparam: %p)\n",pb,LOWORD(wparam),lparam);
//fflush(stderr);
//				PanelButton *pb = &p->buttons[WIDGET_ID_INDEX(LOWORD(wparam))];
				if(pb) {
					widget_event_handler weh = pb->panel->getEventHandler();
					if(weh) {
						weh(pb->panel,PANEL_CLICKED,pb->action,(intptr_t)pb->data,0);
						return true;
					}
				}
			}
			break;
	}
	return false;
}

bool Window::notify(LPNMHDR nmhdr) {
	int t = WIDGET_ID_TYPE(nmhdr->idFrom);
//debug_output("Window::notify(type: %d, id: %d)\n",t,WIDGET_ID_INDEX(nmhdr->idFrom));
//fflush(stderr);
	switch(t) {
		case WIDGET_NOTEBOOK:
			if(nmhdr->code==TCN_SELCHANGE) {
				Notebook *nb = (Notebook *)getWidget((Component)nmhdr->hwndFrom);
				nb->tabEvent();
				return true;
			}
			break;
		case WIDGET_LISTVIEW:
			if(nmhdr->code==NM_CLICK || nmhdr->code==LVN_ITEMACTIVATE) {
				List *l = (List *)getWidget((Component)nmhdr->hwndFrom);
				LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)nmhdr;
				widget_event_handler weh = l->getEventHandler();
				if(weh) {
					weh(l,nmhdr->code==NM_CLICK? LIST_CHANGED : LIST_ACTIVATE,(intptr_t)nmia->iItem,0,0);
					return true;
				}
				return true;
			}
			break;
		case WIDGET_PANEL:
			if(nmhdr->code==TTN_NEEDTEXT) {
//				Panel *p = (Panel *)getWidget((Component)nmhdr->hwndFrom);
//				PanelButton *pb = &p->buttons[WIDGET_ID_INDEX(nmhdr->idFrom)];
				PanelButton *pb = (PanelButton *)getWidget(nmhdr->idFrom);
				LPNMTTDISPINFO ttt = (LPNMTTDISPINFO)nmhdr;
				if(pb && pb->tooltip) {
#ifdef USE_WCHAR
					char2w(tooltip_string,pb->tooltip);
#else
					strncpy(tooltip_string,pb->tooltip,1024);
#endif
					
					ttt->lpszText = tooltip_string;
					ttt->hinst = NULL;
//debug_output("Window::notify(hwnd: %p, pb: %p, id: %d, lParam: %p)\n",nmhdr->hwndFrom,pb,nmhdr->idFrom,ttt->lParam);
//fflush(stderr);
				} else ttt->lpszText = NULL;
			}
	}
	return false;
}

bool Window::drawItem(LPDRAWITEMSTRUCT dis) {
	Widget *w = getWidget((Component)dis->hwndItem);
//debug_output("WM_DRAWITEM hwnd: %p, w: %p\n",dis->hwndItem,w);
	if(w) {
		w->drawItem(dis);
		return true;
	}
	return false;
}


void Window::makeLayout(int x,int y,int w,int h) {
	if(status) {
		RECT r;
//debug_output("Window::makeLayout(status - w: %d, h: %d)\n",w,h);
		SendMessage((HWND)status->getComponent(),WM_SIZE,0,0);
		GetClientRect((HWND)status->getComponent(),&r);
		h -= r.bottom-r.top;
	}
	h -= y;
//debug_output("Window::makeLayout(child: %p, component: %p, x: %d, y: %d, w: %d, h: %d)\n",child,child->getContainer(),x,y,w,h);
	Container::makeLayout(x,y,w,h);
//debug_output("Window::makeLayout(move children)\n");
	if(child)
		for(Widget *w=child; w; w=w->next) w->move();
}
#endif


}; /* namespace tk */
}; /* namespace a */



