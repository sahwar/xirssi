/*
 gui-frame.c : irssi

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

#include "keyboard.h"

#include "gui-frame.h"
#include "gui-entry.h"
#include "gui-tab.h"
#include "gui-window.h"

#include <gdk/gdkkeysyms.h>

GSList *frames;
Frame *active_frame;

static gboolean event_destroy(GtkWidget *window, Frame *frame)
{
	frames = g_slist_remove(frames, frame);
	if (active_frame == frame)
		gui_frame_set_active(frames != NULL ? frames->data : NULL);

	signal_emit("gui frame destroyed", 1, frame);

	frame->widget = NULL;
	frame->notebook = NULL;

	if (frames == NULL) {
		/* last window killed - quit */
		signal_emit("command quit", 1, "");
	}

	gui_frame_unref(frame);
	return FALSE;
}

static gboolean event_focus(GtkWidget *widget, GdkEventFocus *event,
			    Frame *frame)
{
        gui_frame_set_active(frame);
	if (frame->active_tab->active_win != NULL)
		window_set_active(frame->active_tab->active_win);
	return FALSE;
}

char *gui_key2str(GdkEventKey *event)
{
	GString *cmd;
	char *ret;
	int chr, len;

	chr = event->keyval;

	/* get alt/ctrl/shift masks */
	cmd = g_string_new(NULL);
	if (event->state & GDK_SHIFT_MASK) {
		if (chr > 255)
			g_string_prepend(cmd, "shift-");
		else
			chr = i_toupper(chr);
	}

	if (event->state & GDK_MOD1_MASK)
		g_string_prepend(cmd, "meta-");

	if (event->state & GDK_CONTROL_MASK) {
		if (chr > 255)
			g_string_prepend(cmd, "ctrl-");
		else {
			g_string_prepend(cmd, "^");
			chr = i_toupper(chr);
		}
	}

	/* get key name */
	if (event->keyval > 255) {
		len = cmd->len;
		g_string_append(cmd, gdk_keyval_name(event->keyval));
		g_strdown(cmd->str+len);
	} else if (event->keyval == ' ')
		g_string_append(cmd, "space");
	else if (event->keyval == '\n')
		g_string_append(cmd, "return");
	else
		g_string_sprintfa(cmd, "%c", chr);

	ret = cmd->str;
	g_string_free(cmd, FALSE);
	return ret;
}

static gboolean event_key_press(GtkWidget *widget, GdkEventKey *event,
				Frame *frame)
{
	GtkWidget *entry;
	char *str;
	int pos;

	if (event->keyval == GDK_Control_L || event->keyval == GDK_Control_R ||
	    event->keyval == GDK_Meta_L || event->keyval == GDK_Meta_R ||
	    event->keyval == GDK_Alt_L || event->keyval == GDK_Alt_R)
		return FALSE;

	str = gui_key2str(event);
	if (key_pressed(frame->entry->keyboard, str)) {
		g_signal_stop_emission_by_name(G_OBJECT(widget),
					       "key_press_event");
		g_free(str);
		return TRUE;
	}
	g_free(str);

	entry = frame->entry->widget;
	if (!GTK_WIDGET_HAS_FOCUS(entry) &&
	    (event->state & (GDK_CONTROL_MASK|GDK_MOD1_MASK)) == 0) {
		/* normal key pressed, change focus to entry field -
		   the whole text is selected after grab_focus so we need
		   to unselect it so that the text isn't replaced with the
		   next key press */
                pos = gtk_editable_get_position(GTK_EDITABLE(entry));
		gtk_widget_grab_focus(GTK_WIDGET(entry));
		gtk_editable_select_region(GTK_EDITABLE(entry), pos, pos);
	}

	return FALSE;
}

static gboolean event_switch_page(GtkNotebook *notebook, GtkNotebookPage *page,
				  gint page_num, Frame *frame)
{
	GtkWidget *widget;
	Tab *tab;

	widget = gtk_notebook_get_nth_page(notebook, page_num);
	tab = g_object_get_data(G_OBJECT(widget), "irssi tab");

	frame->active_tab = tab;
	window_set_active(tab->active_win);
	return FALSE;
}

Frame *gui_frame_new(void)
{
	GtkWidget *window, *vbox, *notebook, *statusbar;
	Frame *frame;

	frame = g_new0(Frame, 1);
	frame->refcount = 1;

	/* create the window */
	frame->widget = window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy",
			 G_CALLBACK(event_destroy), frame);
	g_signal_connect(G_OBJECT(window), "focus_in_event",
			 G_CALLBACK(event_focus), frame);
	g_signal_connect(GTK_OBJECT(window), "key_press_event",
			 G_CALLBACK(event_key_press), frame);
	gtk_widget_set_usize(window, 600, 400);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	notebook = gtk_notebook_new();
	g_signal_connect(GTK_OBJECT(notebook), "switch_page",
			 G_CALLBACK(event_switch_page), frame);
	frame->notebook = GTK_NOTEBOOK(notebook);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

	/* entry */
	frame->entry = gui_entry_new(frame);
	gtk_box_pack_start(GTK_BOX(vbox), frame->entry->widget, FALSE, FALSE, 0);

	/* statusbar */
	statusbar = gtk_statusbar_new();
	frame->statusbar = GTK_STATUSBAR(statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

	gtk_widget_show_all(window);
	gtk_widget_grab_focus(frame->entry->widget);

	frames = g_slist_prepend(frames, frame);

	signal_emit("gui frame created", 1, frame);
	return frame;
}

void gui_frame_ref(Frame *frame)
{
	frame->refcount++;
}

void gui_frame_unref(Frame *frame)
{
	if (--frame->refcount > 0)
		return;

	g_free(frame);
}

Tab *gui_frame_new_tab(Frame *frame)
{
	Tab *tab;

	tab = gui_tab_new(frame);

	g_object_set_data(G_OBJECT(tab->widget), "irssi tab", tab);
	gtk_notebook_append_page(frame->notebook, tab->widget,
				 GTK_WIDGET(tab->label));
	return tab;
}

void gui_frame_set_active(Frame *frame)
{
	active_frame = frame;
	if (frame != NULL && frame->active_tab != NULL)
		window_set_active(frame->active_tab->active_win);
}

void gui_frame_set_active_window(Frame *frame, Window *window)
{
	gui_entry_set_window(frame->entry, window);
}

void gui_frame_set_active_tab(Tab *tab)
{
	int page;

	page = gtk_notebook_page_num(tab->frame->notebook, tab->widget);
	gtk_notebook_set_current_page(tab->frame->notebook, page);
}
