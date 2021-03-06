#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

#include "fe-windows.h"

#define WINDOW_GUI(window) ((WindowGui *) ((window)->gui_data))

struct _WindowGui {
	GtkWidget *widget;

	GtkTextBuffer *buffer;

	GtkTextTagTable *tagtable;
	GtkTextTag *tag_underline;
	GtkTextTag *tag_monospace;

	PangoFontDescription *font_monospace;
	int indent;
	unsigned int newline:1;

	GSList *views;
	WindowView *active_view;

	Window *window;
};

void gui_window_add_view(WindowGui *window, Tab *tab);
void gui_window_remove_view(WindowView *view);

void gui_window_update_width(WindowGui *window);
/* Returns TRUE if window is visible in any of the frames. */
int gui_window_is_visible(Window *window);

void gui_windows_init(void);
void gui_windows_deinit(void);

#endif
