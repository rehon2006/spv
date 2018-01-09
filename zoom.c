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
zoom_in_cb(GtkWidget* widget, gpointer data) {
 
 zoom(ZOOM_IN);
 
}

void
zoom_width_cb(GtkWidget* widget, gpointer data) {
 
 zoom(ZOOM_WIDTH);

}

void
zoom_height_cb(GtkWidget* widget, gpointer data) {
 
 zoom(ZOOM_HEIGHT);

}

void
zoom_out_cb(GtkWidget* widget, gpointer data) {

 zoom(ZOOM_OUT);

}

void zoom(enum mode mode){
 
 if( mode == ZOOM_IN ){
 
  if( dual_page_mode )
   return;
  
  if( zoom_factor > 5.0 )
   return;
  
  zoom_factor = zoom_factor + 1.0;
  
  gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10+10, (int)(page_height*zoom_factor)+80);
  
 }
 else if( mode == ZOOM_OUT ){
  
  if( dual_page_mode )
   return;
  
  if( zoom_factor <= 1.0 )
   return;
  
  zoom_factor = zoom_factor - 1.0;
  
  if(zoom_factor < 1.0)
   zoom_factor = 1.0;
  
  if(zoom_factor == 1.0)
   gtk_window_resize(GTK_WINDOW(win), (int)page_width+1, (int)page_height+80);
  else
   gtk_window_resize(GTK_WINDOW(win), (int)(page_width*zoom_factor)+(int)(zoom_factor)*10, (int)(page_height*zoom_factor)+80);
  
 }
 else if( mode == ZOOM_WIDTH ){
  
  if( dual_page_mode )
   return;
  
  GdkScreen *screen = gdk_screen_get_default ();
  gint screen_width = gdk_screen_get_width(screen);
  
  double width_zf;
 
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
  #if GTK_CHECK_VERSION(3,18,10)
  width_zf = (sw_alloc.width - 2)/page_width;
  #else
  if(screen_width == 1366)
   width_zf = (sw_alloc.width - 2 - 12 )/page_width;
  else
   width_zf = (sw_alloc.width - 2)/page_width;
  #endif
  
  GtkAdjustment *hadj;

  hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
  
  if(zoom_factor == width_zf)
   return;
  else
   zoom_factor = width_zf;
  
 }
 else if( mode == ZOOM_HEIGHT ){
  
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
   
   return;
  } // end of if( dual_page_mode )
  
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
  
 }
 if( mode == DUAL_PAGE ){
  
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
  
  if(lsurface)
   cairo_surface_destroy (lsurface);
 
 }
 
 //common part
 if( selection_surface ){
 
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area); 
 }
 
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
 
 //common part
 //***********
 
 if( mode == ZOOM_IN ){
 
  if(mode != ZOOM_IN){
  
   if(mode != ZOOM_OUT)
    pre_mode = mode;
   
   mode = ZOOM_IN;
  } 
  
 }else if( mode == ZOOM_OUT ){
   
  if(mode != ZOOM_OUT ){
   if(mode != ZOOM_IN )
    pre_mode = mode;
   mode = ZOOM_OUT;
  }
   
 }
  
}
