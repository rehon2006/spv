/*
 * Copyright (C) 2017-2020 rehon2006, rehon2006@gmail.com
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
 gint x,y,width,height;
 gint page_num;
 struct cm_property* property;
 struct list_head list;
}*current_cm;

struct bp_comment{
 struct note *comment;
 struct bp_comment* next;
};

struct blank_page{
 int page_num;
 struct bp_comment *comment;
 struct blank_page *next, *prev;
}*current_bp, *rcurrent_bp;

struct note_cache{
 struct list_head CM_HEAD;
 struct list_head HR_HEAD;
 struct blank_page *BP_HEAD, *BP_TAIL;
 int page_num;
 struct note_cache *next, *prev;
}*current_nc, *rcurrent_nc, *prev_nc;//current note cache

struct page_entry{
 int p_page;
 struct note_cache *curr_nc;
 struct blank_page *curr_bp;
 struct page_entry *next;
 struct page_entry *prev;
}*PAGES, *current_pe, *rcurrent_pe, *end_pe;

//page comment
struct pc_entry{
 int page_num;
 char *str;
 struct pc_entry *next;
}PC_HEAD,*current_pc;

int comment_click;

gint bp_cpn; // current_page_num for blank page

gboolean draw_cursor;
gboolean invert_color;
gboolean blank_page;

gboolean G_INVERT_COLORS;

PangoFontDescription *G_CM_FONT_DESC;
PangoFontDescription *P_CM_FONT_DESC;

struct color_table *G_HR_COLOR, *P_HR_COLOR;
struct color_table *G_CM_FONT_COLOR, *G_CM_BG_COLOR;
struct color_table *P_CM_FONT_COLOR, *P_CM_BG_COLOR;

struct note_cache *note_cache; //how about renaming it as nc_head?

struct note *font_changed_italic;

gboolean font_changed;

void 
save_note (void);

void 
add_comment (void);

void
insert_bp_before_cb (GtkWidget* widget, gpointer data);

void
insert_bp_after_cb (GtkWidget* widget, gpointer data);

void
del_blank_page_cb (GtkWidget* widget, gpointer data);

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

void
add_page_comment(void);

void
save_page_comment(void);

struct color_table* add_ct_entry(char *ce);

void
save_as(void);

struct note_cache* 
get_current_nc( struct note_cache *curr_nc );

struct note_cache* 
add_nc_entry( struct note_cache *curr_nc);

void insert_bp_cb (gboolean before_after);

#endif /* NOTE_H */