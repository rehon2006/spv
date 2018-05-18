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

#ifndef NOTE_H
#define NOTE_H

#include "list.h"

struct color_table{
 GdkRGBA color;
 guint rc;
 struct color_table* next;
}*COL_TAB;

struct cm_property{
 PangoFontDescription *font_desc;
 struct color_table* font_color;
 struct color_table* bg_color;
};

struct note{
 GtkWidget *comment;
 char *str;
 gint x,y;
 gint page_num;
 struct cm_property* property;
 struct list_head list;
}*current_cm;

struct note_cache{
 struct list_head CM_HEAD;
 struct list_head HR_HEAD;
 int page_num;
 struct note_cache *next;
}*current_nc, *rcurrent_nc;//current note cache

int comment_click;

gboolean draw_cursor;
gboolean invert_color;

PangoFontDescription *G_CM_FONT_DESC;
PangoFontDescription *P_CM_FONT_DESC;

struct color_table *G_HR_COLOR, *P_HR_COLOR;
struct color_table *G_CM_FONT_COLOR, *G_CM_BG_COLOR;
struct color_table *P_CM_FONT_COLOR, *P_CM_BG_COLOR;

struct note_cache *note_cache; //how about renaming it as nc_head?

void 
save_note (void);

void 
add_comment (void);

void 
save_comment(void);

void 
init_note(void);

gboolean 
add_comment_cb (GtkWidget* widget, gpointer data);

void
save_note_cb (GtkWidget* widget, gpointer data);

void
save_comment_cb (GtkWidget* widget, gpointer data);

void 
display_comment(struct list_head *hr_HEAD);

struct color_table* add_ct_entry(char *ce);

#endif /* NOTE_H */