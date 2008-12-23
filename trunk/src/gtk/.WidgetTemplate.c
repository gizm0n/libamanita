

#include <cairo.h>
#include <libamanita/gtk/.WidgetTemplate.h>


static void gtk_template_class_init(GtkTemplateClass *template_class);
static void gtk_template_init(GtkTemplate *template);
static void gtk_template_size_request(GtkWidget *widget,GtkRequisition *requisition);
static void gtk_template_size_allocate(GtkWidget *widget,GtkAllocation *allocation);
static void gtk_template_realize(GtkWidget *widget);
static gboolean gtk_template_expose(GtkWidget *widget,GdkEventExpose *event);
static void gtk_template_paint(GtkWidget *widget);
static void gtk_template_destroy(GtkObject *object);


GtkType gtk_template_get_type(void) {
	static GtkType gtk_template_type = 0;
	if(!gtk_template_type) {
		static const GtkTypeInfo gtk_template_info = {
			"GtkTemplate",sizeof(GtkTemplate),sizeof(GtkTemplateClass),
			(GtkClassInitFunc)gtk_template_class_init,(GtkObjectInitFunc)gtk_template_init,
			NULL,NULL,(GtkClassInitFunc)NULL
		};
		gtk_template_type = gtk_type_unique(GTK_TYPE_WIDGET,&gtk_template_info);
	}
	return gtk_template_type;
}

GtkWidget *gtk_template_new() {
	return GTK_WIDGET(gtk_type_new(gtk_template_get_type()));
}

static void gtk_template_class_init(GtkTemplateClass *template_class) {
	GtkWidgetClass *widget_class;
	GtkObjectClass *object_class;

	widget_class = (GtkWidgetClass *)template_class;
	object_class = (GtkObjectClass *)template_class;

	widget_class->realize = gtk_template_realize;
	widget_class->size_request = gtk_template_size_request;
	widget_class->size_allocate = gtk_template_size_allocate;
	widget_class->expose_event = gtk_template_expose;

	object_class->destroy = gtk_template_destroy;
}

static void gtk_template_init(GtkTemplate *template) {
}

static void gtk_template_size_request(GtkWidget *widget,GtkRequisition *requisition) {
	g_return_if_fail(widget!=NULL);
	g_return_if_fail(GTK_IS_TEMPLATE(widget));
	g_return_if_fail(requisition!=NULL);
	requisition->width = 80;
	requisition->height = 100;
}

static void gtk_template_size_allocate(GtkWidget *widget,GtkAllocation *allocation) {
	g_return_if_fail(widget!=NULL);
	g_return_if_fail(GTK_IS_TEMPLATE(widget));
	g_return_if_fail(allocation!=NULL);
	widget->allocation = *allocation;
	if(GTK_WIDGET_REALIZED(widget)) {
		gdk_window_move_resize(widget->window,allocation->x,allocation->y,allocation->width, allocation->height);
	}
}

static void gtk_template_realize(GtkWidget *widget) {
	GdkWindowAttr attributes;
	guint attributes_mask;
	g_return_if_fail(widget!=NULL);
	g_return_if_fail(GTK_IS_TEMPLATE(widget));
	GTK_WIDGET_SET_FLAGS(widget,GTK_REALIZED);
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = 80;
	attributes.height = 100;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events(widget)|GDK_EXPOSURE_MASK;
	attributes_mask = GDK_WA_X|GDK_WA_Y;
	widget->window = gdk_window_new(gtk_widget_get_parent_window(widget),&attributes,attributes_mask);
	gdk_window_set_user_data(widget->window,widget);
	widget->style = gtk_style_attach(widget->style,widget->window);
	gtk_style_set_background(widget->style,widget->window,GTK_STATE_NORMAL);
}

static gboolean gtk_template_expose(GtkWidget *widget,GdkEventExpose *event) {
	g_return_val_if_fail(widget!=NULL,FALSE);
	g_return_val_if_fail(GTK_IS_TEMPLATE(widget),FALSE);
	g_return_val_if_fail(event!=NULL,FALSE);
	gtk_template_paint(widget);
	return FALSE;
}

static void gtk_template_paint(GtkWidget *widget) {
	cairo_t *cr;
	cr = gdk_cairo_create(widget->window);
	cairo_destroy(cr);
}

static void gtk_template_destroy(GtkObject *object) {
	GtkTemplate *template;
	GtkTemplateClass *template_class;
	g_return_if_fail(object!=NULL);
	g_return_if_fail(GTK_IS_TEMPLATE(object));
	template = GTK_TEMPLATE(object);
	template_class = gtk_type_class(gtk_widget_get_type());
	if(GTK_OBJECT_CLASS(template_class)->destroy) (* GTK_OBJECT_CLASS(template_class)->destroy)(object);
}





