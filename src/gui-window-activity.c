/*
 gui-window-activity.c : irssi

    Copyright (C) 2002 Timo Sirainen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "module.h"
#include "signals.h"

#include "gui-frame.h"
#include "gui-tab.h"
#include "gui-window.h"
#include "gui-window-view.h"

/* Don't send window activity if window is already visible in
   another split window or frame */
static void sig_activity(Window *window)
{
	GSList *tmp;

	if (!gui_window_is_visible(window) || window->data_level == 0)
		return;

	window->data_level = 0;
        window->hilight_color = 0;

	for (tmp = window->items; tmp != NULL; tmp = tmp->next) {
		WI_ITEM_REC *item = tmp->data;

		item->data_level = 0;
		item->hilight_color = 0;
	}

	signal_stop();
}

static void tab_update_activity(Tab *tab)
{
	GList *tmp;
	int data_level;

	/* get highest data level */
	data_level = 0;
	for (tmp = tab->panes; tmp != NULL; tmp = tmp->next) {
		TabPane *pane = tmp->data;

		if (pane->view != NULL &&
		    pane->view->window->window->data_level > data_level)
			data_level = pane->view->window->window->data_level;
	}
	tab->data_level = data_level;
}

static void tab_clear_activity(Tab *tab)
{
	tab->data_level = 0;
}

static void sig_activity_update(Window *window)
{
	GSList *tmp;

	for (tmp = WINDOW_GUI(window)->views; tmp != NULL; tmp = tmp->next) {
		WindowView *view = tmp->data;

		if (window->data_level == 0) {
			if (view->pane->tab->data_level != 0)
				tab_clear_activity(view->pane->tab);
			continue;
		}

		if (view->pane->tab->data_level < window->data_level) {
			/* update tab's main label's color too */
			tab_update_activity(view->pane->tab);
		}
	}
}

void gui_window_activities_init(void)
{
	signal_add_first("window hilight", (SIGNAL_FUNC) sig_activity);
	signal_add_first("window activity", (SIGNAL_FUNC) sig_activity);

	signal_add("window hilight", (SIGNAL_FUNC) sig_activity_update);
	/* we need to use "window changed", so we can clear activity in
	   all split windows in tab. */
	signal_add("window changed", (SIGNAL_FUNC) sig_activity_update);
}

void gui_window_activities_deinit(void)
{
	signal_remove("window hilight", (SIGNAL_FUNC) sig_activity);
	signal_remove("window activity", (SIGNAL_FUNC) sig_activity);

	signal_remove("window hilight", (SIGNAL_FUNC) sig_activity_update);
	signal_remove("window changed", (SIGNAL_FUNC) sig_activity_update);
}
