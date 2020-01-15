/*
 * Copyright (C) 2017-2020 rehon2006, rehon2006@gmail.com
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
#include <cairo-pdf.h>

#include "note.h"
#include "page.h"
#include "gui.h"
#include "highlight.h"
#include "pdf.h"

struct note_cache* get_current_nc( struct note_cache *curr_nc ){
 
 struct note_cache *tmp_nc = NULL;
 
 if( curr_nc ){
  
  if( curr_nc->page_num == current_page_num+1 ){
   return curr_nc;
  }
  else if( curr_nc->page_num > current_page_num+1 ){
   
   tmp_nc = curr_nc->prev;
   
   while( tmp_nc != note_cache ){
    
    if( tmp_nc->page_num == current_page_num+1 ){
     prev_nc = tmp_nc->prev;
     return tmp_nc;
    }
    
    tmp_nc = tmp_nc->prev;
    
   }
   
   if( tmp_nc == note_cache ){
    prev_nc = note_cache;
    return NULL;
   }
   
  }
  else{
  
   tmp_nc = curr_nc->next;
   
   while( tmp_nc ){
    
    if( tmp_nc->page_num == current_page_num+1 ){
     prev_nc = tmp_nc->prev;
     return tmp_nc;
    }
    
    tmp_nc = tmp_nc->next;
    
   }
   
   if( !tmp_nc ){
    return NULL;
   }
   
  }
  
 }
 else{ //if (!curr_nc)
  
  if( prev_nc != note_cache ){
   
   tmp_nc = prev_nc;
   
   if( tmp_nc->page_num == current_page_num + 1 ){
    prev_nc = tmp_nc->prev;
    return tmp_nc;
   }
   else if( tmp_nc->page_num > current_page_num + 1 ){
    
    tmp_nc = prev_nc->prev;
    
    while ( tmp_nc->prev ){
     
     if( tmp_nc->page_num == current_page_num + 1 ){
      prev_nc = tmp_nc->prev;
      return tmp_nc;
     }
     
     tmp_nc = tmp_nc->prev;
     
    }
    
    return NULL;
    
   }
   else{ // tmp_nc->page_num < current_page_num + 1
    
    tmp_nc = prev_nc->next;
    
    while( tmp_nc ){
     
     if( tmp_nc->page_num == current_page_num + 1 ){
      prev_nc = tmp_nc->prev;
      return tmp_nc;
     }
     
     tmp_nc = tmp_nc->next;
     
    }
    
   }
   
  }
  else{
   
   if( !prev_nc->next ){
    return NULL;
   }
   else{
    
    tmp_nc = note_cache->next;
    
    while( tmp_nc ){
     
     if( tmp_nc->page_num == current_page_num+1 ){
      prev_nc = tmp_nc->prev;
      return tmp_nc;
     }
     
     tmp_nc = tmp_nc->next;
     
    }
    
   }
   
  }
  
 }
 
 return tmp_nc;
 
}

struct note_cache* add_nc_entry( struct note_cache *curr_nc){
 
 struct note_cache *nc_tmp = NULL;
 
 if( !note_cache->next ){ // first entry;
  
  note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
  nc_tmp = note_cache->next;
  
  nc_tmp->next = NULL;
  nc_tmp->prev = note_cache;
  
  INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
  INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
  
  //for single page mode
  nc_tmp->page_num = current_page_num + 1;
   
  nc_tmp->BP_HEAD = NULL;
  nc_tmp->BP_TAIL = NULL;
  
 }
 else{
  
  if( !curr_nc ){
   
   struct note_cache *node_nc;
   
   if( prev_nc == note_cache ){
    
    node_nc = note_cache->next;
    
    note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
    nc_tmp = note_cache->next;
    
    nc_tmp->next = node_nc;
    nc_tmp->prev = note_cache;
    
    INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
    INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
    
    nc_tmp->page_num = current_page_num + 1;
    
    //single page mode
    nc_tmp->page_num = current_page_num + 1;
    
    nc_tmp->BP_HEAD = NULL;
    nc_tmp->BP_TAIL = NULL;
    
   }
   else{
    
    if( prev_nc->page_num < current_page_num+1 ){
     
     node_nc = prev_nc->next;
     
     //prev_nc-->node_nc
     
     while( node_nc ){
      
      if( current_page_num+1 < node_nc->page_num ){
       
       prev_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
       nc_tmp = prev_nc->next;
       
       nc_tmp->next = node_nc;
       nc_tmp->prev = prev_nc;
       
       INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
       INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
    
       nc_tmp->page_num = current_page_num + 1;
       
       nc_tmp->BP_HEAD = NULL;
       nc_tmp->BP_TAIL = NULL;
      
       return nc_tmp;
       
      }
      
      prev_nc = prev_nc->next;
      node_nc = node_nc->next;
      
     }
     
     if( !node_nc ){ //add last entry
      
      prev_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
      nc_tmp = prev_nc->next;
      
      nc_tmp->next = node_nc;
      nc_tmp->prev = prev_nc;
       
      INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
      INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
    
      nc_tmp->page_num = current_page_num + 1;
      
      nc_tmp->BP_HEAD = NULL;
      nc_tmp->BP_TAIL = NULL;
      
      return nc_tmp;
      
     }
     
    }//end of if( prev_nc->page_num < current_page_num+1 )
    else if( current_page_num+1 < prev_nc->page_num ){
     
     node_nc = prev_nc;
     prev_nc = prev_nc->prev;
     
     //prev_nc-->node_nc
     
     while( prev_nc->prev ){
      
      if( prev_nc->page_num < current_page_num+1 ){
       
       prev_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
       nc_tmp = prev_nc->next;
       
       nc_tmp->next = node_nc;
       nc_tmp->prev = prev_nc;
       
       INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
       INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
       
       nc_tmp->page_num = current_page_num + 1;
       
       nc_tmp->BP_HEAD = NULL;
       nc_tmp->BP_TAIL = NULL;
      
       return nc_tmp;
       
      }
      
      prev_nc = prev_nc->prev;
      nc_tmp = nc_tmp->prev;
      
     }
     
     if( !prev_nc->prev ){ //prev_nc == note_cache
      
      nc_tmp = prev_nc->next;
      
      prev_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
      prev_nc->next->next = nc_tmp;
      prev_nc->next->prev = prev_nc;
      
      nc_tmp->prev = prev_nc->next;
      nc_tmp = prev_nc->next;
      
      INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
      INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
       
      nc_tmp->page_num = current_page_num + 1;
      
      nc_tmp->BP_HEAD = NULL;
      nc_tmp->BP_TAIL = NULL;
      
      return nc_tmp;
      
     }
     
    }
    
   }
    
  }
 
 }
 
 return nc_tmp;
 
}

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
 
 note->property->font_desc = pango_font_description_from_string (gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button)));
 
 PangoStyle pstyle = pango_font_description_get_style(note->property->font_desc);
 
 if( pstyle == PANGO_STYLE_ITALIC ){
  font_changed_italic = note;
 }
 
 font_changed = TRUE;
 
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
  
  gtk_container_set_border_width(GTK_CONTAINER (content_area), 0);
  
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
   
   //finish editing comment
   
   gtk_widget_destroy (edit_win);
   
   comment_buffer_changed = FALSE;
   
  }
  else if( result == GTK_RESPONSE_APPLY ){
   
   //delete this comment
   
   //aks before deleting it
   
   GtkWidget *qdialog;
   qdialog = gtk_message_dialog_new(GTK_WINDOW(edit_win),
               GTK_DIALOG_DESTROY_WITH_PARENT,
               GTK_MESSAGE_QUESTION,
               GTK_BUTTONS_YES_NO,
               "Do you want to delete this comment?");
   
   gtk_window_set_title(GTK_WINDOW(qdialog), "Question");
   int result = gtk_dialog_run(GTK_DIALOG(qdialog));
   
   if( result == GTK_RESPONSE_YES ){
    gtk_widget_destroy (edit_win);
    gtk_widget_destroy(qdialog);
   }
   else{
    gtk_widget_destroy(qdialog);
    goto redialog;
   }
   
   gtk_widget_hide(note->comment);
   
   if( !blank_page ){
    note->list.next->prev = note->list.prev;
    note->list.prev->next = note->list.next;
    list_del(&note->list);
   }
   else{ //if(blank_page)
    
    if( dual_page_mode ){
     
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
    
    struct bp_comment *prev_cmt = NULL;
    
    struct bp_comment *cmt;
    
    struct blank_page *tmp_bp = NULL;
    
    if( dual_page_mode ){
     
     if(!left_right){
      cmt = current_bp->comment;
      tmp_bp = current_bp;
     }
     else{
      cmt = rcurrent_bp->comment;
      tmp_bp = rcurrent_bp;
     }
     
    }
    else{
     
     cmt = current_bp->comment;
     tmp_bp = current_bp;
    }
    
    while( cmt ){
      
     if( cmt->comment == note ){
      
      if( !prev_cmt ){
       
       tmp_bp->comment = cmt->next;
       
      }
      else{
       prev_cmt->next = cmt->next;
      }
      
      free(cmt);
      
      break;
      
     }
     
     if( !prev_cmt )
      prev_cmt = cmt;
     else
      prev_cmt = prev_cmt->next;
      
     cmt = cmt->next;
       
    }
    
   }
   
   free(note->str);
   gtk_widget_destroy(note->comment);
   free(note);
   
   gtk_widget_destroy (edit_win);
       
  } // end of else if( result == GTK_RESPONSE_APPLY )
  else if( result == GTK_RESPONSE_CANCEL || result == GTK_RESPONSE_DELETE_EVENT ){
   //ask before deleting it
   
   GtkWidget *qdialog;
   qdialog = gtk_message_dialog_new(GTK_WINDOW(edit_win),
               GTK_DIALOG_DESTROY_WITH_PARENT,
               GTK_MESSAGE_QUESTION,
               GTK_BUTTONS_YES_NO,
               "Do you want to quit editing?");
               
   gtk_window_set_title(GTK_WINDOW(qdialog), "Question");
   
   int result = gtk_dialog_run(GTK_DIALOG(qdialog));
   
   if( result == GTK_RESPONSE_YES ){
    gtk_widget_destroy (edit_win);
    gtk_widget_destroy(qdialog);
   }
   else{
    gtk_widget_destroy(qdialog);
    goto redialog;
   }
   
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
    
    if(!blank_page) {
     
     //get rcurrent_nc
     if( !rcurrent_nc ){
     
      struct note_cache *tmp_ptr = note_cache->next;

      struct note_cache *min = note_cache;
     
      while( tmp_ptr ){
    
       if( note->page_num < tmp_ptr->page_num ){
    
        min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
        min->next->next = tmp_ptr;
        min->next->prev = min;
        tmp_ptr = min->next;
     
        break;
    
       }
    
       min = tmp_ptr;
       tmp_ptr = tmp_ptr->next;
    
      }
     
      if( !tmp_ptr ){
       
       //note_cache is empty
       min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
       tmp_ptr = min->next;
       tmp_ptr->next = NULL;
       tmp_ptr->prev = min;
       
      }
     
      INIT_LIST_HEAD(&tmp_ptr->CM_HEAD);
      INIT_LIST_HEAD(&tmp_ptr->HR_HEAD);
      tmp_ptr->page_num = note->page_num;
  
      tmp_ptr->BP_HEAD = NULL;
      tmp_ptr->BP_TAIL = NULL;
  
      rcurrent_nc = tmp_ptr;
      
      rcurrent_pe->curr_nc = tmp_ptr;
     
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
    
    }//end of if (!blank_page)
    else{
     
     //move a comment from pdf page (or blank page) to blank page (or pdf page) or from blank page to another blank page
     
     if( !current_bp && rcurrent_bp ){
      
      //move comment from pdf page to blank page
      
      note->list.next->prev = note->list.prev;
      note->list.prev->next = note->list.next;
      list_del(&note->list);
      
      note->page_num = current_pe->p_page+1;
      
      if( !rcurrent_bp->comment ){
       
       rcurrent_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       rcurrent_bp->comment->comment = note;
       rcurrent_bp->comment->next = NULL;
      
      }
      else{
    
       struct bp_comment *cmt = rcurrent_bp->comment;
    
       rcurrent_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       rcurrent_bp->comment->comment = note;
       rcurrent_bp->comment->next = cmt;
     
      }
      
     }// end of if( !current_bp && rcurrent_bp )
     else if( current_bp && !rcurrent_bp ){
      
      //move comment from blank page to pdf page
      
      struct bp_comment *prev_cmt = NULL;
      struct bp_comment *cmt = current_bp->comment;
    
      while( cmt ){
     
       if( cmt->comment == note ){
      
        if( !prev_cmt ){
         current_bp->comment = cmt->next;
        }
        else{
         prev_cmt->next = cmt->next;
        }
        
        break;
      
       }
     
       if( !prev_cmt )
        prev_cmt = cmt;
       else
        prev_cmt = prev_cmt->next;
      
       cmt = cmt->next;
     
      }
      
      if( cmt ){
       
       note->page_num = rcurrent_pe->p_page+1;
       
       if( !rcurrent_nc ){
       
        if( !current_nc->next ){
         
         current_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
         
         rcurrent_nc = current_nc->next;
         rcurrent_nc->next = NULL;
         rcurrent_nc->prev = current_nc;
         
         rcurrent_pe->curr_nc = rcurrent_nc;
         
         INIT_LIST_HEAD(&rcurrent_nc->CM_HEAD);
         INIT_LIST_HEAD(&rcurrent_nc->HR_HEAD);
         
         rcurrent_nc->BP_HEAD = rcurrent_nc->BP_TAIL = NULL;
         rcurrent_nc->page_num = rcurrent_pe->p_page+1;
         
        }
        else{
        
         struct note_cache *tmp_nc = current_nc->next;
         
         current_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
         
         rcurrent_nc = current_nc->next;
         rcurrent_nc->next =  tmp_nc;
         rcurrent_nc->prev = current_nc;
         tmp_nc->prev = rcurrent_nc;
         
         INIT_LIST_HEAD(&rcurrent_nc->CM_HEAD);
         INIT_LIST_HEAD(&rcurrent_nc->HR_HEAD);
         
         rcurrent_nc->BP_HEAD = rcurrent_nc->BP_TAIL = NULL;
         rcurrent_nc->page_num = rcurrent_pe->p_page+1;
         
        }
        
       }
       
       list_add_tail(&(note->list), &rcurrent_nc->CM_HEAD);
       
       free(cmt);
       
      }
      
     }
     else if( current_bp && rcurrent_bp ){
      
      //move comment for a blank page to next blank page
      
      //current_bp
      struct bp_comment *prev_cmt = NULL;
      struct bp_comment *cmt = current_bp->comment;
    
      while( cmt ){
     
       if( cmt->comment == note ){
      
        if( !prev_cmt ){
         current_bp->comment = cmt->next;
        }
        else{
         prev_cmt->next = cmt->next;
        }
        
        break;
      
       }
     
       if( !prev_cmt )
        prev_cmt = cmt;
       else
        prev_cmt = prev_cmt->next;
      
       cmt = cmt->next;
     
      }
      //current_bp
      
      //rcurrent_bp
      if( cmt ){
       
       if( !rcurrent_bp->comment ){
       
        rcurrent_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        rcurrent_bp->comment->comment = note;
        rcurrent_bp->comment->next = NULL;
      
       }
       else{
    
        struct bp_comment *cmt = rcurrent_bp->comment;
    
        rcurrent_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        rcurrent_bp->comment->comment = note;
        rcurrent_bp->comment->next = cmt;
     
       }
       
       free(cmt);
       
      }
      //rcurrent_bp
      
     }//end of if( current_bp && rcurrent_bp )
     
    }
    
   }
   else if( (pre_left_right == 1 && left_right == 0) || (pre_left_right == 0 && left_right == 0) ){ // comment being moved from right page to left page or move from right page to left page in the begining
    
    if(!blank_page){
    
     if( rcurrent_nc ){
    
      //get current_nc
      //there are 2 situations for current_nc being NULL;
      //1. no note_cache entry for corrensponding page
      //2. note_cache entry for corrensponding page exists, but current_nc does not point to it.
      //but in dual-page mode, if current_nc is NULL, it means that note_cache entry for corresponding page does not exist
      //but in this situation, I should try to get rcurrent_nc->prev
     
      if( !current_nc ){
      
       if( rcurrent_nc->prev ){
       
        rcurrent_nc->prev->next = (struct note_cache*)malloc(sizeof(struct note_cache));
        
        current_nc = rcurrent_nc->prev->next;
       
        current_pe->curr_nc = current_nc;
       
        current_nc->next = rcurrent_nc;
        current_nc->prev = rcurrent_nc->prev;
        
        INIT_LIST_HEAD(&current_nc->CM_HEAD);
        INIT_LIST_HEAD(&current_nc->HR_HEAD);
        current_nc->page_num = current_page_num + 1;
      
        current_nc->BP_HEAD = NULL;
        current_nc->BP_TAIL = NULL;
      
        prev_nc = current_nc->prev;
        
       }
      
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
    
    }//end of if(!blank_page)
    else{
    
     if( !current_bp && rcurrent_bp ){
      
      //move comment from right blank page to left pdf page
      
      //right blank page
      struct bp_comment *prev_cmt = NULL;
      struct bp_comment *cmt = rcurrent_bp->comment;
    
      while( cmt ){
     
       if( cmt->comment == note ){
      
        if( !prev_cmt ){
         rcurrent_bp->comment = cmt->next;
        }
        else{
         prev_cmt->next = cmt->next;
        }
        
        break;
      
       }
     
       if( !prev_cmt )
        prev_cmt = cmt;
       else
        prev_cmt = prev_cmt->next;
      
       cmt = cmt->next;
     
      }
      //right blank page
      
      //left pdf page
      if( cmt ){
       
       if( !current_nc ){
       
        if( rcurrent_nc->prev == note_cache ){
         
         rcurrent_nc->prev = (struct note_cache*)malloc(sizeof(struct note_cache));
         
         current_nc = rcurrent_nc->prev;
         current_nc->next = rcurrent_nc;
         current_nc->prev = note_cache;
         
         note_cache->next = current_nc;
         
         current_pe->curr_nc = current_nc;
         
         INIT_LIST_HEAD(&current_nc->CM_HEAD);
         INIT_LIST_HEAD(&current_nc->HR_HEAD);
         
         current_nc->BP_HEAD = current_nc->BP_TAIL = NULL;
         current_nc->page_num = current_pe->p_page;
         
        }
        else{
        
         struct note_cache *tmp_nc = rcurrent_nc->prev;
         
         rcurrent_nc->prev = (struct note_cache*)malloc(sizeof(struct note_cache));
         
         current_nc = rcurrent_nc->prev;
         current_nc->prev =  tmp_nc;
         current_nc->next = rcurrent_nc;
         tmp_nc->next = current_nc;
         
         INIT_LIST_HEAD(&current_nc->CM_HEAD);
         INIT_LIST_HEAD(&current_nc->HR_HEAD);
         
         current_nc->BP_HEAD = current_nc->BP_TAIL = NULL;
         current_nc->page_num = current_pe->p_page;
         
        }
        
       }
       
       list_add_tail(&(note->list), &current_nc->CM_HEAD);
      
       free(cmt);
       
      }
      //left pdf page
      
     }//end of if( !current_bp && rcurrent_bp )
     else if( current_bp && !rcurrent_bp ){
      
      //move comment from right pdf page to left blank page
      
      //right pdf page
      note->list.next->prev = note->list.prev;
      note->list.prev->next = note->list.next;
      list_del(&note->list);
      //right pdf page
      
      note->page_num = current_pe->p_page+1;
      
      //left blank page
      if( !current_bp->comment ){
       
       current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       current_bp->comment->comment = note;
       current_bp->comment->next = NULL;
      
      }
      else{
    
       struct bp_comment *cmt = current_bp->comment;
    
       current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       current_bp->comment->comment = note;
       current_bp->comment->next = cmt;
     
      }
      //left blank page
      
     }
     else if( current_bp && rcurrent_bp ){
      
      //move from right blank page to left blank page
      
      //rcurrent_bp
      struct bp_comment *prev_cmt = NULL;
      struct bp_comment *cmt = rcurrent_bp->comment;
    
      while( cmt ){
     
       if( cmt->comment == note ){
      
        if( !prev_cmt ){
         rcurrent_bp->comment = cmt->next;
        }
        else{
         prev_cmt->next = cmt->next;
        }
        
        break;
      
       }
     
       if( !prev_cmt )
        prev_cmt = cmt;
       else
        prev_cmt = prev_cmt->next;
      
       cmt = cmt->next;
     
      }
      //rcurrent_bp
      
      //current_bp
      if( cmt ){
       
       if( !current_bp->comment ){
       
        current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        current_bp->comment->comment = note;
        current_bp->comment->next = NULL;
      
       }
       else{
    
        struct bp_comment *cmt = current_bp->comment;
    
        current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        current_bp->comment->comment = note;
        current_bp->comment->next = cmt;
     
       }
       
       free(cmt);
       
      }
      //current_bp
      
     }
    
    }
     
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
  
     PopplerPage *apage = poppler_document_get_page(doc, current_page_num);
 
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
 
 struct color_table *tmp_bc;
 
 if(comment->property){
  
  if(comment->property->bg_color){
   tmp_bc = comment->property->bg_color;
  }
  else{
   tmp_bc = P_CM_BG_COLOR;
  }
  
 }
 else{
  tmp_bc = P_CM_BG_COLOR;
 }
 
 cairo_set_source_rgba (cr, tmp_bc->color.red, 
                            tmp_bc->color.green, 
                            tmp_bc->color.blue, 
                            tmp_bc->color.alpha);
 
 cairo_paint(cr);
 
 gboolean is_italic = FALSE;
 
 PangoLayout *layout;
 
 layout = pango_cairo_create_layout (cr);
 
 PangoFontDescription *tmp_font_desc;
 
 if( comment->property ){
  
  if( comment->property->font_desc )
   tmp_font_desc = pango_font_description_copy(comment->property->font_desc);
  else
   tmp_font_desc = pango_font_description_copy(P_CM_FONT_DESC);
 }
 else{
  tmp_font_desc = pango_font_description_copy(P_CM_FONT_DESC);
 }
 
 if( pango_font_description_get_style(tmp_font_desc) == PANGO_STYLE_ITALIC)
  is_italic = TRUE;
 
 if( !dual_page_mode){
   
  pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);
   
  char *font_str = pango_font_description_to_string(tmp_font_desc);
 
  free(font_str);
   
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
 
 pango_layout_set_text (layout, comment->str, -1);
 
 struct color_table *tmp_fc;
 
 if( comment->property ){
  
  if( comment->property->font_color )
   tmp_fc = comment->property->font_color;
  else
   tmp_fc = P_CM_FONT_COLOR;
  
 }
 else{
  tmp_fc = P_CM_FONT_COLOR;
 }
 
 cairo_set_source_rgba (cr, tmp_fc->color.red, 
                            tmp_fc->color.green, 
                            tmp_fc->color.blue, 
                            tmp_fc->color.alpha);
 
 cairo_move_to (cr, 0.0, 0.0);
 pango_cairo_show_layout (cr, layout);
 
 int width, height;
 
 pango_layout_get_pixel_size(layout ,&width, &height);
 
 GtkAllocation cm_alloc;
 gtk_widget_get_allocation (widget, &cm_alloc);
 
 gint cwidth, cheight;
 cwidth = (gint)(cm_alloc.width/zoom_factor);
 cheight = (gint)(cm_alloc.height/zoom_factor);
 
 PangoRectangle ink_rect, logical_rect;
 pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);
 
 comment->height = (gint)(logical_rect.height/zoom_factor+0.5);
 
 if(is_italic){
  
  comment->width = (gint)(ink_rect.width/zoom_factor+0.5);
  
  gtk_widget_set_size_request(comment->comment, 
  (gint)(comment->width*zoom_factor), (gint)(comment->height*zoom_factor));
  
 }else{
  
  if( ink_rect.width > logical_rect.width ){
  
   comment->width = (gint)(ink_rect.width/zoom_factor+0.5)+2;
   
   gtk_widget_set_size_request(comment->comment, 
  (gint)(comment->width*zoom_factor)-2, (gint)(comment->height*zoom_factor));
   
  }
  else{
   
   comment->width = (gint)(logical_rect.width/zoom_factor+0.5);
   
   gtk_widget_set_size_request(comment->comment, 
  (gint)(comment->width*zoom_factor), (gint)(comment->height*zoom_factor));
   
  }
  
 }
 
 font_changed = FALSE;
 
 //draw cursor on gtkdrawingarea when editing a comment
 if( current_cm && current_cm->comment == widget ){
  
  cairo_set_line_width(cr, 2.0);
  
  if(draw_cursor)
   cairo_set_source_rgba (cr, 0, 0, 0,1);
  else
   cairo_set_source_rgba (cr, 1, 1, 1,0);
  
  PangoRectangle bpos;
  
  pango_layout_index_to_pos ( layout, bindex, &bpos);
 
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

void
add_page_comment(void){
 
 struct pc_entry *pc_tmp = &PC_HEAD;
 
 if( pc_tmp->next == NULL ){
  
  pc_tmp->next = (struct pc_entry*)malloc(sizeof(struct pc_entry));
  pc_tmp->next->page_num = current_page_num+1;
  pc_tmp->next->str = NULL;
  pc_tmp->next->next = NULL;
  
  gtk_text_buffer_set_text (tbuffer, "", -1);
  
  current_pc = pc_tmp->next;
    
 }
 else{
  
  GtkTextIter startIter, endIter;
  gtk_text_buffer_get_start_iter(tbuffer, &startIter);
  gtk_text_buffer_get_end_iter(tbuffer, &endIter);
 
  char *string = gtk_text_buffer_get_text(tbuffer, &startIter, &endIter, TRUE);
  
  while( pc_tmp->next != NULL ){
   
   if( pc_tmp->next->page_num == current_page_num+1 ){
    
    if( pc_tmp->next->str ){
     
     gtk_text_buffer_set_text (tbuffer, pc_tmp->next->str, -1);
     
    }else{
    
     if(string){
      pc_tmp->next->str = (char *)malloc(strlen(string)+1);
      strcpy(pc_tmp->next->str, string);
     }
      
     gtk_text_buffer_set_text (tbuffer, "", -1);
    }
    
    current_pc = pc_tmp->next;
    
    return;
   }
   
   pc_tmp = pc_tmp->next;
  }
  
  gtk_text_buffer_set_text (tbuffer, "", -1);
  
  pc_tmp->next = (struct pc_entry*)malloc(sizeof(struct pc_entry));
  pc_tmp->next->page_num = current_page_num+1;
  pc_tmp->next->str = NULL;
  pc_tmp->next->next = NULL;
  
  current_pc = pc_tmp->next;
 }
 
}

void
save_page_comment(void){
 
 GtkTextIter startIter, endIter;
 gtk_text_buffer_get_start_iter(tbuffer, &startIter);
 gtk_text_buffer_get_end_iter(tbuffer, &endIter);
 
 char *string = gtk_text_buffer_get_text(tbuffer, &startIter, &endIter, TRUE);
 
 if( strlen(string) > 0 ){
  
  //use current_pc
  
  if( current_pc ){
   
   if(current_pc->str){
    
    if( !strcmp(current_pc->str, string) ){
     return;
    }
   
    free(current_pc->str);
   }
    
   current_pc->str = (char*)malloc(strlen(string)+1);
   strcpy(current_pc->str, string);
  }
  else{
   
   struct pc_entry *pc_tmp = &PC_HEAD;
   
   while( pc_tmp->next != NULL ){
    pc_tmp = pc_tmp->next;
   }
   
   pc_tmp->next = (struct pc_entry*)malloc(sizeof(struct pc_entry));
   pc_tmp->next->page_num = current_page_num;
   pc_tmp->next->str = (char*)malloc(strlen(string)+1);
   strcpy(pc_tmp->next->str, string);
   pc_tmp->next->next = NULL;
   
  }
  
 }
 
}

static void build_pages( void ){
 
 int curr_page_num;
 
 if(doc){
  
  if( !dual_page_mode ){
  
   if( !blank_page )
    curr_page_num = current_page_num;
   else 
    curr_page_num = current_page_num-1;
   
  }
  else{
  
   if( !blank_page )
    curr_page_num = current_page_num;
   else{
   
    if( current_bp && !current_bp->next)
     curr_page_num = current_page_num-1;
    else
     curr_page_num = current_page_num;
   
   }
 
  }
 }
 else{
  
  //blank file
  curr_page_num = current_bp->page_num;
  
 }
 
 struct note_cache *note_page = NULL;
 
 if( note_cache->next )
  note_page = note_cache->next;
 
 gint page_num;
 
 if(doc)
  page_num = poppler_document_get_n_pages(doc);
 else{
  
  page_num = 1;
  
 }
 
 int i;
 
 struct page_entry *pe = PAGES;
 struct page_entry *prev_pe = NULL;
 
 if( note_page && note_page->page_num == 0 && doc ){
  
  //blank pages before first pdf page
  
  PAGES = (struct page_entry *)malloc(sizeof(struct page_entry));
  PAGES->p_page = -1;
  PAGES->curr_nc = note_page;
  PAGES->curr_bp = note_page->BP_HEAD;
  PAGES->next = NULL;
  PAGES->prev = NULL;
  pe = PAGES;
  prev_pe = pe;
  
  if( current_bp == pe->curr_bp ){
   current_pe = pe;
  }
  
  //rest of blank pages
  //build blank pages
  struct blank_page *tmp_bp =  note_page->BP_HEAD->next;
   
  //this page contains blank page(s)
  while( tmp_bp ){
    
   pe->next = (struct page_entry *)malloc(sizeof(struct page_entry));
   pe = pe->next;
   pe->p_page = -1;
   pe->curr_nc = note_page;
   pe->curr_bp = tmp_bp;
   pe->next = NULL;
   pe->prev = prev_pe;
   prev_pe = pe;
     
   if( current_bp == tmp_bp ){
    current_pe = pe;
   } 
     
   tmp_bp = tmp_bp->next;
  }
  //rest of blank pages
  
  note_page = note_page->next;
  
 }
 
 for ( i = 0; i< page_num; i++){
  
  if( pe == NULL ){
   
   PAGES = (struct page_entry *)malloc(sizeof(struct page_entry));
   PAGES->p_page = i;
   PAGES->curr_nc = NULL;
   PAGES->curr_bp = NULL;
   PAGES->next = NULL;
   PAGES->prev = NULL;
   pe = PAGES;
   prev_pe = pe;
   
  }
  else{
   
   pe->next = (struct page_entry *)malloc(sizeof(struct page_entry));
   pe = pe->next;
   pe->p_page = i;
   pe->curr_nc = NULL;
   pe->curr_bp = NULL;
   pe->next = NULL;
   pe->prev = prev_pe;
   prev_pe = pe;
   
  }
  
  
  if( curr_page_num == i ){
   
   if( doc ){
   
    if( !current_bp ){
     current_pe = pe; 
    }
    
   }
   else{
    current_pe = pe;
   }
   
  }
  
  if( note_page ){
   
   if( i == note_page->page_num -1 ){
    
    pe->curr_nc = note_page;
   
    struct blank_page *tmp_bp =  note_page->BP_HEAD;
   
    //this page contains blank page(s)
    while( tmp_bp ){
    
     pe->next = (struct page_entry *)malloc(sizeof(struct page_entry));
     pe = pe->next;
     pe->p_page = i;
     pe->curr_nc = note_page;
     pe->curr_bp = tmp_bp;
     pe->next = NULL;
     pe->prev = prev_pe;
     prev_pe = pe;
     
     if( curr_page_num == i ){
      
      if( current_bp ){
       
       if( current_bp == tmp_bp ){
        current_pe = pe;
       }
       
      }
      
     }
     else if ( curr_page_num+1 == i && i+1 == page_num ){
      
      //for last page
      if( current_bp ){
       
       if( current_bp == tmp_bp ){
        current_pe = pe;
       }
       
      }
     }
     
     tmp_bp = tmp_bp->next;
    }
   
    note_page = note_page->next;
   
   }
   else{
    
    if( !doc ){ //blank file
     
     if( i == note_page->page_num ){
      
      pe->curr_nc = note_page;
      
      struct blank_page *tmp_bp =  note_page->BP_HEAD;
      
      pe->curr_bp = tmp_bp;
      
      if( pe->curr_bp == current_bp ){
       current_pe = pe;
      }
      
      tmp_bp = tmp_bp->next;
      
      while( tmp_bp ){
       
       pe->next = (struct page_entry *)malloc(sizeof(struct page_entry));
       pe = pe->next;
       pe->p_page = i;
       pe->curr_nc = note_page;
       pe->curr_bp = tmp_bp;
       pe->next = NULL;
       pe->prev = prev_pe;
       prev_pe = pe;
       
       //current_pe
       if( pe->curr_bp == current_bp ){
        current_pe = pe;
       }
       //current_pe
       
       tmp_bp = tmp_bp->next;
      
      }
      
     }
     
    }//end of if( !doc )
    
   }
   
  }
  
 }
 
}

void init_note(void){
 
 note_cache = (struct note_cache*)malloc(sizeof(struct note_cache));
 INIT_LIST_HEAD(&note_cache->CM_HEAD);
 INIT_LIST_HEAD(&note_cache->HR_HEAD);
 note_cache->BP_HEAD = NULL;
 note_cache->BP_TAIL = NULL;
 note_cache->page_num = -1;
 note_cache->next = NULL;
 note_cache->prev = NULL;
 
 current_nc = NULL;
 rcurrent_nc = NULL;
 prev_nc = note_cache;
 
 current_pc = NULL;
 
 current_bp = rcurrent_bp = NULL;
 
 draw_cursor = FALSE;
 
 invert_color = G_INVERT_COLORS;
 
 blank_page = FALSE;
 
 comment_click = 0;
 
 bp_cpn = -1;
 
 P_CM_FONT_DESC = NULL;
 P_HR_COLOR = NULL;
 P_CM_FONT_COLOR = P_CM_BG_COLOR = NULL;
 
 PC_HEAD.page_num = -1;
 PC_HEAD.str = NULL;
 PC_HEAD.next = NULL;
 
 PAGES = NULL;
 current_pe = rcurrent_pe = NULL;
 
 font_changed_italic = NULL;
 
 font_changed = FALSE;
 
 gchar *full_path = NULL;
 
 if( file_name && doc ){
 
  full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+5+2);
 
  strcpy(full_path, file_path);
  strcat(full_path, "/");
  strcat(full_path, file_name);
 
  strcat(full_path, ".note");
 }
 else if( file_name && !doc ){
  full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+2);
  strcpy(full_path, file_path);
  strcat(full_path, "/");
  strcat(full_path, file_name);
 }
 
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
  
  char *pch, *pch1, *pch2, *pch3, *pch4, *pch5, *pch6, *pch7, *pch8;
 
  //local setting
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
    //P:last visited page number:invert_color:dual_page_mode:highlight_color:cm_font_description:cm_font_color:cm_bg_color:cm_transparency
    
    //new format 
    //P:last visited page number:invert_color:dual_page_mode:cm_font_description:color_table
    
    //new format
    //P:last visited page number-last visited blank page number:invert_color:dual_page_mode:cm_font_description:color_table
    //pch0                                                    pch
    //pch1          pch2
    
    //current page number
    if(pch0){
     
     //format
     //current_page_num-current_blank_page_number
     
     pch = strchr(pch0+1, ':');
    
     if(pch){
     
      *pch = '\0';
      
      char *sep = strchr(pch0+1, '-');
      
      if( sep ){
       
       *sep = '\0';
       
       if( strlen(sep+1) ){ //last visited blanked page
        bp_cpn = atoi(sep+1);
        
        
        blank_page = TRUE;
       }
       
      }
      //else{ // older format
      
      if(strlen(pch0+1)){
       current_page_num = atoi(pch0+1)-1;
      }
      else{
       current_page_num = 0;
      }
       
     }
     
     if( current_page_num == -1 ){
      if(doc)
       page = poppler_document_get_page(doc, 0);
     }else
      page = poppler_document_get_page(doc, current_page_num);
 
     if(!page && doc) {
      printf("Could not open first page of document\n");
      g_object_unref(doc);
      exit(3);
     }
     
     if(doc)
      poppler_page_get_size(page, &page_width, &page_height); 
     
     if(page){
      g_object_unref (G_OBJECT (page));
      page = NULL;
     }
     
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
     
     if( !G_INVERT_COLORS ){
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
     else
      invert_color = TRUE;
     
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
  //local setting
  
  struct note_cache* nc_tmp = NULL;
  struct note_cache* nc_ptr = NULL;
  
  struct note_cache *curr_nc = NULL;
  
  GString *line_buf = NULL;
  char *tmp_line = NULL;
  
  int page_num;
  
  while ( fgets(line, 1000, pfile) != NULL ) {
  
   if(!tmp_line)
    tmp_line = line;
   
   pch0 = strchr(line, ':');
   
   //build note_cache
   if( pch0 ){
    
    pch = strchr( pch0+1,':' );
    
    if( pch ){
     
     *pch = '\0';
     
     page_num = atoi(pch0+1);
     
     if( note_cache->next == NULL ){
      
      note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      nc_ptr = note_cache->next;
      INIT_LIST_HEAD(&nc_ptr->CM_HEAD);
      INIT_LIST_HEAD(&nc_ptr->HR_HEAD);
      
      nc_ptr->BP_HEAD = NULL;
      nc_ptr->BP_TAIL = NULL;
      
      nc_ptr->page_num = page_num;
      nc_ptr->next = NULL;
      nc_ptr->prev = note_cache;
      
      curr_nc = nc_ptr;
      
     }
     else{ //if( note_cache->next != NULL ) 
      
      if( curr_nc->page_num > page_num ){
      
       nc_tmp = curr_nc->prev;
      
       while( nc_tmp->prev ){
       
        if( nc_tmp->page_num == page_num ){
         curr_nc = nc_tmp;
         break;
        }
        else if( nc_tmp->page_num < page_num ){
        
         nc_tmp->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
         nc_tmp->next->prev = nc_tmp;
         nc_tmp->next->next = curr_nc;
         
         nc_tmp = nc_tmp->next;
         
         curr_nc->prev = nc_tmp;
         
         INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
         INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
      
         nc_tmp->BP_HEAD = NULL;
         nc_tmp->BP_TAIL = NULL;
      
         nc_tmp->page_num = page_num;
         
         curr_nc = nc_tmp;
        
         break;
        
        }
       
        curr_nc = curr_nc->prev;
        nc_tmp = nc_tmp->prev;
       
       }
      
       if( !nc_tmp->prev ){ //head
      
        nc_tmp->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      
        nc_tmp->next->prev = nc_tmp;
        nc_tmp->next->next = curr_nc;
        
        nc_tmp = nc_tmp->next;
        
        curr_nc->prev = nc_tmp;
        
        INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
        INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
      
        nc_tmp->BP_HEAD = NULL;
        nc_tmp->BP_TAIL = NULL;
      
        nc_tmp->page_num = page_num;
        curr_nc = nc_tmp;
       
       }
      
      }//end of if( curr_nc->page_num > page_num )
      else if( curr_nc->page_num < page_num ){
      
       nc_tmp = curr_nc->next;
      
       if( nc_tmp ){
      
        while( nc_tmp->next ){
       
         if( nc_tmp->page_num == page_num ){
          curr_nc = nc_tmp;
          break;
         }
         else if( nc_tmp->page_num > page_num ){
        
          curr_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
          curr_nc->next->prev = curr_nc;
          curr_nc->next->next = nc_tmp;
          
          curr_nc = curr_nc->next;
          
          nc_tmp->prev = curr_nc;
          
          INIT_LIST_HEAD(&curr_nc->CM_HEAD);
          INIT_LIST_HEAD(&curr_nc->HR_HEAD);
        
          curr_nc->BP_HEAD = NULL;
          curr_nc->BP_TAIL = NULL;
      
          curr_nc->page_num = page_num;
        
          break;
        
         }
       
         curr_nc = curr_nc->next;
         nc_tmp = nc_tmp->next;
       
        }
      
        if( !nc_tmp->next ){ //last entry
       
         nc_tmp->next = (struct note_cache*)malloc(sizeof(struct note_cache));
         nc_tmp->next->next = NULL;
         nc_tmp->next->prev = nc_tmp;
         
         nc_tmp = nc_tmp->next;
        
         INIT_LIST_HEAD(&nc_tmp->CM_HEAD);
         INIT_LIST_HEAD(&nc_tmp->HR_HEAD);
        
         nc_tmp->BP_HEAD = NULL;
         nc_tmp->BP_TAIL = NULL;
        
         nc_tmp->page_num = page_num;
        
         curr_nc = nc_tmp;
        
        }
      
       }//end of if (nc_tmp)
       else{ // if (!nc_tmp)
       
        //if curr_nc is the last entry
       
        curr_nc->next = (struct note_cache*)malloc(sizeof(struct note_cache));
       
        curr_nc->next->next = NULL;
        curr_nc->next->prev = curr_nc;
        
        curr_nc = curr_nc->next;
        
        INIT_LIST_HEAD(&curr_nc->CM_HEAD);
        INIT_LIST_HEAD(&curr_nc->HR_HEAD);
       
        curr_nc->BP_HEAD = NULL;
        curr_nc->BP_TAIL = NULL;
        
        curr_nc->page_num = page_num;
        
       }
      
      }
      
     } // end of if( note_cache->next != NULL )
     
     if( !blank_page ){ 
      if( page_num == current_page_num+1 ){
       current_nc = curr_nc;
      }
     }
     
     *pch = ':';
     
    }//end of if(pch)
    
   }
   //build note_cache
   
   if( strlen(line) == 999 ){
    
    if( line_buf == NULL ){
     line_buf = g_string_new(NULL);
     
    }
     
    g_string_append(line_buf, line);
    
    tmp_line = line_buf->str;
     
    continue;
    
   }
   else if( line_buf != NULL && strlen(line) == 999 ){
   
    g_string_append(line_buf, line);
    
    continue; 
    
   }
   else if( *tmp_line == '0' || *tmp_line == '2' || *tmp_line == '5'){ //comment
   
    struct note *note = (struct note*)malloc(sizeof(struct note));
    INIT_LIST_HEAD(&note->list);
    note->comment = gtk_drawing_area_new();
    
    if( line_buf != NULL ){
     g_string_append(line_buf, line);
     tmp_line = line_buf->str;
     pch0 = strchr(tmp_line, ':');
    }else
     tmp_line = line;
    
    if( *tmp_line == '2' || *tmp_line == '5' ){
    
     pch = pch1 = pch2 = pch3 = pch4 = pch5 = pch6 = pch7 = NULL;
     
     if( *tmp_line == '5' )
      pch8 = NULL;
      
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
     //     pch1           pch4
     
     //for *tmp_line == '2'
     //new version 20190131
     //2:1:204:130:200:50:font_desc:cm_font_color:cm_bg_color:str
     //  pch     pch3   pch5                                pch7
     //     pch1     pch4
     //                           pch2          pch6
     //---------------------------------------------------------
     //  pch     pch6   pch3                                pch2
     //     pch1     pch7
     //                           pch4          pch5
     
     //for *tmp_line == '5'
     //5:1:1:204:130:200:50:font_desc:cm_font_color:cm_bg_color:str
     //    pch     pch6   pch3                                pch2
     //  pch8  pch1    pch7
     //                           pch4          pch5
     
     if(pch0){
      
      if( *tmp_line == '2' )
       pch = strchr(pch0+1, ':');
      else if(*tmp_line == '5') {
       
       pch8 = strchr(pch0+1, ':');
       
       if(pch8)
        pch = strchr(pch8+1, ':');
        
      }
      
     }
     
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
    
     if( pch5 ){
      pch2 = strchr(pch5+1, ':');
     }
     
     if( pch2 ){
      
      pch6 = strchr(pch2+1, ':');
      
      if( pch6 ){
       
       pch7 = strchr(pch6+1, ':');
       
       if( pch7 ){
        
        //7->2
        //6->5
        //then
        //7->4
        //6->3
        
        char *ctmp;
        ctmp = pch7;
        pch7 = pch2;
        pch2 = ctmp;
        
        ctmp = pch6;
        pch6 = pch5;
        pch5 = ctmp;
        
        ctmp = pch7;
        pch7 = pch4;
        pch4 = ctmp;
        
        ctmp = pch6;
        pch6 = pch3;
        pch3 = ctmp;
        
        *pch6 = *pch7 = '\0' ;
        
       }
       
      }
      
     }
     
     *pch = *pch1 = *pch2 = *pch3 = *pch4 = *pch5 = '\0';
     
     if( pch6 && pch7 ){
     
      note->width = atoi(pch6+1);
      note->height = atoi(pch7+1);
      
      if( *tmp_line == '5' ){
       *pch8 = '\0';
      }
      
     }
     
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
    else{ // *line == '0'
     
     //old version
     //0:15:305:561:test
     //   pch     pch2(pch3)
     //       pch1
     
     //new version
     //0:15:305:561:100:30:test
     //   pch           pch3
     //       pch1 
     //           pch2 pch4
     //------------------------
     //   pch           pch2
     //       pch1 
     //           pch3 pch4
     
     pch = pch1 = pch2 = pch3 = pch4 = NULL;
    
     if(pch0)
      pch = strchr(pch0+1, ':');
    
     if(pch)
      pch1 = strchr(pch+1, ':');
      
     if(pch1)
      pch2 = strchr(pch1+1, ':');
     
     if( pch2 ){
      
      pch4 = strchr(pch2+1, ':');
      
      if( pch4 ){
       
       pch3 = strchr(pch4+1, ':');
       
       if( pch3 ){
        
        char *ctmp = pch3;
        pch3 = pch2;
        pch2 = ctmp;
        
        *pch3 = *pch4 = '\0';
      
        note->width = atoi(pch3+1);
        note->height = atoi(pch4+1);
        
       }
       
      }
      
     }
     
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
    
    if( *tmp_line == '0' || *tmp_line == '2' )
     note->page_num = atoi(pch0+1);
    else if ( *tmp_line == '5' )
     note->page_num = atoi(pch8+1);
     
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
   
    if( *tmp_line == '0' || *tmp_line == '2' )
     list_add_tail(&(note->list), &curr_nc->CM_HEAD);
    else{ // *tmp_line == '5'
     
     if( !curr_nc->BP_HEAD ){
     
      curr_nc->BP_HEAD = (struct blank_page *)malloc(sizeof(struct blank_page));
      curr_nc->BP_HEAD->page_num = atoi(pch8+1);
     
      curr_nc->BP_HEAD->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
      
      curr_nc->BP_HEAD->comment->comment = note;
      curr_nc->BP_HEAD->comment->next = NULL;
      
      curr_nc->BP_HEAD->next = NULL;
      curr_nc->BP_HEAD->prev = NULL;
     
      curr_nc->BP_TAIL = curr_nc->BP_HEAD;
      
      if( curr_nc->BP_HEAD->page_num == bp_cpn ){
       
       if( current_page_num+1 == curr_nc->page_num )
        current_bp = curr_nc->BP_HEAD;
  
      }
     
     }
     else{
     
      struct blank_page *tmp_bp = curr_nc->BP_HEAD;
     
      int bp_pnum = atoi(pch8+1);
     
      while( tmp_bp ){
      
       if( tmp_bp->page_num == bp_pnum )
        break;
       
       tmp_bp = tmp_bp->next;
      
      }
      
      if( !tmp_bp ){ //new blank page
       curr_nc->BP_TAIL->next = (struct blank_page *)malloc(sizeof(struct blank_page));
   
       curr_nc->BP_TAIL->next->next = NULL;
       curr_nc->BP_TAIL->next->prev = curr_nc->BP_TAIL;
      
       curr_nc->BP_TAIL = curr_nc->BP_TAIL->next;
       curr_nc->BP_TAIL->page_num = atoi(pch8+1);
       
       curr_nc->BP_TAIL->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       
       curr_nc->BP_TAIL->comment->comment = note;
       curr_nc->BP_TAIL->comment->next = NULL;
       
       if( curr_nc->BP_TAIL->page_num == bp_cpn ){
        
        if( current_page_num+1 == curr_nc->page_num )
         current_bp = curr_nc->BP_TAIL;
      
       }
     
      }//end of if( !tmp_bp )
      else{ //existing blank pages
       
       if( !tmp_bp->comment ){
       
        tmp_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        tmp_bp->comment->comment = note;
        tmp_bp->comment->next = NULL;
        
       }
       else{
       
        struct bp_comment *cmt = tmp_bp->comment;
        
        tmp_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
        tmp_bp->comment->comment = note;
        tmp_bp->comment->next = cmt;
        
       }
             
      }//end of if( tmp_bp )
      
     }//end of if( curr_nc->BP_HEAD)
     
    }//end of if( *tmp_line == '5')
    
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
    
    if( line_buf ){
     g_string_free(line_buf, FALSE);
     line_buf = NULL;
    }
    
   } // end of if(*line == '2')
   else if( *tmp_line == '1' ){ //highlight region
  
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
    
    list_add_tail(&(hg->list), &curr_nc->HR_HEAD);
    
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
   else if( *tmp_line == '3' ){ //page comment
    
    //format : 3:page_number:string_content
    //         pch0        pch
    
    if( line_buf != NULL && strlen(line_buf->str) >= 999 ){
     g_string_append(line_buf, line);
     tmp_line = line_buf->str;
    }
    else
     tmp_line;
    
    GtkTextIter titer;

    gtk_text_buffer_get_iter_at_offset(tbuffer, &titer, 0);
    
    struct pc_entry *pc_tmp = &PC_HEAD;
     
    if( pc_tmp->next != NULL ){ //this if is redundant
     while( pc_tmp->next != NULL ){
      pc_tmp = pc_tmp->next;
     } 
    }
     
    pc_tmp->next = (struct pc_entry*)malloc(sizeof(struct pc_entry));
    pc_tmp->next->page_num = atoi(pch+1);
    pc_tmp->next->str = NULL;
    pc_tmp->next->next = NULL;
    
    pch0 = strchr(tmp_line, ':');
     
    if(pch0){
    
     pch = strchr(pch0+1,':');
      
     if( pch ){
       
      *pch = '\0';
       
      pc_tmp->next->page_num = atoi(pch0+1);
       
      char *new_line = strrchr(pch+1,'\n');
       
      if(new_line){
       *new_line = '\0';
      }
      
      new_line = strchr(pch+1, 1);
       
      while( new_line ){
        
       *new_line = '\n';
        
       new_line = strchr(new_line+1, 1);
       
      }
       
      pc_tmp->next->str = (char *)malloc(strlen(pch+1)+1);
      strcpy(pc_tmp->next->str, pch+1);
      
      if( pc_tmp->next->str ){
       gtk_text_buffer_insert (tbuffer, &titer, pc_tmp->next->str, -1);
      }
      
     }//end of if( pch )
      
    }//end of if(pch0)
    
   }//end of else if( *line == '3' )
   else if( *tmp_line == '4' ){
   
    //-----
    //comment without property for blank page
    //-----
    
    //comment with default settings
    //4:pre_page_num:page_num: (blank page)
    //             pch     pch1  
    //or
    //4:pre_page_num:page_num:x:y:width:height:str
    //                      pch1 pch4         pch2
    //             pch        pch3    pch5
    //4:0:0:
    //or
    //4:0:0:10:10:100:100:str
    
    if( line_buf != NULL ){
     g_string_append(line_buf, line);
     tmp_line = line_buf->str;
     pch0 = strchr(tmp_line, ':');
    }else
     tmp_line = line;
    
    pch = pch1 = pch2 = pch3 = pch4 = pch5 = NULL;
    
    if( pch0 )
     pch = strchr(pch0+1, ':');
    
    if( pch )
     pch1 = strchr(pch+1, ':');
     
    if( pch1 )
     pch3 = strchr(pch1+1, ':');
    
    *pch = *pch1 = '\0';
    
    if( pch3 ){ 
     
     pch4 = strchr(pch3+1, ':');
     
     if( pch4 ){
      
      pch5 = strchr(pch4+1, ':');
      
      if( pch5 ){
       
       pch2 = strchr(pch5+1, ':');
       
       char *pch_tmp = strchr(pch2+1, '\n');
       *pch_tmp = '\0';
       
       char new_line = 1;
    
       pch_tmp = strchr(pch2+1, new_line);
    
       while( pch_tmp ){
        *pch_tmp = '\n';
        pch_tmp = strchr(pch_tmp+1, new_line);
       }
       
      }
      
     }
     
    }
    
    if( blank_page ){
     if( curr_nc->page_num == current_page_num+1) {
      current_nc = curr_nc;
     }
    }
    
    //build blank page
    if( !curr_nc->BP_HEAD ){
     
     curr_nc->BP_HEAD = (struct blank_page *)malloc(sizeof(struct blank_page));
     curr_nc->BP_HEAD->page_num = atoi(pch+1);
     
     curr_nc->BP_HEAD->comment = NULL;
     curr_nc->BP_HEAD->next = NULL;
     curr_nc->BP_HEAD->prev = NULL;
     
     curr_nc->BP_TAIL = curr_nc->BP_HEAD;
     
     if( curr_nc->BP_HEAD->page_num == bp_cpn ){
      
      if( current_page_num+1 == curr_nc->page_num )
       current_bp = curr_nc->BP_HEAD;
     
     }
     
    }
    else{
     
     struct blank_page *tmp_bp = curr_nc->BP_HEAD;
     
     int bp_pnum = atoi(pch+1);
     
     while( tmp_bp ){
      
      if( tmp_bp->page_num == bp_pnum )
       break;
       
      tmp_bp = tmp_bp->next;
      
     }
     
     if( !tmp_bp ){
      
      curr_nc->BP_TAIL->next = (struct blank_page *)malloc(sizeof(struct blank_page));
   
      curr_nc->BP_TAIL->next->next = NULL;
      curr_nc->BP_TAIL->next->prev = curr_nc->BP_TAIL;
     
      curr_nc->BP_TAIL = curr_nc->BP_TAIL->next;
      curr_nc->BP_TAIL->page_num = atoi(pch+1);
      curr_nc->BP_TAIL->comment = NULL;
     
      if( curr_nc->BP_TAIL->page_num == bp_cpn ){
       
       if( current_page_num+1 == curr_nc->page_num )
        current_bp = curr_nc->BP_TAIL;
       
      }
     
     }
     
    }
    //build blank page
    
    if( pch3 ){
     
     //4:pre_page_num:page_num:x:y:width:height:str
     //                      pch1 pch4         pch2
     //             pch        pch3    pch5
     
     *pch3 = *pch4 = *pch5 = *pch2 = '\0';
     
     struct blank_page *tmp_bp1 = curr_nc->BP_HEAD;
     
     while( tmp_bp1 ){
      
      if( tmp_bp1->page_num == atoi(pch+1) )
       break;
      
      tmp_bp1 = tmp_bp1->next;
      
     }
     
     struct bp_comment *cmt;
     
     if(tmp_bp1){
      
      if( !tmp_bp1->comment ){
       
       tmp_bp1->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
      
       cmt = tmp_bp1->comment;
     
      }
      else{
     
       cmt = tmp_bp1->comment;
      
       while( cmt->next ){
       
        cmt = cmt->next;
       
       }
      
       cmt->next = (struct bp_comment *)malloc(sizeof(struct bp_comment));
       cmt = cmt->next;
     
      }
     }
     
     cmt->next = NULL;
      
     cmt->comment = (struct note*)malloc(sizeof(struct note));
     cmt->comment->comment = gtk_drawing_area_new();
      
     gtk_layout_put (GTK_LAYOUT (layout), cmt->comment->comment, cmt->comment->x, cmt->comment->y);
      
     cmt->comment->x = atoi(pch1+1);
     cmt->comment->y = atoi(pch3+1);
     cmt->comment->width = atoi(pch4+1);
     cmt->comment->height = atoi(pch5+1);
      
     cmt->comment->property = NULL;
      
     cmt->comment->str = (char*)malloc(strlen(pch2+1)+1);
     strcpy(cmt->comment->str, pch2+1);
     
     cmt->comment->page_num = atoi(pch0+1);
    
     gtk_widget_add_events (cmt->comment->comment, 
                             GDK_POINTER_MOTION_MASK | 
                             GDK_BUTTON_PRESS_MASK   | 
                             GDK_BUTTON_RELEASE_MASK |
                             GDK_ENTER_NOTIFY_MASK   |   
                             GDK_LEAVE_NOTIFY_MASK);

     g_signal_connect (G_OBJECT (cmt->comment->comment), "draw", 
                      G_CALLBACK (comment_draw_cb), 
                      cmt->comment);
 
     g_signal_connect (G_OBJECT (cmt->comment->comment), "button-press-event", 
                      G_CALLBACK (comment_button_press_cb),cmt->comment);
  
     g_signal_connect (G_OBJECT (cmt->comment->comment), "motion-notify-event",
                      G_CALLBACK (comment_button_motion_cb), NULL);
  
     g_signal_connect (G_OBJECT (cmt->comment->comment), "button-release-event",
                      G_CALLBACK (comment_button_release_cb), cmt->comment);
     
     if( line_buf ){
      g_string_free(line_buf, FALSE);
      line_buf = NULL;
     }
     
    }//end of if( pch3 )
    
   }//end of else if( *line == '4' )
   
  } // end of while((read = getline(&line, &len, pfile)) != -1)
  
  //existing file without content
  
  if( blank_page ){
   
   if(doc){
   
    page_num = poppler_document_get_n_pages(doc);
   
    if( dual_page_mode ){
    
     if( !current_bp->next ){
     
      if( current_page_num < page_num-1)
       current_page_num++;
    
     }
    
    }
    else{
   
     if( current_page_num < page_num-1)
     
      if( current_page_num >= 0 )
       current_page_num++;
    
    }
    
   }
    
  }
  
  struct note_cache *tnc = note_cache->next;
  
  struct blank_page *bpc, *tail;
  
  struct bp_comment *cmt;
  
  while( tnc ){
   
   bpc = tnc->BP_HEAD;
   
   while( bpc ){
    
    cmt = bpc->comment;
    
    while( cmt ){
     
     cmt = cmt->next;
     
    }
    
    bpc = bpc->next;
    
   }
   
   tnc = tnc->next;
   
  }
  
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
   
   if( doc ){
    page = poppler_document_get_page(doc, 0);
   
    if(!page) {
     printf("Could not open first page of document\n");
     g_object_unref(doc);
     exit(3);
    }
 
    poppler_page_get_size(page, &page_width, &page_height); 
 
    g_object_unref (G_OBJECT (page));
    page = NULL;
    
   }
   else{
    
    //a4 size
    page_width = 595;
    page_height = 842;
    //a4 size
    
   }
   
  }
  
  fclose(pfile);
  
 } // end of if( stat(full_path, &buffer) == 0 )
 else{ //no note file
  
  P_CM_FONT_DESC = G_CM_FONT_DESC;
  P_CM_FONT_COLOR = G_CM_FONT_COLOR;
  P_CM_BG_COLOR = G_CM_BG_COLOR;
  P_HR_COLOR = G_HR_COLOR;
  
  if(file_name){
   
   if(doc){
    
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
   else{
    
    current_page_num = -1;
   
    //a4 size
    page_width = 595;
    page_height = 842;
    //a4 size
    
   }
   
  }
  else{
   
   //for blank file
   
   current_page_num = -1;
   
   //a4 size
   page_width = 595;
   page_height = 842;
   //a4 size
   
  }
  
 }
 
 if(!doc)
  blank_page = TRUE;
 
 if(doc){
  //build PAGES
  build_pages();
  //build PAGES
 }
 else{
  
  if(file_name){
   //open note file
   build_pages();
  }
  else{
   //new file
   
   //create note_cache
   note_cache->next = (struct note_cache*)malloc(sizeof(struct note_cache));
   INIT_LIST_HEAD(&note_cache->next->CM_HEAD);
   INIT_LIST_HEAD(&note_cache->next->HR_HEAD);
   
   note_cache->next->page_num = 0;
   note_cache->next->next = NULL;
   note_cache->next->prev = NULL;
   
   current_nc = note_cache->next;
   
   //create note_cache
   
   //create blank page
   struct blank_page *tmp_bp = (struct blank_page*)malloc(sizeof(struct blank_page));
   
   tmp_bp->page_num = 1;
   tmp_bp->comment = NULL;
   tmp_bp->prev = NULL;
   tmp_bp->next = NULL;
   
   current_nc->BP_HEAD = current_nc->BP_TAIL = tmp_bp;
   
   current_bp = tmp_bp;
   
   //create blank page
   
   //create page entry
   PAGES = (struct page_entry *)malloc(sizeof(struct page_entry));
   PAGES->p_page = 0;
   PAGES->curr_nc = current_nc;
   PAGES->curr_bp = current_bp;
   PAGES->next = NULL;
   PAGES->prev = NULL;
   
   current_pe = PAGES;
   //create page entry
   
  }
  
 }
 
 char *title_str = (char*)malloc(100);
 
 if( !blank_page ){
  
  sprintf(title_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
  
 }
 else{
  
  if(doc){
  
   gint page_num = poppler_document_get_n_pages(doc);
  
   if( current_nc->page_num == page_num ){
  
    sprintf(title_str, "%s page %d-%d/%d",file_name, current_page_num+1,current_bp->page_num, poppler_document_get_n_pages(doc));
  
   }
   else{
   
    if(!dual_page_mode){
    
     if ( current_page_num == -1 )
      sprintf(title_str, "%s page %d-%d/%d",file_name, 0,current_bp->page_num, poppler_document_get_n_pages(doc));
     else
      sprintf(title_str, "%s page %d-%d/%d",file_name, current_page_num,current_bp->page_num, poppler_document_get_n_pages(doc));
   
    }
    else{
    
     sprintf(title_str, "%s page %d-%d/%d",file_name, current_nc->page_num,current_bp->page_num, poppler_document_get_n_pages(doc));
    
    }
   
   }
  
  }
  else{
   
   //new file
   if(!file_name){
    sprintf(title_str, "Untitled page 1/1");
   }
   else{
    //no pdf
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    sprintf(title_str, "%s page %d/%d",file_name, current_bp->page_num, page_num);
    
   }
   
  }
  
 }
 
 gtk_window_set_title(GTK_WINDOW(win), title_str);
 
 free(title_str);
 
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

void save_as(void){
 
 gchar *save_as_str;
 
 //dialog
 GtkWidget *dialog;
 
 dialog = gtk_file_chooser_dialog_new("Save as...", NULL,
             GTK_FILE_CHOOSER_ACTION_SAVE, 
	     "_Cancel", GTK_RESPONSE_CANCEL,
	     "_Ok", GTK_RESPONSE_OK, 
	     NULL);
  
 gint res = gtk_dialog_run(GTK_DIALOG(dialog));
 
 if(res == GTK_RESPONSE_OK){
  
  save_as_str = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  gtk_widget_destroy(dialog);
  
 }
 else if ( res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT){
  gtk_widget_destroy(dialog);
  return;
 }  
  
 //dialog
 
 PopplerPage *pdf_page;
 
 cairo_t *pdf_cr;

 cairo_surface_t *pdf_surface;
 
 int i;
 double width, height;
 
 pdf_surface = cairo_pdf_surface_create (save_as_str, page_width, page_height);
 pdf_cr = cairo_create (pdf_surface);
 
 struct list_head *tmp;
  
 struct note_cache *nc_tmp = NULL;
 struct note *cm_entry;
 struct highlight_region *hr_entry;
 
 PangoLayout *layout;
 PangoFontDescription *tmp_font_desc;
 struct color_table *tmp_bc;
 struct color_table *tmp_fc;
 
 GtkAllocation cm_alloc;
 
 struct blank_page *tmp_bp;
 struct bp_comment *tmp_cmt;
 
 struct page_entry *tmp_pe = PAGES;
 
 gboolean is_italic = FALSE;
 
 while( tmp_pe ){
  
  //---page setup
  if(tmp_pe->p_page == -1)
   pdf_page = NULL;
  else{
   
   if(!tmp_pe->curr_bp){
    
    pdf_page = poppler_document_get_page (doc, tmp_pe->p_page);

    if (pdf_page == NULL && tmp_pe->p_page >= 0 ) {
     printf("poppler fail: page not found\n");
     return;
    }
  
    if( pdf_page ){
     poppler_page_get_size (pdf_page, &width, &height);
     cairo_pdf_surface_set_size (pdf_surface, width, height);
     cairo_save (pdf_cr);
     poppler_page_render_for_printing (pdf_page, pdf_cr);
    }
    
   }
   else
    pdf_page = NULL;
   
  }
  
  if( !tmp_pe->curr_bp ){
   
   if(tmp_pe->curr_nc){
    
    //pdf page
   
    //highlight region
    list_for_each(tmp, &tmp_pe->curr_nc->HR_HEAD){
   
     hr_entry = list_entry(tmp, struct highlight_region, list);
    
     cairo_set_operator(pdf_cr, CAIRO_OPERATOR_DARKEN);
     
     cairo_set_source_rgb (pdf_cr, hr_entry->ce->color.red, 
                                   hr_entry->ce->color.green, 
                                   hr_entry->ce->color.blue);
    
     cairo_rectangle (pdf_cr, hr_entry->x,
                              hr_entry->y, 
                              hr_entry->width,
                              hr_entry->height);
     
     cairo_fill (pdf_cr);
    
    }
    //highlight region
   
    //comment
    list_for_each(tmp, &tmp_pe->curr_nc->CM_HEAD){
    
     cm_entry = list_entry(tmp, struct note, list);
    
     layout = pango_cairo_create_layout (pdf_cr);
     
     //font description
     if( cm_entry->property ){
      
      if( cm_entry->property->font_desc ){
       tmp_font_desc = cm_entry->property->font_desc;
      }
      else{
       tmp_font_desc = P_CM_FONT_DESC;
      }
      
     }
     else{
      tmp_font_desc = P_CM_FONT_DESC;
     }
     
     if( pango_font_description_get_style(tmp_font_desc) == PANGO_STYLE_ITALIC)
      is_italic = TRUE;
     else
      is_italic = FALSE;
     
     pango_font_description_set_absolute_size (tmp_font_desc, 
      pango_font_description_get_size(tmp_font_desc));
     
     pango_layout_set_font_description (layout, tmp_font_desc);
     
     cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y);
     
     //background color
     
     if(cm_entry->property){
      
      if(cm_entry->property->bg_color)
       tmp_bc = cm_entry->property->bg_color;
      else
       tmp_bc = P_CM_BG_COLOR;
     }
     else{
      tmp_bc = P_CM_BG_COLOR;
     }
     
     cairo_set_source_rgba (pdf_cr, tmp_bc->color.red, 
                                    tmp_bc->color.green, 
                                    tmp_bc->color.blue, 
                                    tmp_bc->color.alpha);
     
     cairo_set_operator(pdf_cr, CAIRO_OPERATOR_OVER);
     
     if( is_italic ){
      cairo_rectangle (pdf_cr, cm_entry->x,
                               cm_entry->y, 
                               cm_entry->width+1,
                               cm_entry->height);
     }
     else{
      cairo_rectangle (pdf_cr, cm_entry->x,
                               cm_entry->y,
                               cm_entry->width,
                               cm_entry->height);
      
      
     }
     
     cairo_fill (pdf_cr);
     
     //background color
     
     if( is_italic )
      cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y);
     else
      cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y-0.2);
     
     pango_layout_set_text (layout, cm_entry->str, -1);
     
     //font color
     
     if( cm_entry->property ){
     
      if( cm_entry->property->font_color )
       tmp_fc = cm_entry->property->font_color;
      else
       tmp_fc = P_CM_FONT_COLOR;
     }
     else{
      tmp_fc = P_CM_FONT_COLOR;
     }
     
     cairo_set_source_rgba (pdf_cr, tmp_fc->color.red, 
                                    tmp_fc->color.green, 
                                    tmp_fc->color.blue, 
                                    tmp_fc->color.alpha);
     
     //font color

     pango_cairo_show_layout (pdf_cr, layout);
    
     g_object_unref (layout);
    
    }//end of list_for_each(tmp, &tmp_pe->curr_nc->CM_HEAD)
    
   }
   //comment
   
  }
  else{
  
   //blank page
   
   struct blank_page *tmp_bp = tmp_pe->curr_bp;
   
   cairo_save (pdf_cr);
   
   struct bp_comment *tmp_cmt = tmp_bp->comment;
   
   while( tmp_cmt ){
    
    cm_entry = tmp_cmt->comment;
    
    //font description
    layout = pango_cairo_create_layout (pdf_cr);
    
    if( cm_entry->property ){
      
     if( cm_entry->property->font_desc ){
      tmp_font_desc = cm_entry->property->font_desc;
     }
     else{
      tmp_font_desc = P_CM_FONT_DESC;
     }
      
    }
    else{
     tmp_font_desc = P_CM_FONT_DESC;
    }
    
    if( pango_font_description_get_style(tmp_font_desc) == PANGO_STYLE_ITALIC)
     is_italic = TRUE;
    else
     is_italic = FALSE;
    
    pango_font_description_set_absolute_size (tmp_font_desc, 
      pango_font_description_get_size(tmp_font_desc));
     
     pango_layout_set_font_description (layout, tmp_font_desc);
    
    //font description
    
    cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y);
    
    //background color
    
    if(cm_entry->property){
     
     if(cm_entry->property->bg_color)
      tmp_bc = cm_entry->property->bg_color;
     else
      tmp_bc = P_CM_BG_COLOR;
    }
    else{
     tmp_bc = P_CM_BG_COLOR;
    }
    
    cairo_set_source_rgba (pdf_cr, tmp_bc->color.red, 
                                   tmp_bc->color.green, 
                                   tmp_bc->color.blue, 
                                   tmp_bc->color.alpha);
    
    cairo_set_operator(pdf_cr, CAIRO_OPERATOR_OVER);
    
    if( is_italic ){
     cairo_rectangle (pdf_cr, cm_entry->x,
                              cm_entry->y, 
                              cm_entry->width,
                              cm_entry->height);
    }
    else{
     cairo_rectangle (pdf_cr, cm_entry->x,
                              cm_entry->y, 
                              cm_entry->width,
                              cm_entry->height);
     
    }
    
    cairo_fill (pdf_cr);
    //background color
    
    if( is_italic )
     cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y);
    else
     cairo_move_to (pdf_cr, cm_entry->x, cm_entry->y-0.2);
    
    pango_layout_set_text (layout, cm_entry->str, -1);
    //font description
    
    //font color
    
    if(cm_entry->property){
     
     if( cm_entry->property->font_color )
      tmp_fc = cm_entry->property->font_color;
     else
      tmp_fc = P_CM_FONT_COLOR;
    }
    else{
     tmp_fc = P_CM_FONT_COLOR;
    }
    
    cairo_set_source_rgba (pdf_cr, tmp_fc->color.red, 
                                   tmp_fc->color.green, 
                                   tmp_fc->color.blue, 
                                   tmp_fc->color.alpha);
    
    //font color
    
    pango_cairo_show_layout (pdf_cr, layout);
    
    g_object_unref (layout);
    
    tmp_cmt = tmp_cmt->next;
    
   }
   
  }
  
  cairo_restore (pdf_cr);
  cairo_surface_show_page (pdf_surface);
  
  if(pdf_page)
   g_object_unref (G_OBJECT (pdf_page));
  
  tmp_pe = tmp_pe->next;
  
 }//end of while( tmp_pe )
 
 cairo_destroy (pdf_cr);
 cairo_surface_finish (pdf_surface);

 cairo_surface_destroy (pdf_surface);
 
 g_free(save_as_str);
 
}

void save_note (void) {
 
 gchar *full_path;
 
 if( !doc ){
  
  if( !file_name ){
   
   //dialog
   GtkWidget *dialog;
 
   dialog = gtk_file_chooser_dialog_new("Save Note File", NULL,
             GTK_FILE_CHOOSER_ACTION_SAVE, 
	     "_Cancel", GTK_RESPONSE_CANCEL,
	     "_Ok", GTK_RESPONSE_OK, 
	     NULL);
  
   gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  
  
   if(res == GTK_RESPONSE_OK){
    
    full_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    
    char *pch = NULL;
    
    #ifdef _WIN32
    pch = strrchr(full_path, '\\');
    #else
    pch = strrchr(full_path, '/');
    #endif
    
    *pch = '\0';
    
    file_path = (gchar *)malloc(strlen(full_path)+1);
    strcpy(file_path, full_path);
    
    file_name = (gchar *)malloc(strlen(pch+1)+1);
    strcpy(file_name, pch+1);
    
    #ifdef _WIN32
    *(pch) = '\\';
    #else
    *(pch) = '/';
    #endif
    
    gtk_widget_destroy(dialog);
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    char *title_str = (char*)malloc(100);
    
    sprintf(title_str, "%s page %d/%d",file_name,current_bp->page_num, page_num);
      
    gtk_window_set_title(GTK_WINDOW(win), title_str);
 
    free(title_str);
    
   }
   else if ( res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT){
    gtk_widget_destroy(dialog);
    return;
   }  
  
   //dialog
   
  }
  else{
  
   full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+2);
   strcpy(full_path, file_path);
   strcat(full_path, "/");
   strcat(full_path, file_name);
  
  }
  
 }
 else{
  
  full_path = (gchar *)malloc(strlen(file_path)+strlen(file_name)+5+2);

  strcpy(full_path, file_path);
 
  strcat(full_path, "/");
  strcat(full_path, file_name);
 
  strcat(full_path, ".note");
 
 }
 
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
  
  if( !blank_page ){
  
   gint page_num = poppler_document_get_n_pages(doc);
   
   if( dual_page_mode ){
   
    if( current_page_num+2 == page_num ){
     
     fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_page_num+1,
      invert_color,
      dual_page_mode, font_desc_str, color_table);
     
    }
    else if( current_page_num == page_num ){
    
     fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_page_num,
      invert_color,
      dual_page_mode, font_desc_str, color_table);
      
    }
    else{
    
     fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_page_num+1,
      invert_color,
      dual_page_mode, font_desc_str, color_table);
    
    }
    
   }
   else{
   
    fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_page_num+1,
     invert_color,
     dual_page_mode, font_desc_str, color_table);
     
   }
   
  }
  else{
  
   //replace bp_cpn with current_bp->prev or BP_TAIL
   
   if( !dual_page_mode ){
    fprintf(pfile, "P:%d-%d:%d:%d:%s%s:\n", current_nc->page_num,
     current_bp->page_num,
     invert_color,
     dual_page_mode, font_desc_str, color_table);
    }
    else{
     
     if(!current_bp){
      fprintf(pfile, "P:%d:%d:%d:%s%s:\n", current_nc->page_num,
       invert_color,
       dual_page_mode, font_desc_str, color_table);
     }
     else{
     
      fprintf(pfile, "P:%d-%d:%d:%d:%s%s:\n", current_nc->page_num,
       current_bp->page_num,
       invert_color,
       dual_page_mode, font_desc_str, color_table);
     }
     
    }
    
  }
  
  char *font_color_str, *bg_color_str, *font_str;
  const gchar *content;
  
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
    
    content = cm_entry->str;
    
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
      
      fprintf(pfile, "2:%d:%d:%d:%d:%d:%s:%s:%s:",
                     cm_entry->page_num, 
                     cm_entry->x, 
                     cm_entry->y,
                     cm_entry->width,
                     cm_entry->height, 
                     font_str,
                     font_color_str,
                     bg_color_str);
      }
      else if( font_str && font_color_str && !bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%d:%d:%s:%s::", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height,                     
                      font_str,
                      font_color_str);
       
      }
      else if( font_str && !font_color_str && bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%d:%d:%s::%s:",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height, 
                      font_str,
                      bg_color_str);
       
      }
      else if( !font_str && font_color_str && bg_color_str ){
      
       fprintf(pfile, "2:%d:%d:%d:%d:%d::%s:%s:",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y, 
                      cm_entry->width,
                      cm_entry->height,
                      font_color_str,
                      bg_color_str);
       
      }
      else if( font_str && !font_color_str && !bg_color_str ){
      
       fprintf(pfile, "2:%d:%d:%d:%d:%d:%s:::", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height, 
                      font_str);
       
      }
      else if( !font_str && font_color_str && !bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%d:%d::%s::",
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height, 
                      font_color_str);
       
      }
      else if( !font_str && !font_color_str && bg_color_str ){
       
       fprintf(pfile, "2:%d:%d:%d:%d:%d:::%s:", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height,
                      bg_color_str);
       
      }
      else{
       fprintf(pfile, "0:%d:%d:%d:%d:%d:", 
                      cm_entry->page_num, 
                      cm_entry->x, 
                      cm_entry->y,
                      cm_entry->width,
                      cm_entry->height);
      }
      
    }
    else{
     fprintf(pfile, "0:%d:%d:%d:%d:%d:", 
                    cm_entry->page_num, 
                    cm_entry->x, 
                    cm_entry->y,
                    cm_entry->width,
                    cm_entry->height);
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
    
    while(*content != '\0'){

     if( *content == '\n'){
      
      if(*(content+1) != '\0')
       fprintf(pfile, "%c", 1);
       
     }
     else{
 
      fprintf(pfile, "%c", *content);

     }

     content++;
    }
    
    fprintf(pfile, "%c", '\n');
    
   }//end of list_for_each(tmp, &nc_tmp->CM_HEAD)
   
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
   
   struct blank_page *bp_tmp = nc_tmp->BP_HEAD;
   
   while( bp_tmp ){
    
    //3:pre_page_num-page_num:x:y:width:height:font_desc:cm_font_color:cm_bg_color:str
    //or for a new file
    //3:-page_num:x:y:width:height:font_desc:cm_font_color:cm_bg_color:str
    if( !bp_tmp->comment ){ //it's just a blank page without any comment   
     fprintf(pfile, "4:%d:%d:\n", 
                     nc_tmp->page_num, 
                     bp_tmp->page_num);
    }
    else{ //if( bp_tmp->comment )
     
     struct bp_comment *cmt = bp_tmp->comment;
     
     while( cmt ){
      
      if( !cmt->comment->property ){
       
       fprintf(pfile, "4:%d:%d:%d:%d:%d:%d:", 
                      nc_tmp->page_num, 
                      bp_tmp->page_num,
                      cmt->comment->x,
                      cmt->comment->y,
                      cmt->comment->width,
                      cmt->comment->height);
      
      }
      else{
      
       //font color
       if( cmt->comment->property->font_color == P_CM_FONT_COLOR ){
      
        font_color_str = NULL;
      
       }
       else{
      
        if( cmt->comment->property->font_color ){
         font_color_str = (char*)malloc(10);
   
         sprintf(font_color_str, "#%02X%02X%02X%02X", 
          (int)(cmt->comment->property->font_color->color.red*255),  
          (int)(cmt->comment->property->font_color->color.green*255), 
          (int)(cmt->comment->property->font_color->color.blue*255),
          (int)(cmt->comment->property->font_color->color.alpha*255)); 
        }
        else{
         font_color_str = NULL;
        } 
      
       }
       //font color
      
       //background color
       if( cmt->comment->property->bg_color == P_CM_BG_COLOR ){
        bg_color_str = NULL;
       }
       else{
      
        if( cmt->comment->property->bg_color ){
       
         bg_color_str = (char*)malloc(10);
   
         sprintf(bg_color_str, "#%02X%02X%02X%02X", 
          (int)(cmt->comment->property->bg_color->color.red*255), 
          (int)(cmt->comment->property->bg_color->color.green*255),  
          (int)(cmt->comment->property->bg_color->color.blue*255),
          (int)(cmt->comment->property->bg_color->color.alpha*255)); 
      
        }
        else{
       
         bg_color_str = NULL;
       
        }
      
       }
       //background color
      
       //font descriptor
       if( cmt->comment->property->font_desc == P_CM_FONT_DESC || !cmt->comment->property->font_desc ){
        font_str = NULL;
       }
       else{
        font_str = pango_font_description_to_string(cmt->comment->property->font_desc);
       }
       //font descriptor
      
       if( font_str && font_color_str && bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d:%s:%s:%s:", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_str,
                       font_color_str,
                       bg_color_str);
                       
       }
       else if( font_str && font_color_str && !bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d:%s:%s::", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_str,
                       font_color_str);
        
       }
       else if( font_str && !font_color_str && bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d:%s::%s:", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_str,
                       bg_color_str);
        
       }
       else if( !font_str && font_color_str && bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d::%s:%s:", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_color_str,
                       bg_color_str);
        
       }
       else if( font_str && !font_color_str && !bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d:%s:::", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_str);
        
       }
       else if( !font_str && font_color_str && !bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d::%s::", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       font_color_str);
        
       }
       else if( !font_str && !font_color_str && bg_color_str ){
        
        fprintf(pfile, "5:%d:%d:%d:%d:%d:%d:::%s:", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height, 
                       bg_color_str);
        
       }
       else{
        
        fprintf(pfile, "4:%d:%d:%d:%d:%d:%d:", 
                       nc_tmp->page_num, 
                       bp_tmp->page_num,
                       cmt->comment->x,
                       cmt->comment->y,
                       cmt->comment->width,
                       cmt->comment->height);
        
       }
       
      }//end of if( bp_tmp->comment->comment->property )
      
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
      
      content = cmt->comment->str;
      
      while(*content != '\0'){

       if( *content == '\n'){
      
        if(*(content+1) != '\0')
         fprintf(pfile, "%c", 1);
       
       }
       else{
 
        fprintf(pfile, "%c", *content);

       }

       content++;
      }
    
      fprintf(pfile, "%c", '\n');
    
      cmt = cmt->next;
      
     }// end of while( cmt )
    
    }//end of if( bp_tmp->comment )
    
    bp_tmp = bp_tmp->next;
    
   }//end of while( bp_tmp )
   
   nc_tmp = nc_tmp->next;
   
  }
  
  //page comment
  
  save_page_comment();
  
  struct pc_entry *pc_tmp = PC_HEAD.next;
  
  
  while( pc_tmp != NULL ){
 
   if( pc_tmp->str ){
    char *new_line = strchr(pc_tmp->str, '\n');
   
    while( new_line ){
    
     *new_line = 1;
    
     new_line = strchr(new_line+1, '\n');
    }
   }
   
   if(pc_tmp->str && strcmp(pc_tmp->str, ""))
    fprintf(pfile,"3:%d:%s\n",pc_tmp->page_num, pc_tmp->str);
   
   pc_tmp = pc_tmp->next;
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

 struct note *note = (struct note*)malloc(sizeof(struct note));
 INIT_LIST_HEAD(&note->list);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation(scrolled_window, &sw_alloc);
 
 note->x = 100;
 note->y = (int)(100+gtk_adjustment_get_value(vadj)/zoom_factor);
 
 GtkWidget *new_label = gtk_drawing_area_new();
 gtk_widget_set_size_request (new_label, 180,14*zoom_factor);
 
 gtk_layout_put (GTK_LAYOUT (layout), new_label, note->x*zoom_factor, note->y*zoom_factor);
 
 gtk_widget_show(new_label);
 
 
 note->comment = new_label;
 
 note->width = note->height = -1;
 
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
  
  if( !blank_page )
   list_add_tail(&(note->list), &current_nc->CM_HEAD);
  else{
   
   if(current_bp) {
   
    if( !current_bp->comment ){
    
     current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
    
     current_bp->comment->comment = note;
     current_bp->comment->next = NULL;
    
    }
    else{
    
     struct bp_comment *cmt = current_bp->comment;
    
     current_bp->comment = (struct bp_comment *)malloc(sizeof(struct bp_comment));
     current_bp->comment->comment = note;
     current_bp->comment->next = cmt;
    
    }
   
   }
   else{
    //p1, b1-1
    //add a comment in p1 
    list_add_tail(&(note->list), &current_nc->CM_HEAD);
   }
   
  }
  
 }
 else{ // if ( !current_nc )
  
  struct note_cache *tmp = note_cache->next;
  
  struct note_cache *min = note_cache;
   
  while( tmp ){
    
   if( note->page_num < tmp->page_num ){
    
    min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
    min->next->next = tmp;
    min->next->prev = min;
    tmp = min->next;
     
    break;
    
   }
    
   min = tmp;
   tmp = tmp->next;
   
  }
   
  if( !tmp ){
   //note_cache is empty
   min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
   tmp = min->next;
   tmp->next = NULL;
   tmp->prev = min;
  }
  
  INIT_LIST_HEAD(&tmp->CM_HEAD);
  INIT_LIST_HEAD(&tmp->HR_HEAD);
  tmp->page_num = note->page_num;
  
  tmp->BP_HEAD = NULL;
  tmp->BP_TAIL = NULL;
  
  list_add_tail(&(note->list), &tmp->CM_HEAD);
  
  current_nc = tmp;
  prev_nc = tmp->prev;
  
  current_pe->curr_nc = tmp;
  
 }
 
}

void
del_blank_page_cb (GtkWidget* widget, gpointer user_data) {
 
 if( !current_pe->curr_bp ){
  return;
 }
 else{
  
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Are you sure you want to delete this page?");
  
  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if( result == GTK_RESPONSE_NO ){
   gtk_widget_destroy(dialog);
   return;
  }
  
  gtk_widget_destroy(dialog);
  
 }
 
 struct blank_page *curr_bp = current_bp;
 struct blank_page *tmp_bp = NULL;
 
 current_nc = current_pe->curr_nc;
 
 if( curr_bp ){
  
  //free its resources
  if( curr_bp->comment ){
   
   struct bp_comment *cmt = curr_bp->comment;
   
   while( cmt ){
    
    gtk_widget_hide(cmt->comment->comment);
    free(cmt->comment->str);
    gtk_widget_destroy(cmt->comment->comment);
    free(cmt->comment);
    
    cmt = cmt->next;
    
   }
   
  }
  //free its resources
  
  if( curr_bp != current_nc->BP_HEAD && curr_bp != current_nc->BP_TAIL ){
   
   curr_bp->prev->next = curr_bp->next;
   curr_bp->next->prev = curr_bp->prev;
   current_bp = curr_bp->prev;
   
   //b1, b2, b3
   //     c   r
   
   struct page_entry *tmp_pe = current_pe;
   current_pe = current_pe->prev;
   
   current_pe->next = tmp_pe->next;
   current_pe->next->prev = current_pe;
   free(tmp_pe);
   
   tmp_bp = current_bp->next;
   
   while( tmp_bp ){
   
    tmp_bp->page_num--;

    tmp_bp = tmp_bp->next;
   
   }
   
   free(curr_bp);
   
   //dual_page_cb
   
   page_setup(&surface, NULL, draw_area, NULL);
   
   //title
   char *title_str = (char*)malloc(100);
   
   if(doc)
    sprintf(title_str, "%s page %d-%d/%d",file_name, current_pe->p_page+1,current_bp->page_num, poppler_document_get_n_pages(doc));
   else{
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    if(file_name)
     sprintf(title_str, "%s page %d/%d",
      file_name, current_pe->curr_bp->page_num, page_num);
    else
     sprintf(title_str, "Untitled page %d/%d",
      current_pe->curr_bp->page_num, page_num);
    
   }
      
   gtk_window_set_title(GTK_WINDOW(win), title_str);
 
   free(title_str);
   //title
   
   display_comment(NULL);
   
   return;
   
  }
  else{ //curr_bp == current_nc->BP_HEAD
   
   if( curr_bp == current_nc->BP_HEAD && curr_bp == current_nc->BP_TAIL ){//only one blank page
    
    free(curr_bp);
    current_nc->BP_HEAD = current_nc->BP_TAIL = NULL;
    
    if( doc ){
     gint page_num = poppler_document_get_n_pages(doc);
    
     if( current_page_num < page_num -1 ){
      current_page_num--;
     }
    }
    
    //p1, b1-1, p2
    
    struct page_entry *tmp_pe = current_pe;
    
    if(current_pe->prev){
     
     current_pe = current_pe->prev;
     current_pe->next = tmp_pe->next;
     
     if (current_pe->next)
      current_pe->next->prev = current_pe;
     
    }
    else{
     
     //delete first entry
     
     current_pe = current_pe->next;
     
     if( current_pe ){
      
      current_pe->prev = NULL;
      
      if( dual_page_mode )
       rcurrent_pe = current_pe->next;
     
     }
     else{
      
      if( !doc ){
       
       //delete the only blank page
       
       free(tmp_pe);
       
       //delete note file
       if(file_name){
        
        int rv = remove(file_name);
        
        if(rv) { 
         perror("remove"); 
         return ;
        }
        
       }
       
       gtk_main_quit();
       return;
      }
      
     }
     
    }
    
    free(tmp_pe);
    
    current_page_num = current_pe->p_page;
    
    current_bp = current_pe->curr_bp;
    
    if( !dual_page_mode ){
     
     if(!current_pe->curr_bp)
      blank_page = FALSE;
    
    }
    else{
     
     if( !current_pe->curr_bp && !rcurrent_pe->curr_bp ){
      blank_page = FALSE;
     }
     else{
      rcurrent_bp = rcurrent_pe->curr_bp;
     }
     
    }
    
    page_change();
    
    return;
    
   }
   else if( curr_bp == current_nc->BP_HEAD && curr_bp != current_nc->BP_TAIL ){
    
    current_nc->BP_HEAD = curr_bp->next;
    current_nc->BP_HEAD->prev = NULL;
    current_bp = current_nc->BP_HEAD;
    
    //p1,b1-1,b1-2
    //    c     r
    struct page_entry *tmp_pe = current_pe;
    
    if( current_pe->prev ){
     current_pe = current_pe->prev;
     current_pe->next = tmp_pe->next;
     current_pe->next->prev = current_pe;
    }
    else{
     
     current_pe = current_pe->next;
     
     if( current_pe ){
     
      current_pe->prev = NULL;
     
      if( dual_page_mode ) 
       rcurrent_pe = current_pe->next;
      
     }
     
    }
    
    PAGES = current_pe;
    
    free(tmp_pe);
    
    //decrease page numbers by 1 for blank pages
    tmp_bp = current_bp;
    
    //update the page number for rest of blank pages and their comments
    struct bp_comment *comment = NULL;
    
    while( tmp_bp ){
     
     tmp_bp->page_num--;
     
     comment = tmp_bp->comment;
     
     while( comment ){
      
      comment->comment->page_num--;
      
      comment = comment->next;
      
     }
     
     tmp_bp = tmp_bp->next;
   
    }
    //update the page number for rest of blank pages and their comments
    
    //switch to pdf page
    free(curr_bp);
    
    if(!dual_page_mode){
     
     if( !current_pe->curr_bp )
      blank_page = FALSE;
      
    }
    else{
     
     if( !current_pe->curr_bp && !rcurrent_pe->curr_bp ){
      blank_page = FALSE;
     }
     else{
      
      if(rcurrent_pe)
       rcurrent_bp = rcurrent_pe->curr_bp;
     
     }
     
    }
    
    if( doc ){
    
     gint page_num = poppler_document_get_n_pages(doc);
    
     if( current_page_num < page_num -1 ){
      current_page_num--;
     }
     
    }
    
    current_page_num = current_pe->p_page;
    
    current_bp = current_pe->curr_bp;
    
    //if there is only one blank page after deleting current blank page, enter single page mode
    if( !doc && dual_page_mode && !rcurrent_pe){
     
     dual_page_mode = FALSE;
     dual_page_cb();
     
     char *title_str = (char*)malloc(100);
     
     if(file_name)
      sprintf(title_str, "%s page 1/1", file_name);
     else
      sprintf(title_str, "Untitled page 1/1");
     
     gtk_window_set_title(GTK_WINDOW(win), title_str);
 
     free(title_str);
     
    }
    else
     page_change();
    
    return;
    
   }
   else{ //curr_bp == BP_TAIL, curr_bp != BP_HEAD
    
    //delete last blank page
    
    current_nc->BP_TAIL = curr_bp->prev;
    current_bp = curr_bp->prev;
    current_bp->next = NULL;
    
    //p1, b1-1, b1-2, p2
    //           c     r
    struct page_entry *tmp_pe = current_pe;
    current_pe = current_pe->prev;
    current_pe->next = tmp_pe->next;
    
    if(current_pe->next)
     current_pe->next->prev = current_pe;
     
    free(tmp_pe);
    
    free(curr_bp);
    
    char *title_str = (char*)malloc(100);
    
    if(doc){
     
     page_setup(&surface, NULL, draw_area, NULL);
     
     sprintf(title_str, "%s page %d-%d/%d",file_name, current_pe->p_page+1,current_bp->page_num, poppler_document_get_n_pages(doc));
     
    }
    else{
     
     if( !doc && dual_page_mode && !rcurrent_pe){
      dual_page_mode = FALSE;
      dual_page_cb();
     }
     else
      page_setup(&surface, NULL, draw_area, NULL);
     
     gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
     
     if(file_name)
      sprintf(title_str, "%s page %d/%d",
       file_name, current_pe->curr_bp->page_num, page_num);
     else
      sprintf(title_str, "Untitled page %d/%d",
       current_pe->curr_bp->page_num, page_num);
     
    }
    
    gtk_window_set_title(GTK_WINDOW(win), title_str);
 
    free(title_str);
    
    display_comment(NULL);
    
    return;
    
   }
   
  }
  
 }
 
}

void insert_bp_cb (gboolean before_after) {

 if( !dual_page_mode ){
  
  if( da_selection_region ){
   cairo_region_destroy(da_selection_region);
   da_selection_region = NULL;
  }
  
 }
 else{
  
  if( !rcurrent_bp ){
  
   if( da_selection_region ){
    cairo_region_destroy(da_selection_region);
    da_selection_region = NULL;
   }
   
  }
  
 }
 
 //hide current_nc's comments
 struct list_head *tmph;
 struct note *cm_entry;
 
 current_nc = current_pe->curr_nc;
 
 if(!dual_page_mode){
 
  if( current_nc ){
  
   if( !blank_page ){ //add first blank page
  
    list_for_each(tmph, &current_nc->CM_HEAD){

     cm_entry = list_entry(tmph, struct note, list);
   
     gtk_widget_hide(cm_entry->comment);
     
    }//list_for_each(tmp, &current_nc->CM_HEAD)
   
   }
   
  }
 }
 else{
  
  if( rcurrent_bp ){
   
   struct bp_comment *cmt = rcurrent_bp->comment;
     
   while( cmt ){
  
    gtk_widget_hide(cmt->comment->comment);
    cmt = cmt->next;
      
   }
   
  }
  else{
   
   if( rcurrent_nc ){
   
    list_for_each(tmph, &rcurrent_nc->CM_HEAD){

      cm_entry = list_entry(tmph, struct note, list);
   
       gtk_widget_hide(cm_entry->comment);
      
     }
    }
   
  }
  
 }
 //hide current_nc's comments
 
 blank_page = TRUE;
 
 if(!dual_page_mode){
 
  //hide comments from previous blank page
  if( current_bp ){
   
   struct bp_comment *cmt = current_bp->comment;
   
   while( cmt ){
    
    gtk_widget_hide(cmt->comment->comment);
    cmt = cmt->next;
      
   }
  }
 }
 //hide comments from previous blank page
 
 if( before_after ){
  
  //append a blank page
  
  if( !current_nc ){
   
   //old solution
  
   struct note_cache *tmp_nc = note_cache->next;
   struct note_cache *min = note_cache;

   while( tmp_nc ){
    
    if( current_page_num+1 < tmp_nc->page_num ){
    
     min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
     min->next->next = tmp_nc;
     min->next->prev = min;
     tmp_nc = min->next;
     
     break;
    
    }
    
    min = tmp_nc;
    tmp_nc = tmp_nc->next;
    
   }
   
   if( !tmp_nc ){
    //note_cache is empty
    min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
    tmp_nc = min->next;
    tmp_nc->next = NULL;
    tmp_nc->prev = min;
   }
   //old solution
   
   current_pe->curr_nc = tmp_nc;
   
   INIT_LIST_HEAD(&tmp_nc->CM_HEAD);
   INIT_LIST_HEAD(&tmp_nc->HR_HEAD);
   
   tmp_nc->page_num = current_pe->p_page+1;
   
   tmp_nc->BP_HEAD = tmp_nc->BP_TAIL = NULL;
   
   current_nc = tmp_nc;
   current_pe->curr_nc = current_nc;
  }
  
  //blank page initialization
  
  if( !current_pe->curr_bp ){
   
   //current page is a pdf page
   struct blank_page *tmp_bp = (struct blank_page*)malloc(sizeof(struct blank_page));
   
   tmp_bp->page_num = 1;
   tmp_bp->comment = NULL;
   tmp_bp->prev = NULL;
   tmp_bp->next = current_nc->BP_HEAD;
   
   if(current_nc->BP_HEAD)
    current_nc->BP_HEAD->prev = tmp_bp;
   
   current_nc->BP_HEAD = tmp_bp;
   
   if(!current_nc->BP_TAIL)
    current_nc->BP_TAIL = current_nc->BP_HEAD;
    
   current_bp = tmp_bp;
   
   tmp_bp = tmp_bp->next;
   
   while( tmp_bp ){
    
    tmp_bp->page_num++;
    
    tmp_bp = tmp_bp->next;
    
   }
   
  }
  else{
   
   //current page is a blank page
   
   struct blank_page *tmp_bp = current_pe->curr_bp;
   
   struct blank_page* next_bp = tmp_bp->next;
   
   tmp_bp->next = (struct blank_page*)malloc(sizeof(struct blank_page));
   
   tmp_bp->next->page_num = tmp_bp->page_num+1;
   
   tmp_bp->next->comment = NULL;
   
   tmp_bp->next->prev = tmp_bp;
   
   
   tmp_bp = tmp_bp->next;
   
   tmp_bp->next = next_bp;
   
   if(next_bp)
    next_bp->prev = tmp_bp;
   else
    current_pe->curr_nc->BP_TAIL = tmp_bp;
   
   current_bp = tmp_bp;
   
   tmp_bp = tmp_bp->next;
   
   while( tmp_bp ){
    
    tmp_bp->page_num++;
    
    tmp_bp = tmp_bp->next;
    
   }
   
  }
  
  //blank page initialization
  
  //update pages
  
  struct page_entry *tmp_pe = (struct page_entry *)malloc(sizeof(struct page_entry));
  
  tmp_pe->curr_nc = current_nc;
  tmp_pe->curr_bp = current_bp;
  
  tmp_pe->p_page = current_pe->p_page;
  
  tmp_pe->next = current_pe->next;
  tmp_pe->prev = current_pe;
  
  if( current_pe->next )
   current_pe->next->prev = tmp_pe;
   
  current_pe->next = tmp_pe;
  
  //update pages
  
  //reassgin current_pe and rcurrent_pe
  
  if( !dual_page_mode ){
   current_pe = tmp_pe;
   
   gint page_num = 0;
  
   if(doc)
    page_num = poppler_document_get_n_pages(doc);
   else{
    
    page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
   }
   
   //title
   char *title_str = (char*)malloc(100);
   
   if(doc)
    sprintf(title_str, "%s page %d-%d/%d",file_name, current_pe->p_page+1,current_bp->page_num, poppler_document_get_n_pages(doc));
   else{
    
    if(file_name)
     sprintf(title_str, "%s page %d/%d",file_name, current_bp->page_num, page_num);
    else
     sprintf(title_str, "Untitled page %d/%d", current_bp->page_num, page_num);
     
   }
   
   gtk_window_set_title(GTK_WINDOW(win), title_str);
 
   free(title_str);
   //title
   
  }
  else{
  
   rcurrent_pe = tmp_pe;
   
   rcurrent_bp = current_bp;
   current_bp = current_pe->curr_bp;
   
   
   if( !doc ){
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    //title
    char *title_str = (char*)malloc(100);
    
    if(file_name)
     sprintf(title_str, "%s page %d/%d",file_name, current_bp->page_num, page_num);
    else
     sprintf(title_str, "Untitled page %d/%d", current_bp->page_num, page_num);
     
    gtk_window_set_title(GTK_WINDOW(win), title_str);
 
    free(title_str);
    //title
    
   }
   
  }
  //reassgin current_pe and rcurrent_pe
  
 }
 else{
  
  //prepend a blank page
  
  if( !current_bp ){
   
   //current page is a pdf page
   
   if( current_pe->prev ){
    
    if( !current_pe->prev->curr_nc ){
     
     //if previous page does not have a note cache
     //creae a new one for it
     
     //old solution
  
     struct note_cache *tmp_nc = note_cache->next;
     struct note_cache *min = note_cache;

     while( tmp_nc ){
    
      if( current_page_num < tmp_nc->page_num ){
    
       min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
       min->next->next = tmp_nc;
       min->next->prev = min;
       tmp_nc = min->next;
     
       break;
    
      }
    
      min = tmp_nc;
      tmp_nc = tmp_nc->next;
    
     }
   
     if( !tmp_nc ){
      //note_cache is empty
      min->next = (struct note_cache*)malloc(sizeof(struct note_cache));
      tmp_nc = min->next;
      tmp_nc->next = NULL;
      tmp_nc->prev = min;
     }
     //old solution
     
     current_pe->prev->curr_nc = tmp_nc;
     current_nc = tmp_nc;
     
     INIT_LIST_HEAD(&tmp_nc->CM_HEAD);
     INIT_LIST_HEAD(&tmp_nc->HR_HEAD);
     
     tmp_nc->page_num = current_pe->p_page;
   
     tmp_nc->BP_HEAD = tmp_nc->BP_TAIL = NULL;
     
    }
    else{
     
     //previous page has a note cache
     
     current_nc = current_pe->prev->curr_nc;
     
    }
    
   }
   else{
    
    //current_pe is PAGES
    //add page entry before PAGES
    
    //this is a first pdf page 
    
    //create a note cache for page 0
    current_nc = (struct note_cache*)malloc(sizeof(struct note_cache));
    current_nc->next = note_cache->next;
    
    if(note_cache->next)
     note_cache->next->prev = current_nc;
    
    current_nc->prev = note_cache;
    note_cache->next = current_nc;
    
    INIT_LIST_HEAD(&current_nc->CM_HEAD);
    INIT_LIST_HEAD(&current_nc->HR_HEAD);
    
    current_nc->page_num = 0;
   
    current_nc->BP_HEAD = current_nc->BP_TAIL = NULL;
    
   }
   
   //prepend new blank page
   
   if( !current_nc->BP_HEAD ){
   
    current_nc->BP_HEAD = (struct blank_page*)malloc(sizeof(struct blank_page));
    
    current_nc->BP_TAIL = current_nc->BP_HEAD;
    
    current_nc->BP_HEAD->page_num = 1;
    
    current_nc->BP_HEAD->comment = NULL;
    current_nc->BP_HEAD->next = NULL;
   
    current_nc->BP_HEAD->prev = NULL;
  
    current_bp = current_nc->BP_HEAD;
    
   }
   else{
    
    current_nc->BP_TAIL->next = (struct blank_page*)malloc(sizeof(struct blank_page));
     
    current_nc->BP_TAIL->next->next = NULL;
    current_nc->BP_TAIL->next->prev = current_nc->BP_TAIL;
     
    current_nc->BP_TAIL = current_nc->BP_TAIL->next;
     
    current_nc->BP_TAIL->page_num = current_nc->BP_TAIL->prev->page_num+1;
     
    current_nc->BP_TAIL->comment = NULL;
     
    current_bp = current_nc->BP_TAIL;
    
   }
   
  }//end of if( !current_bp )
  else{
   
   struct blank_page *tmp_bp = NULL; 
   
   //current page is a blank page
   
   if( current_bp == current_nc->BP_HEAD ){
    
    current_nc->BP_HEAD->prev = (struct blank_page*)malloc(sizeof(struct blank_page));
    
    tmp_bp = current_nc->BP_HEAD->prev;
    tmp_bp->next = current_nc->BP_HEAD;
    tmp_bp->prev = NULL;
    
    tmp_bp->page_num = 1;
    tmp_bp->comment = NULL;
    
    current_bp = tmp_bp;
    current_nc->BP_HEAD = tmp_bp;
    
   }
   else{
    
    tmp_bp = current_bp->prev;
    
    current_bp->prev = (struct blank_page*)malloc(sizeof(struct blank_page));
    
    tmp_bp->next = current_bp->prev;
    current_bp->prev->prev = tmp_bp;
    current_bp->prev->next = current_bp;
    
    tmp_bp = current_bp->prev;
    tmp_bp->page_num = current_bp->page_num;
    
    tmp_bp->comment = NULL;
    
    current_bp = tmp_bp;
    
   }
   
   tmp_bp = tmp_bp->next;
   
   while( tmp_bp ){
    
    tmp_bp->page_num++;
    
    tmp_bp = tmp_bp->next;
    
   }
   
  }//end of if(current_bp)
  
  //prepend new page entry
  
  struct page_entry *tmp_pe = (struct page_entry *)malloc(sizeof(struct page_entry));
  
  tmp_pe->curr_nc = current_nc;
  tmp_pe->curr_bp = current_bp;
  
  if(current_bp->next)
   tmp_pe->p_page = current_pe->p_page;
  else
   tmp_pe->p_page = current_nc->page_num-1;
  
  tmp_pe->next = current_pe;
  tmp_pe->prev = current_pe->prev;
  
  if( current_pe->prev )
   current_pe->prev->next = tmp_pe;
  
  current_pe->prev = tmp_pe;
  
  if( dual_page_mode ){
   
   rcurrent_pe = current_pe;
   
   if( rcurrent_bp ){
   
    //hide comments of right page
    struct bp_comment *cmt = rcurrent_bp->comment;
     
    while( cmt ){
  
     gtk_widget_hide(cmt->comment->comment);
     cmt = cmt->next;
      
    }
    //hide comments of right page
    
   }
   
   rcurrent_bp = current_bp->next;
  
  }
  
  current_pe = tmp_pe;
  
  if(!current_pe->prev)
   PAGES = current_pe;
  
  char *title_str = (char*)malloc(100);
  
  if(doc){
  
   sprintf(title_str, "%s page %d-%d/%d",file_name, current_pe->p_page+1,current_bp->page_num, poppler_document_get_n_pages(doc));
   
  }
  else{
   
   gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
   
   if(file_name)
    sprintf(title_str, "%s page %d/%d", file_name, current_pe->curr_bp->page_num, page_num);
   else
    sprintf(title_str, "Untitled page %d/%d", current_pe->curr_bp->page_num, page_num);
    
  }
  //title
  
  gtk_window_set_title(GTK_WINDOW(win), title_str);
 
  free(title_str);
  //title
  
  display_comment(NULL);
  
 }//end of before_after == 1
 
 page_setup(&surface, NULL, draw_area, NULL);
 
 if(dual_page_mode)
  page_setup(&lsurface, NULL, ldraw_area, NULL);
 
}

void
insert_bp_before_cb (GtkWidget* widget, gpointer user_data) {
 insert_bp_cb(FALSE);
}

void
insert_bp_after_cb (GtkWidget* widget, gpointer user_data) {
 insert_bp_cb(TRUE);
}

gboolean 
add_comment_cb (GtkWidget* widget, gpointer user_data) {

 add_comment();
 
 return TRUE;
}

void display_comment(struct list_head *cm_HEAD){
 
 struct list_head *tmp;
 struct note *cm_entry;
 
 if(cm_HEAD){
 
  list_for_each(tmp, cm_HEAD){
   
   cm_entry = list_entry(tmp, struct note, list);
   gtk_widget_set_visible (cm_entry->comment, TRUE);
  
   gtk_widget_hide (cm_entry->comment);
   gtk_widget_show (cm_entry->comment);
  
   if(!dual_page_mode){
   
    gtk_widget_show(cm_entry->comment);
    gtk_widget_queue_draw (cm_entry->comment);
     
    gtk_layout_move(GTK_LAYOUT(layout),
                    cm_entry->comment,
                    (gint)(cm_entry->x*zoom_factor)+width_offset,
                    (gint)(cm_entry->y*zoom_factor)+height_offset);
   
   }else{ //dual-page mode
    
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation(scrolled_window, &sw_alloc);
   
    GtkAllocation da_alloc, lda_alloc;
    gtk_widget_get_allocation(draw_area, &da_alloc);
    gtk_widget_get_allocation(ldraw_area, &lda_alloc);
   
    if( &(*cm_HEAD) == &current_nc->CM_HEAD ){ //left page
    
     if( azoom_factor == 0.0 ){
    
      int height;
      
      height = ( sw_alloc.height - 2 - da_alloc.height )/2;
      
      if(height < 0)
       height = 0;
     
      gtk_layout_move( GTK_LAYOUT(layout), 
                       cm_entry->comment, 
                       (gint)(cm_entry->x*zoom_factor),
                       (gint)(cm_entry->y*zoom_factor)+height);
      
     }
     else{ //page with different size
    
      if( azoom_factor < 0 ){ //left page
      
       int height;
    
       height = ( sw_alloc.height - 2 - lda_alloc.height )/2;
      
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
     
      int height = ( sw_alloc.height - 2 - da_alloc.height )/2;
    
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
                        cm_entry->comment,
                        (gint)(cm_entry->x*azoom_factor)+lda_alloc.width, 
                        (gint)(cm_entry->y*azoom_factor)+daa.y);
      
      }
     
     }
  
    }
   
    gtk_widget_hide(cm_entry->comment);
    gtk_widget_show(cm_entry->comment);
  
   }
  
  }//list_for_each(tmp, cm_HEAD
 } //end of if(cm_HEAD)
 
 struct bp_comment *cmt;
 
 if( blank_page ){
  
  if( !dual_page_mode ){
   
   if( current_bp ){
   
    cmt = current_bp->comment;
    
    while( cmt ){
   
     gtk_widget_show(cmt->comment->comment);
     gtk_widget_queue_draw (cmt->comment->comment);
     
     gtk_layout_move(GTK_LAYOUT(layout),
                     cmt->comment->comment,
                     (gint)(cmt->comment->x*zoom_factor)+width_offset,
                     (gint)(cmt->comment->y*zoom_factor)+height_offset);
    
     cmt = cmt->next;
     
    }
    
   }
   
  }
  else{ //dual page mode
   
   if( current_bp ){
    
    cmt = current_bp->comment;
    
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation(scrolled_window, &sw_alloc);
    
    GtkAllocation lda_alloc;
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
    
    int height = ( sw_alloc.height - 2 - lda_alloc.height )/2;
    
    if(height < 0)
     height = 0;
    
    while( cmt ){
    
     gtk_layout_move( GTK_LAYOUT(layout), 
                      cmt->comment->comment, 
                      (gint)(cmt->comment->x*zoom_factor),
                      (gint)(cmt->comment->y*zoom_factor)+height);
     
     gtk_widget_hide(cmt->comment->comment);
     gtk_widget_show(cmt->comment->comment);
     
     cmt = cmt->next;
    
    }
    
   }
   
   if( rcurrent_bp ){
    
    struct bp_comment *cmt = rcurrent_bp->comment;
   
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation(scrolled_window, &sw_alloc);
   
    GtkAllocation da_alloc;
    gtk_widget_get_allocation (draw_area, &da_alloc);
    
    int height = ( sw_alloc.height - 2 - da_alloc.height )/2;
    
    if(height < 0)
     height = 0;
    
    while( cmt ){
     
     gtk_layout_move( GTK_LAYOUT(layout), 
                     cmt->comment->comment, 
                     (gint)(cmt->comment->x*zoom_factor)+da_alloc.width,
                     (gint)(cmt->comment->y*zoom_factor)+height);
     
     gtk_widget_hide(cmt->comment->comment);
     gtk_widget_show(cmt->comment->comment);
     
     cmt = cmt->next;
     
    }
    
   }
   
  }
  
 }
 
}
