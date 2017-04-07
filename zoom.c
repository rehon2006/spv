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

#include "list.h"
#include "zoom.h"

void
zoom_in(void) {
 
 if (zoom_factor != 5.0 && zoom_factor <= 5.0){

  if(selection_region){
   cairo_region_destroy(selection_region);
   selection_region = NULL;
  }

  
  if(last_region || selection_region ){
   
   cairo_region_t *invert_region;
   
   invert_region = cairo_region_copy(selection_region);
   if(mode == TEXT_SELECTION){
    invertRegion(invert_region);
    if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
     invertRegion(invert_region);
    }
    pre_mode = TEXT_SELECTION;

   }
   else if( mode == TEXT_HIGHLIGHT ){

    invert_highlight_Region(invert_region, 1);

    if(invert_region)
     cairo_region_destroy(invert_region);

   }

   gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
  }


  zoom_factor = zoom_factor + 1.0;
  
  gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+30, 680);

  GdkPixbuf *temp_pixbuf;
  temp_pixbuf = pixbuf;

  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

  cairo_surface_t *s;
  
  s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

  cairo_t* cr;
  cr = cairo_create(s);

  cairo_scale(cr, zoom_factor, zoom_factor);
 
  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   if(current_page_num > page_num-1) current_page_num = page_num-1;
   page = poppler_document_get_page(doc, current_page_num);
  }
  
  poppler_page_render(page, cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
  cairo_set_source_rgb (cr, 1., 1., 1.);

  cairo_paint (cr);

  cairo_destroy (cr);

  pixbuf = gdk_pixbuf_get_from_surface(s, 0, 0, width, height);
  
  if(temp_pixbuf != NULL){
   g_object_unref(temp_pixbuf);
  }

  cairo_surface_destroy (s);

  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
  

  g_object_unref (G_OBJECT (page));
  page = NULL;

    
  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
   
  gtk_layout_set_size (GTK_LAYOUT(layout), width+30, height+1);
 

  GList *list, *iter;
  list = gtk_container_get_children (GTK_CONTAINER (layout));

  for (iter = list; iter; iter = iter->next){

   GtkWidget *a = GTK_WIDGET (iter->data);
   if (gtk_widget_get_visible (a) && ! GTK_IS_CONTAINER(a)){

    gint wx, wy;
    gtk_widget_translate_coordinates(a, gtk_widget_get_toplevel(a), 0, 0,  &wx, &wy);
     
    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                            gtk_label_get_text(GTK_LABEL(a)));
    
    gtk_label_set_markup(GTK_LABEL(a), markup);
    g_free(markup);

    GtkAllocation al;
    gtk_widget_get_allocation (a, &al);
    
    gtk_layout_move(GTK_LAYOUT(layout),
                    a,
                    (gint)(al.x*zoom_factor/(zoom_factor-1.0))+width_offset,
                    (gint)(al.y*zoom_factor/(zoom_factor-1.0))
                    );
    
   }
  }
 
  g_list_free (list);
  
  struct list_head *tmp;

  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
  
   if( tmp1->page_num == current_page_num + 1 ){
   
    text_highlight_release((int)(tmp1->x*zoom_factor), 
                           (int)(tmp1->y*zoom_factor), 
                           (int)((tmp1->x+tmp1->width)*zoom_factor), 
                           (int)((tmp1->y+tmp1->height)*zoom_factor), 
                           tmp1->color_name);
   
   }

  } 

 
  if( find_ptr ){
   invert_search_region();
  }
 

  if(mode != ZOOM_IN){
   if(mode != ZOOM_OUT)
    pre_mode = mode;
   mode = ZOOM_IN;
  }
 
 }// end of if (zoom_factor != 5.0 && zoom_factor <= 5.0)

}

void
zoom_in_cb(GtkWidget* widget, gpointer data) {

 zoom_in();

}

void
zoom_width(void) {


 if(selection_region){
   cairo_region_destroy(selection_region);
   selection_region = NULL;
 }


 if(last_region || selection_region ){
   
  cairo_region_t *invert_region;
   
  invert_region = cairo_region_copy(selection_region);
  if(mode == TEXT_SELECTION){

   invertRegion(invert_region);
    
   if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
    invertRegion(invert_region);
   }
   pre_mode = TEXT_SELECTION;

  }
  else if( mode == TEXT_HIGHLIGHT ){
    
   invert_highlight_Region(invert_region, 1);

   if(invert_region)
    cairo_region_destroy(invert_region);
  }
   
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }

 GtkAdjustment *hadj, *vadj;

 hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
 vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));

 double w_width = gtk_adjustment_get_page_size(hadj);

 GdkScreen *screen = gdk_screen_get_default ();
 
 gint win_width, win_height;
 gtk_window_get_size(GTK_WINDOW(win), &win_width, &win_height);

 double width_zf = (double) win_width/page_width;
 
 width_zf = (int)(width_zf+0.5);
 
 if(width_zf == zoom_factor)
  return;
 
 if(width_zf > 5.0)
  width_zf = 5.0;

 if( (int)(page_width*width_zf) > gdk_screen_get_width(screen) - 10 )
  width_zf = width_zf - 1.0;
  
 double pre_zf = zoom_factor;
 
 zoom_factor = width_zf;

 GdkPixbuf *temp_pixbuf;
 temp_pixbuf = pixbuf;

 gint width, height;

 width = (gint)((page_width*width_zf)+0.5);
 height = (gint)((page_height*width_zf)+0.5);

 cairo_surface_t *temp_s;
 temp_s = s;

 cairo_surface_t *s;
 s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cairo_t* cr;
 cr = cairo_create(s);

 cairo_scale(cr, width_zf, width_zf);

 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  if(current_page_num > page_num-1) current_page_num = page_num-1;
  page = poppler_document_get_page(doc, current_page_num);
 }

 poppler_page_render(page, cr);

 cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
 cairo_set_source_rgb (cr, 1., 1., 1.);

 cairo_paint (cr);

 cairo_destroy (cr);

 pixbuf = gdk_pixbuf_get_from_surface(s, 0, 0, width, height);
 
 if(temp_pixbuf != NULL){
  g_object_unref(temp_pixbuf);
 }

 cairo_surface_destroy (s);

 gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);

 g_object_unref (G_OBJECT (page));
 page = NULL;

 GtkAllocation w_alloc;
 gtk_widget_get_allocation (event_box, &w_alloc);
 
 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);

  GList *list, *iter;
  list = gtk_container_get_children (GTK_CONTAINER (layout));

  for (iter = list; iter; iter = iter->next){

   GtkWidget *a = GTK_WIDGET (iter->data);
   if (gtk_widget_get_visible (a) && ! GTK_IS_CONTAINER(a)){

    gint wx, wy;
    gtk_widget_translate_coordinates(a, gtk_widget_get_toplevel(a), 0, 0,  &wx, &wy);

    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), gtk_label_get_text(GTK_LABEL(a)));
    
    gtk_label_set_markup(GTK_LABEL(a), markup);
    g_free(markup);

    GtkAllocation al;
    gtk_widget_get_allocation (a, &al);

    if( pre_zf < zoom_factor ){
     //zoom in
     gtk_layout_move(GTK_LAYOUT(layout),
                    a,
                    (gint)(al.x*zoom_factor/(zoom_factor-1.0)),
                    (gint)(al.y*zoom_factor/(zoom_factor-1.0))
                    );
    }
    else {
     //zoom out
     gtk_layout_move(GTK_LAYOUT(layout),
                    a,
                    (gint)(al.x*zoom_factor/(zoom_factor+1.0)),
                    (gint)(al.y*zoom_factor/(zoom_factor+1.0))
                    );
 
    }

   }
  }
  g_list_free (list);

  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);

   if( tmp1->page_num == current_page_num + 1 ){
    
    text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name);
    
   }

  }

  if( find_ptr ){
   invert_search_region();
  }


}

void
zoom_width_cb(GtkWidget* widget, gpointer data) {
 
 zoom_width();

}

void
zoom_out() {

 if (zoom_factor != 1.0 && zoom_factor >= 1.0){

  if(selection_region){
   cairo_region_destroy(selection_region);
   selection_region = NULL;
  }

  if(last_region || selection_region ){
   
   cairo_region_t *invert_region;
   
   invert_region = cairo_region_copy(selection_region);
   if(mode == TEXT_SELECTION){

    invertRegion(invert_region);
   
    if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
     invertRegion(invert_region);
    }
    pre_mode = TEXT_SELECTION;

   }
   else if( mode == TEXT_HIGHLIGHT ){
   
    invert_highlight_Region(invert_region, 1);

    if(invert_region)
     cairo_region_destroy(invert_region);
   }
   
   gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
  }

  zoom_factor = zoom_factor - 1.0;

  if(zoom_factor == 2.0)
   gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+35, 680);
  else
   gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor), 680);
  

  GdkPixbuf *temp_pixbuf;
  temp_pixbuf = pixbuf;

  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

  cairo_surface_t *s;
  
  s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  
  cairo_t* cr;
  cr = cairo_create(s);

  cairo_scale(cr, zoom_factor, zoom_factor);

  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   if(current_page_num > page_num-1) current_page_num = page_num-1;
   page = poppler_document_get_page(doc, current_page_num);
  }

  poppler_page_render(page, cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
  cairo_set_source_rgb (cr, 1., 1., 1.);

  cairo_paint (cr);

  cairo_destroy (cr);

  pixbuf = gdk_pixbuf_get_from_surface(s, 0, 0, width, height);

  if(temp_pixbuf != NULL){
   g_object_unref(temp_pixbuf);
  }

  cairo_surface_destroy (s);

  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);

  g_object_unref (G_OBJECT (page));
  page = NULL;

  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
  
  gtk_layout_set_size (GTK_LAYOUT(layout), width, height+1);

  GList *list, *iter;
  list = gtk_container_get_children (GTK_CONTAINER (layout));

  for (iter = list; iter; iter = iter->next){

   GtkWidget *a = GTK_WIDGET (iter->data);
   if (gtk_widget_get_visible (a) && ! GTK_IS_CONTAINER(a)){

    gint wx, wy;
    gtk_widget_translate_coordinates(a, gtk_widget_get_toplevel(a), 0, 0,  &wx, &wy);

    GtkAllocation al;
    gtk_widget_get_allocation (a, &al);

    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), gtk_label_get_text(GTK_LABEL(a)));
    
    gtk_label_set_markup(GTK_LABEL(a), markup);
    g_free(markup);

    gtk_layout_move(GTK_LAYOUT(layout),
                    a,
                    (gint)(al.x*zoom_factor/(zoom_factor+1.0))+width_offset,
                    (gint)(al.y*zoom_factor/(zoom_factor+1.0))
                    );
   
   }
  }
  g_list_free (list);

  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   if( tmp1->page_num == current_page_num + 1 ){
    
    text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name);
    
   }

  }

  if( find_ptr ){
   invert_search_region();
  }
  
  if( mode != ZOOM_OUT ){
   if( mode != ZOOM_IN )
    pre_mode = mode;
   mode = ZOOM_OUT;
  }

 } // end of if (zoom_factor != 1.0 && zoom_factor >= 1.0)

}


void
zoom_out_cb(GtkWidget* widget, gpointer data) {

 zoom_out();

}
