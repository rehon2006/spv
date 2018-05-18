/*
 * Copyright (C) 2017-2018 rehon2006, rehon2006@gmail.com
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
#include "note.h"
#include "highlight.h"
#include "page.h"
#include "pdf.h"
#include "gui.h"
  
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
  
  gtk_window_resize( GTK_WINDOW(win), 
                     (int)(page_width*zoom_factor)+(int)(zoom_factor)*10+10, 
                     (int)(page_height*zoom_factor)+80);
  
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
  
  if( dual_page_mode ){
   
   #ifdef __APPLE__
   page_change();
   #endif
   
   return;
  }
  
  double width_zf;
 
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
  #if GTK_CHECK_VERSION(3,18,10)
  width_zf = (sw_alloc.width - 2)/page_width;
  #else
  width_zf = (sw_alloc.width - 2 - 12 )/page_width;
  #endif
  
  if(zoom_factor < 0)
   zoom_factor = width_zf;
 
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
  
 }// end of else if( mode == ZOOM_HEIGHT )
 
 page_setup(&surface, &da_selection_region, draw_area, current_nc);

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
