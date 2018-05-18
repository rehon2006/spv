/*
 * Copyright (C) 2017-2018 rehon2006, rehon2006@gmail.com
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
#include "main.h"

GtkWidget* win;
GtkWidget *scrolled_window;

GtkWidget *toolbar;

GtkToolItem *highlight_color;

gboolean TOOL_BAR_VISIBLE;

GtkTextBuffer *buffer;

GtkWidget *layout;

GtkWidget *draw_area;
GtkWidget *ldraw_area;

gint da_width, da_height;

//for showing cursor when editing a comment
gint cursor_pos, pre_cursor_pos;
gint bindex;
GtkTextIter cur_cursor, pre_cursor;
gint pre_str_pos, str_pos;
//for showing cursor when editing a comment

GtkClipboard *clipboard;

GdkCursorType da_cursor;

gchar* page_change_str;

guint pre_keyval;

guint screen_height, screen_width;

gboolean cursor_enable;

double start_x, start_y;
double stop_x, stop_y;

gint pre_sw_width, pre_sw_height;
gint pre_da_width;

GString *selected_text;

int width_offset, height_offset;
int awidth_offset, aheight_offset;

GtkAllocation daa, ldaa;

double lstart_x, lstart_y;

gboolean full_screen;

int left_right;// 0 for left page, 1 for right page

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
GtkWidget *prefMi;

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
GtkWidget *cursor_modeMi;
GtkWidget *continuous_modeMi;

GtkWidget *goMi;
GtkWidget *nextpageMi;
GtkWidget *prepageMi;

GtkWidget *modeMi;
    
GtkWidget *text_selectionMi;
GtkWidget *text_highlightMi;
GtkWidget *erase_text_highlightMi;

GtkWidget *noteMi;

GtkWidget *add_commentMi;
GtkWidget *save_noteMi;
GtkWidget *save_commentMi;

GtkWidget *hc_button;
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
                     
void
toggle_hide_toolbar(void);

void
on_destroy(GtkWidget* widget, gpointer data);

static gboolean
scrolled_window_keypress_cb(GtkWidget *widget, GdkEventKey *event, gpointer data);

static gboolean
da_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer data);

gboolean
draw_area_draw( GtkWidget *widget, cairo_t *cr, gpointer data );

gboolean
touchpad_cb(GtkWidget *widget, GdkEvent *event, gpointer data);

static void
text_highlight_mode_cb(GtkWidget* widget, gpointer data);

static void
text_selection_mode_cb(GtkWidget* widget, gpointer data);

static void
erase_text_highlight_mode_cb(GtkWidget* widget, gpointer data);

void
on_findtext_key_release(GtkWidget *findtext, GdkEventKey *event, gpointer user_data);

gboolean 
time_handler(GtkWidget *widget);

static void
size_allocate_cb( GtkWidget *win, GtkAllocation *allocation, gpointer data);

void
textbuffer_changed_cb(GtkTextBuffer *textbuffer, gpointer user_data);

gboolean
cursor_position_changed_cb(GtkTextBuffer *textbuffer);

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
