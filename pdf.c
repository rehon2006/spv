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

void init_pdf( char *path ){
 
 GError* err = NULL;
 
 page_changed = FALSE;
 
 zoom_factor = 1.0;
 
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
    
 gboolean success;
    
 success = poppler_page_get_text_layout(page, &areas, &n_areas); 
 
 if(success) areas_ptr = areas;
 
 poppler_page_get_size(page, &page_width, &page_height); 
 
 gint width, height;
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);
  
 s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(s);
    
 cairo_save(cr);
 cairo_scale(cr, zoom_factor, zoom_factor);

 poppler_page_render(page, cr); 

 cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
 cairo_set_source_rgb (cr, 1., 1., 1.);
  
 cairo_paint (cr);

 cairo_destroy (cr);

 pixbuf = gdk_pixbuf_get_from_surface(s, 0, 0, width, height);
    
 cairo_surface_destroy (s);  
 
 m_PageImage = gtk_image_new ();
 gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 
}

cairo_region_t *
create_region_from_poppler_region (GList *region, gdouble scale){

 GList *l;
 cairo_region_t *retval;
 retval = cairo_region_create ();

 for (l = region; l; l = g_list_next (l)) {

  PopplerRectangle   *rectangle;
  cairo_rectangle_int_t rect;
  rectangle = (PopplerRectangle *)l->data;
  rect.x = (gint) ((rectangle->x1 * scale) + 0.5);
  rect.y = (gint) ((rectangle->y1 * scale) + 0.5);
  rect.width  = (gint) (((rectangle->x2 - rectangle->x1) * scale) + 0.5);
		rect.height = (gint) (((rectangle->y2 - rectangle->y1) * scale) + 0.5);
  cairo_region_union_rectangle (retval, &rect);
  poppler_rectangle_free (rectangle);
 }

 return retval;

}

void
getPagePosition (gint widgetX, gint widgetY, gint *pageX, gint *pageY){

 gint horizontalPadding = PAGE_VIEW_PADDING;
 gint verticalPadding = PAGE_VIEW_PADDING;
 GdkPixbuf *page = gtk_image_get_pixbuf (GTK_IMAGE (m_PageImage));

 if ( NULL != page ){

  GtkAllocation *alloc = g_new(GtkAllocation,1);
  gtk_widget_get_allocation(m_PageImage, alloc);

  horizontalPadding =
     (alloc->width - gdk_pixbuf_get_width (page)) / 2;
  verticalPadding =
     (alloc->height - gdk_pixbuf_get_height (page)) / 2;

 }
    
 double h_scroll, v_scroll; 
 GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment (
         GTK_SCROLLED_WINDOW (scrolled_window));
 h_scroll =  gtk_adjustment_get_value (hAdjustment);

 GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment (
         GTK_SCROLLED_WINDOW (scrolled_window));
 v_scroll =  gtk_adjustment_get_value (vAdjustment);


 GtkAllocation w_alloc;
 gtk_widget_get_allocation (event_box, &w_alloc);

 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
   
 *pageX = widgetX - horizontalPadding + (gint)h_scroll - width_offset ;

 #if GTK_CHECK_VERSION(3,12,3)
  *pageY = widgetY - verticalPadding + (gint)v_scroll;
 #else
  if(TOOL_BAR_VISIABLE){
   *pageY = widgetY - verticalPadding + (gint)v_scroll - 70;
  }else{
   *pageY = widgetY - verticalPadding + (gint)v_scroll - 35;
  }
 #endif
    
}
