#ifndef __GUI_TAB_H
#define __GUI_TAB_H

struct _Tab {
	Frame *frame;

	GtkWidget *widget;
	GtkWidget *tab_label_widget;
	GtkLabel *label;
	GtkBox *tab_label_box;

	GtkPaned *first_paned, *last_paned;
	GList *panes;

	NicklistView *nicklist;
	Window *active_win;
	int data_level;

	unsigned int destroying:1;
};

struct _TabPane {
	Tab *tab;

	GtkWidget *widget, *focus_widget;
	GtkLabel *label;
	GtkBox *box, *titlebox;
	WindowView *view;

	unsigned int focused:1;
};

Tab *gui_tab_new(Frame *frame);

GtkPaned *gui_tab_add_paned(Tab *tab);
TabPane *gui_tab_pane_new(Tab *tab);
void gui_tab_pack_panes(Tab *tab);

Tab *gui_tab_get_page(Frame *frame, int page);

void gui_tab_set_active(Tab *tab);
void gui_tab_set_active_window(Tab *tab, Window *window);
void gui_tab_set_active_window_item(Tab *tab, Window *window);
void gui_tab_update_active_window(Tab *tab);

void gui_reset_tab_labels(Frame *frame);
void gui_tab_set_focus_colors(GtkWidget *widget, int focused);

void gui_tabs_init(void);
void gui_tabs_deinit(void);

#endif
