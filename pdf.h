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

#ifndef PDF_H
#define PDF_H

#include <poppler.h>
#include "gui.h"

PopplerDocument* doc;
PopplerPage* page, *lpage;

guint n_areas;
guint line_offset;

gchar *file_path;
gchar *file_name;

PopplerRectangle * areas;
PopplerRectangle * areas_ptr;
PopplerRectangle *areas_line[50];
int line_count;

int word_not_found;
GList *find_ptr, *find_ptr_head;

int FONT_SIZE;

gboolean PDF_BACKGROUND_COLOR_CHANGED; // true for changing new color, false for color white
double background_color[3];

double zoom_factor;

cairo_region_t *last_region;
cairo_region_t *selection_region;

cairo_region_t *llast_region;
cairo_region_t *lselection_region;

cairo_t* cr;

double page_width;
double page_height;

GdkPixbuf *pixbuf;
GdkPixbuf *lpixbuf;

gint current_page_num;
gint find_current_page_num;

cairo_rectangle_int_t pre_rec; 
cairo_rectangle_int_t *hr; 
cairo_rectangle_int_t *ihr; 

cairo_region_t *
create_region_from_poppler_region (GList *region, gdouble scale);

void init_pdf(char *path);

void get_newline( PopplerPage* page );

#endif /* PDF_H */