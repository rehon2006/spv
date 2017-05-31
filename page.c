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

#include "page.h"

void page_change(void){

 GtkAdjustment *hadj, *vadj;

 hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   
 vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   
 gdouble hposition = gtk_adjustment_get_value (hadj);
 
 hposition = gtk_adjustment_get_value (hadj);
 
 GdkPixbuf *temp_pixbuf;
 temp_pixbuf = pixbuf;

 gint width, height;

 char page_str[100];
 
 if( mode == TEXT_SELECTION )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 else if ( mode == TEXT_HIGHLIGHT )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
 else if ( mode == ERASE_TEXT_HIGHLIGHT )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
 else
  sprintf(page_str, "%s page %d/%d",file_name, current_page_num+1, poppler_document_get_n_pages(doc));
 
 gtk_window_set_title(GTK_WINDOW(win), page_str);

 page_changed = TRUE;

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
   
  else if( mode == ERASE_TEXT_HIGHLIGHT ){
    
   invert_highlight_Region(invert_region, 1);
    
   if(invert_region)
    cairo_region_destroy(invert_region);
   
  }
   
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }

 page = poppler_document_get_page(doc, current_page_num);

 poppler_page_get_size(page, &page_width, &page_height); 
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);

 cairo_surface_t *surface;
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(surface);

 cairo_scale(cr, zoom_factor, zoom_factor);

 poppler_page_render(page, cr);

 cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
 
 if(PDF_BACKGROUND_COLOR_CHANGED)
  cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
 else
  cairo_set_source_rgb (cr, 1., 1., 1.);
 
 cairo_paint (cr);

 cairo_destroy (cr);

 pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);

 if(temp_pixbuf != NULL){
  g_object_unref(temp_pixbuf);
 }

 cairo_surface_destroy (surface); 
 
 gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height+1);
 
 struct list_head *tmp;

 list_for_each(tmp, &NOTE_HEAD){

  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  
  if( tmp1->page_num == current_page_num+1 ){
   const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
   char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
   gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
   g_free(markup);

   gtk_widget_show(tmp1->comment);
  }else
   gtk_widget_hide(tmp1->comment);

 } 

 list_for_each(tmp, &HR_HEAD){

  struct highlight_region *tmp1;
  tmp1= list_entry(tmp, struct highlight_region, list);

  if(tmp1->page_num ==  current_page_num +1 ){
   
   text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name);
   
  }

 }

 if(find_ptr){
  find_ptr = NULL;
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
 }
  
 g_object_unref (G_OBJECT (page));
 page = NULL;
}

void
next_page_cb(GtkWidget* widget, gpointer data) {

 next_page();

}

void
prev_page(void) {

 gint page_num = poppler_document_get_n_pages(doc);

 if(current_page_num > 0)
  current_page_num--;

 if (current_page_num == page_num)
  current_page_num--;

  if( current_page_num >= 0 ){
  
   GtkAdjustment *hadj, *vadj;

   hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   
   gtk_adjustment_set_value(hadj, gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj) );
  
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   page_change();
  }
 
}

void
prev_page_cb(GtkWidget* widget, gpointer data) {

 prev_page();

}

void
next_page(void) {

 gint page_num = poppler_document_get_n_pages(doc);

 if(current_page_num < page_num-1){
  
  current_page_num++;
  
  if( current_page_num < page_num ){

   GtkAdjustment *hadj, *vadj;

   hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
 
   gtk_adjustment_set_value(hadj, gtk_adjustment_get_page_size(hadj) - gtk_adjustment_get_upper(hadj));
   
   gtk_adjustment_set_value(vadj, gtk_adjustment_get_page_size(vadj) - gtk_adjustment_get_upper(vadj));
   
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
   page_change();
 
  }

 }
}