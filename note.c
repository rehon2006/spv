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
#include <sys/types.h>
#include <sys/stat.h>

#include "note.h"
#include "page.h"
#include "gui.h"
#include "highlight.h"
#include "pdf.h"

struct color_table* add_ct_entry(char *ct_entry){
 
 struct color_table *ct_tmp = COL_TAB;
 
 GdkRGBA color;
 
 char ctemp = *(ct_entry+7);
 *(ct_entry+7) = '\0';
 gdk_rgba_parse(&color, ct_entry);
 *(ct_entry+7) = ctemp;
 color.alpha = strtol (ct_entry+7,NULL,16)/255.0;
 
 while( ct_tmp->next ){
 
  if( gdk_rgba_equal(&color,&ct_tmp->next->color ) ){
   return ct_tmp->next;
  }
  
  ct_tmp = ct_tmp->next;
  
 }
 
 ct_tmp->next = (struct color_table *)malloc(sizeof(struct color_table));
 ct_tmp->next->rc = 1;
 ct_tmp->next->color = color; 
 ct_tmp->next->next = NULL;
 
 return ct_tmp->next;
 
}

static gboolean draw_cursor_cb(GtkWidget *widget){

 if(draw_cursor){
  draw_cursor = FALSE;
 }
 else{
  draw_cursor = TRUE;
 }
 
 gtk_widget_queue_draw(widget);
 
 return TRUE;
 
}

static void cm_bg_color_set_cb(GtkWidget *widget, gpointer user_data){
 
 struct note* note = (struct note*)user_data;
 
 GtkWidget *colorseldlg = gtk_color_chooser_dialog_new ("Select Color for Comment's Background", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  GdkRGBA color;
   
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
 
  char *ct_str = (char*)malloc(10);
   
  sprintf(ct_str, "#%02X%02X%02X%02X", (int)(color.red*255), 
                                       (int)(color.green*255), 
                                       (int)(color.blue*255),
                                       (int)(color.alpha*255)); 
 
  note->property->bg_color = add_ct_entry(ct_str);
   
  free(ct_str);
  
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
 
  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha);
  
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
   
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
 }
 
 gtk_widget_destroy (colorseldlg);
 
}

static void cm_font_color_set_cb(GtkWidget *widget, gpointer user_data){

 struct note* note = (struct note*)user_data;
 
 GtkWidget *colorseldlg;
 
 colorseldlg = gtk_color_chooser_dialog_new ("Select Font Color for Comment", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  GdkRGBA color;
  
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
  
  char *ct_str = (char*)malloc(10);
   
  sprintf(ct_str, "#%02X%02X%02X%02X", (int)(color.red*255), 
                                       (int)(color.green*255), 
                                       (int)(color.blue*255),
                                       (int)(color.alpha*255)); 
   
  note->property->font_color = add_ct_entry(ct_str);
   
  free(ct_str);
 
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
   
  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha);
   
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
  
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);

 }
 
 gtk_widget_destroy (colorseldlg);
  
}

static void cm_font_changed(GtkFontButton *font_button, gpointer user_data) {
 
 struct note *note = (struct note *)user_data;
 
 PangoFontDescription *tmp_desc = pango_font_description_from_string (gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button)));

 if( !pango_font_description_equal(tmp_desc, P_CM_FONT_DESC) ){
  note->property->font_desc = tmp_desc;
 }

 gtk_widget_queue_draw(note->comment);
 
}

static gboolean
comment_button_press_cb(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
 
 lstart_x = event->x;
 lstart_y = event->y;
 
 if ( event->button == 1 ){
  
  if( comment_click == 0 ){
  
   comment_click = 1;
   
   GtkAllocation comment_alloc;
   gtk_widget_get_allocation (widget, &comment_alloc);
   
   int dp_width = (int)(zoom_factor*page_width);
     
   if( comment_alloc.x > dp_width + 1 || event->x > dp_width + 1){
    
    left_right = 1;
   }else{
     
    left_right = 0;
   }
     
   if(left_right == 1 && event->x < 0)
    left_right = 0;
   
  }
 }
 else if( event->button == 3 ){
  
  struct note* note = (struct note*)user_data;
  
  current_cm = note;
  
  guint tid = g_timeout_add(500, (GSourceFunc) draw_cursor_cb, (gpointer) note->comment);
  draw_cursor_cb(note->comment);
  draw_cursor = TRUE;
  
  GtkWidget *edit_win = 
           gtk_dialog_new_with_buttons ("Edit",
                                       NULL,
                                       GTK_DIALOG_MODAL,
                                       "_Cancel",
                                       GTK_RESPONSE_CANCEL,
                                       "_OK",
                                       GTK_RESPONSE_OK,
                                       "Delete this comment",
                                       GTK_RESPONSE_APPLY,
                                       "Property",
                                       GTK_RESPONSE_YES,
                                       NULL);
                                       
  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (edit_win));
      
  GtkWidget *view_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (content_area), view_box);

  GtkWidget *textview = gtk_text_view_new ();
  GtkTextBuffer *buffer = gtk_text_buffer_new (NULL);
         
  const char *contents = note->str;
  size_t len = strlen (contents);
  gtk_text_buffer_set_text (buffer, contents, len);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), buffer);
  
  bindex = len;
         
  g_signal_connect(G_OBJECT( buffer ), "changed",
     G_CALLBACK(textbuffer_changed_cb), note);
  
  g_signal_connect (G_OBJECT( buffer ), "notify::cursor-position", 
     G_CALLBACK (cursor_position_changed_cb), NULL);
   
  gtk_box_pack_start(GTK_BOX(view_box), textview, TRUE, TRUE, 0);
         
  gtk_widget_show_all(edit_win);

  int result;
  
  redialog:
  
  result = gtk_dialog_run(GTK_DIALOG(edit_win));
     
  if( result == GTK_RESPONSE_OK ){
    
   GtkTextIter startIter;
   GtkTextIter endIter;
   gtk_text_buffer_get_start_iter(buffer, &startIter);
   gtk_text_buffer_get_end_iter(buffer, &endIter);
   
   gtk_widget_destroy (edit_win);
   
  }
  else if( result == GTK_RESPONSE_APPLY ){
   
   //delete this comment
   
   gtk_widget_hide(note->comment);
   
   note->list.next->prev = note->list.prev;
   note->list.prev->next = note->list.next;
   list_del(&note->list);
   free(note->str);
   gtk_widget_destroy(note->comment);
   free(note);
   
   gtk_widget_destroy (edit_win);
       
  } // end of else if( result == GTK_RESPONSE_APPLY )
  else if( result == GTK_RESPONSE_CANCEL || result == GTK_RESPONSE_DELETE_EVENT ){
   gtk_widget_destroy (edit_win);
  }
  else if( result == GTK_RESPONSE_YES ){
  
   GtkWidget *cm_property_win = 
           gtk_dialog_new_with_buttons ("Property",
                                       NULL,
                                       GTK_DIALOG_MODAL,
                                       "_CANCEL",
                                       GTK_RESPONSE_CANCEL,
                                       "_OK",
                                       GTK_RESPONSE_OK,
                                       NULL);
   
   GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (cm_property_win));
      
   GtkWidget *cm_property_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   gtk_container_add (GTK_CONTAINER (content_area), cm_property_box);
   
   GtkWidget *cm_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
   
   GtkWidget *cm_font_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
   if( !note->property ){
  
    note->property = (struct cm_property*)malloc(sizeof(struct cm_property));
    note->property->font_desc = NULL;
    note->property->font_color = NULL;
    note->property->bg_color = NULL;
    
   }
   
   //cm font
   GtkWidget *font_button;
   
   if(!note->property->font_desc)
    font_button = gtk_font_button_new_with_font(pango_font_description_to_string(P_CM_FONT_DESC));
   else
    font_button = gtk_font_button_new_with_font(pango_font_description_to_string(note->property->font_desc));
   
   GtkWidget *font_label = gtk_label_new("Font:");
   
   g_signal_connect(G_OBJECT(font_button), "font_set",
                    G_CALLBACK(cm_font_changed), note);
   
   gtk_box_pack_start(GTK_BOX(cm_font_box), font_label, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX(cm_font_box), font_button, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX(cm_vbox), cm_font_box, TRUE, TRUE, 0);
   
   //cm_font
   
   //cm_font_color
   GtkWidget *cm_font_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
   GtkWidget *cm_font_color_label = gtk_label_new("Font Color:");
   
   cairo_surface_t * surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
   cairo_t *cr = cairo_create(surface);
   
   if( note->property->font_color ){
    
    cairo_set_source_rgba (cr, note->property->font_color->color.red, 
                               note->property->font_color->color.green, 
                               note->property->font_color->color.blue, 
                               note->property->font_color->color.alpha);
   }
   else{
    cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                               P_CM_FONT_COLOR->color.green, 
                               P_CM_FONT_COLOR->color.blue, 
                               P_CM_FONT_COLOR->color.alpha);
   }
   
   cairo_paint(cr);
   
   GtkWidget *cm_font_color_button = gtk_button_new();
   
   GtkWidget *cm_font_color_button_Image = gtk_image_new_from_surface(surface);
   gtk_button_set_image(GTK_BUTTON(cm_font_color_button),cm_font_color_button_Image); 
   
   gtk_widget_set_can_focus(cm_font_color_button, FALSE);
   
   g_signal_connect(G_OBJECT(cm_font_color_button), "clicked",
                  G_CALLBACK(cm_font_color_set_cb), note); 
   
   gtk_box_pack_start(GTK_BOX(cm_font_color_box), cm_font_color_label, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX(cm_font_color_box), cm_font_color_button, TRUE, TRUE, 0);
   
   gtk_box_pack_start(GTK_BOX(cm_vbox), cm_font_color_box, TRUE, TRUE, 0);
   
   //cm_font_color
   
   //cm_background_color
   
   GtkWidget *cm_bg_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
   GtkWidget *cm_bg_color_label = gtk_label_new("Background Color:");
   
   if(surface)
    cairo_surface_destroy (surface);
  
   if(cr)
    cairo_destroy (cr);
   
   surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
   cr = cairo_create(surface);
  
   if( note->property->bg_color ){
    cairo_set_source_rgba (cr, note->property->bg_color->color.red, 
                               note->property->bg_color->color.green, 
                               note->property->bg_color->color.blue, 
                               note->property->bg_color->color.alpha);
   }
   else{
    cairo_set_source_rgba (cr, P_CM_BG_COLOR->color.red, 
                               P_CM_BG_COLOR->color.green, 
                               P_CM_BG_COLOR->color.blue, 
                               P_CM_BG_COLOR->color.alpha);
   }
   
   cairo_paint(cr);
   
   GtkWidget *cm_bg_color_button = gtk_button_new();
   
   GtkWidget *cm_bg_color_button_Image = gtk_image_new_from_surface(surface);
   
   gtk_button_set_image(GTK_BUTTON(cm_bg_color_button),cm_bg_color_button_Image); 
   
   if(surface)
    cairo_surface_destroy (surface);
  
   if(cr)
    cairo_destroy (cr);
   
   gtk_widget_set_can_focus(cm_bg_color_button, FALSE);
   
   g_signal_connect(G_OBJECT(cm_bg_color_button), "clicked",
                  G_CALLBACK(cm_bg_color_set_cb), note);
   
   gtk_box_pack_start(GTK_BOX(cm_bg_color_box), cm_bg_color_label, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX(cm_bg_color_box), cm_bg_color_button, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX(cm_vbox), cm_bg_color_box, TRUE, TRUE, 0);
   
   //cm_background_color
   
   gtk_box_pack_start(GTK_BOX(cm_property_box), cm_vbox, TRUE, TRUE, 0);
   
   gtk_widget_show_all(cm_property_win);

   int result = gtk_dialog_run(GTK_DIALOG(cm_property_win));
   
   if( result == GTK_RESPONSE_CANCEL || result == GTK_RESPONSE_DELETE_EVENT ){
    gtk_widget_destroy (cm_property_win);
   }
   else if( result == GTK_RESPONSE_OK ){
    gtk_widget_destroy (cm_property_win);
   }
  
   goto redialog;
   
  }
  
  current_cm = NULL;
  g_source_remove(tid);
  draw_cursor = FALSE;
  pre_cursor_pos = -1;
  
 }
 
 return TRUE;
}

static gboolean
comment_button_motion_cb (GtkWidget *widget, GdkEventMotion *event, gpointer user_data){
 
 if( comment_click == 1 ){
  GtkAllocation alloc;
  gtk_widget_get_allocation (widget, &alloc);
  
  gint newx, newy;
  
  newx = (gint)((event->x - lstart_x)) + alloc.x;
  newy = (gint)(event->y- lstart_y) + alloc.y;
  
  gtk_layout_move (GTK_LAYOUT (layout), widget, newx, newy);
  gdk_window_invalidate_rect (gtk_layout_get_bin_window (GTK_LAYOUT (layout)), NULL, FALSE);
  
 }
 
 return TRUE;
}

static gboolean
comment_button_release_cb (GtkWidget *widget, GdkEventButton *event, gpointer user_data){
 
 if( comment_click == 1 ){
  
  struct note* note = (struct note*)user_data;
  
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
  
  GtkAllocation comment_alloc;
  gtk_widget_get_allocation (widget, &comment_alloc);
  
  if(dual_page_mode){
   
   int pre_left_right = left_right;
   
   GtkAllocation da_alloc, lda_alloc;
   
   gtk_widget_get_allocation (draw_area, &da_alloc);
   gtk_widget_get_allocation (ldraw_area, &lda_alloc);
  
   int dp_width = lda_alloc.width;
  
   if( comment_alloc.x > dp_width + 1 || event->x > dp_width + 1){
      
    left_right = 1;
   }else{
     
    left_right = 0;
   }
     
   if(left_right == 1 && event->x < 0)
    left_right = 0;
  
   if(left_right){ // right_page
    note->page_num = current_page_num + 2;
   }else{ // left page
    note->page_num = current_page_num + 1;  
   }
   if( pre_left_right == 0 && left_right == 1 ){ // comment is moved from left page to right page
    
    //get rcurrent_nc
    if( !rcurrent_nc ){
   
     struct note_cache *tmp = note_cache->next;
     
     note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
     note_cache->next->next = tmp;
     tmp = note_cache->next;
  
     INIT_LIST_HEAD(&tmp->CM_HEAD);
     INIT_LIST_HEAD(&tmp->HR_HEAD);
     tmp->page_num = current_page_num + 2;
     
     rcurrent_nc = tmp;
   
     tmp = note_cache->next;
   
     while( tmp ){
    
      tmp = tmp->next;
    
     } 
   
    }
    //get rcurrent_nc
    
    struct list_head *tmp;
    struct note *cm_entry = NULL;
    struct list_head *q;
  
    list_for_each_safe(tmp, q, &current_nc->CM_HEAD){  
  
     cm_entry = list_entry(tmp, struct note, list);
     
     if( cm_entry == note ){
      
      struct list_head *prev, *next;
      prev = cm_entry->list.prev;
      next = cm_entry->list.next;
      
      list_add_tail(&(note->list), &rcurrent_nc->CM_HEAD);
      
      prev->next = next;
      next->prev = prev;
      
      break;
     }
     
    }
    
   }
   else if( (pre_left_right == 1 && left_right == 0) || (pre_left_right == 0 && left_right == 0) ){ // comment being moved from right page to left page or move from right page to left page in the begining
   
    if( rcurrent_nc ){
    
     //get current_nc
     if( !current_nc ){
   
      struct note_cache *tmp = note_cache->next;
     
      note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      note_cache->next->next = tmp;
      tmp = note_cache->next;
  
      INIT_LIST_HEAD(&tmp->CM_HEAD);
      INIT_LIST_HEAD(&tmp->HR_HEAD);
      tmp->page_num = current_page_num + 1;
     
      current_nc = tmp;
   
     }
     //get current_nc
    
    
     struct list_head *tmp;
     struct note *cm_entry = NULL;
     struct list_head *q;
  
     list_for_each_safe(tmp, q, &rcurrent_nc->CM_HEAD){  
  
      cm_entry = list_entry(tmp, struct note, list);
     
      if( cm_entry == note ){
      
       struct list_head *prev, *next;
       prev = cm_entry->list.prev;
       next = cm_entry->list.next;
      
       list_add_tail(&(note->list), &current_nc->CM_HEAD);
      
       prev->next = next;
       next->prev = prev;
      
       break;
      }
     
     }//end of list_for_each_safe(tmp, q, &rcurrent_nc->CM_HEAD)
    
    }//end of if( rcurrent_nc )
    
   }
   
  }// end of if(dual_page_mode)
  
  if(!dual_page_mode){
  
   if( da_width >= sw_alloc.width ){
    note->x = (gint)((event->x - lstart_x + comment_alloc.x)/zoom_factor)+1;
   }
   else{
    note->x = (gint)((event->x - lstart_x + comment_alloc.x - width_offset )/zoom_factor)+1;
   }
   
   if( da_height >= sw_alloc.height ){
    note->y = (gint)((event->y - lstart_y + comment_alloc.y)/zoom_factor)+1;
   }
   else{
    note->y = (gint)((event->y - lstart_y + comment_alloc.y - height_offset)/zoom_factor)+1;
   }
   
  }
  else{ //dual-page mode
   
   GtkAllocation da_alloc, lda_alloc;
   
   gtk_widget_get_allocation (draw_area, &da_alloc);
   gtk_widget_get_allocation (ldraw_area, &lda_alloc);
   
   if( azoom_factor == 0.0 ){
    
    if( left_right ){ // right page
     
     note->x = (gint)((event->x - lstart_x + comment_alloc.x -lda_alloc.width)/zoom_factor)+1;
    }
    else{
     note->x = (gint)((event->x - lstart_x + comment_alloc.x)/zoom_factor)+1;
    }
    
    if( lda_alloc.height >= sw_alloc.height ){
     note->y = (gint)((event->y - lstart_y + comment_alloc.y)/zoom_factor)+1;
    }
    else{
     note->y = (gint)((event->y - lstart_y + comment_alloc.y - height_offset)/zoom_factor)+1;
    }
    
   }
   else if( azoom_factor < 0 ){ // left page
     
    if( left_right ){ // right page
     
     note->x = (gint)((event->x - lstart_x + comment_alloc.x -lda_alloc.width)/zoom_factor)+1;
     
    }
    else{
     note->x = (gint)((event->x - lstart_x + comment_alloc.x)/(-azoom_factor))+1;
     
    }
    
    if( lda_alloc.height >= sw_alloc.height ){
     
     if( left_right ){ // move to right page
      note->y = (gint)((event->y - lstart_y + comment_alloc.y)/zoom_factor)+1;
     }
     else{
      note->y = (gint)((event->y - lstart_y + comment_alloc.y)/(-azoom_factor))+1;
     }
     
    }
    else{
     
     double awidth, aheight, bwidth, bheight;
  
     PopplerPage* apage = poppler_document_get_page(doc, current_page_num);
 
     poppler_page_get_size(apage, &awidth, &aheight); 
 
     apage = poppler_document_get_page(doc, current_page_num+1);
  
     poppler_page_get_size(apage, &bwidth, &bheight); 
   
     g_object_unref (G_OBJECT (apage));
     apage = NULL;
    
     if( left_right ){ //move to right page
      
      int height = ( sw_alloc.height - 2 - (int)(bheight*zoom_factor) )/2;  
      
      if( height<0 )
       height = 0;
      
      note->y = (gint)((event->y - lstart_y + comment_alloc.y - height)/zoom_factor)+1;
     
     }
     else{
      
      int height;
      
      if( pre_sw_height == 0)
       height = ( sw_alloc.height - 2 - (int)(aheight*(-azoom_factor)) )/2;
      else
       height = ( sw_alloc.height - 100 - 2 - (int)(aheight*(-azoom_factor)) )/2;
      
      if(!full_screen) 
       note->y = (gint)((event->y - lstart_y + comment_alloc.y - height)/(-azoom_factor))+1;
      else
       note->y = (gint)((event->y - lstart_y + comment_alloc.y - height -50 )/(-azoom_factor))+1;
     
     }
    
    }
    
   }
   else{ // right page
    
    if( left_right ){ // right page
     
     note->x = (gint)((event->x - lstart_x + comment_alloc.x -lda_alloc.width)/azoom_factor)+1;
    }
    else{
     note->x = (gint)((event->x - lstart_x + comment_alloc.x)/zoom_factor)+1;
    }
    
    if( da_alloc.height >= sw_alloc.height ){
     
     if( left_right ){ // move to right page
      note->y = (gint)((event->y - lstart_y + comment_alloc.y)/azoom_factor)+1;
     }
     else{
      note->y = (gint)((event->y - lstart_y + comment_alloc.y)/zoom_factor)+1;
     }
     
    }
    else{
     
     double awidth, aheight, bwidth, bheight;
  
     PopplerPage* apage = poppler_document_get_page(doc, current_page_num);
 
     poppler_page_get_size(apage, &awidth, &aheight); 
 
     apage = poppler_document_get_page(doc, current_page_num+1);
  
     poppler_page_get_size(apage, &bwidth, &bheight); 
   
     g_object_unref (G_OBJECT (apage));
     apage = NULL;
     
     if( left_right ){ //move to right page
      
      int height;
      
      if( pre_sw_height == 0)
       height = ( sw_alloc.height - 2 - (int)(bheight*azoom_factor) )/2;
      else
       height = ( sw_alloc.height - 100 - 2 - (int)(bheight*azoom_factor) )/2;
      
      if( height<0 )
       height = 0;
      
      if(!full_screen)
       note->y = (gint)((event->y - lstart_y + comment_alloc.y - height)/azoom_factor)+1;
      else
       note->y = (gint)((event->y - lstart_y + comment_alloc.y - height - 50)/azoom_factor)+1;
       
     }
     else{
     
      int height = ( sw_alloc.height - 2 - (int)(aheight*zoom_factor) )/2;
      
      if( height<0 )
       height = 0;
     
      note->y = (gint)((event->y - lstart_y + comment_alloc.y - height)/zoom_factor)+1;
     
     }
    
    }
    
   }
   
  }
 
  if(dual_page_mode){
     
   if(note->y < 0 || event->y > (gint)((page_height*zoom_factor)+0.5)){
      
    note->x = 100;
    note->y = 100;
      
    gtk_layout_move(GTK_LAYOUT(layout), note->comment, note->x, note->y);
   }
     
  }
  
  comment_click = 0;
  
 }
 
 return TRUE;
}

static gboolean comment_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data){

 struct note *comment = (struct note *)user_data;
 
 if(comment->property){
 
  if(comment->property->bg_color){
  
   cairo_set_source_rgba (cr, comment->property->bg_color->color.red, 
                              comment->property->bg_color->color.green, 
                              comment->property->bg_color->color.blue,
                              comment->property->bg_color->color.alpha);
  }
  else{
   cairo_set_source_rgba (cr, P_CM_BG_COLOR->color.red, 
                              P_CM_BG_COLOR->color.green, 
                              P_CM_BG_COLOR->color.blue, 
                              P_CM_BG_COLOR->color.alpha);
  }
  
 }
 else{
  
  cairo_set_source_rgba (cr, P_CM_BG_COLOR->color.red, 
                             P_CM_BG_COLOR->color.green, 
                             P_CM_BG_COLOR->color.blue, 
                             P_CM_BG_COLOR->color.alpha);
  
 }
  
 cairo_paint(cr);

 PangoLayout *layout;
 
 layout = pango_cairo_create_layout (cr);
 
 if( comment->property ){//->font_desc
  
  if( comment->property->font_desc ){
   
   PangoFontDescription *tmp_font_desc = pango_font_description_copy(comment->property->font_desc);
   if( !dual_page_mode){
    pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
   }
   else{ // dual_page_mode
   
    GtkAllocation da_alloc, lda_alloc, cm_alloc;

    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
    
    if( azoom_factor == 0.0 ){
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
    }
    else if( azoom_factor < 0 ){ //left page
     
     gtk_widget_get_allocation (widget, &cm_alloc);
      
     if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*(-azoom_factor));
     }
     else{
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
     }
     
    }
    else{ //right page
      
     gtk_widget_get_allocation (widget, &cm_alloc);
      
     if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
     }
     else{
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*azoom_factor);
     }
     
    }
   
   }
   
   pango_layout_set_font_description (layout, tmp_font_desc);
  
   pango_font_description_free (tmp_font_desc);
  
  }else{
  
   PangoFontDescription *tmp_font_desc = pango_font_description_copy(P_CM_FONT_DESC);
   
   if( !dual_page_mode ){
   
    pango_font_description_set_absolute_size (tmp_font_desc, 
    pango_font_description_get_size(P_CM_FONT_DESC)*zoom_factor);
    
   }
   else{
  
    GtkAllocation da_alloc, lda_alloc, cm_alloc;

    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
     
    if( azoom_factor == 0.0 ){
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
    }
    else if( azoom_factor < 0 ){ //left page
     
     gtk_widget_get_allocation (widget, &cm_alloc);
      
     if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*(-azoom_factor));
     }
     else{
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
     }

    }
    else{ //right page
      
     gtk_widget_get_allocation (widget, &cm_alloc);
      
     if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
     }
     else{
      pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*azoom_factor);
     }
      
    
    }
   
   }
    
   pango_layout_set_font_description (layout, tmp_font_desc);
  
   pango_font_description_free (tmp_font_desc);
  
  }
 
 }
 else{
  
  PangoFontDescription *tmp_font_desc = pango_font_description_copy(P_CM_FONT_DESC);
  
  if( !dual_page_mode ){
  
   pango_font_description_set_absolute_size (tmp_font_desc, 
   pango_font_description_get_size(P_CM_FONT_DESC)*zoom_factor);
  }
  else{
   
   GtkAllocation da_alloc, lda_alloc, cm_alloc;

   gtk_widget_get_allocation (draw_area, &da_alloc);
   gtk_widget_get_allocation (ldraw_area, &lda_alloc);
   
   if( azoom_factor == 0.0 ){
    pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
   }
   else if( azoom_factor < 0 ){ // left page
    
    gtk_widget_get_allocation (widget, &cm_alloc);
      
    if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*(-azoom_factor));
    }
    else{
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
    }
   
   }
   else{ // right page
    
    gtk_widget_get_allocation (widget, &cm_alloc);
      
    if( cm_alloc.x > 0 && cm_alloc.x<=lda_alloc.width ){
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
    }
    else{
     pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*azoom_factor);
    }
  
   }

  }
  
  pango_layout_set_font_description (layout, tmp_font_desc);
  
  pango_font_description_free (tmp_font_desc);
  
 }
 
 pango_layout_set_text (layout, comment->str, -1);
 
 if(comment->property){
 
  if( comment->property->font_color ){
 
   cairo_set_source_rgba (cr, comment->property->font_color->color.red, 
                              comment->property->font_color->color.green, 
                              comment->property->font_color->color.blue, 
                              comment->property->font_color->color.alpha);
   
  }else{

   cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                              P_CM_FONT_COLOR->color.green, 
                              P_CM_FONT_COLOR->color.blue, 
                              P_CM_FONT_COLOR->color.alpha);
   
  }
  
 }
 else{

  cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                             P_CM_FONT_COLOR->color.green, 
                             P_CM_FONT_COLOR->color.blue, 
                             P_CM_FONT_COLOR->color.alpha);
 
 } 
 
 cairo_move_to (cr, 0.0, 0.0);
 pango_cairo_show_layout (cr, layout);
 
 int width, height;
 
 pango_layout_get_pixel_size(layout ,&width, &height);
 
 gtk_widget_set_size_request(comment->comment, width, height);
 
 //draw cursor on gtkdrawingarea when editing a comment
 if( current_cm && current_cm->comment == widget ){
  
  cairo_set_line_width(cr, 2.0);
  
  if(draw_cursor)
   cairo_set_source_rgba (cr, 0, 0, 0,1);
  else
   cairo_set_source_rgba (cr, 1, 1, 1,0);
  
  PangoRectangle bpos;
  
  pango_layout_index_to_pos ( layout,
                              bindex,
                              &bpos);
 
  if( str_pos >= pre_str_pos ){ 
   
   if( str_pos == strlen(comment->str) ){ 
    cairo_move_to(cr, (bpos.x+bpos.width)/PANGO_SCALE, bpos.y/PANGO_SCALE);
    cairo_line_to(cr, (bpos.x+bpos.width)/PANGO_SCALE, (bpos.y+ bpos.height)/PANGO_SCALE);
   }
   else{
    cairo_move_to(cr, (bpos.x)/PANGO_SCALE, bpos.y/PANGO_SCALE);
    cairo_line_to(cr, (bpos.x)/PANGO_SCALE, (bpos.y+ bpos.height)/PANGO_SCALE);
   }
   
  }
  else if( str_pos < pre_str_pos ){
  
   if( str_pos > 0 ){
    cairo_move_to(cr, (bpos.x)/PANGO_SCALE, bpos.y/PANGO_SCALE);
    cairo_line_to(cr, (bpos.x)/PANGO_SCALE, (bpos.y+ bpos.height)/PANGO_SCALE);
   }
   else{
    cairo_move_to(cr, 5, bpos.y/PANGO_SCALE);
    cairo_line_to(cr, 5, (bpos.y+ bpos.height)/PANGO_SCALE);
   }
   
  }
  
  cairo_stroke(cr);
 }
 
 g_object_unref (layout);
 
 return TRUE;
 
}

void init_note(void){
 
 note_cache = (struct note_cache*)malloc(sizeof(struct note_cache));
 INIT_LIST_HEAD(&note_cache->CM_HEAD);
 INIT_LIST_HEAD(&note_cache->HR_HEAD);
 note_cache->page_num = -1;
 note_cache->next = NULL;
 
 current_nc = NULL;
 rcurrent_nc = NULL;
 
 draw_cursor = FALSE;
 
 invert_color = FALSE;
 
 comment_click = 0;
 
 P_CM_FONT_DESC = NULL;
 P_HR_COLOR = NULL;
 P_CM_FONT_COLOR = P_CM_BG_COLOR = NULL;
 
 gchar *full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+5+2);

 strcpy(full_path, file_path);
 strcat(full_path, "/");
 strcat(full_path, file_name);
 
 strcat(full_path, ".note");

 struct stat buffer;
 
 //file exists
 if( stat(full_path, &buffer) == 0 ){
 
  FILE *pfile;
  
  char line[1000];
  size_t len = 0;
  ssize_t read;
  
  pfile = fopen(full_path, "r");
  
  if(pfile == NULL)
   exit(EXIT_FAILURE);
 
  char *pch0 = NULL;
  
  char *pch, *pch1, *pch2, *pch3,*pch4, *pch5, *pch6;
 
  if( fgets(line, 1000, pfile) != NULL ){
   
   pch0 = strchr(line, ':');
   
   if(pch0)
    *pch0 = '\0';
   
   if(*line != 'P'){
    
    //for old format
    
    char *page_num = strchr(line, ':');
    
    if( page_num )
     current_page_num = atoi(page_num+1)-1;
    
    page_change();
   }
   else{
    
    //format
    //P:last visit page:invert_color:dual_page_mode:highlight_color:cm_font_description:cm_font_color:cm_bg_color:cm_transparency
    
    //new format 
    //P:last visit page:invert_color:dual_page_mode:cm_font_description:color_table
   
    //current page number
    if(pch0){
    
     pch = strchr(pch0+1, ':');
    
     if(pch){
     
      *pch = '\0';
      
      if(strlen(pch0+1)){
       current_page_num = atoi(pch0+1)-1;
      }
      else{
       current_page_num = 0;
      }
      
     }
     
     page = poppler_document_get_page(doc, current_page_num);
 
     if(!page) {
      printf("Could not open first page of document\n");
      g_object_unref(doc);
      exit(3);
     }
 
     poppler_page_get_size(page, &page_width, &page_height); 
 
     g_object_unref (G_OBJECT (page));
     page = NULL;
     
     draw_area = gtk_drawing_area_new();  
     ldraw_area = gtk_drawing_area_new();  
   
     da_width = (gint)((page_width*zoom_factor)+0.5);  
     da_height = (gint)((page_height*zoom_factor)+0.5);  
   
     pre_da_width = da_width;  
   
     gtk_widget_set_size_request (draw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));  
   
     gtk_widget_hide(ldraw_area);  
      
     g_signal_connect (G_OBJECT (draw_area), "draw",   
                       G_CALLBACK (draw_area_draw),   
                       NULL);  
   
     g_signal_connect (G_OBJECT (ldraw_area), "draw",   
                       G_CALLBACK (draw_area_draw),   
                       NULL); 
       
     layout = gtk_layout_new(NULL, NULL);
 
     gint width, height;
 
     width = (gint)((page_width*zoom_factor)+0.5);
     height = (gint)((page_height*zoom_factor)+0.5);
     
     gtk_widget_set_size_request (ldraw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));  
     
     gtk_layout_set_size(GTK_LAYOUT(layout), width, height);  
     
     gtk_layout_put (GTK_LAYOUT (layout), ldraw_area, 0, 0);  
     gtk_layout_put (GTK_LAYOUT (layout), draw_area, 0, 0); 
     
    }
    
    //invert_color
    if(pch){
     pch1 = strchr(pch+1, ':');
     
     if(pch1){
      *pch1 = '\0';
     } 
   
    }
    
    //dual_page_mode
    if(pch1){
     pch2 = strchr(pch1+1, ':');
     
     if(pch2){
      *pch2 = '\0';
     } 
   
    }
    
    //cm_font_description
    if(pch2){
     
     pch3 = strchr(pch2+1, ':');
     
     if(pch3){
      *pch3 = '\0';
      
      if(strlen(pch2+1)){
       P_CM_FONT_DESC = pango_font_description_from_string (pch2+1);
      }
      else{
       P_CM_FONT_DESC = G_CM_FONT_DESC;
      }
      
     }
     
    }
    
    //color_table
    if( pch3 ){
     
     pch4 = strchr(pch3+1, ':');
     
     if(pch4){
      *pch4 = '\0';
     
      if( strlen(pch3+1) == 0 ){
       P_HR_COLOR = G_HR_COLOR;
      }
      else{
       P_HR_COLOR = add_ct_entry(pch3+1); 
      }
      
     }
     
    }
    
    if( pch4 ){
     
     pch5 = strchr(pch4+1, ':');
     
     if(pch5){
      *pch5 = '\0';
      
      if( strlen(pch4+1) == 0 ){
       P_CM_FONT_COLOR = G_CM_FONT_COLOR;
      }
      else{
       P_CM_FONT_COLOR = add_ct_entry(pch4+1);
      }
      
     }
     
    }
    
    if( pch5 ){
     
     pch6 = strchr(pch5+1, ':');
     
     if(pch6){
      *pch6 = '\0';
      
      if( strlen(pch5+1) == 0 ){
       P_CM_BG_COLOR = G_CM_BG_COLOR;
      }
      else{
       P_CM_BG_COLOR = add_ct_entry(pch5+1);
      }
      
     }
    
    }
 
    while( pch6 ){
    
     pch5 = strchr(pch6+1, ':');
     
     if( pch5 ){
      *pch5 = '\0';
     
      add_ct_entry(pch6+1);
      
     }
    
     pch6 = pch5;
     
    }
  
    if(pch1){
     
     if( strlen(pch+1) ){
      if(*(pch+1) == '1')
       invert_color = TRUE;
      else
       invert_color = FALSE;
      
     }
     else{
      invert_color = FALSE;
      
     }
     
    }
    
    if(pch2){
     
     if( strlen(pch1+1) ){
      
      if( *(pch1+1) == '1' ){
       dual_page_mode = TRUE;
       
      }
      else
       dual_page_mode = FALSE;
      
     }
     else{
      dual_page_mode = FALSE;
     }
     
    }
    
   }
  
  }
  
  struct note_cache* nc_tmp = NULL;
  struct note_cache* nc_ptr = NULL;
  
  while ( fgets(line, 1000, pfile) != NULL) {
  
   pch0 = strchr(line, ':');
  
   if( *line == '0' || *line == '2' ){ //comment
    
    struct note *note = (struct note*)malloc(sizeof(struct note));
    INIT_LIST_HEAD(&note->list);
    note->comment = gtk_drawing_area_new();
    
    if( *line == '2'){ 
     pch = pch1 = pch2 = pch3 = pch4 = pch5 = NULL;
   
     //old version
     //0:15:305:561:test
     //   pch     pch2(pch3)
     //       pch1
     //new version
     //0:1:177:11:Serif Bold Italic 24.95703125px:comment
     //or
     //0:1:177:11::comment
     //new version
     //0:1:204:130:serif Bold 11px:rgba(0,0,0,0):rgba(255,255,0,0.5):New Comment
    
     //new version 
     //0:1:204:130:font_desc:cm_font_color:cm_bg_color:str
     //  pch    pch3                      pch5      pch2
     //     pch1                 pch4
    
     if(pch0)
      pch = strchr(pch0+1, ':');
    
     if(pch)
      pch1 = strchr(pch+1, ':');
    
     if(pch1)
      pch3 = strchr(pch1+1, ':');
    
     if(pch3){
      pch4 = strchr(pch3+1, ':');
     } 
    
     if(pch4){
      pch5 = strchr(pch4+1, ':');
     }
    
     if(pch5){
      pch2 = strchr(pch5+1, ':');
     }
     
     *pch = *pch1 = *pch2 = *pch3 = *pch4 = *pch5 = '\0';
    
     note->property = (struct cm_property*)malloc(sizeof(struct cm_property));
     note->property->font_desc = NULL;
     note->property->font_color = NULL;
     note->property->bg_color = NULL;
     
     //font description
     if( strlen(pch3+1) ){
     
      note->property->font_desc = pango_font_description_from_string (pch3+1);
      
     }
     
     //font color
     if( strlen(pch4+1) ){
     
      note->property->font_color = add_ct_entry(pch4+1);
      
     }
     
     //background color
     if( strlen(pch5+1) ){
     
      note->property->bg_color = add_ct_entry(pch5+1);
      
     }
     
    }//*line == '2'
    else{
     
     pch = pch1 = pch2 = NULL;
    
     if(pch0)
      pch = strchr(pch0+1, ':');
    
     if(pch)
      pch1 = strchr(pch+1, ':');
      
     if(pch1)
      pch2 = strchr(pch1+1, ':');
     
     *pch = *pch1 = *pch2 = '\0';
    
     note->property = NULL;
     
    }
    
    char new_line = 1;
    char *pch_tmp = strchr(pch2+1, '\n');
    *pch_tmp = '\0';
    
    pch_tmp = strchr(pch2+1, new_line);
    
    while( pch_tmp ){
     *pch_tmp = '\n';
      pch_tmp = strchr(pch_tmp+1, new_line);
    }
    
    note->x = atoi(pch+1);
    note->y = atoi(pch1+1);
    
    gtk_layout_put (GTK_LAYOUT (layout), note->comment, note->x, note->y);
     
    note->str = (char*)malloc(strlen(pch2+1)+1);
    strcpy(note->str, pch2+1);
    
    note->page_num = atoi(pch0+1);
    
    gtk_widget_add_events (note->comment, GDK_POINTER_MOTION_MASK | 
                                          GDK_BUTTON_PRESS_MASK   | 
                                          GDK_BUTTON_RELEASE_MASK |
                                          GDK_ENTER_NOTIFY_MASK   |   
                                          GDK_LEAVE_NOTIFY_MASK);
 
    g_signal_connect (G_OBJECT (note->comment), "draw", 
                      G_CALLBACK (comment_draw_cb), 
                      note);
 
    g_signal_connect (G_OBJECT (note->comment), "button-press-event", 
                      G_CALLBACK (comment_button_press_cb),note);
  
    g_signal_connect (G_OBJECT (note->comment), "motion-notify-event",
                      G_CALLBACK (comment_button_motion_cb), NULL);
  
    g_signal_connect (G_OBJECT (note->comment), "button-release-event",
                      G_CALLBACK (comment_button_release_cb), note);
   
    gtk_widget_queue_draw(note->comment);
    
    if( note_cache->next == NULL ){
    
     note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
     nc_ptr = note_cache->next;
     INIT_LIST_HEAD(&nc_ptr->CM_HEAD);
     INIT_LIST_HEAD(&nc_ptr->HR_HEAD);
     nc_ptr->page_num = note->page_num;
     nc_ptr->next = NULL;
     
     list_add_tail(&(note->list), &nc_ptr->CM_HEAD);
     
    }
    else{
     
     nc_tmp = note_cache->next;
     
     while( nc_tmp ){
      
      if( note->page_num == nc_tmp->page_num ){
       list_add_tail(&(note->list), &nc_tmp->CM_HEAD);
       break;
      }
      
      nc_tmp = nc_tmp->next;
     }
     
     if( !nc_tmp ){
      
      nc_ptr->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      nc_ptr = nc_ptr->next;
      INIT_LIST_HEAD(&nc_ptr->CM_HEAD);
      INIT_LIST_HEAD(&nc_ptr->HR_HEAD);
      nc_ptr->page_num = note->page_num;
      nc_ptr->next = NULL;
      
      list_add_tail(&(note->list), &nc_ptr->CM_HEAD);
      
     }
     
    }
    
    if( !current_nc ){
    
     if( current_page_num+1 == note->page_num ){
      current_nc = nc_ptr;
     }
     
    }
    
    if( dual_page_mode ){
     
     if( current_page_num+2 == note->page_num ){
      rcurrent_nc = nc_ptr;
     }
     
    }
    
    gtk_widget_hide(note->comment);

    if(note->page_num == current_page_num+1){
     gtk_widget_show(note->comment);
    }
    
   } // end of if(*line == '2')
   else if( *line == '1' ){ //highlight region
  
    //highlight region format :
    // <highlight region id>:<page number>:color:x:y:x+width:y+height
    //1:1:00bfff:88:82:213:100
    
    //new format
    //1:1:#FFFF00FF:88:82:213:100
    //  pch       pch1  pch3
    //              pch2    pch4
    //
    //or
    //1:1:88:82:213:100
    //  pch    pch2 
    //     pch1    pch3
    //
      
    pch = strchr(pch0+1, ':');
    pch1 = strchr(pch+1, ':');
    pch2 = strchr(pch1+1, ':');
    pch3 = strchr(pch2+1, ':');
    pch4 = strchr(pch3+1, ':');
    
    struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));
    
    hg->page_num = atoi(pch0+1);
    
    if(pch4){
    
     *pch = *pch1 = *pch2  = *pch3 = *pch4 = '\0';
     
     hg->x = atoi(pch1+1);
     hg->y = atoi(pch2+1);
     hg->width = atoi(pch3+1) - atoi(pch1+1);
     hg->height = atoi(pch4+1) - atoi(pch2+1);
     
    }else{
    
     *pch = *pch1 = *pch2  = *pch3 = '\0';
     
     hg->x = atoi(pch+1);
     hg->y = atoi(pch1+1);
     hg->width = atoi(pch2+1) - atoi(pch+1);
     hg->height = atoi(pch3+1) - atoi(pch1+1);
     
    } 
   
    GdkRGBA color;//movet to outside the loop
   
    if( strlen(pch+1) == 9 ){  //#FFFF00FF
     hg->ce = add_ct_entry(pch+1);
    }
    else if( strlen(pch+1) == 6 ){ //00bfff
    
     char *color_str = (char *)malloc(10);
     
     strcpy(color_str, "#");
     strcat(color_str, pch+1);
     strcat(color_str, "ff");
     
     hg->ce = add_ct_entry(color_str);
     
     free(color_str);
     
    }
    else{
     hg->ce = P_HR_COLOR;
    }
    
    if( note_cache->next == NULL ){
    
     note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
     nc_ptr = note_cache->next;
     INIT_LIST_HEAD(&nc_ptr->CM_HEAD);
     INIT_LIST_HEAD(&nc_ptr->HR_HEAD);
     nc_ptr->page_num = hg->page_num;
     nc_ptr->next = NULL;
     
     list_add_tail(&(hg->list), &nc_ptr->HR_HEAD);
     
    }
    else{
     
     nc_tmp = note_cache->next;
     
     while( nc_tmp ){
      
      if( hg->page_num == nc_tmp->page_num ){
       list_add_tail(&(hg->list), &nc_tmp->HR_HEAD);
       break;
      }
      
      nc_tmp = nc_tmp->next;
     }
     
     if( !nc_tmp ){
      
      nc_ptr->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      nc_ptr = nc_ptr->next;
      INIT_LIST_HEAD(&nc_ptr->CM_HEAD);
      INIT_LIST_HEAD(&nc_ptr->HR_HEAD);
      nc_ptr->page_num = hg->page_num;
      nc_ptr->next = NULL;
      
      list_add_tail(&(hg->list), &nc_ptr->HR_HEAD);
      
     }
     
    }
    
    if( !current_nc ){
    
     if( current_page_num+1 == hg->page_num ){
      current_nc = nc_ptr;
     }
     
    }
    
    if( dual_page_mode ){
     
     if( current_page_num+2 == hg->page_num ){
      rcurrent_nc = nc_ptr;
     }
     
    }
    
   } //end of if(*line == '1')

  } // end of while((read = getline(&line, &len, pfile)) != -1)
  
  //existing file without content
  
  if( !P_HR_COLOR ){
   P_HR_COLOR = G_HR_COLOR;
  }
 
  if( !P_CM_FONT_DESC ){
   P_CM_FONT_DESC = G_CM_FONT_DESC;
  }
  
  if( !P_CM_FONT_COLOR ){
   P_CM_FONT_COLOR = G_CM_FONT_COLOR;
  }
  
  if( !P_CM_BG_COLOR ){
   P_CM_BG_COLOR = G_CM_BG_COLOR;
  }
  
  if( current_page_num == -1 ){
   
   current_page_num = 0;
   
   page = poppler_document_get_page(doc, current_page_num);
 
   if(!page) {
    printf("Could not open first page of document\n");
    g_object_unref(doc);
    exit(3);
   }
 
   poppler_page_get_size(page, &page_width, &page_height); 
 
   g_object_unref (G_OBJECT (page));
   page = NULL;
   
  }
  
  fclose(pfile);
  
 } // end of if( stat(full_path, &buffer) == 0 )
 else{
 
  P_CM_FONT_DESC = G_CM_FONT_DESC;
  P_CM_FONT_COLOR = G_CM_FONT_COLOR;
  P_CM_BG_COLOR = G_CM_BG_COLOR;
  P_HR_COLOR = G_HR_COLOR;
  
  current_page_num = 0;
   
  page = poppler_document_get_page(doc, current_page_num);
 
  if(!page) {
   printf("Could not open first page of document\n");
   g_object_unref(doc);
   exit(3);
  }
 
  poppler_page_get_size(page, &page_width, &page_height); 
 
  g_object_unref (G_OBJECT (page));
  page = NULL;
  
 }
 
 char *page_str = (char*)malloc(100);
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 if(full_path)
  free(full_path);

}

void save_comment(void){
 gchar *full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+8+2);

 strcpy(full_path, file_path);

 strcat(full_path, "/");
 strcat(full_path, file_name);

 strcat(full_path, ".comment");
 
 FILE *pfile;
 pfile = fopen(full_path,"w");
 
 if( pfile != NULL ){
 
  struct note_cache *nc_tmp = NULL;
  
  if( note_cache )
   nc_tmp = note_cache->next;
  
  struct list_head *tmp;
  struct note *cm_entry;
  
  while( nc_tmp ){
  
   list_for_each(tmp, &nc_tmp->CM_HEAD){
  
    cm_entry = list_entry(tmp, struct note, list);
   
    fprintf(pfile, "%s\n", cm_entry->str);
   
   }
   
   nc_tmp = nc_tmp->next;
   
  }
  
  fclose(pfile);
 }//end of if( pfile != NULL )
 
}

void
save_comment_cb (GtkWidget* widget, gpointer data) {

 save_note();
 save_comment();

}

void save_note (void) {
 
 gchar *full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+5+2);

 strcpy(full_path, file_path);
 
 strcat(full_path, "/");
 strcat(full_path, file_name);
 
 strcat(full_path, ".note");
 
 FILE *pfile;
 pfile = fopen(full_path,"w");
  
 if( pfile != NULL ){
 
  char *font_desc_str;
  
  if( pango_font_description_equal(G_CM_FONT_DESC, P_CM_FONT_DESC) ){
   font_desc_str = "";
  }
  else{
   font_desc_str = pango_font_description_to_string(P_CM_FONT_DESC);
  }
  
  char color_table[500] = {0};
  
  char *color_str = (char*)malloc(10);
  
  struct color_table *ct_tmp = NULL;
  
  if( G_HR_COLOR != P_HR_COLOR ){
   
   sprintf(color_str, "#%02X%02X%02X%02X", (int)(P_HR_COLOR->color.red*255),  
                                           (int)(P_HR_COLOR->color.green*255), 
                                           (int)(P_HR_COLOR->color.blue*255),
                                           (int)(P_HR_COLOR->color.alpha*255)); 
   
   strcat(color_table, ":");
   strcat(color_table, color_str);
   
   ct_tmp = P_HR_COLOR->next;
   
  }
  else{
   
   strcat(color_table, ":");
   
  }
  
  if( G_CM_FONT_COLOR != P_CM_FONT_COLOR ){
   
   sprintf(color_str, "#%02X%02X%02X%02X", (int)(P_CM_FONT_COLOR->color.red*255),  
                                           (int)(P_CM_FONT_COLOR->color.green*255), 
                                           (int)(P_CM_FONT_COLOR->color.blue*255),
                                           (int)(P_CM_FONT_COLOR->color.alpha*255)); 
   
   strcat(color_table, ":");
   strcat(color_table, color_str);
   
   ct_tmp = P_CM_FONT_COLOR->next;
   
  }
  else{
   
   strcat(color_table, ":");
   
  }
  
  if( G_CM_BG_COLOR != P_CM_BG_COLOR ){
   
   sprintf(color_str, "#%02X%02X%02X%02X", (int)(P_CM_BG_COLOR->color.red*255),  
                                           (int)(P_CM_BG_COLOR->color.green*255), 
                                           (int)(P_CM_BG_COLOR->color.blue*255),
                                           (int)(P_CM_BG_COLOR->color.alpha*255)); 
   
   strcat(color_table, ":");
   strcat(color_table, color_str);
   
   ct_tmp = P_CM_BG_COLOR->next;
   
  }
  else{
   
   strcat(color_table, ":");
   
  }
  
  //rest of color enrties
  while( ct_tmp ){
   
   sprintf(color_str, "#%02X%02X%02X%02X", (int)(ct_tmp->color.red*255),  
                                           (int)(ct_tmp->color.green*255), 
                                           (int)(ct_tmp->color.blue*255),
                                           (int)(ct_tmp->color.alpha*255)); 
   
   strcat(color_table, ":");
   strcat(color_table, color_str);
   
   ct_tmp = ct_tmp->next;
   
  }
 
  if( color_str )
   free(color_str);
  
  //new format 
  //P:last visit page:invert_color:dual_page_mode:cm_font_description:color_table
  
  fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_page_num+1,
   //gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ),
   invert_color,
   //gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(dual_pageMi) ),
   dual_page_mode,
   font_desc_str, color_table);
 
  char *font_color_str, *bg_color_str, *font_str;
  
  font_color_str = bg_color_str = font_str = NULL;
  
  struct list_head *tmp;
  
  struct note_cache *nc_tmp = NULL;
  struct note *cm_entry;
  struct highlight_region *hr_entry;
  
  if( note_cache )
   nc_tmp = note_cache->next;
  
  while( nc_tmp ){
   
   list_for_each(tmp, &nc_tmp->CM_HEAD){
    
    cm_entry = list_entry(tmp, struct note, list);
   
    const gchar *content = cm_entry->str;
    
    if(cm_entry->property){
    
     if( cm_entry->property->font_color == P_CM_FONT_COLOR ){
      
      font_color_str = NULL;
      
     }
     else{
      
      if( cm_entry->property->font_color ){
       font_color_str = (char*)malloc(10);
   
       sprintf(font_color_str, "#%02X%02X%02X%02X", 
                              (int)(cm_entry->property->font_color->color.red*255),  
                              (int)(cm_entry->property->font_color->color.green*255), 
                              (int)(cm_entry->property->font_color->color.blue*255),
                              (int)(cm_entry->property->font_color->color.alpha*255)); 
      }
      else{
       font_color_str = NULL;
      } 
      
     }
     
     if( cm_entry->property->bg_color == P_CM_BG_COLOR ){
      bg_color_str = NULL;
     }
     else{
      
      if( cm_entry->property->bg_color ){
       
       bg_color_str = (char*)malloc(10);
   
       sprintf(bg_color_str, "#%02X%02X%02X%02X", 
                             (int)(cm_entry->property->bg_color->color.red*255), 
                             (int)(cm_entry->property->bg_color->color.green*255),  
                             (int)(cm_entry->property->bg_color->color.blue*255),
                             (int)(cm_entry->property->bg_color->color.alpha*255)); 
      
      }
      else{
       
        bg_color_str = NULL;
       
      }
      
     }
     
     if( cm_entry->property->font_desc == P_CM_FONT_DESC || !cm_entry->property->font_desc ){
      font_str = NULL;
     }
     else{
      font_str = pango_font_description_to_string(cm_entry->property->font_desc);
     }
     
     if( font_str && font_color_str && bg_color_str ){
     
      fprintf(pfile, "2:%d:%d:%d:%s:%s:%s:",
                     cm_entry->page_num, 
                     cm_entry->x, 
                     cm_entry->y, 
                     font_str,
                     font_color_str,
                     bg_color_str);
      }
      else if( font_str && font_color_str && !bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%s:%s::", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      font_str,
                      font_color_str);
       
      }
      else if( font_str && !font_color_str && bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%s::%s:",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      font_str,
                      bg_color_str);
       
      }
      else if( !font_str && font_color_str && bg_color_str ){
      
       fprintf(pfile, "2:%d:%d:%d::%s:%s:",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      font_color_str,
                      bg_color_str);
       
      }
      else if( font_str && !font_color_str && !bg_color_str ){
      
       fprintf(pfile, "2:%d:%d:%d:%s:::", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      font_str);
       
      }
      else if( !font_str && font_color_str && !bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d::%s::",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      font_color_str);
       
      }
      else if( !font_str && !font_color_str && bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:::%s:", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      bg_color_str);
       
      }
      else{
       fprintf(pfile, "0:%d:%d:%d:", cm_entry->page_num, cm_entry->x, cm_entry->y);
      }
      
    }
    else{
     fprintf(pfile, "0:%d:%d:%d:", cm_entry->page_num, cm_entry->x, cm_entry->y);
    }
    
    if( font_color_str ){
     free(font_color_str);
     font_color_str = NULL;
    }
    
    if( bg_color_str ){
     free(bg_color_str);
     bg_color_str = NULL;
    }
    
    if(font_str){
     free(font_str);
     font_str = NULL;
    }
    
    const gchar* b = content;
   
    while(*b != '\0'){

     if( *b == '\n'){
      
      if(*(b+1) != '\0')
       fprintf(pfile, "%c", 1);
       
     }
     else{
 
      fprintf(pfile, "%c", *b);

     }

     b++;
    }

    fprintf(pfile, "%c", '\n');
    
   }
   
   char* hr_color_str = NULL;
   
   //highlight region
   list_for_each(tmp, &nc_tmp->HR_HEAD){

    hr_entry = list_entry(tmp, struct highlight_region, list);
    
    if( hr_entry->ce != P_HR_COLOR ){
     
     if( hr_entry->ce ){
      
      hr_color_str = (char*)malloc(10);
   
      sprintf(hr_color_str, "#%02X%02X%02X%02X", (int)(hr_entry->ce->color.red*255),  
                                                 (int)(hr_entry->ce->color.green*255), 
                                                 (int)(hr_entry->ce->color.blue*255),
                                                 (int)(hr_entry->ce->color.alpha*255)); 
     }
      
    }
    
    //1:1:00bfff:88:82:213:100
    
    if( hr_color_str ){
     fprintf(pfile, "1:%d:%s:%d:%d:%d:%d\n", 
                    hr_entry->page_num, 
                    hr_color_str, 
                    hr_entry->x, 
                    hr_entry->y, 
                    hr_entry->x + hr_entry->width, 
                    hr_entry->y + hr_entry->height);
                    
     free(hr_color_str);
     hr_color_str = NULL;
    }
    else{
     fprintf(pfile, "1:%d:%d:%d:%d:%d\n", 
                    hr_entry->page_num, 
                    hr_entry->x, 
                    hr_entry->y, 
                    hr_entry->x + hr_entry->width, 
                    hr_entry->y + hr_entry->height);
    }
   
   }
   //highlight region
   
   nc_tmp = nc_tmp->next;
   
  }
 
  fclose(pfile);

 }// end of if( pfile != NULL )
 
 if(full_path)
  g_free(full_path);

}

void
save_note_cb (GtkWidget* widget, gpointer data) {

 save_note();

}

void 
add_comment (void) {

 GtkAdjustment *vadj =
  gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

 GtkWidget *new_label = gtk_drawing_area_new();
 gtk_widget_set_size_request (new_label, 180,14*zoom_factor);
 
 gtk_layout_put (GTK_LAYOUT (layout), new_label, (int)(100*zoom_factor), (int)(100*zoom_factor)+gtk_adjustment_get_value(vadj));
 
 gtk_widget_show(new_label);

 struct note *note = (struct note*)malloc(sizeof(struct note));
 INIT_LIST_HEAD(&note->list);
 note->comment = new_label;
 note->x = (int)(100*zoom_factor);
 note->y = (int)(100*zoom_factor)+gtk_adjustment_get_value(vadj);
 note->page_num = current_page_num + 1;
 
 note->property = NULL;
 
 note->str = (char*)malloc(strlen("New Comment")+1);
 strcpy(note->str, "New Comment");
 
 gtk_widget_add_events (new_label, GDK_POINTER_MOTION_MASK | 
                                   GDK_BUTTON_PRESS_MASK   | 
                                   GDK_BUTTON_RELEASE_MASK |
                                   GDK_ENTER_NOTIFY_MASK   |   
                                   GDK_LEAVE_NOTIFY_MASK);
 
 g_signal_connect (G_OBJECT (new_label), "draw", 
                   G_CALLBACK (comment_draw_cb), 
                   note);
 
 g_signal_connect (G_OBJECT (new_label), "button-press-event", 
                   G_CALLBACK (comment_button_press_cb),note);
  
 g_signal_connect (G_OBJECT (new_label), "motion-notify-event",
                   G_CALLBACK (comment_button_motion_cb), NULL);
  
 g_signal_connect (G_OBJECT (new_label), "button-release-event",
                   G_CALLBACK (comment_button_release_cb), note);

 if( current_nc ){
  list_add_tail(&(note->list), &current_nc->CM_HEAD);
 }
 else{
  
  struct note_cache *tmp = note_cache->next;
  note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
  note_cache->next->next = tmp;
  tmp = note_cache->next;
  
  INIT_LIST_HEAD(&tmp->CM_HEAD);
  INIT_LIST_HEAD(&tmp->HR_HEAD);
  tmp->page_num = note->page_num;
  
  list_add_tail(&(note->list), &tmp->CM_HEAD);
  
  current_nc = tmp;
  
 }
 
}

gboolean 
add_comment_cb (GtkWidget* widget, gpointer user_data) {

 add_comment();
 
 return TRUE;
}

void display_comment(struct list_head *cm_HEAD){
 
 struct list_head *tmp;
 struct note *cm_entry;
 
 list_for_each(tmp, cm_HEAD){
   
  cm_entry = list_entry(tmp, struct note, list);
  
  if(!dual_page_mode){
   
   gtk_widget_show(cm_entry->comment);
   gtk_widget_queue_draw (cm_entry->comment);
     
   gtk_layout_move(GTK_LAYOUT(layout),
                   cm_entry->comment,
                   (gint)(cm_entry->x*zoom_factor)+width_offset,
                   (gint)(cm_entry->y*zoom_factor)+height_offset);
   
  }else{ //dual-page mode
   
   double awidth, aheight, bwidth, bheight;
   
   PopplerPage* apage = poppler_document_get_page(doc, current_page_num);
 
   poppler_page_get_size(apage, &awidth, &aheight); 
 
   apage = poppler_document_get_page(doc, current_page_num+1);
  
   poppler_page_get_size(apage, &bwidth, &bheight); 
 
   GtkAllocation sw_alloc;
   gtk_widget_get_allocation(scrolled_window, &sw_alloc);
   
   GtkAllocation da_alloc, lda_alloc;
   gtk_widget_get_allocation(draw_area, &da_alloc);
   gtk_widget_get_allocation(ldraw_area, &lda_alloc);
   
   if( &(*cm_HEAD) == &current_nc->CM_HEAD ){ //left page
    
    if( azoom_factor == 0.0 ){
    
     int height;
    
     height = ( sw_alloc.height - 2 - (int)(bheight*zoom_factor) )/2;
      
      if(height < 0)
       height = 0;
     
     gtk_layout_move( GTK_LAYOUT(layout), 
                      cm_entry->comment, 
                      (gint)(cm_entry->x*zoom_factor),                       (gint)(cm_entry->y*zoom_factor)+height);
     
    }
    else{ //page with different size
    
     if( azoom_factor < 0 ){ //left page
      
      int height;
    
      height = ( sw_alloc.height - 2 - (int)(aheight*(-azoom_factor)) )/2;
      
      gtk_layout_move( GTK_LAYOUT(layout), 
                       cm_entry->comment, 
                       (gint)(cm_entry->x*(-azoom_factor)), 
                       (gint)(cm_entry->y*(-azoom_factor))+ldaa.y);
     
     } // end of if( azoom_factor < 0 ), left page
     else{ //zoom_factor > 0, left page
     
      gtk_layout_move( GTK_LAYOUT(layout), 
                       cm_entry->comment, 
                       (gint)(cm_entry->x*zoom_factor), 
                       (gint)(cm_entry->y*zoom_factor)+ldaa.y);  
      
     }
     
    }
   
   }
   else if( &(*cm_HEAD) == &rcurrent_nc->CM_HEAD ){ //right page
    
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation(scrolled_window, &sw_alloc);
     
    GtkAllocation da_alloc, lda_alloc, cm_alloc;
    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
    
    if( azoom_factor == 0.0 ){
     
     int height = ( sw_alloc.height - 2 - (int)(bheight*zoom_factor) )/2;
    
     if(height < 0)
      height = 0;
     
     gtk_layout_move( GTK_LAYOUT(layout), 
                      cm_entry->comment, 
                      (gint)(cm_entry->x*zoom_factor)+lda_alloc.width, 
                      (gint)(cm_entry->y*zoom_factor)+height);
     
    }
    else{
     
     if( azoom_factor < 0 ){
      
      gtk_layout_move( GTK_LAYOUT(layout), 
                        cm_entry->comment, 
                        (gint)(cm_entry->x*zoom_factor)+lda_alloc.width, 
                        (gint)(cm_entry->y*zoom_factor)+daa.y);
     
     }
     else{ //right page, azoom_factor >0
      
       gtk_layout_move( GTK_LAYOUT(layout), 
                          cm_entry->comment,                           (gint)(cm_entry->x*azoom_factor)+lda_alloc.width, 
                          (gint)(cm_entry->y*azoom_factor)+daa.y);
      
     
     }
     
    }
  
   }
   
   gtk_widget_hide(cm_entry->comment);
   gtk_widget_show(cm_entry->comment);
  
  }
  
 }//list_for_each(tmp, cm_HEAD

}
