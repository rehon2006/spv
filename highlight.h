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

#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "gui.h"
#include "pdf.h"

struct highlight_region{
 gint x,y;
 gint width, height;
 char *color_name; // format #rrggbb
 gint page_num;
 struct list_head list;
};

guchar hc[3];     

void text_highlight(gint x1, gint y1, gint x2, gint y2, gint option);

void text_highlight_release(gint x1, gint y1, gint x2, gint y2, gchar* color_name, gint option);

void invertArea (gint x1, gint y1, gint x2, gint y2, int option);

void invertRegion (cairo_region_t* region, int option);

void invert_text_highlight(gint x1, gint y1, gint x2, gint y2, gint option);

void invert_highlight_Region (cairo_region_t* region, gint option);

void invert_search_region(void);

void
highlight_Region (cairo_region_t* region, int option);

#endif /* HIGHLIGHT_H */