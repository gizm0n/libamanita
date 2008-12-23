
#ifndef _LIBAMANITA_GTK_WIDGETTEMPLATE_H
#define _LIBAMANITA_GTK_WIDGETTEMPLATE_H

/**
 * Replace "template" in various letter cases with the name you prefer for your widget.
 */


#include <gtk/gtk.h>

G_BEGIN_DECLS


#define GTK_TEMPLATE(obj) GTK_CHECK_CAST(obj,gtk_template_get_type(),GtkTemplate)
#define GTK_TEMPLATE_CLASS(klass) GTK_CHECK_CLASS_CAST(klass,gtk_template_get_type(),GtkTemplateClass)
#define GTK_IS_TEMPLATE(obj) GTK_CHECK_TYPE(obj,gtk_template_get_type())

typedef struct _GtkTemplate GtkTemplate;
typedef struct _GtkTemplateClass GtkTemplateClass;

struct _GtkTemplate {
	GtkWidget widget;
	gint sel;
};

struct _GtkTemplateClass {
	GtkWidgetClass parent_class;
};

GtkType gtk_template_get_type(void);
GtkWidget *gtk_template_new();

G_END_DECLS


#endif /* _LIBAMANITA_GTK_WIDGETTEMPLATE_H */




