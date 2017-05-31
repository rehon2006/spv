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

#ifndef SEARCH_H
#define SEARCH_H

#include "main.h"
#include "gui.h"
#include "page.h"

GList *find_ptr, *find_ptr_head;

void find_prev( GtkWidget *findbar );

void find_next( GtkWidget *findbar );

void search_N( GtkWidget *findbar );

void search_n( GtkWidget *findbar );

void search_slash( void );

void find_text_cb( GtkWidget *widget, gpointer user_data );

void find_exit_cb( GtkWidget *widget, gpointer user_data );

#endif /* SEARCH_H */