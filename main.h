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

#ifndef MAIN_H
#define MAIN_H

#include "list.h"

enum mode{
 TEXT_SELECTION,
 TEXT_INPUT,
 TEXT_HIGHLIGHT,
 ERASE_TEXT_HIGHLIGHT,
 TEXT_SEARCH_NEXT,
 TEXT_SEARCH_PREV,
 PAGE_CHANGE,
 ZOOM_IN,
 ZOOM_OUT,
 ZOOM_WIDTH,
 ZOOM_HEIGHT,
 DUAL_PAGE
};

enum mode mode, pre_mode;

#endif /* MAIN_H */