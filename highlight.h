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

#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "list.h"
 
struct highlight_region{
 gint x,y;
 gint width, height;
 struct color_table* ce;
 gint page_num;
 struct list_head list;
};    

void 
invertArea (gint x1, gint y1, gint x2, gint y2, int option);

void 
invert_search_region(void);

void 
save_highlight(GtkWidget *widget, struct list_head *hr_HEAD, double zoom_factor);

void 
erase_highlight(GtkWidget *widget, struct list_head *hr_HEAD, double zoom_factor);

#endif /* HIGHLIGHT_H */
