
#include "../_config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#include <exdisp.h>
#include <oaidl.h>
#endif
#include <amanita/gui/aBrowser.h>

#if defined(__linux__)
#include <webkit/webkit.h>


void webview_hover_event_callback(WebKitWebView *webview,gchar *title,gchar *uri,gpointer data) {
//	aBrowser *browser = (aBrowser *)data;
/*	app.handleAction(ACTION_SHOW_STATUS,0,(void *)(uri? uri : aString::blank));*/
}

gboolean webview_navigation_decision_event_callback(WebKitWebView *webview,WebKitWebFrame *frame,WebKitNetworkRequest *request,
																	WebKitWebNavigationAction *action,WebKitWebPolicyDecision *decision,gpointer data) {
//	aBrowser *browser = (aBrowser *)data;
/*	int reason = webkit_web_navigation_action_get_reason(action);
	if(reason>=WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED && reason<=WEBKIT_WEB_NAVIGATION_REASON_FORM_RESUBMITTED) {
		TextPage &page = *(TextPage *)data;
//		const gchar *uri = webkit_web_navigation_action_get_original_uri(action);
		const gchar *uri = webkit_network_request_get_uri(request);
app.printf("TextPage::webview_navigation_decision_event_callback(uri=\"%s\",reason=%d)",uri,reason);
		webkit_web_policy_decision_ignore(decision);
		app.handleAction(ACTION_SHOW_STATUS,0,(void *)aString::blank);
		page.navigate(uri);
		return TRUE;
	} else */return FALSE;
}


#elif defined(WIN32)
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <mshtml.h>

#ifdef UNICODE 
const WCHAR *amanita_browser_class = BROWSER_CLASS;
#else
const char *amanita_browser_class = BROWSER_CLASS;
#endif



BSTR char2BSTR(char *c) {
	if(!c || !*c) return 0;
	int l = strlen(c);
	WCHAR *w = (WCHAR *)malloc(sizeof(WCHAR)*(l+1));
	MultiByteToWideChar(CP_ACP,0,c,-1,w,l+1);
	BSTR b = SysAllocString(w);
	free(w);
	return b;
}

char *BSTR2char(BSTR b) {
	if(b) {
		int l = SysStringLen(b);
		char *c = (char *)malloc(l+1);
		if(WideCharToMultiByte(CP_ACP,0,b,l+1,(char*)c,l+1,0,0)) return c;
		else free(c);
	}
	return 0;
}


#define DISPID_BEFORENAVIGATE						100	// this is sent before navigation to give a chance to abort
#define DISPID_NAVIGATECOMPLETE					101	// in async, this is sent when we have enough to show
#define DISPID_STATUSTEXTCHANGE					102
#define DISPID_QUIT									103
#define DISPID_DOWNLOADCOMPLETE					104
#define DISPID_COMMANDSTATECHANGE				105
#define DISPID_DOWNLOADBEGIN						106
#define DISPID_NEWWINDOW							107	// sent when a new window should be created
#define DISPID_PROGRESSCHANGE						108	// sent when download progress is updated
#define DISPID_WINDOWMOVE							109	// sent when main window has been moved
#define DISPID_WINDOWRESIZE						110	// sent when main window has been sized
#define DISPID_WINDOWACTIVATE						111	// sent when main window has been activated
#define DISPID_PROPERTYCHANGE						112	// sent when the PutProperty method is called
#define DISPID_TITLECHANGE							113	// sent when the document title changes
#define DISPID_TITLEICONCHANGE					114	// sent when the top level window icon may have changed.

#define DISPID_FRAMEBEFORENAVIGATE				200
#define DISPID_FRAMENAVIGATECOMPLETE			201
#define DISPID_FRAMENEWWINDOW						204

// Printing events
#define DISPID_PRINTTEMPLATEINSTANTIATION		225	// Fired to indicate that a print template is instantiated
#define DISPID_PRINTTEMPLATETEARDOWN			226	// Fired to indicate that a print templete is completely gone
#define DISPID_UPDATEPAGESTATUS					227	// Fired to indicate that the spooling status has changed

#define DISPID_BEFORENAVIGATE2					250	// hyperlink clicked on
#define DISPID_NEWWINDOW2							251
#define DISPID_NAVIGATECOMPLETE2					252	// UIActivate new document
#define DISPID_ONQUIT								253
#define DISPID_ONVISIBLE							254	// sent when the window goes visible/hidden
#define DISPID_ONTOOLBAR							255	// sent when the toolbar should be shown/hidden
#define DISPID_ONMENUBAR							256	// sent when the menubar should be shown/hidden
#define DISPID_ONSTATUSBAR							257	// sent when the statusbar should be shown/hidden
#define DISPID_ONFULLSCREEN						258	// sent when kiosk mode should be on/off
#define DISPID_DOCUMENTCOMPLETE					259	// new document goes ReadyState_Complete
#define DISPID_ONTHEATERMODE						260	// sent when theater mode should be on/off
#define DISPID_ONADDRESSBAR						261	// sent when the address bar should be shown/hidden
#define DISPID_WINDOWSETRESIZABLE				262	// sent to set the style of the host window frame
#define DISPID_WINDOWCLOSING						263	// sent before script window.close closes the window
#define DISPID_WINDOWSETLEFT						264	// sent when the put_left method is called on the WebOC
#define DISPID_WINDOWSETTOP						265	// sent when the put_top method is called on the WebOC
#define DISPID_WINDOWSETWIDTH						266	// sent when the put_width method is called on the WebOC
#define DISPID_WINDOWSETHEIGHT					267	// sent when the put_height method is called on the WebOC
#define DISPID_CLIENTTOHOSTWINDOW				268	// sent during window.open to request conversion of dimensions
#define DISPID_SETSECURELOCKICON					269	// sent to suggest the appropriate security icon to show
#define DISPID_FILEDOWNLOAD						270	// Fired to indicate the File Download dialog is opening
#define DISPID_NAVIGATEERROR						271	// Fired to indicate the a binding error has occured
#define DISPID_PRIVACYIMPACTEDSTATECHANGE		272	// Fired when the user's browsing experience is impacted

//#include <mshtmdid.h>
#define DISPID_WINDOWOBJECT						(-5500)
#define DISPID_LOCATIONOBJECT						(-5506)
#define DISPID_HISTORYOBJECT						(-5507)
#define DISPID_NAVIGATOROBJECT					(-5508)
#define DISPID_SECURITYCTX							(-5511)
#define DISPID_AMBIENT_DLCONTROL					(-5512)
#define DISPID_AMBIENT_USERAGENT					(-5513)
#define DISPID_SECURITYDOMAIN						(-5514)

#define DLCTL_DLIMAGES								0x00000010
#define DLCTL_VIDEOS									0x00000020
#define DLCTL_BGSOUNDS								0x00000040
#define DLCTL_NO_SCRIPTS							0x00000080
#define DLCTL_NO_JAVA								0x00000100
#define DLCTL_NO_RUNACTIVEXCTLS					0x00000200
#define DLCTL_NO_DLACTIVEXCTLS					0x00000400
#define DLCTL_DOWNLOADONLY							0x00000800
#define DLCTL_NO_FRAMEDOWNLOAD					0x00001000
#define DLCTL_RESYNCHRONIZE						0x00002000
#define DLCTL_PRAGMA_NO_CACHE						0x00004000
#define DLCTL_FORCEOFFLINE							0x10000000
#define DLCTL_NO_CLIENTPULL						0x20000000
#define DLCTL_SILENT									0x40000000
#define DLCTL_OFFLINEIFNOTCONNECTED				0x80000000
#define DLCTL_OFFLINE								DLCTL_OFFLINEIFNOTCONNECTED


typedef enum tagDOCHOSTUIDBLCLK {
	DOCHOSTUIDBLCLK_DEFAULT = 0,
	DOCHOSTUIDBLCLK_SHOWPROPERTIES = 1,
	DOCHOSTUIDBLCLK_SHOWCODE = 2
} DOCHOSTUIDBLCLK;

typedef enum tagDOCHOSTUIFLAG {
	DOCHOSTUIFLAG_DIALOG = 0x00000001,
	DOCHOSTUIFLAG_DISABLE_HELP_MENU = 0x00000002,
	DOCHOSTUIFLAG_NO3DBORDER = 0x00000004,
	DOCHOSTUIFLAG_SCROLL_NO = 0x00000008,
	DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE = 0x00000010,
	DOCHOSTUIFLAG_OPENNEWWIN = 0x00000020,
	DOCHOSTUIFLAG_DISABLE_OFFSCREEN = 0x00000040,
	DOCHOSTUIFLAG_FLAT_SCROLLBAR = 0x00000080,
	DOCHOSTUIFLAG_DIV_BLOCKDEFAULT = 0x00000100,
	DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY = 0x00000200,
	DOCHOSTUIFLAG_OVERRIDEBEHAVIORFACTORY = 0x00000400,
	DOCHOSTUIFLAG_CODEPAGELINKEDFONTS = 0x00000800,
	DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8 = 0x00001000,
	DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 = 0x00002000,
	DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE = 0x00004000,
	DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION = 0x00010000,
	DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION = 0x00020000,
	DOCHOSTUIFLAG_THEME = 0x00040000,
	DOCHOSTUIFLAG_NOTHEME = 0x00080000,
	DOCHOSTUIFLAG_NOPICS = 0x00100000,
	DOCHOSTUIFLAG_NO3DOUTERBORDER = 0x00200000,
	DOCHOSTUIFLAG_DISABLE_EDIT_NS_FIXUP = 0x00400000,
	DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK = 0x00800000,
	DOCHOSTUIFLAG_DISABLE_UNTRUSTEDPROTOCOL = 0x01000000,
	DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATIONS = 0x04000000
} DOCHOSTUIFLAG;

typedef enum tagDOCHOSTUITYPE {
	DOCHOSTUITYPE_BROWSE = 0,
	DOCHOSTUITYPE_AUTHOR = 1
} DOCHOSTUITYPE;


extern "C" {
	const GUID IID_IDocHostUIHandler = { 0xBD3F23C0,0xD43E,0x11CF,{0x89,0x3B,0x00,0xAA,0x00,0xBD,0xCE,0x1A} };
	const GUID IID_IDocHostShowUI = { 0xC4D244B0,0xD43E,0x11CF,{0x89,0x3B,0x00,0xAA,0x00,0xBD,0xCE,0x1A} };

	typedef struct _DOCHOSTUIINFO {
		ULONG cbSize;
		DWORD dwFlags;
		DWORD dwDoubleClick;
		OLECHAR *pchHostCss;
		OLECHAR *pchHostNS;
	} DOCHOSTUIINFO;

	#undef INTERFACE
	#define INTERFACE IDocHostUIHandler
	DECLARE_INTERFACE_(IDocHostUIHandler,IUnknown) {
		STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
		STDMETHOD_(ULONG,AddRef)(THIS) PURE;
		STDMETHOD_(ULONG,Release)(THIS) PURE;
		STDMETHOD(ShowContextMenu)(THIS_ DWORD,POINT *,IUnknown *,IDispatch *) PURE;
		STDMETHOD(GetHostInfo)(THIS_ DOCHOSTUIINFO *) PURE;
		STDMETHOD(ShowUI)(THIS_ DWORD,IOleInPlaceActiveObject *,IOleCommandTarget *,IOleInPlaceFrame *,IOleInPlaceUIWindow *) PURE;
		STDMETHOD(HideUI)(THIS) PURE;
		STDMETHOD(UpdateUI)(THIS) PURE;
		STDMETHOD(EnableModeless)(THIS_ BOOL) PURE;
		STDMETHOD(OnDocWindowActivate)(THIS_ BOOL) PURE;
		STDMETHOD(OnFrameWindowActivate)(THIS_ BOOL) PURE;
		STDMETHOD(ResizeBorder)(THIS_ LPCRECT,IOleInPlaceUIWindow *,BOOL) PURE;
		STDMETHOD(TranslateAccelerator)(THIS_ LPMSG,const GUID *,DWORD) PURE;
		STDMETHOD(GetOptionKeyPath)(THIS_ LPOLESTR *,DWORD) PURE;
		STDMETHOD(GetDropTarget)(THIS_ IDropTarget *,IDropTarget **) PURE;
		STDMETHOD(GetExternal)(THIS_ IDispatch **) PURE;
		STDMETHOD(TranslateUrl)(THIS_ DWORD,OLECHAR *,OLECHAR **) PURE;
		STDMETHOD(FilterDataObject)(THIS_ IDataObject *,IDataObject **) PURE;
	};


	#undef INTERFACE
	#define INTERFACE IDocHostShowUI
	DECLARE_INTERFACE_(IDocHostShowUI,IUnknown) {
		STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
		STDMETHOD_(ULONG,AddRef)(THIS) PURE;
		STDMETHOD_(ULONG,Release)(THIS) PURE;
		STDMETHOD(ShowHelp)(THIS_ HWND,LPOLESTR,UINT,DWORD,POINT,IDispatch *) PURE;
		STDMETHOD(ShowMessage)(THIS_ HWND,LPOLESTR,LPOLESTR,DWORD,LPOLESTR,DWORD,LRESULT *) PURE;
	};
}

class aBrowser;

class _EventSink : public IDispatch,public IDocHostUIHandler,public IDocHostShowUI {
friend class aBrowser;

private:
private:
	enum {
		HANDLER_ENABLEMODELESS,
		HANDLER_FILTERDATAOBJECT,
		HANDLER_GETDROPTARGET,
		HANDLER_GETEXTERNAL,
		HANDLER_GETHOSTINFO,
		HANDLER_GETOPTIONKEYPATH,
		HANDLER_HIDEUI,
		HANDLER_ONDOCWINDOWACTIVATE,
		HANDLER_ONFRAMEWINDOWACTIVATE,
		HANDLER_RESIZEBORDER,
		HANDLER_SHOWCONTEXTMENU,
		HANDLER_SHOWUI,
		HANDLER_TRANSLATEACCELERATOR,
		HANDLER_TRANSLATEURL,
		HANDLER_UPDATEUI,
		HANDLER_SHOWHELP,
		HANDLER_SHOWMESSAGE,
	};

	aBrowser *browser;
	volatile LONG ref;
	bool loading;
	char url[1024];
	HRESULT settings[17];

	HRESULT handleEvent(int event) { return settings[event]; }

public:
	_EventSink(aBrowser *b);
	~_EventSink() {}

	// *** IUnknown ***
	STDMETHODIMP QueryInterface(REFIID riid,PVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef(void) { return InterlockedIncrement(&ref); }
	STDMETHODIMP_(ULONG) Release(void) { if(InterlockedDecrement(&ref)==0) delete this;return ref; }
	// *** IDispatch ***
	STDMETHODIMP GetIDsOfNames(REFIID,OLECHAR **,unsigned int,LCID,DISPID *pdispid);
	STDMETHODIMP GetTypeInfo(unsigned int,LCID,ITypeInfo **) { return E_NOTIMPL; }
	STDMETHODIMP GetTypeInfoCount(unsigned int *) { return E_NOTIMPL; }
	STDMETHODIMP Invoke(DISPID disp,REFIID riid,LCID lcid,WORD flags,DISPPARAMS *params,VARIANT *result,EXCEPINFO *info,unsigned int *err);
	// IDocHostUIHandler
	STDMETHODIMP EnableModeless(BOOL enable) { return handleEvent(HANDLER_ENABLEMODELESS); }
	STDMETHODIMP FilterDataObject(IDataObject *,IDataObject **) { return handleEvent(HANDLER_FILTERDATAOBJECT); }
	STDMETHODIMP GetDropTarget(IDropTarget *,IDropTarget **) { return handleEvent(HANDLER_GETDROPTARGET); }
	STDMETHODIMP GetExternal(IDispatch **) { return handleEvent(HANDLER_GETEXTERNAL); }
	STDMETHODIMP GetHostInfo(DOCHOSTUIINFO *info);
	STDMETHODIMP GetOptionKeyPath(LPOLESTR *,DWORD) { return handleEvent(HANDLER_GETOPTIONKEYPATH); }
	STDMETHODIMP HideUI(void) { return handleEvent(HANDLER_HIDEUI); }
	STDMETHODIMP OnDocWindowActivate(BOOL) { return handleEvent(HANDLER_ONDOCWINDOWACTIVATE); }
	STDMETHODIMP OnFrameWindowActivate(BOOL) { return handleEvent(HANDLER_ONFRAMEWINDOWACTIVATE); }
	STDMETHODIMP ResizeBorder(LPCRECT,IOleInPlaceUIWindow *,BOOL) { return handleEvent(HANDLER_RESIZEBORDER); }
	STDMETHODIMP ShowContextMenu(DWORD,POINT *,IUnknown *,IDispatch *) { return handleEvent(HANDLER_SHOWCONTEXTMENU); }
	STDMETHODIMP ShowUI(DWORD,IOleInPlaceActiveObject *,IOleCommandTarget *,IOleInPlaceFrame *,IOleInPlaceUIWindow *) { return handleEvent(HANDLER_SHOWUI); }
	STDMETHODIMP TranslateAccelerator(LPMSG,const GUID *,DWORD) { return handleEvent(HANDLER_TRANSLATEACCELERATOR); }
	STDMETHODIMP TranslateUrl(DWORD,OLECHAR *,OLECHAR **) { return handleEvent(HANDLER_TRANSLATEURL); }
	STDMETHODIMP UpdateUI(void) { return handleEvent(HANDLER_UPDATEUI); }
	// IDocHostShowUI
	STDMETHODIMP ShowHelp(HWND hwnd,LPOLESTR pszHelpFile,UINT uCommand,DWORD dwData,POINT ptMouse,IDispatch *pDispatchObjectHit) { return handleEvent(HANDLER_SHOWHELP); }
	STDMETHODIMP ShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT *plResult) { return handleEvent(HANDLER_SHOWMESSAGE); }

	/* Only used for testing which event is happening. Can safely be removed. */
	void printInvokeParams(DISPID disp,WORD flags,DISPPARAMS *params);
	void printVariant(VARIANT *v,int l);
};

_EventSink::_EventSink(aBrowser *b) : browser(b),ref(0),loading(false) {
	HRESULT s[17] = { E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL,E_NOTIMPL };
	*url = '\0';
	memcpy(settings,s,sizeof(settings));
}

STDMETHODIMP _EventSink::QueryInterface(REFIID riid,PVOID *ppv) {
	if(riid==IID_IUnknown) *ppv = (IUnknown *)(IDocHostShowUI *)this;
	else if(riid==IID_IDispatch) *ppv =(IDispatch *)this;
	else if(riid==IID_IDocHostUIHandler) *ppv = (IDocHostUIHandler *)this;
	else if(riid==IID_IDocHostShowUI) *ppv = (IDocHostShowUI *)this;
	else { *ppv = 0;return E_NOINTERFACE; }
	AddRef();
	return S_OK;
}

STDMETHODIMP _EventSink::GetIDsOfNames(REFIID,OLECHAR **,unsigned int,LCID,DISPID *pdispid) {
	*pdispid = DISPID_UNKNOWN;
	return DISP_E_UNKNOWNNAME;
}

STDMETHODIMP _EventSink::Invoke(DISPID disp,REFIID riid,LCID lcid,WORD flags,DISPPARAMS *params,VARIANT *result,EXCEPINFO *info,unsigned int *err) {
	if(disp!=102 && disp!=113) printInvokeParams(disp,flags,params);
	switch(disp) {
		case DISPID_BEFORENAVIGATE2:
		{
//			if(loading) { *params->rgvarg[0].pboolVal = TRUE;return; }
			char *str = BSTR2char(params->rgvarg[5].pvarVal->bstrVal);
fprintf(stderr,"_EventSink::BeforeNavigate2(loading=%d,str='%s')\n",loading,str);
fflush(stderr);
			if(str && *str && strncmp(str,"about:blank",11)!=0) {
				strcpy(this->url,str);
				//WPARAM w = GetWindowLong((HWND)browser->handle,GWL_ID)&0xFFFF;
				loading = true;//!SendMessage((HWND)browser->parent,WM_COMMAND,w|(BROWSER_ONCLICK<<16),(LPARAM)this->url);
				*params->rgvarg[0].pboolVal = VARIANT_FALSE;
//				if(loading) PostMessage(browser->hparent,WM_COMMAND,w|(BROWSER_ONREDIRECT<<16),(LPARAM)this->url);
			}
			free(str);
			break;
		}
		case DISPID_NAVIGATECOMPLETE2:loading = false;break;
		case DISPID_DOCUMENTCOMPLETE:loading = false;break;
//		case DISPID_STATUSTEXTCHANGE:break;
//		case DISPID_TITLECHANGE:break;
//		case DISPID_NEWWINDOW2:*params->rgvarg[0].pboolVal = VARIANT_TRUE;break;
		case DISPID_AMBIENT_DLCONTROL:result->vt = VT_I4,result->lVal = DLCTL_DLIMAGES|DLCTL_NO_JAVA;break;
		case DISPID_NAVIGATEERROR:*params->rgvarg[0].pboolVal = VARIANT_TRUE;break;
break;
		default:return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}

STDMETHODIMP _EventSink::GetHostInfo(DOCHOSTUIINFO *info) {
	return E_NOTIMPL;
/*	info->cbSize = sizeof(DOCHOSTUIINFO);
	info->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER|DOCHOSTUIFLAG_FLAT_SCROLLBAR;
//	info->dwDoubleClick |= DOCHOSTUIDBLCLK_SHOWCODE;
	return S_OK;
*/
}

void _EventSink::printInvokeParams(DISPID disp,WORD flags,DISPPARAMS *params) {
	static const int dispids[] = { 100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,
		200,201,204,225,226,227,
		250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272 };
	static const char *dispnms[] = { "DISPID_BEFORENAVIGATE","DISPID_NAVIGATECOMPLETE","DISPID_STATUSTEXTCHANGE","DISPID_QUIT","DISPID_DOWNLOADCOMPLETE",
		"DISPID_COMMANDSTATECHANGE","DISPID_DOWNLOADBEGIN","DISPID_NEWWINDOW","DISPID_PROGRESSCHANGE","DISPID_WINDOWMOVE",
		"DISPID_WINDOWRESIZE","DISPID_WINDOWACTIVATE","DISPID_PROPERTYCHANGE","DISPID_TITLECHANGE","DISPID_TITLEICONCHANGE",
		"DISPID_FRAMEBEFORENAVIGATE","DISPID_FRAMENAVIGATECOMPLETE","DISPID_FRAMENEWWINDOW","DISPID_PRINTTEMPLATEINSTANTIATION",
		"DISPID_PRINTTEMPLATETEARDOWN","DISPID_UPDATEPAGESTATUS","DISPID_BEFORENAVIGATE2","DISPID_NEWWINDOW2",
		"DISPID_NAVIGATECOMPLETE2","DISPID_ONQUIT","DISPID_ONVISIBLE","DISPID_ONTOOLBAR","DISPID_ONMENUBAR","DISPID_ONSTATUSBAR",
		"DISPID_ONFULLSCREEN","DISPID_DOCUMENTCOMPLETE","DISPID_ONTHEATERMODE","DISPID_ONADDRESSBAR","DISPID_WINDOWSETRESIZABLE",
		"DISPID_WINDOWCLOSING","DISPID_WINDOWSETLEFT","DISPID_WINDOWSETTOP","DISPID_WINDOWSETWIDTH","DISPID_WINDOWSETHEIGHT",
		"DISPID_CLIENTTOHOSTWINDOW","DISPID_SETSECURELOCKICON","DISPID_FILEDOWNLOAD","DISPID_NAVIGATEERROR",
		"DISPID_PRIVACYIMPACTEDSTATECHANGE" };

	int i;
	const char *d = "";
	for(i=sizeof(dispids)/sizeof(int); i>=0; i--) if(dispids[i]==disp) { d = dispnms[i];break; }
	fprintf(stderr,"_EventSink::printInvokeParams(%d,'%s',flags=%d) {\n",(int)disp,d,flags);
	for(int i=0; i<(int)params->cArgs; i++) printVariant(&params->rgvarg[i],1);
	for(int i=0; i<(int)params->cNamedArgs; i++) fprintf(stderr,"\tdispid=%d\n",(int)params->rgdispidNamedArgs[i]);
	fprintf(stderr,"}\n");
	fflush(stderr);
}

/* Only used for testing which event is happening. Can safely be removed. */
void _EventSink::printVariant(VARIANT *v,int l) {
	int i;
	for(i=0; i<l; i++) putc('\t',stderr);
	fprintf(stderr,"vt=%d",v->vt);
	switch(v->vt) {
		case VT_UI1:fprintf(stderr,",bVal=%d\n",v->bVal);break;
		case VT_I2:fprintf(stderr,",iVal=%d\n",v->iVal);break;
		case VT_I4:fprintf(stderr,",lVal=%lu\n",v->lVal);break;
		case VT_R4:fprintf(stderr,",fltVal=%f\n",v->fltVal);break;
		case VT_R8:fprintf(stderr,",dblVal=%lf\n",v->dblVal);break;
		case VT_BOOL:fprintf(stderr,",boolVal=%d\n",v->boolVal);break;
		case VT_ERROR:fprintf(stderr,",scode=%ld\n",v->scode);break;
		case VT_CY:fprintf(stderr,",cyVal{Lo=%lu,Hi=%ld}\n",v->cyVal.Lo,v->cyVal.Hi);break;
		case VT_DATE:fprintf(stderr,",date=%lf\n",v->date);break;
		case VT_BSTR:
		{
			char *str = BSTR2char(v->bstrVal);
			fprintf(stderr,",bstrVal='%s'\n",str? str : "");
			if(str) free(str);
			break;
		}
		case VT_UNKNOWN:fprintf(stderr,",punkVal=%lu\n",(unsigned long)v->punkVal);break;
		case VT_DISPATCH:fprintf(stderr,",pdispVal=%lu\n",(unsigned long)v->pdispVal);break;
		case VT_ARRAY:fprintf(stderr,",parray=%lu\n",(unsigned long)v->parray);break;
		case VT_EMPTY:fprintf(stderr,",none\n");break;
		default:
			if((v->vt&VT_BYREF)==VT_BYREF) {
				switch(v->vt&(~VT_BYREF)) {
					case VT_UI1:fprintf(stderr,",pbVal=%d\n",*v->pbVal);break;
					case VT_I2:fprintf(stderr,",piVal=%d\n",*v->piVal);break;
					case VT_I4:fprintf(stderr,",plVal=%lu\n",*v->plVal);break;
					case VT_R4:fprintf(stderr,",pfltVal=%f\n",*v->pfltVal);break;
					case VT_R8:fprintf(stderr,",pdblVal=%lf\n",*v->pdblVal);break;
					case VT_BOOL:fprintf(stderr,",pboolVal=%d\n",*v->pboolVal);break;
					case VT_ERROR:fprintf(stderr,",pscode=%lu\n",(unsigned long)*v->pscode);break;
					case VT_CY:fprintf(stderr,",pcyVal{Lo=%lu,Hi=%ld}\n",v->pcyVal->Lo,v->pcyVal->Hi);break;
					case VT_DATE:fprintf(stderr,",pdate=%lu\n",(unsigned long)*v->pdate);break;
					case VT_BSTR:fprintf(stderr,",pbstrVal=%lu\n",(unsigned long)*v->pbstrVal);break;
					case VT_UNKNOWN:fprintf(stderr,",ppunkVal=%lu\n",(unsigned long)*v->ppunkVal);break;
					case VT_DISPATCH:fprintf(stderr,",ppdispVal=%lu\n",(unsigned long)*v->ppdispVal);break;
					case VT_VARIANT:
						fprintf(stderr,",pvarVal {\n");
						printVariant(v->pvarVal,l+1);
						for(i=0; i<l; i++) putc('\t',stderr);
						fprintf(stderr,"}\n");
						break;
					default:fprintf(stderr,"\n");
				}
			} else fprintf(stderr,"\n");
	}
}


class _Container : public IOleClientSite,public IOleInPlaceSite,public IOleInPlaceFrame,public IOleControlSite,public IDispatch {
friend class aBrowser;

private:
	LONG ref;
	HWND hwnd;
	IUnknown *unknown;
	RECT rect;
	aBrowser *browser;
	IWebBrowser2 *msie;
	_EventSink *event;
	DWORD cookie;

	void ConnectEvents();
	void DisconnectEvents();

	IConnectionPoint *GetConnectionPoint(REFIID);

public:
	_Container(aBrowser *b);
	~_Container();

	// *** IUnknown ***
	STDMETHODIMP QueryInterface(REFIID riid,PVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef(void) { return InterlockedIncrement(&ref); }
	STDMETHODIMP_(ULONG) Release(void) { if(InterlockedDecrement(&ref)==0) delete this;return ref; }
	// *** IOleClientSite ***
	STDMETHODIMP SaveObject() { return E_NOTIMPL; }
	STDMETHODIMP GetMoniker(DWORD,DWORD,LPMONIKER *) { return E_NOTIMPL; }
	STDMETHODIMP GetContainer(LPOLECONTAINER *) { return E_NOINTERFACE; }
	STDMETHODIMP ShowObject() { return S_OK; }
	STDMETHODIMP OnShowWindow(BOOL) { return S_OK; }
	STDMETHODIMP RequestNewObjectLayout() { return E_NOTIMPL; }
	// *** IOleWindow ***
	STDMETHODIMP GetWindow(HWND *phwnd) { *phwnd = hwnd;return S_OK; }
	STDMETHODIMP ContextSensitiveHelp(BOOL) { return E_NOTIMPL; }
	// *** IOleInPlaceSite ***
	STDMETHODIMP CanInPlaceActivate(void) { return S_OK; }
	STDMETHODIMP OnInPlaceActivate(void) { return S_OK; }
	STDMETHODIMP OnUIActivate(void) { return S_OK; }
	STDMETHODIMP GetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppIIPUIWin,LPRECT prcPosRect,
					LPRECT prcClipRect,LPOLEINPLACEFRAMEINFO pFrameInfo);
	STDMETHODIMP Scroll(SIZE) { return E_NOTIMPL; }
	STDMETHODIMP OnUIDeactivate(BOOL) { return E_NOTIMPL; }
	STDMETHODIMP OnInPlaceDeactivate(void) { return S_OK; }
	STDMETHODIMP DiscardUndoState(void) { return E_NOTIMPL; }
	STDMETHODIMP DeactivateAndUndo(void) { return E_NOTIMPL; }
	STDMETHODIMP OnPosRectChange(LPCRECT) { return S_OK; }
	// *** IOleInPlaceUIWindow ***
	STDMETHODIMP GetBorder(LPRECT) { return E_NOTIMPL; }
	STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS) { return E_NOTIMPL; }
	STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS) { return E_NOTIMPL; }
	STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject *,LPCOLESTR) { return E_NOTIMPL; }
	// *** IOleInPlaceFrame ***
	STDMETHODIMP InsertMenus(HMENU,LPOLEMENUGROUPWIDTHS) { return E_NOTIMPL; }
	STDMETHODIMP SetMenu(HMENU,HOLEMENU,HWND) {return E_NOTIMPL; }
	STDMETHODIMP RemoveMenus(HMENU) { return E_NOTIMPL; }
	STDMETHODIMP SetStatusText(LPCOLESTR) { return E_NOTIMPL; }
	STDMETHODIMP EnableModeless(BOOL) { return E_NOTIMPL; }
	STDMETHODIMP TranslateAccelerator(LPMSG,WORD) { return S_OK; }
	// *** IOleControlSite ***
	STDMETHODIMP OnControlInfoChanged(void) { return E_NOTIMPL; }
	STDMETHODIMP LockInPlaceActive(BOOL) { return E_NOTIMPL; }
	STDMETHODIMP GetExtendedControl(IDispatch **) { return E_NOTIMPL; }
	STDMETHODIMP TransformCoords(POINTL *,POINTF *,DWORD) { return E_NOTIMPL; }
	STDMETHODIMP TranslateAccelerator(LPMSG,DWORD) { return E_NOTIMPL; }
	STDMETHODIMP OnFocus(BOOL) { return E_NOTIMPL; }
	STDMETHODIMP ShowPropertyFrame(void) { return E_NOTIMPL; }
	// *** IDispatch ***
	STDMETHODIMP GetIDsOfNames(REFIID,OLECHAR **,unsigned int,LCID,DISPID *pdispid) { *pdispid = DISPID_UNKNOWN;return DISP_E_UNKNOWNNAME; }
	STDMETHODIMP GetTypeInfo(unsigned int,LCID,ITypeInfo **) { return E_NOTIMPL; }
	STDMETHODIMP GetTypeInfoCount(unsigned int *) { return E_NOTIMPL; }
	STDMETHODIMP Invoke(DISPID,REFIID,LCID,WORD,DISPPARAMS *,VARIANT *,EXCEPINFO *,unsigned int *) { return DISP_E_MEMBERNOTFOUND; }

	void add();
	void remove();
	void setLocation(int,int,int,int);
	void setVisible(bool);
	void setFocus(bool);
	HRESULT getMSIE();
};


_Container::_Container(aBrowser *b){
fprintf(stderr,"_Container::_Container(1,widget=%p)\n",(aWidget *)b);
fflush(stderr);
	browser = b;
	msie = 0;
	ref = 0;
	hwnd = (HWND)browser->handle;
	unknown = 0;
fprintf(stderr,"_Container::_Container(2)\n");
fflush(stderr);
	SetRectEmpty(&rect);
fprintf(stderr,"_Container::_Container(3)\n");
fflush(stderr);
	event = new _EventSink(browser);
	event->AddRef();
fprintf(stderr,"_Container::_Container(4)\n");
fflush(stderr);
	AddRef();
	add();
	getMSIE();
}

_Container::~_Container() {
	if(msie) { msie->Release();msie = 0; }
	event->Release();
	unknown->Release();
}

STDMETHODIMP _Container::QueryInterface(REFIID riid,PVOID *ppv) {
	if(riid==IID_IUnknown) *ppv = (IUnknown *)(IOleClientSite *)this;
	else if(riid==IID_IOleClientSite) *ppv = (IOleClientSite *)this;
	else if(riid==IID_IOleInPlaceSite) *ppv = (IOleInPlaceSite *)this;
	else if(riid==IID_IOleInPlaceFrame) *ppv = (IOleInPlaceFrame *)this;
	else if(riid==IID_IOleInPlaceUIWindow)*ppv = (IOleInPlaceUIWindow *)this;
	else if(riid==IID_IOleControlSite) *ppv = (IOleControlSite *)this;
//	else if(riid==IID_IOleWindow) *ppv = (IID_IOleWindow *)this;
	else if(riid==IID_IDispatch) *ppv = (IDispatch *)this;
	else {
		if(riid==IID_IDocHostUIHandler) *ppv = (IDocHostUIHandler *)event;
		else if(riid==IID_IDocHostShowUI) *ppv = (IDocHostShowUI *)event;
		else { ppv = 0;return E_NOINTERFACE; }
		event->AddRef();
		return S_OK;
	}
	AddRef();
	return S_OK;
}

STDMETHODIMP _Container::GetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppIIPUIWin,LPRECT prcPosRect,
				LPRECT prcClipRect,LPOLEINPLACEFRAMEINFO pFrameInfo) {
	RECT rc;
	*ppFrame =(IOleInPlaceFrame *)this;
	*ppIIPUIWin = 0;
	GetClientRect(hwnd,&rc);
	prcPosRect->left				= 0;
	prcPosRect->top				= 0;
	prcPosRect->right				= rc.right;
	prcPosRect->bottom			= rc.bottom;
	CopyRect(prcClipRect,prcPosRect);
	pFrameInfo->cb					= sizeof(OLEINPLACEFRAMEINFO);
	pFrameInfo->fMDIApp			= FALSE;
	pFrameInfo->hwndFrame		= hwnd;
	pFrameInfo->haccel			= 0;
	pFrameInfo->cAccelEntries	= 0;
	(*ppFrame)->AddRef();
	return S_OK;
}

void _Container::add() {
	CLSID clsid;
fprintf(stderr,"_Container::add(1)\n");
fflush(stderr);
	CLSIDFromString((WCHAR *)L"Shell.Explorer",&clsid);
	CoCreateInstance(clsid,0,CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,IID_IUnknown,(PVOID *)&unknown);
	IOleObject *pioo;
fprintf(stderr,"_Container::add(2)\n");
fflush(stderr);
	unknown->QueryInterface(IID_IOleObject,(PVOID *)&pioo);
	pioo->SetClientSite(this);
	pioo->Release();
	IPersistStreamInit *ppsi;
fprintf(stderr,"_Container::add(3)\n");
fflush(stderr);
	unknown->QueryInterface(IID_IPersistStreamInit,(PVOID *)&ppsi);
	ppsi->InitNew();
	ppsi->Release();
	ConnectEvents();
	setVisible(true);
	setFocus(true);
fprintf(stderr,"_Container::add(4)\n");
fflush(stderr);
}

void _Container::remove() {
	IOleObject *pioo;
	unknown->QueryInterface(IID_IOleObject,(PVOID *)&pioo);
	pioo->Close(OLECLOSE_NOSAVE);
	pioo->SetClientSite(0);
	pioo->Release();
	IOleInPlaceObject *pipo;
	unknown->QueryInterface(IID_IOleInPlaceObject,(PVOID *)&pipo);
	pipo->UIDeactivate();
	pipo->InPlaceDeactivate();
	pipo->Release();
	DisconnectEvents();
}

void _Container::setLocation(int x,int y,int cx,int cy) {
	rect.left   = x;
	rect.top    = y;
	rect.right  = cx;
	rect.bottom = cy;
	IOleInPlaceObject *pipo;
	unknown->QueryInterface(IID_IOleInPlaceObject,(PVOID *)&pipo);
	pipo->SetObjectRects(&rect,&rect);
	pipo->Release();
}

void _Container::setVisible(bool bVisible) {
	IOleObject *pioo;
	unknown->QueryInterface(IID_IOleObject,(PVOID *)&pioo);
	if(bVisible) {
		pioo->DoVerb(OLEIVERB_INPLACEACTIVATE,0,this,0,hwnd,&rect);
		pioo->DoVerb(OLEIVERB_SHOW,0,this,0,hwnd,&rect);
	} else pioo->DoVerb(OLEIVERB_HIDE,0,this,0,hwnd,0);
	pioo->Release();
}

void _Container::setFocus(bool bFocus) {
	IOleObject *pioo;
	if(bFocus) {
		unknown->QueryInterface(IID_IOleObject,(PVOID *)&pioo);
		pioo->DoVerb(OLEIVERB_UIACTIVATE,0,this,0,hwnd,&rect);
		pioo->Release();
	}
}

void _Container::ConnectEvents() {
	IConnectionPoint *pcp = GetConnectionPoint(DIID_DWebBrowserEvents2);
//	if(SUCCEEDED(pcp->Advise(event,&cookie)))
//		fprintf(stderr,"_Container::ConnectEvents(advised)\n");
	pcp->Advise((IUnknown *)(IDispatch *)event,&cookie);
	pcp->Release();
}

void _Container::DisconnectEvents() {
	IConnectionPoint *pcp;
	pcp = GetConnectionPoint(DIID_DWebBrowserEvents2);
	pcp->Unadvise(cookie);
	pcp->Release();
}

IConnectionPoint* _Container::GetConnectionPoint(REFIID riid) {
	IConnectionPointContainer *pcpc;
	unknown->QueryInterface(IID_IConnectionPointContainer,(PVOID *)&pcpc);
	IConnectionPoint *pcp;
	pcpc->FindConnectionPoint(riid,&pcp);
	pcpc->Release();
	return pcp;
}

HRESULT _Container::getMSIE() {
	return unknown->QueryInterface(IID_IWebBrowser2,(PVOID *)&msie);
}


LRESULT CALLBACK AmanitaBrowserProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	aBrowser *b = 0;
fprintf(stderr,"AmanitaBrowserProc(msg=%u,wparam=%u,lparam=%lu)\n",msg,wparam,lparam);
fflush(stderr);
	if(msg==WM_CREATE) {
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		b = (aBrowser *)cs->lpCreateParams;
		SetProp(hwnd,amanita_browser_class,(HANDLE)b);
		b->handle = (aHandle)hwnd;
		b->parent = (aHandle)cs->hwndParent;
		b->container = new _Container(b);
		if(cs->lpszName && *cs->lpszName) b->setUrl(cs->lpszName);
		return 0;
	} else if((b=(aBrowser *)GetProp(hwnd,amanita_browser_class))) {
		if(msg==WM_SIZE) {
			b->container->setLocation(0,0,LOWORD(lparam),HIWORD(lparam));
			return 0;
		} else if(msg==WM_DESTROY) {
			if(b->container) {
				b->container->remove();
				b->container->Release();
				b->container = 0;
			}
			RemoveProp(hwnd,amanita_browser_class);
			return 0;
		}
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
#endif




aBrowser::aBrowser(widget_event_handler weh) : aWidget(weh,WIDGET_BROWSER) {
	/* Inherited from aWidget: */
	id = 0;
	parent = 0;
	handle = 0;
	text = 0;
//	sizer = 0;

#if defined(__linux__)
#elif defined(WIN32)
	container = 0;
	stamp = 0;
//	scrollX = GetSystemMetrics(SM_CXHSCROLL)+2;
//	scrollY = GetSystemMetrics(SM_CYVSCROLL)+2;

fprintf(stderr,"aBrowser::aBrowser(widget=%p)\n",(aWidget *)this);
fflush(stderr);
#endif
}

aBrowser::~aBrowser() {
fprintf(stderr,"aBrowser::~aBrowser(widget=%p)\n",(aWidget *)this);
fflush(stderr);
}

aHandle aBrowser::create(aHandle p,int s) {
	parent = p;
#if defined(__linux__)
	GtkWidget *scroll,*webkit;
	scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	webkit = webkit_web_view_new();
	WebKitWebSettings *settings = webkit_web_settings_new();
	g_object_set(settings,"auto-load-images",TRUE,"auto-shrink-images",FALSE,"default-encoding","UTF-8","enable-plugins",FALSE,NULL);
	webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webkit),settings);
	g_signal_connect(G_OBJECT(webkit),"hovering-over-link",G_CALLBACK(webview_hover_event_callback),this);
	g_signal_connect(G_OBJECT(webkit),"navigation-policy-decision-requested",G_CALLBACK(webview_navigation_decision_event_callback),this);
//	g_signal_connect(G_OBJECT(webkit),"navigation-requested",G_CALLBACK(webview_navigation_event_callback),this);
	gtk_container_add(GTK_CONTAINER(scroll),webkit);
	handle = (aHandle)webkit;
	return (aHandle)scroll;
#elif defined(WIN32)
	return aWidget::create(p,s);
#endif
}

void aBrowser::setUrl(const char *url) {
	if(!url || !*url) return;
//fprintf(stderr,"aBrowser::setUrl('%s')\n",url);
//fflush(stderr);
#if defined(__linux__)
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(handle),url);

#elif defined(WIN32)
	stamp = (intptr_t)url;
	int len = lstrlen(url)+ 1;
	WCHAR *w;
	w = (WCHAR *)malloc(sizeof(WCHAR)*len);
	MultiByteToWideChar(CP_ACP,0,url,-1,w,len);
	VARIANT v;
	VariantInit(&v);
	v.vt = VT_BSTR;
	v.bstrVal = SysAllocString(w);
	free(w);

	if(stamp!=(intptr_t)url) return;
	container->msie->Navigate2(&v,0,0,0,0);
	VariantClear(&v);
#endif
}


void aBrowser::setHtmlContent(const char *html) {
#ifdef WIN32
	static const SAFEARRAYBOUND ArrayBound = {1, 0};
#endif

	if(!html || !*html) return;
	
#if defined(__linux__)
	webkit_web_view_load_string(WEBKIT_WEB_VIEW(handle),html,"text/html","UTF-8","file://./");

#elif defined(WIN32)
	LPDISPATCH d;
	IHTMLDocument2 *h;
	SAFEARRAY *a;
	VARIANT url;
	VARIANT *v;
	BSTR bstr = 0;

	container->event->loading = true,stamp = (intptr_t)html;

	VariantInit(&url);
	url.vt = VT_BSTR;
	url.bstrVal = SysAllocString(L"about:blank");
	container->msie->Navigate2(&url,0,0,0,0);
	VariantClear(&url);
	if(!pumpMessages(stamp)) return;
	if(!container->msie->get_Document(&d)) {
		if(!d->QueryInterface(IID_IHTMLDocument2,(void**)&h)) {
			if((a=SafeArrayCreate(VT_VARIANT,1,(SAFEARRAYBOUND *)&ArrayBound))) {
				if(!SafeArrayAccessData(a,(void**)&v)) {
					v->vt = VT_BSTR;
					wchar_t *buffer;
					DWORD size = MultiByteToWideChar(CP_ACP,0,html,-1,0,0);
					if((buffer=(wchar_t *)GlobalAlloc(GMEM_FIXED,sizeof(wchar_t)*size))) {
						MultiByteToWideChar(CP_ACP,0,html,-1,buffer,size);
						bstr = SysAllocString(buffer);
						GlobalFree(buffer);
						if((v->bstrVal=bstr)) h->write(a);
					}
				}
				SafeArrayDestroy(a);
			}
			h->Release();
		}
		d->Release();
	}
#endif
}


#if defined(__linux__)

#elif defined(WIN32)
bool aBrowser::pumpMessages(intptr_t st) {
	MSG msg;
	while(container->event->loading && stamp==st) if(PeekMessage(&msg,0,0,0,PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (stamp==st);
}

#endif

