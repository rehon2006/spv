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

#ifndef PAGE_H
#define PAGE_H

#include "main.h"
#include "list.h"

struct note_cache;

void 
page_change(void);

void 
prev_page(void);
void 
prev_page_cb(GtkWidget* widget, gpointer data);

void 
next_page(void);
void 
next_page_cb(GtkWidget* widget, gpointer data);

void 
dual_page_cb(void);

void 
page_setup(cairo_surface_t **surface, cairo_region_t **da_selection_region, GtkWidget *draw_area, struct note_cache *current_nc);

#endif /* PAGE_H */
