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
 
 gtk_widget_show(levent_box);
 
 zoom_dual_page();//right page
 
 GdkPixbuf *temp_pixbuf;
 temp_pixbuf = lpixbuf;
 
 lpage = poppler_document_get_page(doc, current_page_num);

 poppler_page_get_size(lpage, &page_width, &page_height); 
 
 gint width, height;
 
 width = (gint)(page_width*zoom_factor+0.5);
 height = (gint)(page_height*zoom_factor+0.5);

 cairo_surface_t *surface;
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
 cairo_t* cr = cairo_create(surface);

 cairo_scale(cr, zoom_factor, zoom_factor);

 poppler_page_render(lpage, cr);

 cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
 
 if(PDF_BACKGROUND_COLOR_CHANGED)
  cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
 else
  cairo_set_source_rgb (cr, 1., 1., 1.);
 
 cairo_paint (cr);

 cairo_destroy (cr);

 lpixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);

 if(temp_pixbuf != NULL){
  g_object_unref(temp_pixbuf);
 }

 cairo_surface_destroy (surface); 
 
 if(!lm_PageImage){
  lm_PageImage = gtk_image_new ();
  gtk_container_add(GTK_CONTAINER(levent_box), lm_PageImage);
 }
 
 gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 gtk_widget_show(lm_PageImage);
 
 //highlight region
 struct list_head *tmp;

 list_for_each(tmp, &HR_HEAD){

  struct highlight_region *tmp1;
  tmp1= list_entry(tmp, struct highlight_region, list);
 
  if(tmp1->page_num ==  current_page_num +1 ){
    
     text_highlight_release((int)(tmp1->x*zoom_factor), 
                            (int)(tmp1->y*zoom_factor), 
                            (int)((tmp1->x+tmp1->width)*zoom_factor), 
                            (int)((tmp1->y+tmp1->height)*zoom_factor), 
                            tmp1->color_name,
                            2); 
   
    }
  
 }
 //highlight region
 
 //comments
  list_for_each(tmp, &NOTE_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
   
   if( tmp1->page_num == current_page_num+1 ){
   
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
    gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
    g_free(markup);

    gtk_widget_show(tmp1->comment);
   }else if( tmp1->page_num != current_page_num+2 ){
    gtk_widget_hide(tmp1->comment);
   }
   
  }
  //comments
  
  if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
   invertArea(0 ,0 ,width ,height,1);
   if(lpage)
    invertArea(0 ,0 ,width ,height,2);
  }
  
}

void dual_page_cb(void){

 if ( gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dual_pageMi)) ){
 
  gint page_num = poppler_document_get_n_pages(doc);
  
  if( page_num > 1 ){
   
   if( mode == TEXT_SELECTION ){
   
    if( selection_region ){
    
     invertRegion(selection_region,1);
     cairo_region_destroy(selection_region);
     selection_region = NULL; 
    
    }
    
    if( last_region ){
    
     invertRegion(last_region,1);
     cairo_region_destroy(last_region);
     last_region = NULL;
    
    }

   }
   
   dual_page();
   
  }
 }
 else{
  
  if(llast_region){
   cairo_region_destroy(llast_region);
   llast_region = NULL;
  }
  
  if(lselection_region){
   cairo_region_destroy(lselection_region);
   lselection_region = NULL;
  }
  
  if(lpixbuf != NULL){
   g_object_unref(lpixbuf);
   lpixbuf = NULL;
  }
  
  if(lm_PageImage){
   gtk_widget_destroy(lm_PageImage);
   lm_PageImage = NULL;
  }
  
  if(lpage){
   g_object_unref (G_OBJECT (lpage));
   lpage = NULL;
  }
  
  if(page){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
  zoom_width();
  
  gtk_widget_hide(levent_box);
  
 }
}

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
 
 if(lselection_region){
  cairo_region_destroy(lselection_region);
  lselection_region = NULL;
 }

 if(last_region || selection_region ){
   
  cairo_region_t *invert_region;
   
  invert_region = cairo_region_copy(selection_region);
  if(mode == TEXT_SELECTION){
    
   invertRegion(invert_region,1);
    
   if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
    invertRegion(invert_region,1);
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
 
 if(llast_region || lselection_region ){
  
  cairo_region_t *invert_region;
   
  invert_region = cairo_region_copy(lselection_region);
  if(mode == TEXT_SELECTION){
    
   invertRegion(invert_region,2);
   
   if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
    invertRegion(invert_region,2);
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
   
  gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
 }
 
 gint page_num = poppler_document_get_n_pages(doc);
 
 if(lpage){
 
  GdkPixbuf *temp_pixbuf;
  temp_pixbuf = lpixbuf;

  g_object_unref (G_OBJECT (lpage));
  lpage = NULL;
  
  if(current_page_num == page_num-1){
   current_page_num--;
  }
  
  lpage = poppler_document_get_page(doc, current_page_num);

  poppler_page_get_size(lpage, &page_width, &page_height);

  gint width, height;
 
  width = (gint)(page_width*zoom_factor+0.5);
  height = (gint)(page_height*zoom_factor+0.5);

  cairo_surface_t *surface;
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
 
  cairo_t* cr = cairo_create(surface);

  cairo_scale(cr, zoom_factor, zoom_factor);

  poppler_page_render(lpage, cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
 
  if(PDF_BACKGROUND_COLOR_CHANGED)
   cairo_set_source_rgb (cr, background_color[0], background_color[1], background_color[2]);
  else
   cairo_set_source_rgb (cr, 1., 1., 1.);
  
  cairo_paint (cr);

  cairo_destroy (cr);

  lpixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);

  if(temp_pixbuf != NULL){
   g_object_unref(temp_pixbuf);
  }

  cairo_surface_destroy (surface); 

  gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);

  gtk_layout_set_size (GTK_LAYOUT(layout), width, height);

  gtk_widget_show(lm_PageImage);

  //highlight region
  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   if(tmp1->page_num ==  current_page_num +1 ){
    
    text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name,2); 
   
   }

  }
  //highlight region

  //comments
  list_for_each(tmp, &NOTE_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
  
   if( tmp1->page_num == current_page_num+1 ){
    
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
    gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
    g_free(markup);

    gtk_widget_show(tmp1->comment);
   }else{
    gtk_widget_hide(tmp1->comment);
   }
   
  }
  //comments
 
 }

 if( !lpage )
  page = poppler_document_get_page(doc, current_page_num);
 else{
  if(current_page_num == page_num-1)
   page = poppler_document_get_page(doc, current_page_num);
  else
   page = poppler_document_get_page(doc, current_page_num+1);
 }
 
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
 else{
  cairo_set_source_rgb (cr, 1., 1., 1.);
 }
 
 cairo_paint (cr);
 cairo_destroy (cr);
 
 pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);

 if(temp_pixbuf != NULL){
  g_object_unref(temp_pixbuf);
 }

 cairo_surface_destroy (surface); 
 
 gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 
 gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 
 struct list_head *tmp;

 list_for_each(tmp, &NOTE_HEAD){

  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  
  if(!lpage){
   if( tmp1->page_num == current_page_num+1 ){
   
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
    gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
    g_free(markup);

    gtk_widget_show(tmp1->comment);
   }else{
    gtk_widget_hide(tmp1->comment);
   }
   
  }else{ //dual-page mode
   
   if( tmp1->page_num == current_page_num+2 ){
   
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
    gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
    g_free(markup);
    
    int dp_width = (int)(zoom_factor*page_width);
    
    gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, (gint)(tmp1->x*zoom_factor)+dp_width+1, (gint)(tmp1->y*zoom_factor)+height_offset);
    
    
    gtk_widget_show(tmp1->comment);
   }
   else if( tmp1->page_num == current_page_num+1 ){
   
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                                           gtk_label_get_text(GTK_LABEL(tmp1->comment)));
   
    gtk_label_set_markup(GTK_LABEL(tmp1->comment), markup);
    g_free(markup);

    gtk_widget_show(tmp1->comment);
   
    gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, (gint)(tmp1->x*zoom_factor), (gint)(tmp1->y*zoom_factor)+height_offset);
   
   }else{
    gtk_widget_hide(tmp1->comment);
   }
  
  }

 } 
 
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);
   
   if(!lpage){
    if(tmp1->page_num ==  current_page_num +1 ){
    
     text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name,1); 
   
    }
   }
   else{
   
    if(tmp1->page_num ==  current_page_num +2 ){
    
     text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name,1); 
   
    }
   
   }

  }
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  invertArea(0 ,0 ,width ,height,1);
  if(lpage)
   invertArea(0 ,0 ,width ,height,2);
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
 
 if ( lpage ){ //dual-page mode
  
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
  
  if(lpage){
   
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