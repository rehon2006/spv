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
zoom_dual_page(void) {
 
 //right page
 
 gint width, height;
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);
 
 if(surface)
  cairo_surface_destroy (surface);
  
 if(cr)
  cairo_destroy (cr);
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(surface);
 
 cairo_scale(cr, zoom_factor, zoom_factor);
 
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
    
  if(PDF_BACKGROUND_COLOR_CHANGED){
   cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
  }
  else{
   cairo_set_source_rgb (cr, 1., 1., 1.);
  }
    
 }
 
 cairo_paint (cr);
 
 gtk_widget_set_size_request (draw_area, width, height);
 
 da_width = width;
 da_height = height;
 
 PopplerRectangle area = { 0, 0, 0, 0 };
 
 area.x2 = width;
 area.y2 = height;
 
 if(da_selection_region){
  cairo_region_destroy(da_selection_region);
  
  da_selection_region = poppler_page_get_selected_region (page,
                                                          zoom_factor,
                                                          POPPLER_SELECTION_GLYPH,
                                                          &area);
 }
 
 gtk_widget_queue_draw (draw_area);
 
 if(page){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
 //highlight region
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
 }
 else{
  cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
 }
 
 GdkRGBA color;
 
 struct list_head *tmp;

 list_for_each(tmp, &HR_HEAD){

  struct highlight_region *tmp1;
  tmp1= list_entry(tmp, struct highlight_region, list);
  
  if(tmp1->page_num ==  current_page_num +2 ){
  
   char *rgb = (char*)malloc(8);
   sprintf(rgb, "#%s", tmp1->color_name);

   gdk_rgba_parse(&color, rgb);
   
   cairo_set_source_rgb (cr, color.red, color.green, color.blue);
   
   free(rgb);
   
   cairo_rectangle (cr, (int)(tmp1->x),
                        (int)(tmp1->y), 
                        (int)(tmp1->width),
                        (int)(tmp1->height));
     
   cairo_fill (cr);
   
  }

 }
 //highlight region
 
 //comments
 list_for_each(tmp, &NOTE_HEAD){

  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  
  if( tmp1->page_num == current_page_num+2 ){
    
    gtk_widget_show(tmp1->comment);
    
   }else if( tmp1->page_num != current_page_num+1 ){
    
    gtk_widget_hide(tmp1->comment);
   }
 
}
//comments

gtk_widget_queue_draw (draw_area);
 
}

void
zoom_in(void) {
 
 if( dual_page_mode )
  return;
 
 if (zoom_factor != 5.0 && zoom_factor <= 5.0){
  
  if( selection_surface ){
 
   cairo_surface_destroy (selection_surface);
   selection_surface = NULL;
   
   gtk_widget_queue_draw (draw_area); 
  }
 
  zoom_factor = zoom_factor + 1.0;
  
  gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10+10, (int)(page_height*zoom_factor)+80);
  
  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);
  
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

  cr = cairo_create(surface);

  cairo_scale(cr, zoom_factor, zoom_factor);
 
  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   
   if(current_page_num > page_num-1) 
    current_page_num = page_num-1;
    
   page = poppler_document_get_page(doc, current_page_num);
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
  
  if(da_selection_region){
   cairo_region_destroy(da_selection_region);
  
   da_selection_region = poppler_page_get_selected_region (page,
                                                           zoom_factor,
                                                           POPPLER_SELECTION_GLYPH,
                                                           &area);
 
  }
  
  gtk_widget_set_size_request (draw_area, width, height);
 
  da_width = width;
  da_height = height;
  
  gtk_widget_queue_draw (draw_area);
  
  g_object_unref (G_OBJECT (page));
  page = NULL;

  gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
  
  if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
   cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
  }
  else{
   cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
  }
  
  GdkRGBA color;
  
  struct list_head *tmp;

  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   if( tmp1->page_num == current_page_num + 1 ){
   
    char *rgb = (char*)malloc(8);
    sprintf(rgb, "#%s", tmp1->color_name);

    gdk_rgba_parse(&color, rgb);
   
    cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
    free(rgb);
    
    cairo_rectangle (cr, (int)(tmp1->x),
                         (int)(tmp1->y), 
                         (int)(tmp1->width),
                         (int)(tmp1->height));
    cairo_fill (cr);
    
    
   }
   
  }
  
  gtk_widget_queue_draw (draw_area);
  
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

 if( dual_page_mode )
  return;
 
 if( selection_surface ){
 
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area); 
 }
 
 GdkScreen *screen = gdk_screen_get_default ();
 gint screen_height = gdk_screen_get_height(screen);
 
 double width_zf;
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 #if GTK_CHECK_VERSION(3,18,10)
 width_zf = (sw_alloc.width - 2)/page_width;
 #else
 width_zf = (sw_alloc.width - 2 - 12 )/page_width;
 #endif
 
 if(zoom_factor == width_zf)
  return;
 else
  zoom_factor = width_zf;

 gint width, height;

 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);
 
 if(surface)
  cairo_surface_destroy (surface);
  
 if(cr)
  cairo_destroy (cr);
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(surface);

 cairo_scale(cr, zoom_factor, zoom_factor);
 
 if(page == NULL){
  
  gint page_num = poppler_document_get_n_pages(doc);
  
  if(current_page_num > page_num-1) 
   current_page_num = page_num-1;
  
  page = poppler_document_get_page(doc, current_page_num);
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
 
 gtk_widget_set_size_request (draw_area, width, height);
 
 da_width = width;
 da_height = height;
 
 PopplerRectangle area = { 0, 0, 0, 0 };
 
 area.x2 = width;
 area.y2 = height;
 
 if(da_selection_region){
  cairo_region_destroy(da_selection_region);
  
  da_selection_region = poppler_page_get_selected_region (page,
                                                          zoom_factor,
                                                          POPPLER_SELECTION_GLYPH,
                                                          &area);
 
 }
 
 gtk_widget_queue_draw (draw_area);
 
 if( page ){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
 }
 else{
  cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
 }
 
 GdkRGBA color;
 
 struct list_head *tmp;
  
 list_for_each(tmp, &HR_HEAD){

  struct highlight_region *tmp1;
  tmp1= list_entry(tmp, struct highlight_region, list);

  if( tmp1->page_num == current_page_num + 1 ){
   
   char *rgb = (char*)malloc(8);
   sprintf(rgb, "#%s", tmp1->color_name);

   gdk_rgba_parse(&color, rgb);
   
   cairo_set_source_rgb (cr, color.red, color.green, color.blue);
   
   free(rgb);
   
   cairo_rectangle (cr, (int)(tmp1->x),
                        (int)(tmp1->y), 
                        (int)(tmp1->width),
                        (int)(tmp1->height));
   
   cairo_fill (cr);
   
    
  }

 }

 gtk_widget_queue_draw (draw_area);
 
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
 
 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 GtkAllocation mb_alloc;
 gtk_widget_get_allocation (menubar, &mb_alloc);
 
 GtkAllocation tb_alloc;
 gtk_widget_get_allocation(toolbar, &tb_alloc);
 
 GtkAllocation fb_alloc;
 gtk_widget_get_allocation(findbar, &fb_alloc);
 
 if( dual_page_mode ){
  
  if(page_width < page_height)
   return;
  
  int dp_width = (int)(zoom_factor*page_width);
  
  if(!TOOL_BAR_VISIBLE){
  
   if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) ){
    
    GdkScreen *screen = gdk_screen_get_default ();
    int screen_height = gdk_screen_get_height(screen);
    
    height_offset = ( screen_height - 2 - da_height )/2;
    
   }
   else{
    height_offset = ( win_alloc.height-mb_alloc.height-3 - da_height)/2;
   }
   
   int dp_width = (int)(zoom_factor*page_width);
   
   if( page_width >= page_height ){
    
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
    
   }
   else{
   
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
   
   }
   
  }
  else{
  
   height_offset = ( win_alloc.height-tb_alloc.height-mb_alloc.height-4 - da_height )/2;
   
  
   if( page_width >= page_height ){
    
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
    
   }
   else{
    
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
    
   }
   
  }
  
  struct list_head *tmp;
 
  list_for_each(tmp, &NOTE_HEAD){ 
  
   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
   
   if(tmp1->page_num == current_page_num + 1){
    
    gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor)+height_offset);
   
   }
   else if(tmp1->page_num == current_page_num + 2){
    
    gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+dp_width+1,
         (gint)(tmp1->y*zoom_factor)+height_offset);
   
   }
   
  }
  
  return;
 }
 
 if( selection_surface ){
 
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area); 
 }
 
 GdkScreen *screen = gdk_screen_get_default ();
 gint screen_height = gdk_screen_get_height(screen);
 
 double width_zf;
 
 if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) ){
 
  if(TOOL_BAR_VISIBLE){
   
   if( KEY_BUTTON_SEARCH ){
    
    width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height-4)/page_height;
    
   }
   else{
    
    width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height-fb_alloc.height-5)/page_height;
    
   }
   
  }
  else{ // toolbar is not visible
  
   if( KEY_BUTTON_SEARCH ){
    
    width_zf = (win_alloc.height-mb_alloc.height-3)/page_height;
    
   }
   else{
    
    width_zf = (win_alloc.height-tb_alloc.height-mb_alloc.height+3)/page_height;
    
   }
   
  }
 }
 else{ //fullscreen mode
 
  if(TOOL_BAR_VISIBLE){
   
   if( KEY_BUTTON_SEARCH ){
    width_zf = (screen_height-2 - tb_alloc.height-1)/page_height;
   }
   else{
    width_zf = (screen_height-2 - tb_alloc.height - fb_alloc.height-2)/page_height;
   }
   
  }else{
  
   if( KEY_BUTTON_SEARCH ){
    width_zf = (screen_height-2)/page_height;
   }
   else{
    width_zf = (screen_height -2 - fb_alloc.height-1)/page_height;
   }
   
  }
 }
 
 if( zoom_factor == width_zf )
  return;
 else
  zoom_factor = width_zf;

 gint width, height;
 
 width = (gint)((page_width*width_zf)+0.5);
 height = (gint)((page_height*width_zf)+0.5);
 
 if(surface)
  cairo_surface_destroy (surface);
  
 if(cr)
  cairo_destroy (cr);
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cr = cairo_create(surface);
 
 cairo_scale(cr, width_zf, width_zf);

 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  
  if(current_page_num > page_num-1) 
   current_page_num = page_num-1;
   
  page = poppler_document_get_page(doc, current_page_num);
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
 
 if(da_selection_region){
  cairo_region_destroy(da_selection_region);
  
  da_selection_region = poppler_page_get_selected_region (page,
                                                          zoom_factor,
                                                          POPPLER_SELECTION_GLYPH,
                                                          &area);
 
 }
 
 gtk_widget_set_size_request (draw_area, width, height);
 
 da_width = width;
 da_height = height;

 gtk_widget_queue_draw (draw_area);
 
 
 if( page ){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
  if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
   cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
  }
  else{
   cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
  }
  
  GdkRGBA color;
  
  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);

   if( tmp1->page_num == current_page_num + 1 ){
    
    char *rgb = (char*)malloc(8);
    sprintf(rgb, "#%s", tmp1->color_name);

    gdk_rgba_parse(&color, rgb);
   
    cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
    free(rgb);
    
    cairo_rectangle (cr, (int)(tmp1->x),
                         (int)(tmp1->y), 
                         (int)(tmp1->width),
                         (int)(tmp1->height));
    cairo_fill (cr);
    
   }

  }
  
  gtk_widget_queue_draw (draw_area);
  
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
 
 if( dual_page_mode )
  return;
 
 if (zoom_factor != 1.0 && zoom_factor >= 1.0){
  
  if( selection_surface ){
 
   cairo_surface_destroy (selection_surface);
   selection_surface = NULL;
  
   gtk_widget_queue_draw (draw_area); 
  }
  
  zoom_factor = zoom_factor - 1.0;
  
  if(zoom_factor < 1.0)
   zoom_factor = 1.0;
  
  if(zoom_factor == 1.0)
   gtk_window_resize(GTK_WINDOW(win), (int)page_width+1, (int)page_height+80);
  else
   gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10, (int)(page_height*zoom_factor)+80);

  gint width, height;

  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  
  cr = cairo_create(surface);

  cairo_scale(cr, zoom_factor, zoom_factor);

  if(page == NULL){
   gint page_num = poppler_document_get_n_pages(doc);
   
   if(current_page_num > page_num-1) current_page_num = page_num-1;
   
   page = poppler_document_get_page(doc, current_page_num);
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
  
  if(da_selection_region){
   cairo_region_destroy(da_selection_region);
  
   da_selection_region = poppler_page_get_selected_region (page,
                                                           zoom_factor,
                                                           POPPLER_SELECTION_GLYPH,
                                                           &area);
 
  }
  
  gtk_widget_set_size_request (draw_area, width, height);
  
  da_width = width;
  da_height = height;
  
  gtk_widget_queue_draw (draw_area);

  g_object_unref (G_OBJECT (page));
  page = NULL;
  
  gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
  
  if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
   cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
  }
  else{
   cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
  }
  
  GdkRGBA color;
  
  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   if( tmp1->page_num == current_page_num + 1 ){
    
    char *rgb = (char*)malloc(8);
    sprintf(rgb, "#%s", tmp1->color_name);

    gdk_rgba_parse(&color, rgb);
   
    cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
    free(rgb);
    
    cairo_rectangle (cr, (int)(tmp1->x),
                         (int)(tmp1->y), 
                         (int)(tmp1->width),
                         (int)(tmp1->height));
    cairo_fill (cr);
    
   }

  }
  
  gtk_widget_queue_draw (draw_area);
  
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
