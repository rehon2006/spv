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

#ifndef PAGE_H
#define PAGE_H

#include "main.h"
#include "gui.h"
#include "pdf.h"

#include "list.h"

gboolean page_changed;

void page_change(void);

void prev_page(void);
void prev_page_cb(GtkWidget* w, gpointer data);

void next_page(void);
void next_page_cb(GtkWidget* w, gpointer data);

#endif /* PAGE_H */
