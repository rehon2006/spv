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

static void dual_page(void){
 
 gtk_widget_show(ldraw_area);
 
 if(lpage){
  g_object_unref (G_OBJECT (lpage));
  lpage = NULL;
 }

 GdkScreen *screen = gdk_screen_get_default ();
 char tmp_zf[10];
 
 if(page_width >= page_height)
  sprintf(tmp_zf, "%.3f", ( (gdk_screen_get_width(screen)+1 )/2.0/ page_width));
 else{
  
  #if GTK_CHECK_VERSION(3,18,10)
  sprintf(tmp_zf, "%.3f", ( (gdk_screen_get_width(screen)-4 )/2.0/ page_width));
  #else
  sprintf(tmp_zf, "%.3f", ( (gdk_screen_get_width(screen)-18 )/2.0/ page_width));
  #endif
 }

 double width_zf = atof(tmp_zf);
 
 if(zoom_factor == width_zf)
  return;
 else
  zoom_factor = width_zf;

 if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
  current_page_num--;
  lpage = poppler_document_get_page(doc, current_page_num);
 }else
  lpage = poppler_document_get_page(doc, current_page_num);
  
 poppler_page_get_size(lpage, &page_width, &page_height);
  
 if(lsurface)
  cairo_surface_destroy (lsurface);

 //left page
 page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);

 struct note_cache* nc_tmp = NULL;
 
 if( note_cache )
  nc_tmp = note_cache->next;
 
 while( nc_tmp ){
  
  if( nc_tmp->page_num == current_page_num +2 ){
   rcurrent_nc = nc_tmp;
   break;
  }
  else{
   rcurrent_nc = NULL;
  }
  
  nc_tmp = nc_tmp->next;
  
 }
 
 //right page
 page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc);
 
}

void dual_page_cb(void){

 if ( gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dual_pageMi)) ){
 
  gint page_num = poppler_document_get_n_pages(doc);
  
  if( page_num > 1 ){
   
   dual_page_mode = TRUE;
   
   dual_page();
   
  }
 }
 else{ // exit dual-page mode
 
  if(lpage){
   g_object_unref (G_OBJECT (lpage));
   lpage = NULL;
  }
  
  if(page){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
  dual_page_mode = FALSE;
  
  zoom(ZOOM_WIDTH);
  
  gtk_widget_hide(ldraw_area);
  
  if(lsurface){
   cairo_surface_destroy (lsurface);
   lsurface = NULL;
  }
  
  if( rcurrent_nc ){
  
   struct list_head *tmp;
  
   list_for_each(tmp, &rcurrent_nc->CM_HEAD){

    struct note *tmp1;
    tmp1= list_entry(tmp, struct note, list);
   
    if( gtk_widget_is_visible(tmp1->comment) ){
     gtk_widget_hide(tmp1->comment);
    }
   
   }//list_for_each(tmp, &current_nc->CM_HEAD)
  
   rcurrent_nc = NULL;
  
  }
   
 }
 
}

void page_change(void){

 gint width, height;

 char *page_str = (char*)malloc(100);
 
 if( mode == TEXT_SELECTION )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 else if ( mode == TEXT_HIGHLIGHT )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
 else if ( mode == ERASE_TEXT_HIGHLIGHT )
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
 else
  sprintf(page_str, "%s page %d/%d",file_name, current_page_num+1, poppler_document_get_n_pages(doc));
 
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 struct list_head *tmp;
 
 if( current_nc ){
  
  list_for_each(tmp, &current_nc->CM_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
   
   if( gtk_widget_is_visible(tmp1->comment) ){
    gtk_widget_hide(tmp1->comment);
   }
   
  }//list_for_each(tmp, &current_nc->CM_HEAD)
  
 }
 
 if( rcurrent_nc ){
  
  list_for_each(tmp, &rcurrent_nc->CM_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
   
   if( gtk_widget_is_visible(tmp1->comment) ){
    gtk_widget_hide(tmp1->comment);
   }
   
  }//list_for_each(tmp, &current_nc->CM_HEAD)
  
 }
 
 //hide previous page's comment
 
 struct note_cache* nc_tmp = NULL;
 
 if( note_cache )
  nc_tmp = note_cache->next;
 
 while( nc_tmp ){
  
  if( current_page_num == nc_tmp->page_num -1 ){
   current_nc = nc_tmp;
   break;
  }
  else{
   current_nc = NULL;
  }
 
  nc_tmp = nc_tmp->next;
  
 }
 //get current_nc
 
 //get rcurrent_nc
 if( note_cache )
  nc_tmp = note_cache->next;
  
 while( nc_tmp ){
   
  if( nc_tmp->page_num == current_page_num +2 ){
   rcurrent_nc = nc_tmp;
   break;
  }
  else{
   rcurrent_nc = NULL;
  }
 
  nc_tmp = nc_tmp->next;
  
 }
  
 //get rcurrent_nc
 
 if( dual_page_mode ){
  //left page
  page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);
 
  //right page
  page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc); 
  
  if( current_nc )
   display_comment(&current_nc->CM_HEAD);
  
  if( rcurrent_nc )
   display_comment(&rcurrent_nc->CM_HEAD);
  
 }
 else{
 
  page_setup(&surface, &da_selection_region, draw_area, current_nc);
  
  if(current_nc)
   display_comment(&current_nc->CM_HEAD);
  
 }
 
}

void
next_page_cb(GtkWidget* widget, gpointer data) {

 next_page();

}

void
prev_page(void) {

 gint page_num = poppler_document_get_n_pages(doc);
 
 if ( dual_page_mode ){ //dual-page mode
  
  if(current_page_num > 0 && word_not_found ){
   current_page_num = current_page_num -2;
  }

  if (current_page_num == page_num)
   current_page_num = current_page_num -2;
  
  if(current_page_num == -1)
   current_page_num = 0;
 
 }else{ //single-page mode
  
  if(current_page_num > 0)
   current_page_num--;

  if (current_page_num == page_num)
   current_page_num--;
 
 }
 
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
  
  if(dual_page_mode){
   
   if( current_page_num +3 == page_num )
    current_page_num++;
   else if (current_page_num + 2 == page_num)
    return;
   else{ 
    current_page_num = current_page_num + 2;
   }
  }else{
  
   current_page_num++;
  
  }
  
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

void page_setup(cairo_surface_t **surface, cairo_region_t **da_selection_region, GtkWidget *draw_area, struct note_cache *current_nc){

 if( selection_surface ){
 
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area); 
 }
 
 gint width, height;

 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);

 if(*surface)
  cairo_surface_destroy (*surface);
  
 if(cr)
  cairo_destroy (cr);
 
 *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

 cr = cairo_create(*surface);

 cairo_scale(cr, zoom_factor, zoom_factor);
 
 if( !dual_page_mode ){
 
  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   
   if(current_page_num > page_num-1) 
    current_page_num = page_num-1;
   
   page = poppler_document_get_page(doc, current_page_num);
  }
  
 }
 else{ //for dual page mode
  
  if( *surface == lsurface ){ //left page
   
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
    current_page_num--;
    page = poppler_document_get_page(doc, current_page_num);
   }else
    page = poppler_document_get_page(doc, current_page_num);
   
  }
  else {// right page
   
   gint page_num = poppler_document_get_n_pages(doc);
 
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
 
   if( current_page_num+1 <= page_num-1 ){
  
    page = poppler_document_get_page(doc, current_page_num+1);
  
   }
   else if( current_page_num == page_num-1 ){
    page = poppler_document_get_page(doc, current_page_num);
   } 
   
  }
  
 }

 poppler_page_render(page, cr);
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
   
  cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
  cairo_set_source_rgb (cr, 1., 1., 1.);
  cairo_paint (cr);
   
  cairo_set_operator (cr, CAIRO_OPERATOR_DIFFERENCE); 
  cairo_set_source_rgb (cr, 1., 1., 1.);
   
 }
 else{
  
  cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);

  if(PDF_BACKGROUND_COLOR_CHANGED)
   cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
  else
   cairo_set_source_rgb (cr, 1., 1., 1.);
    
 }
  
 cairo_paint (cr);
 
 PopplerRectangle area = { 0, 0, 0, 0 };
 
 area.x2 = width;
 area.y2 = height;
 
 if(*da_selection_region){
  cairo_region_destroy(*da_selection_region);
 }
 
 *da_selection_region = poppler_page_get_selected_region (page,
                                                          zoom_factor,
                                                          POPPLER_SELECTION_GLYPH,
                                                          &area);
  
 gtk_widget_set_size_request (draw_area, width, height);
 
 da_width = width;
 da_height = height;
  
 gtk_widget_queue_draw (draw_area);
  
 if( page ){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }

 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
 GdkRGBA color;
  
 struct list_head *tmp;

 if( current_nc ){
  
  list_for_each(tmp, &current_nc->HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   char *rgb = (char*)malloc(8);
   sprintf(rgb, "#%s", tmp1->color_name);

   gdk_rgba_parse(&color, rgb);
   
   if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
     
    cairo_set_source_rgb (cr, 1, 1 , 1);
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
   
    cairo_rectangle (cr, tmp1->x,
                         tmp1->y, 
                         tmp1->width,
                         tmp1->height);
      
    cairo_fill (cr);
      
   }
   
   cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
   
   cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
   free(rgb);
   
   cairo_rectangle (cr, tmp1->x,
                        tmp1->y, 
                        tmp1->width,
                        tmp1->height);
   cairo_fill (cr);
    
  }//list_for_each(tmp, &HR_HEAD
  
 }
  
 gtk_widget_queue_draw (draw_area);
  
 if( find_ptr ){
  invert_search_region();
 }
 
}