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

#include "highlight.h"

void
highlight_Region (cairo_region_t* region, int option){

 int count;

 count = cairo_region_num_rectangles(region);

 if(hr)
  free(hr);

 hr = (cairo_rectangle_int_t *)malloc(sizeof(cairo_rectangle_int_t)*(count+1));

 hr[count].x = -1;
 hr[count].y = -1;
 hr[count].width = -1;
 hr[count].height = -1;
 
 while(count--){

  cairo_rectangle_int_t rec;
  cairo_region_get_rectangle(region, count, &rec);
  
  hr[count].x = rec.x;
  hr[count].y = rec.y;
  hr[count].width = rec.width;
  hr[count].height = rec.height;

  text_highlight(rec.x, rec.y, rec.x + rec.width, rec.y + rec.height, option);
 }
 
}

void text_highlight_release(gint x1, gint y1, gint x2, gint y2, gchar *color_name, gint option){
 
 guchar *h_r, *h_g, *h_b;

 h_r = (guchar *)malloc(3);
 *h_r = color_name[0];
 *(h_r+1) = color_name[1];
 *(h_r+2) = '\0';
 
 h_g = (guchar *)malloc(3);
 *h_g = color_name[2];
 *(h_g+1) = color_name[3];
 *(h_g+2) = '\0';
 
 h_b = (guchar *)malloc(3);
 *h_b = color_name[4];
 *(h_b+1) = color_name[5];
 *(h_b+2) = '\0';
 
 guint n_channels;
 guchar *data;
 guint rowstride;
 
 if( option == 1 ){
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  data = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
 }
 else if( option == 2 ){
  n_channels = gdk_pixbuf_get_n_channels (lpixbuf);
  data = gdk_pixbuf_get_pixels (lpixbuf);
  rowstride = gdk_pixbuf_get_rowstride (lpixbuf);
 }
 else
  return;
 
 gint x, y;

 guchar *p;
 
 for (  x = x1; x < x2; x++  ){
  for ( y = y1; y < y2; y++ ){
   
   p = data + x * n_channels + y * rowstride;
   
   if( p[0] != 0 && p[1] != 0 && p[2] != 0 ){ 
   
    p[0] = ( 255 - ( p[0] ^ 0x00 ) ) & strtol(h_r, NULL, 16);
    p[0] = ( 255 - ( p[0] ^ 0x00 ) ) & strtol(h_r, NULL, 16);
    
    p[1] = ( 255 - ( p[1] ^ 0xff ) ) & strtol(h_g, NULL, 16);
    p[1] = ( 255 - ( p[1] ^ 0xff ) ) & strtol(h_g, NULL, 16);
    
    p[2] = ( 255 - ( p[2] ^ 0xff ) ) & strtol(h_b, NULL, 16);
    p[2] = ( 255 - ( p[2] ^ 0xff ) ) & strtol(h_b, NULL, 16);
   }

  }
 }
 
 if( option == 1 )
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 else if( option == 2)
  gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
  
}

void text_highlight(gint x1, gint y1, gint x2, gint y2, gint option){
 
 guint n_channels;
 guchar *data;
 guint rowstride;
 
 if( option == 1 ){
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  data = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
 }
 else if( option == 2 ){
  n_channels = gdk_pixbuf_get_n_channels (lpixbuf);
  data = gdk_pixbuf_get_pixels (lpixbuf);
  rowstride = gdk_pixbuf_get_rowstride (lpixbuf);
 }
 else
  return;
 
 gint x, y;
 
 guchar *p;
 
 for (  x = x1; x < x2; x++  ){
  for ( y = y1; y < y2; y++ ){

   p = data + x * n_channels + y * rowstride; 
   
   p[0] = (255-p[0]) ^ 0x00;
   p[1] = (255-p[1]) ^ 0xff;
   p[2] = (255-p[2]) ^ 0xff;
  
  }// end of for ( y = y1; y < y2; y++ )
 }// end of for (  x = x1; x < x2; x++  )

}

void invertArea (gint x1, gint y1, gint x2, gint y2, int option){
 
 if(!inverted)
  inverted = 1;
 else
  inverted = 0;
 
 guint n_channels;
 guchar *data;
 guint rowstride;
 
 if( option == 1 ){
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  data = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
 }
 else if( option == 2 ){
  n_channels = gdk_pixbuf_get_n_channels (lpixbuf);
  data = gdk_pixbuf_get_pixels (lpixbuf);
  rowstride = gdk_pixbuf_get_rowstride (lpixbuf);
 }
 else
  return;
 
 gint x, y;
 
 guchar *p;
 
 for ( x = x1; x < x2; x++ ){
  for ( y = y1; y < y2; y++ ){
       
   // Calculate pixel's offset into the data array. 
   p = data + x * n_channels + y * rowstride;
              
   p[0] = 255 - p[0];
   p[1] = 255 - p[1];
   p[2] = 255 - p[2];
       
  }
 }
 
 if( !lpage ){
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }
 else{
  if(option == 2)
   gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
  else if( option == 1)
   gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }
}

void
invertRegion (cairo_region_t* region, int option){

 int count = cairo_region_num_rectangles(region);
    
 while(count--){
    
  cairo_rectangle_int_t rec;
  cairo_region_get_rectangle(region, count, &rec);
  
  invertArea(rec.x, rec.y, rec.x + rec.width, rec.y + rec.height, option);
        
 }
   
}

void invert_text_highlight(gint x1, gint y1, gint x2, gint y2, gint option){

 guint n_channels;
 guchar *data;
 guint rowstride;
 
 if( option == 1 ){
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  data = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
 }
 else if( option == 2 ){
  n_channels = gdk_pixbuf_get_n_channels (lpixbuf);
  data = gdk_pixbuf_get_pixels (lpixbuf);
  rowstride = gdk_pixbuf_get_rowstride (lpixbuf);
 }
 else
  return;
 
 gint x, y;
 
 guchar *p;

 for (  x = x1; x < x2; x++  ){
  for ( y = y1; y < y2; y++ ){

   p = data + x * n_channels + y * rowstride; 

   p[0] = (255-p[0]) ^ 0x00;
   p[1] = (255-p[1]) ^ 0xff;
   p[2] = (255-p[2]) ^ 0xff;
 
  }
 }
 
 if( option == 1 ){
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }
 else if( option == 2)
  gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
}

void
invert_highlight_Region (cairo_region_t* region, gint option){

 int count;
 
 count = cairo_region_num_rectangles(region);
 
 if(ihr)
  free(ihr);

 ihr = (cairo_rectangle_int_t *)malloc(sizeof(cairo_rectangle_int_t)*(count+1));
  
 ihr[count].x = -1;
 ihr[count].y = -1;
 ihr[count].width = -1;
 ihr[count].height = -1;

 while(count--){
  
  cairo_rectangle_int_t rec;
  cairo_region_get_rectangle(region, count, &rec);
  
  ihr[count].x = rec.x;
  ihr[count].y = rec.y;
  ihr[count].width = rec.width;
  ihr[count].height = rec.height;

  invert_text_highlight(rec.x, rec.y, rec.x + rec.width, rec.y + rec.height, option);
   
 }

}

void invert_search_region(void){

 GtkAdjustment *vadj =
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));	
 	
 PopplerRectangle *rect = find_ptr->data;
 
 if( (gint)rect->y2 > (gint)rect->y1 ) {
   
  rect->y1 = page_height - rect->y1;
  rect->y2 = page_height - rect->y2;
 }
 
 if(!lpage){
 
  invertArea( (gint)(rect->x1*zoom_factor), 
              (gint)(rect->y2*zoom_factor),
              (gint)(rect->x2*zoom_factor), 
              (gint)(rect->y1*zoom_factor), 
              1);
 
 }
 else{ // dual-page mode
   gint pos = g_list_position(find_ptr_head, find_ptr);
   
   if( mode == TEXT_SEARCH_NEXT){
    if( pos < g_list_position(find_ptr_head, rmatches) || rmatches == NULL){ // left page 
    
     invertArea( (gint)(rect->x1*zoom_factor), 
                (gint)(rect->y2*zoom_factor),
                (gint)(rect->x2*zoom_factor), 
                (gint)(rect->y1*zoom_factor), 
                2);
   
    }
    else{ // right page
     invertArea( (gint)(rect->x1*zoom_factor), 
                (gint)(rect->y2*zoom_factor),
                (gint)(rect->x2*zoom_factor), 
                (gint)(rect->y1*zoom_factor), 
                1);
    }
   }
   else if(mode == TEXT_SEARCH_PREV){
    if( pos <= g_list_position(find_ptr_head, rmatches) ){ // right page 
    
     invertArea( (gint)(rect->x1*zoom_factor), 
                (gint)(rect->y2*zoom_factor),
                (gint)(rect->x2*zoom_factor), 
                (gint)(rect->y1*zoom_factor), 
                1);
   
    }
    else{ // left page
     invertArea( (gint)(rect->x1*zoom_factor), 
                (gint)(rect->y2*zoom_factor),
                (gint)(rect->x2*zoom_factor), 
                (gint)(rect->y1*zoom_factor), 
                2);
    }
   }
 }
  
 int i;
 double region_y1 = (rect->y1)*zoom_factor;
 double range = gtk_adjustment_get_upper(vadj)/(zoom_factor+1);

 for( i = 1; i<= (int)zoom_factor + 1; i++ ){
  
  if( region_y1 >(i*range-range) && region_y1 < (range*i) ){
   
   gtk_adjustment_set_value(vadj, (i-1)*range );
   
  }
 }
 
}