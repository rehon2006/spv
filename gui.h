/*
 * Copyright (C) 2017 rehon2006, rehon2006@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_H
#define GUI_H

#include "list.h"
#include "pdf.h"
#include "main.h"
#include "zoom.h"
#include "highlight.h"
#include "note.h"
#include "search.h"
#include "page.h"

GtkWidget* win;
GtkWidget *scrolled_window;

GtkWidget *toolbar;
GtkWidget *comboBox;

GtkToolItem *highlight_color;
GdkPixbuf *hc_pixbuf;

gboolean TOOL_BAR_VISIBLE;

GtkTextBuffer *buffer;

GtkWidget *layout;

GtkWidget *m_PageImage;
GtkWidget *lm_PageImage;

GtkWidget *comment;

GtkWidget *levent_box;
GtkWidget *event_box;

GtkWidget *selection_widget;

GtkClipboard *clipboard;

GdkCursor *sel_cursor;

int pressed; 

int layout_move;

gchar* page_change_str;

guint pre_keyval;

gboolean cursor_enable;

int newline_y;

double pre_motion_x;

int inverted;

GtkAllocation child_alloc;

double start_x, start_y;

gint pre_sw_width;
gint pre_eb_width;

GString *selected_text;

int width_offset, height_offset;

gint have_selection;

double lstart_x, lstart_y;

int left_right;// 0 for left page, 1 for right page

gint layout_motion_handler_id;
gint layout_release_handler_id;

gint event_box_motion_handler_id;
gint event_box_release_handler_id;

gint levent_box_motion_handler_id;
gint levent_box_release_handler_id;

gint sw_motion_handler_id;

int scroll_count;
guint scroll_time;
int scroll_zoom;

GtkWidget *vbox;

GtkWidget *findbar;
GtkWidget *findtext;
    
GtkWidget *menubar;
GtkWidget *fileMenu;
GtkWidget *editMenu;
GtkWidget *viewMenu;
   
GtkWidget *goMenu;
GtkWidget *modeMenu;
GtkWidget *noteMenu;

GtkWidget *fileMi;
GtkWidget *quitMi;

GtkWidget *editMi;
GtkWidget *copyMi;
GtkWidget *findMi;

GtkWidget *viewMi;
   
GtkWidget *zoominMi;
GtkWidget *zoomoutMi;
    
GtkWidget *zoomwidthMi;
GtkWidget *zoomheightMi;

GtkWidget *hide_toolbarMi;
GtkWidget *change_background_colorMi;
GtkWidget *dual_pageMi;
GtkWidget *full_screenMi;
GtkWidget *inverted_colorMi;

GtkWidget *goMi;
GtkWidget *nextpageMi;
GtkWidget *prepageMi;

GtkWidget *modeMi;
    
GtkWidget *normalMi;
GtkWidget *text_highlightMi;
GtkWidget *erase_text_highlightMi;

GtkWidget *noteMi;

GtkWidget *add_commentMi;
GtkWidget *save_noteMi;
GtkWidget *save_commentMi;

GtkWidget *hc_button;
GdkPixbuf *hc_pixbuf;
GtkWidget *hc_button_Image;

GtkWidget *zoom_in_button;
GtkWidget *zoom_out_button;
GtkWidget *zoom_width_button;
GtkWidget *prev_page_button;
GtkWidget *next_page_button;
GtkWidget *text_selection_mode_button;
GtkWidget *text_highlight_mode_button;
GtkWidget *erase_text_highlight_mode_button;
GtkWidget *add_comment_button;
GtkWidget *save_note_button;
GtkToolItem *save_note_item;

GtkWidget *find_button;
GtkWidget *find_exit_button;

GtkWidget *find_hbox;
GtkWidget *findNext_button;
GtkWidget *findPrev_button;
gboolean KEY_BUTTON_SEARCH; //true for keyboard search, false for button search

GtkAccelGroup *accel_group;

int sa_count;

struct list_head NOTE_HEAD, HR_HEAD;

void
event_box_press( GdkEventButton *event );

void
layout_press( GdkEventButton *event );

gboolean
layout_button_release_event (GtkWidget      *widget,
                             GdkEventButton *event,
                             gpointer        user_data);
                                                        
gboolean
layout_motion_notify_event (GtkWidget      *widget,
                            GdkEventMotion *event,
                            gpointer        user_data);

gboolean
inside_obj( GdkEventButton *event );

GtkWidget * 
get_layout_child(GtkWidget      *layout, 
                 GdkEventButton *event);
                
void
sw_button_press_cb( GtkWidget* widget, GdkEventButton *event, gpointer data );

void
sw_button_motion_cb( GtkWidget* widget, GdkEventMotion *event, gpointer data );

void
event_box_release_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data );

void
toggle_hide_toolbar(void);

void
on_destroy(GtkWidget* widget, gpointer data);

static gboolean
scrolled_window_keypress_cb(GtkWidget *widget, GdkEventKey *event, gpointer data);

static gboolean
scrolled_window_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer data);

gboolean
touchpad_cb(GtkWidget *widget, GdkEvent *event, gpointer data);

static void
text_highlight_mode_cb(GtkWidget* widget, gpointer data);

static void
text_selection_mode_cb(GtkWidget* widget, gpointer data);

static void
erase_text_highlight_mode_cb(GtkWidget* widget, gpointer data);

static void
levent_box_motion_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data );

static void
event_box_motion_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data );

void
on_findtext_key_release(GtkWidget *findtext, GdkEventKey *event, gpointer user_data);

gboolean time_handler(GtkWidget *widget);

static void
size_allocate_cb( GtkWidget *win, GtkAllocation *allocation, gpointer data);

void
textbuffer_changed_cb(GtkTextBuffer *textbuffer, gpointer user_data);

void
text_highlight_mode_change(void);

void
text_selection_mode_change(void);

void
erase_text_highlight_mode_change(void);

void 
copy_text(void);

void init_gui(void);

void full_screen_cb(void);

#endif /* GUI_H */
