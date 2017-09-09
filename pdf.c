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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <poppler.h>
#include <assert.h>
#include <string.h>

#if __linux__
#include <linux/limits.h>
#elif __APPLE__
#include <sys/param.h>
#endif

#include "list.h"
#include "pdf.h"

void get_newline( PopplerPage* page ){

 PopplerRectangle * areas_tmp, *nt_ptr;
 areas_tmp = areas;
 nt_ptr = areas_tmp++;
 
 int count = 0;
 
 while( areas_tmp <= areas+n_areas-1 ){
  
  if( (*areas_tmp).y2 == (*areas_tmp).y1 ){
  
   PopplerRectangle tmp_rect;
   tmp_rect.x1 = (*(areas_tmp-1)).x1;
   tmp_rect.y1 = (*(areas_tmp-1)).y1;
   tmp_rect.x2 = (*(areas_tmp-1)).x2;
   tmp_rect.y2 = (*(areas_tmp-1)).y2;
   
   if( (*(areas_tmp+1)).y1 != (*(areas_tmp-1)).y1 && (*(areas_tmp+1)).y2 != (*(areas_tmp-1)).y2 ){
    areas_line[count] = areas_tmp;
    count++;
   }
   
  }
  areas_tmp++;
  nt_ptr++;
 
 }
 
 int i;
 for( i = count; i < 50; i++ ){
  areas_line[i] = NULL;
 }
 
}

void init_pdf( char *path ){
 
 draw_count = 0;
 
 GError* err = NULL;
 
 dual_page_mode = FALSE;
 
 PDF_BACKGROUND_COLOR_CHANGED = FALSE;
 
 background_color[0] = 199/255.0;
 background_color[1] = 237/255.0;
 background_color[2] = 204/255.0;
 
 GFile *file= g_file_new_for_commandline_arg(path);
 
 doc = poppler_document_new_from_gfile(file, NULL, NULL, &err);
 
 if (!doc) {
  
  g_object_unref(err);
  
  exit(2);
 }
 
 char *pch, *ppch;
 
 #ifdef _WIN32
 pch = strrchr(path, '\\');
 ppch = strchr(path, '\\');
 #else
 pch = strrchr(path, '/');
 ppch = strchr(path, '/');
 #endif
 
 *(pch) = '\0';

 file_path = (gchar *)malloc(strlen(ppch)+1);
 
 strcpy(file_path, ppch);
 
 #ifdef _WIN32
 *(pch) = '\\';
 #else
 *(pch) = '/';
 #endif
 
 file_name = (gchar *)malloc(strlen(pch+1)+1);
  
 strcpy(file_name, pch+1);
 
 page = poppler_document_get_page(doc, current_page_num);
 
 if(!page) {
  printf("Could not open first page of document\n");
  g_object_unref(doc);
  exit(3);
 }
 
 areas = areas_ptr = NULL;
 
 selection_surface = NULL;
 
 gboolean success;
    
 success = poppler_page_get_text_layout(page, &areas, &n_areas); 
 
 if(success) areas_ptr = areas;
 
 //get_newline(page);
 line_count = 0;
 line_offset = 0;
 
 press_rl = 1;
 
 poppler_page_get_size(page, &page_width, &page_height); 
 
 GdkScreen *screen = gdk_screen_get_default ();
 
 if(page_height >= gdk_screen_get_height(screen))
  zoom_factor = 0.5;
 else
  zoom_factor = 1.0;
 
 gint width, height;
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);

 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(surface);
 
 lsurface = NULL;
 
 cairo_save(cr);
 
 cairo_scale(cr, zoom_factor, zoom_factor);

 poppler_page_render(page, cr); 
 
 cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
 
 if(PDF_BACKGROUND_COLOR_CHANGED)
  cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
 else
  cairo_set_source_rgb (cr, 1., 1., 1.);
  
 cairo_paint (cr);
 
 PopplerRectangle area = { 0, 0, 0, 0 };
 
 poppler_page_get_size (page, &area.x2, &area.y2);
 da_selection_region = poppler_page_get_selected_region (page,
                                                         1.0,
                                                         POPPLER_SELECTION_GLYPH,
                                                         &area);
 
 lda_selection_region = NULL;
 
 //for text selection
 da_glyph_color.red = CLAMP ((guint) (1 * 65535), 0, 65535);
 da_glyph_color.green = CLAMP ((guint) (1* 65535), 0, 65535);
 da_glyph_color.blue = CLAMP ((guint) (1* 65535), 0, 65535);
 
 da_background_color.red = CLAMP ((guint) (0* 65535), 0, 65535);
 da_background_color.green = CLAMP ((guint) (0 * 65535), 0, 65535);
 da_background_color.blue = CLAMP ((guint) (0 * 65535), 0, 65535);
 //for text selection
 
}
