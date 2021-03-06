#ifndef __ENTRY_H
#define __ENTRY_H

#include "keyboard.h"

struct _Entry {
	GtkWidget *widget;
	GtkEntry *entry;

	Window *active_win;
	Keyboard *keyboard;

	unsigned int last_dead_key:1;
};

Entry *gui_entry_new(Frame *frame);

void gui_entry_set_window(Entry *entry, Window *window);

#endif
