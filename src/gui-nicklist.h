#ifndef __GUI_NICKLIST_H
#define __GUI_NICKLIST_H

#include "nicklist.h"

enum {
	NICKLIST_COL_PTR,
	NICKLIST_COL_NAME,
	NICKLIST_COL_PIXMAP
};

struct _Nicklist {
	Channel *channel;

	GtkListStore *store;
	GSList *views;

	int nicks, ops, halfops, voices, normal;
};

Nicklist *gui_nicklist_new(Channel *channel);
void gui_nicklist_destroy(Nicklist *nicklist);

void gui_nicklists_init(void);
void gui_nicklists_deinit(void);

#endif
