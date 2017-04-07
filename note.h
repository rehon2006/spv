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

#ifndef NOTE_H
#define NOTE_H

#include "list.h"
#include "gui.h"

#include "page.h"

void save_note (void);

void add_comment (void);

void save_comment(void);

void init_note( void );

void add_comment_cb (GtkWidget* w, gpointer data);

void
save_note_cb (GtkWidget* w, gpointer data);

void
save_comment_cb (GtkWidget* w, gpointer data);

#endif /* NOTE_H */