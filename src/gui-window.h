#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

#include "fe-windows.h"

#define WINDOW_GUI(window) ((WindowGui *) ((window)->gui_data))

struct _WindowGui {
	GtkWidget *widget;

	GtkTextBuffer *buffer;
	GtkTextTagTable *tagtable;

	GSList *views;
	WindowView *active_view;

	Window *window;
};

void gui_window_add_view(WindowGui *window, Tab *tab);
void gui_window_remove_view(WindowGui *window, WindowView *view);

void gui_window_update_width(WindowGui *window);

void gui_windows_init(void);
void gui_windows_deinit(void);

#endif
