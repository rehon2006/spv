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
  
  gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10+10, (int)(page_height*zoom_factor)+80);

  GdkPixbuf *temp_pixbuf;
  temp_pixbuf = pixbuf;

  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

  cairo_surface_t *surface;
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

  cairo_t* cr;
  cr = cairo_create(surface);

  cairo_scale(cr, zoom_factor, zoom_factor);
 
  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   
   if(current_page_num > page_num-1) 
    current_page_num = page_num-1;
    
   page = poppler_document_get_page(doc, current_page_num);
  }
  
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
  
  g_object_unref (G_OBJECT (page));
  page = NULL;

  gtk_layout_set_size (GTK_LAYOUT(layout), width+5, height+1);
 
  GList *list, *iter;
  list = gtk_container_get_children (GTK_CONTAINER (layout));

  for (iter = list; iter; iter = iter->next){

   GtkWidget *child = GTK_WIDGET (iter->data);
   
   if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child)){

    gint wx, wy;
    
    gtk_widget_translate_coordinates(child, gtk_widget_get_toplevel(child), 0, 0,  &wx, &wy);
     
    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                            gtk_label_get_text(GTK_LABEL(child)));
                                            
    gtk_label_set_markup(GTK_LABEL(child), markup);
    g_free(markup);

    GtkAllocation al;
    gtk_widget_get_allocation (child, &al);
    
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

 GdkScreen *screen = gdk_screen_get_default ();
 gint screen_height = gdk_screen_get_height(screen);
 
 double width_zf;
 
 if( page_width >= page_height ){ //landscape
 
  char tmp_zf[10];
   
  #if GTK_CHECK_VERSION(3,12,3)
   sprintf(tmp_zf, "%.3f", ( gdk_screen_get_width(screen) - 8 ) / page_width);
  #else
   sprintf(tmp_zf, "%.3f", ( gdk_screen_get_width(screen) - 21 ) / page_width);
  #endif 
  
  width_zf = atof(tmp_zf);
  
 }
 else{ //portrait, page_height > page_width
  
  char tmp_zf[10];
  
  #if GTK_CHECK_VERSION(3,12,3)
   sprintf(tmp_zf, "%.3f", ( gdk_screen_get_width(screen) - 8 ) / page_width);
  #else
   sprintf(tmp_zf, "%.3f", ( gdk_screen_get_width(screen) - 21 ) / page_width);
  #endif 
  
  width_zf = atof(tmp_zf);
  
 }
 
 if(zoom_factor == width_zf)
  return;
 else
  zoom_factor = width_zf;
 
 GdkPixbuf *temp_pixbuf;
 temp_pixbuf = pixbuf;

 gint width, height;

 width = (gint)((page_width*width_zf)+0.5);
 height = (gint)((page_height*width_zf)+0.5);

 cairo_surface_t *surface;
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cairo_t* cr;
 
 cr = cairo_create(surface);

 cairo_scale(cr, width_zf, width_zf);

 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  if(current_page_num > page_num-1) current_page_num = page_num-1;
  page = poppler_document_get_page(doc, current_page_num);
 }

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

 g_object_unref (G_OBJECT (page));
 page = NULL;
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);

 GList *list, *iter;
 list = gtk_container_get_children (GTK_CONTAINER (layout));

 for (iter = list; iter; iter = iter->next){

  GtkWidget *child = GTK_WIDGET (iter->data);
  
  if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child)){

   gint wx, wy;
   
   gtk_widget_translate_coordinates(child, gtk_widget_get_toplevel(child), 0, 0,  &wx, &wy);

   const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
   char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                                   gtk_label_get_text(GTK_LABEL(child)));
                                                   
   gtk_label_set_markup(GTK_LABEL(child), markup);
   g_free(markup);

   GtkAllocation al;
  
   gtk_widget_get_allocation (child, &al);
   
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
zoom_height(void) {

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
 
 GdkScreen *screen = gdk_screen_get_default ();
 gint screen_height = gdk_screen_get_height(screen);
 
 double width_zf;
 
 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 GtkAllocation mb_alloc;
 gtk_widget_get_allocation (menubar, &mb_alloc);
 
 GtkAllocation tb_alloc;
 gtk_widget_get_allocation (toolbar, &tb_alloc);
 
 GtkAllocation fb_alloc;
 gtk_widget_get_allocation (findbar, &fb_alloc);
  
 if( page_width >= page_height ){ //landscape
 
  if(TOOL_BAR_VISIBLE){
   
   if( KEY_BUTTON_SEARCH ){
    
    
    if( screen_height == 768 )
     width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height*1.2)/page_height;
    else if ( screen_height == 1080 ) //for 1920x1080
     width_zf = (win_alloc.height-tb_alloc.height-fb_alloc.height-mb_alloc.height)/page_height;
    
   }
   else{
    
    if( screen_height == 768 )
     width_zf = (win_alloc.height-tb_alloc.height-fb_alloc.height-mb_alloc.height*1.3)/page_height;
    else if( screen_height == 1080 )//for 1920x1080
     width_zf = (win_alloc.height-tb_alloc.height-fb_alloc.height*2-mb_alloc.height*1.2)/page_height;
    
   }
   
  }
  else{ // toolbar is not visible
  
   if( KEY_BUTTON_SEARCH ){
   
    if( screen_height == 768 )
     width_zf = (win_alloc.height-mb_alloc.height*1.19)/page_height;
    else if ( screen_height == 1080 ) //for 1920x1080
     width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height)/page_height;
   
   }
   else{
  
    if( screen_height == 768 ) 
     width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height*0.9)/page_height;
    else if( screen_height == 1080 )//for 1920x1080
     width_zf = (win_alloc.height-tb_alloc.height-fb_alloc.height-mb_alloc.height)/page_height;
   
   }
   
  }
 
 }
 else{ //portrait 
  
  if(TOOL_BAR_VISIBLE){
   
   if( screen_height == 768 )
    width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height*1.2)/page_height;
   else if ( screen_height == 1080 ) //for 1920x1080
    width_zf = (win_alloc.height-tb_alloc.height-fb_alloc.height-mb_alloc.height)/page_height;
  
  }
  else{
  
   if( screen_height == 768 ) 
    width_zf = (win_alloc.height-mb_alloc.height*1.2)/page_height;
   else if( screen_height == 1080 )//for 1920x1080
    width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height)/page_height;
   
  }
  
 }
  
 if( zoom_factor == width_zf )
  return;
 else
  zoom_factor = width_zf;
 
 GdkPixbuf *temp_pixbuf;
 temp_pixbuf = pixbuf;

 gint width, height;
 
 width = (gint)((page_width*width_zf)+0.5);
 height = (gint)((page_height*width_zf)+0.5);
 
 cairo_surface_t *surface;
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cairo_t* cr;
 
 cr = cairo_create(surface);
 
 cairo_scale(cr, width_zf, width_zf);

 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  if(current_page_num > page_num-1) current_page_num = page_num-1;
  page = poppler_document_get_page(doc, current_page_num);
 }
 
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
  g_message("temp_pixbuf != NULL");
  g_object_unref(temp_pixbuf);
 }
 
 cairo_surface_destroy (surface);

 gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);

 g_object_unref (G_OBJECT (page));
 page = NULL;
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
 GList *list, *iter;
 list = gtk_container_get_children (GTK_CONTAINER (layout));

 for (iter = list; iter; iter = iter->next){

  GtkWidget *child = GTK_WIDGET (iter->data);
  
  if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child)){

   gint wx, wy;
  
   gtk_widget_translate_coordinates(child, gtk_widget_get_toplevel(child), 0, 0,  &wx, &wy);

   const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
   char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                                  gtk_label_get_text(GTK_LABEL(child)));
                                                  
   gtk_label_set_markup(GTK_LABEL(child), markup);
   g_free(markup);

   GtkAllocation al;
   
   gtk_widget_get_allocation (child, &al);
   
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
zoom_height_cb(GtkWidget* widget, gpointer data) {
 
 zoom_height();

}

void
zoom_out(void) {
 
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
  
  if(zoom_factor < 1.0)
   zoom_factor = 1.0;
  
  if(zoom_factor == 1.0)
   gtk_window_resize(GTK_WINDOW(win), (int)page_width+1, (int)page_height+80);
  else
   gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10, (int)(page_height*zoom_factor)+80);
  
  GdkPixbuf *temp_pixbuf;
  temp_pixbuf = pixbuf;

  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

  cairo_surface_t *surface;
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  
  cairo_t* cr;
  
  cr = cairo_create(surface);

  cairo_scale(cr, zoom_factor, zoom_factor);

  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   if(current_page_num > page_num-1) current_page_num = page_num-1;
   page = poppler_document_get_page(doc, current_page_num);
  }

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

  g_object_unref (G_OBJECT (page));
  page = NULL;
  
  gtk_layout_set_size (GTK_LAYOUT(layout), width, height+1);

  GList *list, *iter;
  list = gtk_container_get_children (GTK_CONTAINER (layout));

  for (iter = list; iter; iter = iter->next){

   GtkWidget *child = GTK_WIDGET (iter->data);
   
   if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child)){

    gint wx, wy;
    
    gtk_widget_translate_coordinates(child, gtk_widget_get_toplevel(child), 0, 0,  &wx, &wy);

    GtkAllocation al;
    
    gtk_widget_get_allocation (child, &al);

    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                                   gtk_label_get_text(GTK_LABEL(child)));
                                                   
    gtk_label_set_markup(GTK_LABEL(child), markup);
    g_free(markup);

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
