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

#ifndef ZOOM_H
#define ZOOM_H

#include "gui.h"

#include "main.h"

void zoom_in(void);
void zoom_in_cb(GtkWidget* widget, gpointer data);

void zoom_out(void);
void zoom_out_cb(GtkWidget* widget, gpointer data);

void zoom_width(void);
void zoom_width_cb(GtkWidget* widget, gpointer data);

void zoom_height(void);
void zoom_height_cb(GtkWidget* widget, gpointer data);

#endif /* ZOOM_H */
