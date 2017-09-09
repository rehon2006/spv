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

#include <pango/pangocairo.h>

#include "list.h"
#include "gui.h"

static void toggle_cursor_mode_cb( GtkWidget *widget, gpointer user_data ){

 if( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(cursor_modeMi) ) ){
 
  g_timeout_add(500, (GSourceFunc) time_handler, (gpointer) win);
  time_handler(win);
 
 }

}

void toggle_dual_page_cb(GtkWidget *widget, gpointer user_data){
 
 dual_page_cb();

}

void full_screen_cb(void){

 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) ){
  
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
  
  gtk_window_fullscreen (GTK_WINDOW (win));
  gtk_widget_hide (menubar);
  
  gint width, height;
   
  gtk_layout_get_size(GTK_LAYOUT(layout), &width, &height);
   
  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
  
  if( win_alloc.height > da_height )
    zoom_height();
  
 }
 else{ 
 
  gtk_window_unfullscreen (GTK_WINDOW (win));
  gtk_widget_show (menubar);
  
  if( dual_page_mode && page_width >= page_height){
   
   int dp_width = (int)(zoom_factor*page_width);
   
   GtkAllocation win_alloc;
   gtk_widget_get_allocation (win, &win_alloc);

   GtkAllocation mb_alloc;
   gtk_widget_get_allocation (menubar, &mb_alloc);
   
   GtkAllocation sw_alloc;
   gtk_widget_get_allocation (scrolled_window, &sw_alloc);
   
   height_offset = (sw_alloc.height - da_height -100)/2;
   
   gint width, height;
   
   gtk_layout_get_size(GTK_LAYOUT(layout), &width, &height);
   
   gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
   gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
   
   struct list_head *tmp;

   list_for_each(tmp, &NOTE_HEAD){

    struct note *tmp1;
    tmp1= list_entry(tmp, struct note, list);
   
    if( tmp1->page_num == current_page_num+2 ){
    
     int dp_width = (int)(zoom_factor*page_width);
    
     gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, 
                     (gint)(tmp1->x*zoom_factor)+dp_width+1, 
                     (gint)(tmp1->y*zoom_factor)+height_offset);
    
    
     gtk_widget_show(tmp1->comment);
    
    }
    else if( tmp1->page_num == current_page_num+1 ){
    
    
     gtk_widget_show(tmp1->comment);
   
     gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, (gint)(tmp1->x*zoom_factor), (gint)(tmp1->y*zoom_factor)+height_offset);
    
     }else{
      gtk_widget_hide(tmp1->comment);
     }
 

   } 
   
  }else if( !dual_page_mode )
   zoom_width();
  
 }
 
}

static void toggle_full_screen_cb(GtkWidget *widget, gpointer user_data){
 
 full_screen_cb();
 
}

static void inverted_color_cb(void){
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
 
  da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
  da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
  da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
 
  da_background_color.red = CLAMP ((guint) (1* 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (1 * 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (1 * 65535), 0, 65535);
 
 }
 else{
  da_glyph_color.red = CLAMP ((guint) (1 * 65535), 0, 65535);
  da_glyph_color.green = CLAMP ((guint) (1* 65535), 0, 65535);
  da_glyph_color.blue = CLAMP ((guint) (1* 65535), 0, 65535);
 
  da_background_color.red = CLAMP ((guint) (0* 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (0 * 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (0 * 65535), 0, 65535);
 }
 
 page_change();
 
}

static void toggle_inverted_color_cb(GtkWidget *widget, gpointer user_data){
 
 inverted_color_cb();
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  gtk_widget_set_sensitive( change_background_colorMi, FALSE );
 }
 else{
  gtk_widget_set_sensitive( change_background_colorMi, TRUE );
 }
 
}

static void change_background_color_cb(GtkWidget *widget, gpointer user_data){
 
 if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  if ( ! gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))){
   PDF_BACKGROUND_COLOR_CHANGED = FALSE;
  }else{ 
   PDF_BACKGROUND_COLOR_CHANGED = TRUE;
  }
 }
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(widget) ) ){
  gtk_widget_set_sensitive( inverted_colorMi, FALSE );
 }
 else{
  gtk_widget_set_sensitive( inverted_colorMi, TRUE );
 }
 
 page_change();
 
}

void toggle_hide_toolbar(void){
 
 if( TOOL_BAR_VISIBLE ){
  
  gtk_widget_hide(toolbar);
  TOOL_BAR_VISIBLE = FALSE;
    
 } else {
    
  gtk_widget_show(toolbar);
  TOOL_BAR_VISIBLE = TRUE;
    
 }
  
 GdkScreen *screen = gdk_screen_get_default ();
 
 if( gdk_screen_get_height(screen) > (int)(page_height*zoom_factor) )
  zoom_height();
    
}

void copy_text(void){

 if (selection_surface){
  
  GtkClipboard *clipboard = 
     gtk_clipboard_get_for_display(gdk_display_get_default(),
                                   GDK_SELECTION_CLIPBOARD);
     
  gtk_clipboard_set_text (clipboard, selected_text->str, -1);
 
 }
        
}

static void copy_text_cb(GtkWidget *widget, gpointer user_data) {
 
 copy_text();
 
}

static void find_next_button_clicked_cb(GtkWidget *widget, gpointer user_data) {
 
 if( mode != TEXT_SEARCH_NEXT  && pre_mode != TEXT_SEARCH_PREV && word_not_found == 1 ){
  
  //first search forward 
 
  pre_mode = TEXT_SELECTION;
  mode = TEXT_SEARCH_NEXT;
  
  find_current_page_num = current_page_num;
  
  if( pre_keyval == GDK_KEY_question )
   if(find_ptr_head)
    find_ptr_head = g_list_reverse(find_ptr_head);
    
  pre_keyval = GDK_KEY_slash;
  
  find_next(findbar);
  
 }
 else if( mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SEARCH_NEXT && word_not_found == 0 ){
  
  //search forward after searching previous word
  
  if(find_ptr_head)
   find_ptr_head = g_list_reverse(find_ptr_head);
 
  pre_keyval = GDK_KEY_slash;
  
  pre_mode = TEXT_SELECTION;
  mode = TEXT_SEARCH_NEXT;
  
  search_n(findbar);
  
 }
 else{
  
  //keeping searching next word
 
  pre_keyval = GDK_KEY_n;
 
  search_n(findbar);
 
 }
 
}

static void find_prev_button_clicked_cb(GtkWidget *widget, gpointer user_data) {
 
 if( mode != TEXT_SEARCH_PREV  && pre_mode != TEXT_SEARCH_NEXT && word_not_found == 1 ){
 
  pre_mode = TEXT_SELECTION;
  mode = TEXT_SEARCH_PREV;
  
  find_current_page_num = current_page_num;
  
  if( pre_keyval == GDK_KEY_slash )
   if(find_ptr_head)
    find_ptr_head = g_list_reverse(find_ptr_head);

  pre_keyval = GDK_KEY_question;
  
  find_prev(findbar);
  
 }
 else if( mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SEARCH_PREV && word_not_found == 0 ){
  
  if(find_ptr_head)
   find_ptr_head = g_list_reverse(find_ptr_head);
  
  pre_keyval = GDK_KEY_question;
  
  search_n(findbar);
  
 }
 else{
  
  if( mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SELECTION ){
   if(find_ptr_head)
    find_ptr_head = g_list_reverse(find_ptr_head);
  }
  
  search_N(findbar);
 
  if( mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SEARCH_PREV  ){
   
   pre_mode = TEXT_SEARCH_NEXT;
   mode = TEXT_SEARCH_PREV;
   
  }
  
  pre_keyval = GDK_KEY_N;
 
 }
 
}

static void toggle_hide_toolbar_cb(GtkWidget *widget, gpointer user_data) {
 
 if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
 
  TOOL_BAR_VISIBLE = TRUE;
  toggle_hide_toolbar();
  
 }
 else{
  
  TOOL_BAR_VISIBLE = FALSE;
  
 }
 
}

static void color_set(int option){
 
 GtkWidget *colorseldlg;
 
 if(option == 1)
  colorseldlg = gtk_color_chooser_dialog_new ("Select Highlight Color", NULL);
 else if(option == 2)
  colorseldlg = gtk_color_chooser_dialog_new ("Select Background Color", NULL);
 else
  return;
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){ 
 
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
  
  if(option == 1){
  
   text_highlight_mode_change();
  
   hc[0] = (guchar)(color.red*255);
   hc[1] = (guchar)(color.green*255);
   hc[2] = (guchar)(color.blue*255);
  
   char hc_color_name[9];
  
   sprintf(hc_color_name, "%02x%02x%02x00",hc[0],hc[1],hc[2]);
 
   unsigned long hc_color = strtoul(hc_color_name, NULL, 16);
   gdk_pixbuf_fill(hc_pixbuf, hc_color);
  
   gtk_image_set_from_pixbuf(GTK_IMAGE(hc_button_Image),hc_pixbuf);
  }
  else if(option == 2){
   background_color[0] = color.red;
   background_color[1] = color.green;
   background_color[2] = color.blue;
   
   PDF_BACKGROUND_COLOR_CHANGED = TRUE;
   gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(change_background_colorMi), TRUE);
   page_change();
  }
  
  da_background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (color.green* 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (color.blue* 65535), 0, 65535);
  
 }
 
 gtk_widget_destroy (colorseldlg);
    
}

static void color_set_cb(GtkWidget *widget, gpointer userdata){
 color_set(1);
}

static gboolean draw_area_draw( GtkWidget *widget, cairo_t *cr, gpointer data ){
 
 draw_count++;
 
 cairo_save (cr);
 
 if(widget == draw_area){
  cairo_set_source_surface (cr, surface, 0, 0);
 }
 else if(widget == ldraw_area && lsurface ){
  cairo_set_source_surface (cr, lsurface, 0, 0);
 }
 
 cairo_paint (cr);
 
 cairo_restore (cr);
 
 if (selection_surface ) {
   
   if( widget == draw_area  ){
    
    if( press_rl ){
     cairo_set_source_surface (cr, selection_surface, 0, 0);
     cairo_paint (cr);
    }
    
   }
   else if( widget == ldraw_area  ){
    
    if( !press_rl ){
     cairo_set_source_surface (cr, selection_surface, 0, 0);
     cairo_paint (cr);
    }
    
   }
   
  }
 
 return TRUE;
 
}

static void
da_size_allocate_cb( GtkWidget *win, GtkAllocation *allocation, gpointer data){

 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);

 if( da_width > pre_da_width ){
  
  if(width_offset < 0)
   width_offset = 0;
  
 }
 else if( da_width < pre_da_width ){
  
  if(width_offset < 0)
   width_offset = 0;
  
 }
 
}

static gboolean
selections_render( GtkWidget *widget ){
 
 PopplerRectangle doc_area;
 
 doc_area.x1 = start_x/zoom_factor;
 doc_area.y1 = start_y/zoom_factor;
 doc_area.x2 = stop_x/zoom_factor;
 doc_area.y2 = stop_y/zoom_factor;
 
 if (selection_surface)
   cairo_surface_destroy (selection_surface);
 
 cairo_t *cr;
 
 selection_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
						 (gint)((page_width*zoom_factor)+0.5), 
						 (gint)((page_height*zoom_factor)+0.5));
  
 cr = cairo_create (selection_surface);
 
 cairo_scale(cr, zoom_factor, zoom_factor);
 
 if( !dual_page_mode ){
 
  if(!page){
   page = poppler_document_get_page(doc, current_page_num);
  }
 
  poppler_page_render_selection (page, cr,
				 &doc_area, &da_doc_area,
				 POPPLER_SELECTION_GLYPH,
				 &da_glyph_color,
				 &da_background_color);
				 
  gtk_widget_queue_draw (draw_area);
 }
 else{ //dual-page mode
 
  if( widget == draw_area ){
   
   if(!page){
    page = poppler_document_get_page(doc, current_page_num+1);
   }
   
   poppler_page_render_selection (page, cr,
				 &doc_area, &da_doc_area,
				 POPPLER_SELECTION_GLYPH,
				 &da_glyph_color,
				 &da_background_color);
   
   gtk_widget_queue_draw (draw_area);
  }
  else if( widget == ldraw_area ){
   
   if(!lpage){
    lpage = poppler_document_get_page(doc, current_page_num);
   }
   
   poppler_page_render_selection (lpage, cr,
				  &doc_area, &da_doc_area,
				  POPPLER_SELECTION_GLYPH,
				  &da_glyph_color,
				  &da_background_color);
  
   gtk_widget_queue_draw (ldraw_area); 
   
   if(lpage){
    g_object_unref (G_OBJECT (lpage));
    lpage = NULL;
   }
   
  }
  
  if(page){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
 }
 
 cairo_destroy (cr);

 da_doc_area = doc_area;
 
 if(page){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 return TRUE;
  
}

static gboolean
update_cursor (GdkCursorType cursor_type, GtkWidget *widget){
 
 GdkWindow *window = gtk_widget_get_window (widget);

 GdkCursor *cursor = NULL;

 if (cursor_type == da_cursor)
  return FALSE;

 if (cursor_type != GDK_LAST_CURSOR) {
  cursor = gdk_cursor_new_for_display (gtk_widget_get_display (widget),
				       cursor_type);
 }

 da_cursor = cursor_type;
 
 gdk_window_set_cursor (window, cursor);
 gdk_flush ();
 
 if (cursor)
  g_object_unref (cursor);
 
 return TRUE;
 
}

static gboolean
da_touch_cb(GtkWidget *widget, GdkEventTouch *event, gpointer data){

 g_message("da_touch");

}

static gboolean
da_button_press (GtkWidget *widget, GdkEventButton *event, gpointer data){
 
 start_x = event->x;
 start_y = event->y;
 
 stop_x = event->x;
 stop_y = event->y;
 
 if (selection_surface){
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area);

  if(dual_page_mode)
   gtk_widget_queue_draw (ldraw_area);
  
  if(widget == draw_area){
   press_rl = 1;
   
  }
  else if(widget == ldraw_area){
   press_rl = 0;
  }
  
 }
 
 if( find_ptr ){
         
  invert_search_region();

  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
    
  if( dual_page_mode ){
   lmatches = rmatches = NULL;
  }
    
 }
 
 return TRUE;

}

static gboolean
da_motion_notify (GtkWidget *widget, GdkEventMotion *event, gpointer data){

 if( start_x != -1.0){
  
  gdouble page_width, page_height;
  
  stop_x = event->x;
  stop_y = event->y;
  
  selections_render( widget );
  
 }
 else{
 
  gboolean over_text;
  
  if( widget == draw_area ){
   over_text = cairo_region_contains_point (da_selection_region,
                                           event->x,
                                           event->y);
   press_rl = 1;
   
  }
  else if( widget == ldraw_area ){
   over_text = cairo_region_contains_point (lda_selection_region,
                                            event->x,
                                            event->y);
                                            
   press_rl = 0;
   
  }
 
  update_cursor(over_text ? GDK_XTERM : GDK_LAST_CURSOR, widget);
 
 }
 
 return TRUE;
 
}

static gboolean
da_button_release (GtkWidget *widget, GdkEventButton *event, gpointer data){

 if(start_y == -1.0)
  return FALSE;
 
 if(mode == TEXT_HIGHLIGHT || mode == ERASE_TEXT_HIGHLIGHT){
 
 PopplerRectangle rect;
 
 rect.x1 = da_doc_area.x1;
 rect.y1 = da_doc_area.y1;
 rect.x2 = da_doc_area.x2;
 rect.y2 = da_doc_area.y2;
 
 if(rect.y1 == rect.y2)
        rect.y2++;
 
 if(rect.x1 == rect.x2)
        rect.x2++;
 
 GList *selections;
 
 if( !dual_page_mode ){ 
 
  if( !page ){
   page = poppler_document_get_page(doc, current_page_num);
  }
  
  selections = poppler_page_get_selection_region(page, 1.0,
                                                 POPPLER_SELECTION_GLYPH, 
                                                 &rect);
  
 }
 else{
 
  if( widget == draw_area ){
   
   if( !page ){
    page = poppler_document_get_page(doc, current_page_num+1);
   }
   
   selections = poppler_page_get_selection_region(page, 1.0,
                                                  POPPLER_SELECTION_GLYPH, 
                                                  &rect);
   
  }
  else if( widget == ldraw_area ){
   
   if( !lpage ){
    lpage = poppler_document_get_page(doc, current_page_num);
   }
   
   selections = poppler_page_get_selection_region(lpage, 1.0,
                                                  POPPLER_SELECTION_GLYPH, 
                                                  &rect);
   
  }
 
 }
 
 PopplerRectangle *rectangle;
 
 GList *selection = g_list_first (selections);
 
 cairo_rectangle_int_t *tmp_hr= NULL;
 
 if( mode == TEXT_HIGHLIGHT ){
  
  if(hr)
   free(hr);
  
  guint sel_count = g_list_length(selection);
  
  hr = (cairo_rectangle_int_t *)malloc(sizeof(cairo_rectangle_int_t)*(sel_count+1));
  
  tmp_hr = hr;
  
  hr[sel_count].x = -1;
  hr[sel_count].y = -1;
  hr[sel_count].width = -1;
  hr[sel_count].height = -1;
 
 }
 else if( mode == ERASE_TEXT_HIGHLIGHT ){
  
  if(ihr)
   free(ihr);
  
  guint sel_count = g_list_length(selection);
  
  ihr = (cairo_rectangle_int_t *)malloc(sizeof(cairo_rectangle_int_t)*(sel_count+1));
  
  tmp_hr = ihr;
  
  ihr[sel_count].x = -1;
  ihr[sel_count].y = -1;
  ihr[sel_count].width = -1;
  ihr[sel_count].height = -1;
  
 }
  
 for ( ; NULL != selection ; selection = g_list_next (selection)) {
  
  
  rectangle = (PopplerRectangle *)selection->data;
  
  tmp_hr->x = (int)(rectangle->x1*zoom_factor+0.5);
  tmp_hr->y = (int)(rectangle->y1*zoom_factor+0.5);
  tmp_hr->width = (int)((rectangle->x2-rectangle->x1)*zoom_factor+0.5);
  tmp_hr->height = (int)((rectangle->y2-rectangle->y1)*zoom_factor+0.5);
  tmp_hr++;
   
 }
 
 if( mode == TEXT_HIGHLIGHT ){
 
  save_highlight( widget );
  
  if(hr){
   free(hr);
   hr = NULL;
  }
  
 }
 else if( mode == ERASE_TEXT_HIGHLIGHT ){
 
  erase_highlight( widget );
  
  if(ihr){
   free(ihr);
   ihr = NULL;
  }
  
 }
 
 da_doc_area.x1 = 0.0;
 da_doc_area.x2 = 0.0;
 da_doc_area.y1 = 0.0;
 da_doc_area.y2 = 0.0;
 
 g_list_free(selections);
 
 if(page){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 if(lpage){
  g_object_unref (G_OBJECT (lpage));
  lpage = NULL;
 }
  
  gtk_widget_queue_draw (draw_area);
 } // end of if(mode == TEXT_HIGHLIGHT || mode == ERASE_TEXT_HIGHLIGHT)
 else if( mode == TEXT_SELECTION ){
  
  if(selected_text){
   g_string_free(selected_text, FALSE);
  }
  
  selected_text = g_string_new (NULL);
  
  char* temp_text= NULL;
  
  if( !dual_page_mode ){
   
   if(page == NULL){
    page = poppler_document_get_page(doc, current_page_num);
   }
  
   temp_text = 
        poppler_page_get_selected_text(page,
                                       POPPLER_SELECTION_GLYPH, 
                                       &da_doc_area);
  
  }
  else{ //dual-page mode
  
   if( widget == draw_area ){
   
    if(page == NULL){
     page = poppler_document_get_page(doc, current_page_num+1);
    }
  
    temp_text = 
        poppler_page_get_selected_text(page,
                                       POPPLER_SELECTION_GLYPH, 
                                       &da_doc_area);
   
   }
   else if( widget == ldraw_area ){
   
    if(lpage == NULL){
     lpage = poppler_document_get_page(doc, current_page_num);
    }
  
    temp_text = 
         poppler_page_get_selected_text(lpage,
                                        POPPLER_SELECTION_GLYPH, 
                                        &da_doc_area);
   
   }
  
  }
  
  g_string_append (selected_text, temp_text);
  g_string_append (selected_text, "\n");
  
  GtkClipboard *clipboard;
  
  #if __linux__
  clipboard = 
     gtk_clipboard_get_for_display(gdk_display_get_default(),
                                   GDK_SELECTION_PRIMARY);
  #else
  clipboard = 
     gtk_clipboard_get_for_display(gdk_display_get_default(),
                                   GDK_SELECTION_CLIPBOARD);
  #endif
 
  gtk_clipboard_set_text (clipboard, temp_text, -1);
  
  if(temp_text)
   free(temp_text); 
  
  if( page ){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
  if( lpage ){
   g_object_unref (G_OBJECT (lpage));
   lpage = NULL;
  }
  
 }
 
 start_x = start_y = stop_x = stop_y= -1.0;
 
 return TRUE;
 
}

void init_gui(void){

 inverted = 0;
 
 current_page_num = 0;

 mode = pre_mode = TEXT_SELECTION;
 
 KEY_BUTTON_SEARCH = TRUE;
 
 hr = ihr = NULL;
 
 hc[0] = 0xff;
 hc[1] = 0xff;
 hc[2] = 0x00;
 
 color.red = 1.0;
 color.green = 1.0;
 color.blue = 0.0;
 
 width_offset = 0;
 
 left_right = 0;
 
 word_not_found = 1;

 find_ptr = find_ptr_head = NULL;
 lmatches = rmatches = NULL;
 page_change_str = NULL;

 FONT_SIZE = 11;

 INIT_LIST_HEAD(&NOTE_HEAD);
 INIT_LIST_HEAD(&HR_HEAD);
 
 selected_text = NULL;
   
 layout_move = 0;

 start_x = start_y = stop_x = stop_y= -1.0;
 
 pre_keyval = GDK_KEY_Escape;
 
 da_cursor = GDK_LAST_CURSOR;
    
 scroll_count = 0; 
 scroll_time = 0;
 scroll_zoom = -1;
  
 accel_group = gtk_accel_group_new();
 
 win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_window_add_accel_group(GTK_WINDOW(win), accel_group);
 
 //*****
 //cursor_enable = TRUE;
 //*****
 
 GdkScreen *screen = gdk_screen_get_default ();
 
 if( page_height > page_width ){ //portrait
  if(page_height >= gdk_screen_get_height(screen)){
   gtk_widget_set_size_request(win, (int)(page_width/2)+30, gdk_screen_get_height(screen)/2 - 80);
  }else{
   gtk_widget_set_size_request(win, (int)page_width+30, gdk_screen_get_height(screen) - 80);
  }
 }else{ //landscape
  if(page_height >= gdk_screen_get_height(screen)){
   gtk_widget_set_size_request(win, (int)(page_width/2)+1, (int)page_height/2 + 80);
  }else{
   gtk_widget_set_size_request(win, (int)page_width+1, (int)page_height + 80);
  }
 }
 
 g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(on_destroy), NULL);

 vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    
 gtk_container_add(GTK_CONTAINER(win), vbox);
 
 find_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
 
 findbar = gtk_toolbar_new();
 gtk_toolbar_set_style (GTK_TOOLBAR (findbar), GTK_TOOLBAR_BOTH_HORIZ); 
 
 findtext = gtk_entry_new ();
 gtk_entry_set_text(GTK_ENTRY(findtext), "");
 gtk_editable_set_editable(GTK_EDITABLE(findtext), TRUE);
 
 g_signal_connect(G_OBJECT(findtext), "key-release-event", G_CALLBACK(on_findtext_key_release), findbar);

 gtk_container_add(GTK_CONTAINER(find_hbox), findtext);
 
 GtkToolItem *findbox_item = gtk_tool_item_new ();
 
 gtk_container_add (GTK_CONTAINER (findbox_item), find_hbox);
 gtk_toolbar_insert (GTK_TOOLBAR (findbar), findbox_item, -1);
 
 findNext_button = gtk_button_new_with_label("Find Next");
 g_signal_connect(G_OBJECT(findNext_button), "clicked", 
       G_CALLBACK(find_next_button_clicked_cb), NULL);
 
 findPrev_button = gtk_button_new_with_label("Find Previous");
 g_signal_connect(G_OBJECT(findPrev_button), "clicked", 
       G_CALLBACK(find_prev_button_clicked_cb), NULL);
 
 gtk_container_add(GTK_CONTAINER(find_hbox), findPrev_button);
 gtk_container_add(GTK_CONTAINER(find_hbox), findNext_button);
 
 find_exit_button = gtk_button_new_from_icon_name ("window-close", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(find_exit_button, FALSE);
 g_signal_connect(G_OBJECT(find_exit_button), "clicked", 
        G_CALLBACK(find_exit_cb), NULL);
        
 gtk_container_add(GTK_CONTAINER(find_hbox), find_exit_button);
 
 scrolled_window = gtk_scrolled_window_new(NULL,NULL);
 
 gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
 gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 1);
 
 g_signal_connect(G_OBJECT(scrolled_window), "size-allocate", G_CALLBACK(size_allocate_cb), NULL);
 
 //*****Menubar Initialization*****
 
 menubar = gtk_menu_bar_new();
 
 fileMenu = gtk_menu_new();
 editMenu = gtk_menu_new();
 viewMenu = gtk_menu_new();
    
 goMenu = gtk_menu_new();
 modeMenu = gtk_menu_new();
 noteMenu = gtk_menu_new();

 fileMi = gtk_menu_item_new_with_label("File");
 quitMi = gtk_menu_item_new_with_label("Quit");

 g_signal_connect(G_OBJECT(quitMi), "activate",
     G_CALLBACK(on_destroy), NULL);
 
 editMi = gtk_menu_item_new_with_label("Edit");
 copyMi = gtk_menu_item_new_with_label("Copy");
 g_signal_connect(G_OBJECT(copyMi), "activate",
     G_CALLBACK(copy_text_cb), NULL);
 findMi = gtk_menu_item_new_with_label("Find");
 g_signal_connect(G_OBJECT(findMi), "activate",
     G_CALLBACK(find_text_cb), NULL);
    
 viewMi = gtk_menu_item_new_with_label("View");
    
 zoominMi = gtk_menu_item_new_with_label("Zoom In");
 g_signal_connect(G_OBJECT(zoominMi), "activate",
     G_CALLBACK(zoom_in_cb), NULL);
 zoomoutMi = gtk_menu_item_new_with_label("Zoom Out");
 g_signal_connect(G_OBJECT(zoomoutMi), "activate",
     G_CALLBACK(zoom_out_cb), NULL);
    
 zoomwidthMi = gtk_menu_item_new_with_label("Zoom Width");
 g_signal_connect(G_OBJECT(zoomwidthMi), "activate",
     G_CALLBACK(zoom_width_cb), NULL);
     
 zoomheightMi = gtk_menu_item_new_with_label("Zoom Height");
 g_signal_connect(G_OBJECT(zoomheightMi), "activate",
     G_CALLBACK(zoom_height_cb), NULL);
 
 hide_toolbarMi = gtk_check_menu_item_new_with_label("Hide Toolbar");
 g_signal_connect(G_OBJECT(hide_toolbarMi), "activate", 
        G_CALLBACK(toggle_hide_toolbar_cb), NULL);
 
 change_background_colorMi = gtk_check_menu_item_new_with_label("Change Background Color");
 g_signal_connect(G_OBJECT(change_background_colorMi), "activate", 
        G_CALLBACK(change_background_color_cb), NULL);
 
 dual_pageMi = gtk_check_menu_item_new_with_label("Dual Page");
 g_signal_connect(G_OBJECT(dual_pageMi), "activate", 
        G_CALLBACK(toggle_dual_page_cb), NULL);
 
 full_screenMi = gtk_check_menu_item_new_with_label("Full Screen");
 g_signal_connect(G_OBJECT(full_screenMi), "activate", 
        G_CALLBACK(toggle_full_screen_cb), NULL);
 
 inverted_colorMi = gtk_check_menu_item_new_with_label("Invert Colors");
 g_signal_connect(G_OBJECT(inverted_colorMi), "activate", 
        G_CALLBACK(toggle_inverted_color_cb), NULL);
 
 //******       
 //cursor_modeMi = gtk_check_menu_item_new_with_label("Cursor mode");
 //g_signal_connect(G_OBJECT(cursor_modeMi), "activate", 
 //       G_CALLBACK(toggle_cursor_mode_cb), NULL);
 
 //continuous_modeMi = gtk_check_menu_item_new_with_label("Continuous mode");
 //g_signal_connect(G_OBJECT(continuous_modeMi), "activate", 
 //       G_CALLBACK(toggle_continuous_mode_cb), NULL);
 //******
 
 goMi = gtk_menu_item_new_with_label("Go");
 nextpageMi = gtk_menu_item_new_with_label("Next Page");
 prepageMi = gtk_menu_item_new_with_label("Previous Page");

 modeMi = gtk_menu_item_new_with_label("Mode");
    
 normalMi = gtk_menu_item_new_with_label("Normal mode");
 text_highlightMi = gtk_menu_item_new_with_label("Text Highlight mode");
 erase_text_highlightMi = gtk_menu_item_new_with_label("Erase Text Highlight mode");

 noteMi = gtk_menu_item_new_with_label("Note");
 
 add_commentMi = gtk_menu_item_new_with_label("Add a Comment");
 save_noteMi = gtk_menu_item_new_with_label("Save Notes");
 save_commentMi = gtk_menu_item_new_with_label("Save Comments");
    
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);
 
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMi), editMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), copyMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), findMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), editMi);
 
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMi), viewMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), zoominMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), zoomoutMi);
    
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), zoomwidthMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), zoomheightMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), hide_toolbarMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), change_background_colorMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), dual_pageMi);
 gtk_widget_add_accelerator(dual_pageMi, "activate", accel_group,
                     GDK_KEY_d, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), full_screenMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), inverted_colorMi);
 gtk_widget_add_accelerator(inverted_colorMi, "activate", accel_group,
                     GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 //******
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), cursor_modeMi);
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), continuous_modeMi);
 //******
 
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), viewMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(goMi), goMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(goMenu), prepageMi);
 g_signal_connect(G_OBJECT(prepageMi), "activate",
     G_CALLBACK(prev_page_cb), NULL);
 gtk_widget_add_accelerator(prepageMi, "activate", accel_group,
                     GDK_KEY_Page_Up, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 gtk_menu_shell_append(GTK_MENU_SHELL(goMenu), nextpageMi);
 g_signal_connect(G_OBJECT(nextpageMi), "activate",
     G_CALLBACK(next_page_cb), NULL);
    
 gtk_widget_add_accelerator(nextpageMi, "activate", accel_group,
                     GDK_KEY_Page_Down, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), goMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(modeMi), modeMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), normalMi);
 g_signal_connect(G_OBJECT( normalMi), "activate",
     G_CALLBACK(text_selection_mode_cb), NULL);
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), text_highlightMi);
 g_signal_connect(G_OBJECT( text_highlightMi), "activate",
     G_CALLBACK(text_highlight_mode_cb), NULL);
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), erase_text_highlightMi);
 g_signal_connect(G_OBJECT( erase_text_highlightMi), "activate",
     G_CALLBACK(erase_text_highlight_mode_cb), NULL);
    
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), modeMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(noteMi), noteMenu);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), add_commentMi);
 
 g_signal_connect(G_OBJECT( add_commentMi), "activate",
     G_CALLBACK(add_comment_cb), NULL);

 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), save_noteMi);
 g_signal_connect(G_OBJECT( save_noteMi), "activate",
     G_CALLBACK(save_note_cb), NULL);
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), save_commentMi);
 g_signal_connect(G_OBJECT( save_commentMi), "activate",
     G_CALLBACK(save_comment_cb), NULL);
    
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), noteMi);

 gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
 
 //*****Menubar Initialization*****
 
 toolbar = gtk_toolbar_new();
 gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
 
 TOOL_BAR_VISIBLE = FALSE;
 
 GtkWidget *toolbar_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
 
 gtk_box_set_spacing(GTK_BOX(toolbar_box), 1);
 
 hc_pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 20, 20);
 char hc_color_name[9];
  
 sprintf(hc_color_name, "%02x%02x%02x00",hc[0],hc[1],hc[2]);
 
 unsigned long hc_color = strtoul (hc_color_name, NULL, 16);
 
 gdk_pixbuf_fill(hc_pixbuf, hc_color);
 
 hc_button = gtk_button_new();
 
 hc_button_Image = gtk_image_new_from_pixbuf(hc_pixbuf);
 gtk_button_set_image(GTK_BUTTON(hc_button),hc_button_Image); 
 gtk_widget_set_can_focus(hc_button, FALSE);
 g_signal_connect(G_OBJECT(hc_button), "clicked",
                  G_CALLBACK(color_set_cb), NULL); 
 
 gtk_widget_set_can_focus(hc_button, FALSE);
 
 gtk_container_add (GTK_CONTAINER (toolbar_box), hc_button);
 
 highlight_color = gtk_tool_item_new ();
 
 gtk_container_add(GTK_CONTAINER(highlight_color), toolbar_box);
 
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), highlight_color, -1);
 
 save_note_button = gtk_button_new_from_icon_name ("document-save", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(save_note_button, FALSE);
 g_signal_connect(G_OBJECT(save_note_button), "clicked", 
        G_CALLBACK(save_note), NULL);

 zoom_in_button = gtk_button_new_from_icon_name ("zoom-in", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(zoom_in_button, FALSE);
 g_signal_connect(G_OBJECT(zoom_in_button), "clicked", 
        G_CALLBACK(zoom_in), NULL);
 
 zoom_out_button = gtk_button_new_from_icon_name ("zoom-out", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(zoom_out_button, FALSE);
 g_signal_connect(G_OBJECT(zoom_out_button), "clicked", 
        G_CALLBACK(zoom_out), NULL);
 
 zoom_width_button = gtk_button_new_from_icon_name ("zoom-fit-best", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(zoom_width_button, FALSE);
 g_signal_connect(G_OBJECT(zoom_width_button), "clicked", 
        G_CALLBACK(zoom_width), NULL);
 
 prev_page_button = gtk_button_new_from_icon_name ("go-previous", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(prev_page_button, FALSE);
 g_signal_connect(G_OBJECT(prev_page_button), "clicked", 
        G_CALLBACK(prev_page), NULL);
 
 next_page_button = gtk_button_new_from_icon_name ("go-next", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(next_page_button, FALSE);
 g_signal_connect(G_OBJECT(next_page_button), "clicked", 
        G_CALLBACK(next_page), NULL);
 
 add_comment_button = gtk_button_new_from_icon_name ("list-add", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(add_comment_button, FALSE);
 g_signal_connect(G_OBJECT(add_comment_button), "clicked", 
        G_CALLBACK(add_comment), NULL);
 
 text_selection_mode_button = gtk_button_new();
  
 GtkWidget *text_selection_mode_Image = gtk_image_new_from_file("images/text_selection_mode.png");
 gtk_button_set_image(GTK_BUTTON(text_selection_mode_button),text_selection_mode_Image);
 gtk_widget_set_can_focus(text_selection_mode_button, FALSE);
 g_signal_connect(G_OBJECT(text_selection_mode_button), "clicked", 
        G_CALLBACK(text_selection_mode_change), NULL);
 
 text_highlight_mode_button  = gtk_button_new();
 
 GtkWidget *text_highlight_mode_Image = gtk_image_new_from_file("images/text_highlight_mode.png");
 gtk_button_set_image(GTK_BUTTON(text_highlight_mode_button),text_highlight_mode_Image);
 gtk_widget_set_can_focus(text_highlight_mode_button, FALSE);
 g_signal_connect(G_OBJECT(text_highlight_mode_button), "clicked", 
        G_CALLBACK(text_highlight_mode_change), NULL);
 
 erase_text_highlight_mode_button = gtk_button_new();
 
 GtkWidget *erase_text_highlight_mode_Image = gtk_image_new_from_file("images/erase_text_highlight_mode.png");
 gtk_button_set_image(GTK_BUTTON(erase_text_highlight_mode_button),erase_text_highlight_mode_Image);
 gtk_widget_set_can_focus(erase_text_highlight_mode_button, FALSE);
 g_signal_connect(G_OBJECT(erase_text_highlight_mode_button), "clicked", 
        G_CALLBACK(erase_text_highlight_mode_change), NULL);
 
 find_button = gtk_button_new_from_icon_name ("edit-find", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(find_button, FALSE);
 g_signal_connect(G_OBJECT(find_button), "clicked", 
        G_CALLBACK(find_text_cb), NULL);
        
 gtk_container_add (GTK_CONTAINER (toolbar_box), save_note_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), zoom_in_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), zoom_out_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), zoom_width_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), prev_page_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), next_page_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), add_comment_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), text_selection_mode_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), text_highlight_mode_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), erase_text_highlight_mode_button);
 gtk_container_add (GTK_CONTAINER (toolbar_box), find_button);

 gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
 
 gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
 
 gtk_box_pack_end(GTK_BOX(vbox), findbar, FALSE, FALSE, 0);
 
 draw_area = gtk_drawing_area_new();
 ldraw_area = gtk_drawing_area_new();
 
 da_width = (gint)((page_width*zoom_factor)+0.5);
 da_height = (gint)((page_height*zoom_factor)+0.5);
 
 pre_da_width = da_width;
 
 gtk_widget_set_size_request (draw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));
 
 gtk_widget_hide(ldraw_area);
 
 gtk_widget_add_events (draw_area, GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK );
 
 gtk_widget_add_events (ldraw_area, GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK );
 
 g_signal_connect (G_OBJECT (draw_area), "draw", 
                   G_CALLBACK (draw_area_draw), 
                   NULL);
 
 g_signal_connect (G_OBJECT (ldraw_area), "draw", 
                   G_CALLBACK (draw_area_draw), 
                   NULL);
 
 g_signal_connect(G_OBJECT(draw_area), "size-allocate", G_CALLBACK(da_size_allocate_cb), NULL);
 
 g_signal_connect (G_OBJECT (draw_area), "button-press-event",
	           G_CALLBACK (da_button_press),
		   NULL);
 
 g_signal_connect (G_OBJECT (draw_area), "motion-notify-event",
	           G_CALLBACK (da_motion_notify),
		   NULL);
		   
 g_signal_connect (G_OBJECT (draw_area), "button-release-event",
	           G_CALLBACK (da_button_release),
		   NULL);
 
 g_signal_connect (G_OBJECT (ldraw_area), "button-press-event",
	           G_CALLBACK (da_button_press),
		   NULL);
 
 g_signal_connect (G_OBJECT (ldraw_area), "motion-notify-event",
	           G_CALLBACK (da_motion_notify),
		   NULL);
		   
 g_signal_connect (G_OBJECT (ldraw_area), "button-release-event",
	           G_CALLBACK (da_button_release),
		   NULL);
 
 g_signal_connect(G_OBJECT (draw_area), "scroll-event", 
                  G_CALLBACK(da_scroll_cb),
                  NULL);
                  
 g_signal_connect(G_OBJECT (ldraw_area), "scroll-event", 
                  G_CALLBACK(da_scroll_cb),
                  NULL);
                  
  g_signal_connect(G_OBJECT (draw_area), "touch-event", 
                  G_CALLBACK(da_touch_cb),
                  NULL);
 
 
 //drawarea
 
 layout = gtk_layout_new(NULL, NULL);
 
 gint width, height;
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);
 
 gtk_layout_set_size(GTK_LAYOUT(layout), width, height);
     
 gtk_widget_add_events (layout, GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
 
 g_signal_connect (G_OBJECT (layout), "button-press-event", 
                   G_CALLBACK (layout_press), 
                   NULL);
 
 g_signal_connect (G_OBJECT (layout), "motion-notify-event",
	           G_CALLBACK (layout_motion_notify_event),
		   NULL);
		   
 g_signal_connect (G_OBJECT (layout), "button-release-event",
	           G_CALLBACK (layout_button_release_event),
		   NULL);
 
 gtk_layout_put (GTK_LAYOUT (layout), ldraw_area, 0, 0);
 gtk_layout_put (GTK_LAYOUT (layout), draw_area, 0, 0);
    
 gtk_container_add(GTK_CONTAINER(scrolled_window), layout);
   
 g_signal_connect(scrolled_window, "key-press-event", G_CALLBACK(scrolled_window_keypress_cb), findbar);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 pre_sw_width = sw_alloc.width;

 gtk_widget_show_all(win);

 gtk_widget_hide(findbar);
 gtk_widget_hide(toolbar);
 
}

static void size_decrease_cb(GtkAllocation *allocation){
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
 
 GtkAllocation tb_alloc;
 gtk_widget_get_allocation(toolbar, &tb_alloc);
 
 if( allocation->height <= da_height ){
    
  if( allocation->width <= da_width ){
   gtk_layout_move(GTK_LAYOUT(layout), draw_area, 0, 0);
  }
  else{
   #if GTK_CHECK_VERSION(3,18,10)
   width_offset = (allocation->width - da_width)/2;
   #else
   width_offset = (allocation->width - da_width)/2-7;
   #endif 
   
   if( !dual_page_mode){
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, width_offset, 0);
   }
   else{
    int dp_width = (int)(zoom_factor*page_width);
    
    if(page_width >= page_height){//landscape
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
     
    }
    else{//portrait
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, 0);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, 0);
     
    }
   
   }
     
  }
    
 }else{
   
   width_offset = (allocation->width - da_width)/2;
   
   if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) ){
    
    GdkScreen *screen = gdk_screen_get_default ();
    int screen_height = gdk_screen_get_height(screen);
    
    height_offset = ( screen_height - 2 - da_height )/2;
    
   }
   else{
    
    height_offset =  (allocation->height - da_height)/2;
    
   }
   
   if( !dual_page_mode ){
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, width_offset, height_offset);
    
   }
   else{
    int dp_width = (int)(zoom_factor*page_width);
    
    if(page_width >= page_height){//landscape
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
     
    }
    else{//portrait
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,dp_width+1, 0);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, 0);
     
    }
    
   } 
 }
 
 //comment
 struct list_head *tmp;

  list_for_each(tmp, &NOTE_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
    
    if( zoom_factor > 1.0 ){
     
     if( allocation->height <= da_height ){
     
      if( allocation->width >= da_width ){
      
       if( !dual_page_mode ){
        gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
       }
       else{ // dual-page mode
         
         int dp_width = (int)(zoom_factor*page_width);
         
         height_offset = (allocation->height - da_height)/2;
         width_offset = (allocation->width - da_width)/2;
         
         if(height_offset < 0)
          height_offset = 0;
         
         //left page
         if(tmp1->page_num == current_page_num + 1){
          gtk_layout_move(GTK_LAYOUT(layout),
           tmp1->comment,
           (gint)(tmp1->x*zoom_factor),
           (gint)(tmp1->y*zoom_factor)+height_offset);
          }
         //left page
         
         if( page_width >= page_height ){
          
          if( tmp1->page_num == current_page_num + 2 ){
           
           gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+1,
            (gint)(tmp1->y*zoom_factor)+height_offset);
         
          }
         }
         else{
         
          if( tmp1->page_num == current_page_num + 2 ){
           
           gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+2,
            (gint)(tmp1->y*zoom_factor)+height_offset);
         
          }
         
         }
         
       }
       
      }
      else{
       
       gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor));
      
      }
      
     }
     else{
      
      if( !dual_page_mode ){
      
       height_offset = (allocation->height - da_height)/2;
       
       if( height_offset < 0 )
        height_offset = 0;
       
       gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
      }
      else{ //dual-page mode
       
       int dp_width = (int)(zoom_factor*page_width);
       
       if(tmp1->page_num == current_page_num + 1){
        
         if(page_width < page_height){ 
         
          gtk_layout_move(GTK_LAYOUT(layout),
                          tmp1->comment,
                          (gint)(tmp1->x*zoom_factor),
                          (gint)(tmp1->y*zoom_factor));
         
         }
         else{ //landscape
          
          height_offset = (allocation->height - da_height)/2;
          
          gtk_layout_move(GTK_LAYOUT(layout),
                          tmp1->comment,
                          (gint)(tmp1->x*zoom_factor),
                          (gint)(tmp1->y*zoom_factor)+height_offset);
          
         }
         
       }
       else if(tmp1->page_num == current_page_num + 2){
        
        if(page_width < page_height){

         gtk_layout_move(GTK_LAYOUT(layout),
                         tmp1->comment,
                         (gint)((tmp1->x)*zoom_factor)+dp_width+2,
                         (gint)(tmp1->y*zoom_factor));
        
        }
        else{
        
         height_offset = (allocation->height - da_height)/2;
         
         gtk_layout_move(GTK_LAYOUT(layout),
                         tmp1->comment,
                         (gint)((tmp1->x)*zoom_factor)+dp_width+2,
                         (gint)(tmp1->y*zoom_factor)+height_offset);
         
        }
        
       } //end of else if(tmp1->page_num == current_page_num + 2
       
      }
         
     }
     
    }
    else if( zoom_factor == 1.0 ){
     
     if( allocation->height > da_height ){
      
      height_offset =  (allocation->height - da_height)/2;
      
      if( height_offset < 0 )
       height_offset = 0;
       
      gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
     
     }
     else{
     
      gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
     }
    }
    else{ //zoom_factor < 1.0
     
     height_offset =  (allocation->height - da_height)/2;
     
     gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
    
    }
   
  }
 //comment
 
}

static void size_increase_cb(GtkAllocation *allocation){
 
 if( allocation->height <= da_height ){
  
  if( allocation->width <= da_width ){
  
   gtk_layout_move(GTK_LAYOUT(layout), draw_area, 0, 0);
   
  }else{
   
   if(zoom_factor == 1.0){ //check
    if(page_width >= page_height){
     width_offset = (allocation->width - da_width)/2;
    }else{
     #if GTK_CHECK_VERSION(3,12,3)
      width_offset = (allocation->width - da_width)/2;
     #else
      width_offset = (allocation->width - da_width)/2-7;
     #endif 
    }
   }else{
    #if GTK_CHECK_VERSION(3,18,10)
     width_offset = (allocation->width - da_width)/2;
    #else
     width_offset = (allocation->width - da_width)/2-7;
    #endif 
    
   }
   
   GtkAllocation sw_alloc;
   gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
   GtkAllocation win_alloc;
   gtk_widget_get_allocation (win, &win_alloc);
   
   if(width_offset < 0)
    width_offset = 2;
   
   if(dual_page_mode){ //dual-page mode
   
    int dp_width = (int)(zoom_factor*page_width); 
     
    if(page_width >= page_height){
     
     height_offset =  (allocation->height - da_height)/2;
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
     
    }
    else{
     
     gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width+2, 0);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, 0);
     
    }
    
   }else{
    if( !dual_page_mode ){
     gtk_layout_move(GTK_LAYOUT(layout), draw_area, width_offset, 0);
    }
    else{ // dual-page mode
     
     int dp_width = (int)(zoom_factor*page_width); 
     
     if(page_width >= page_height){
     
      height_offset =  (allocation->height - da_height)/2;
     
      gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width+1, height_offset);
      gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
     
     }
     else{
     
      gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width+2, 0);
      gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, 0);
     
     }
     
    }
    
   }
   
  }
    
 } // end of if( allocation->height <= da_height )
 else{
  
  gtk_widget_set_size_request (draw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));
  
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
  
  GdkScreen *screen = gdk_screen_get_default ();
  int screen_width = gdk_screen_get_width(screen);
  
  height_offset =  (allocation->height - da_height)/2;
  width_offset = (allocation->width - da_width)/2-7;
  
  if(height_offset < 0)
   height_offset = 0;
   
  if(width_offset < 0)
   width_offset = 0;
  
  if( dual_page_mode ){
   int dp_width = (int)(zoom_factor*page_width);
   
   gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width+1, height_offset);
   gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
   
  }else{
   gtk_layout_move(GTK_LAYOUT(layout), draw_area, width_offset, height_offset);
  }
  
 } //this should be checked
  
 //comment
 
 struct list_head *tmp;
 
 list_for_each(tmp, &NOTE_HEAD){ 
  
  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  
  if( zoom_factor > 1.0 ){
     
   if( allocation->height <= da_height ){
    
    if( allocation->width >= da_width ){
     
     //pdf's height >= window's height
     //pdf's width <= windows' width 
       
     //event_box has higer height
     
     if( !dual_page_mode ){
     
      gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
         
      if(tmp1->page_num != current_page_num+1)
       gtk_widget_hide(tmp1->comment);
     }
     else{ //dual-page mode
      
      if( page_width < page_height ){
       
       height_offset = (allocation->height - da_height)/2;
       width_offset = (allocation->width - da_width)/2;
       
       if(height_offset < 0)
        height_offset = 0;
       
       //left page
       if(tmp1->page_num == current_page_num + 1){
        gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor)+height_offset);
       }
       //left page
       
       int dp_width = (int)(zoom_factor*page_width);
       
       //right page
       if( page_width >= page_height ){
          
          if( tmp1->page_num == current_page_num + 2 ){  
           
           gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+1,
            (gint)(tmp1->y*zoom_factor)+height_offset);
         
          }
         }
         else{
         
          if( tmp1->page_num == current_page_num + 2 ){
         
           gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+2,
            (gint)(tmp1->y*zoom_factor)+height_offset);
         
          }
         
         }
         //right page
       
      }
     
     }
         
    }
    else{
       
     //event_box has wider width and higer height
     
     gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor)); 
      
    }
   } 
   else{ //if( allocation->height < eb_alloc.height )
    
    if(!dual_page_mode){ 
    
     height_offset =  (allocation->height - da_height)/2;
     
     gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
    }
    else{ //dual-page mode
    
     if(height_offset < 0)
      height_offset = 0;
    
     //left page
     if(tmp1->page_num == current_page_num + 1){
      gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor));
     }
     //left page
     
     int dp_width = (int)(zoom_factor*page_width);
       
       //right page
       if( page_width >= page_height ){
          
        if( tmp1->page_num == current_page_num + 2 ){  
           
         gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+1,
            (gint)(tmp1->y*zoom_factor));
         
        }
       }
       else{
         
        if( tmp1->page_num == current_page_num + 2 ){
         
         gtk_layout_move(GTK_LAYOUT(layout),
            tmp1->comment,
            (gint)(tmp1->x*zoom_factor)+dp_width+2,
            (gint)(tmp1->y*zoom_factor));
         
        }
         
       }
       //right page
    }
         
   }
     
  }
  else if( zoom_factor == 1.0 ){
   
   if( allocation->height > da_height ){
    
    height_offset =  (allocation->height - da_height)/2;
     
    gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
     
   }
   else{
    
    gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
   }
    
  }
  else{ //zoom_factor < 1.0
   
   height_offset =  (allocation->height - da_height)/2;
    
   gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
    
  }
  
 }
  
 //comment
   
}



static void
size_allocate_cb( GtkWidget *win, GtkAllocation *allocation, gpointer data){
 
 sa_count++;
 
 if( pre_sw_width == allocation->width+1 ){
  ;
 }else if(pre_sw_width > allocation->width){
  
  //window gets smaller
  //zoom out
  
  if(allocation->width >= (gint)(page_width*(zoom_factor-1.0)) && allocation->width <= (gint)(page_width*(zoom_factor)) )
   zoom_out();
  
  int z_count = pre_sw_width/allocation->width;
  
  if( z_count > 1 ){
   while(z_count>0){
    zoom_out();
    z_count--;
   }
  }
  
  size_decrease_cb(allocation);
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
 }
 else if(pre_sw_width < allocation->width){
  
  //window gets bigger
  //zoom in
  
  GtkAllocation w_alloc;
  gtk_widget_get_allocation (win, &w_alloc);
  
  GdkScreen *screen = gdk_screen_get_default ();
  int screen_width = gdk_screen_get_width(screen);
  
  if(screen_width == w_alloc.width+2)
   zoom_width();
  
  if(allocation->width >= (gint)(page_width*(zoom_factor+1.0)) && allocation->width <= (gint)(page_width*(zoom_factor+2.0)) )
   zoom_in();
  
  size_increase_cb(allocation);
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 else{  //pre_sw_width == allocation->width
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 
 
 if(da_width > pre_da_width ){
 
  //1, zoom in
  
  size_increase_cb(allocation);
  
  pre_da_width = da_width;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 else if( da_width < pre_da_width){ 
  //2, zoom out
  
  size_decrease_cb(allocation);
  
  pre_da_width = da_width;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 
} // end of size_allocate_cb

gboolean time_handler(GtkWidget *widget) {
 
 if( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(cursor_modeMi) ) ){
  
  invertArea((int)((*areas_ptr).x1*zoom_factor),
             (int)((*areas_ptr).y1*zoom_factor),
             (int)((*areas_ptr).x2*zoom_factor),
             (int)((*areas_ptr).y2*zoom_factor),1);
             
  return TRUE;
 }else
  return FALSE;
}

void
on_findtext_key_release(GtkWidget *findtext, GdkEventKey *event, gpointer user_data){

 GtkWidget *findbar = (GtkWidget *)user_data;

  if( event->keyval == GDK_KEY_Escape){
  
   gtk_widget_hide(findbar);
   gtk_entry_set_text(GTK_ENTRY(findtext), "");
   gtk_widget_grab_focus (scrolled_window);
   
   KEY_BUTTON_SEARCH = TRUE;
   
   GdkScreen *screen = gdk_screen_get_default ();

   if( gdk_screen_get_height(screen) > (int)(page_height*zoom_factor) ){
    zoom_height();
   }
   
   /*
   if( find_ptr ){
         
    invert_search_region();

    g_list_free(find_ptr_head);
    find_ptr_head = NULL;
    find_ptr = NULL;
    
    if( dual_page_mode ){
     lmatches = rmatches = NULL;
    }
    
   }
   */
   
   return;
  }
  else if(event->keyval == GDK_KEY_Return){
   
   find_current_page_num = current_page_num;
   
   if(KEY_BUTTON_SEARCH){
   
    gtk_widget_grab_focus (findbar);
    if(mode == TEXT_SEARCH_NEXT){
 
     find_next(findbar);
    }else if(mode == TEXT_SEARCH_PREV){
    
     find_prev(findbar);
    }
    gtk_widget_hide(findbar);
   
    gtk_widget_grab_focus (scrolled_window);
   }
   else{
   
    gtk_widget_grab_focus (findbar);
    
   }
   
   return;
  }
  else if( event->keyval == GDK_KEY_space ){
  
   if (event->state & GDK_CONTROL_MASK){
    g_message("ctrl+space");
  
   }
  
  }
  else{
   
   gtk_widget_grab_focus (findbar);
   
  }

}

void erase_text_highlight_mode_change(void) {
 
 if( find_ptr ){
  
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
	
 if(mode != TEXT_HIGHLIGHT && mode != ERASE_TEXT_HIGHLIGHT)
  pre_mode = mode;

 mode = ERASE_TEXT_HIGHLIGHT;
 
 char *page_str = (char*)malloc(100);
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
 da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
 da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
 
 da_background_color.red = CLAMP ((guint) ((114.0/255)* 65535), 0, 65535);
 da_background_color.green = CLAMP ((guint) ((237.0/255) * 65535), 0, 65535);
 da_background_color.blue = CLAMP ((guint) ((236.0/255)* 65535), 0, 65535);
 
}

static void
erase_text_highlight_mode_cb(GtkWidget* widget, gpointer data) {

 erase_text_highlight_mode_change();
 
}

void text_selection_mode_change(void){

 if( find_ptr ){
   invert_search_region();
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
   find_ptr = NULL;
 }

 pre_mode = mode = TEXT_SELECTION;
 
 char *page_str = (char*)malloc(100);
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
 
  da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
  da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
  da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
 
  da_background_color.red = CLAMP ((guint) (1* 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (1 * 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (1 * 65535), 0, 65535);
 
 }
 else{
  da_glyph_color.red = CLAMP ((guint) (1 * 65535), 0, 65535);
  da_glyph_color.green = CLAMP ((guint) (1* 65535), 0, 65535);
  da_glyph_color.blue = CLAMP ((guint) (1* 65535), 0, 65535);
 
  da_background_color.red = CLAMP ((guint) (0* 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (0 * 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (0 * 65535), 0, 65535);
 }
 
}

static void
text_selection_mode_cb(GtkWidget* widget, gpointer data) {

 text_selection_mode_change();

}

void
text_highlight_mode_change(void) {
 
 if( find_ptr ){
   
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
 
 if(mode != TEXT_HIGHLIGHT)
  pre_mode = mode;

 mode = TEXT_HIGHLIGHT;

 char *page_str = (char*)malloc(100);
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
 da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
 da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
 
 da_background_color.red = CLAMP ((guint) (1* 65535), 0, 65535);
 da_background_color.green = CLAMP ((guint) (1 * 65535), 0, 65535);
 da_background_color.blue = CLAMP ((guint) (0 * 65535), 0, 65535);
 
}

static void
text_highlight_mode_cb(GtkWidget* widget, gpointer data) {

 text_highlight_mode_change();

}

gboolean
touchpad_cb(GtkWidget *widget, GdkEvent *event, gpointer data){
 
 g_message("touchpad swipe");
 
 return TRUE;
}

static gboolean
da_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer data){
 
 GtkScrollType direction = GTK_SCROLL_NONE;
 gboolean horizontal = FALSE;
 gboolean returnValue = TRUE;

 if( event->scroll.state & GDK_CONTROL_MASK ){
   
  if( event->scroll.direction == GDK_SCROLL_UP ) {
      zoom_in(); 
  }else if ( event->scroll.direction == GDK_SCROLL_DOWN ) {
       zoom_out();
  }
  else if( event->scroll.direction == GDK_SCROLL_SMOOTH ){
   
   g_message("event->scroll.delta_x = %f", event->scroll.delta_x);
   g_message("event->scroll.delta_y = %f", event->scroll.delta_y);
    
  }
   
  return FALSE;
   
 }
 else if( event->scroll.direction == GDK_SCROLL_SMOOTH ){
  
  gdouble delta_x, delta_y; 
    
  scroll_zoom = -1;
    
  if( event->scroll.delta_x == -0.0 )
   scroll_count = 0;
    
  if( event->scroll.delta_x <  -5 ){
    
   if( scroll_count == 0 ){
    GtkAdjustment *hadj = 
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

    gdouble hposition = gtk_adjustment_get_value (hadj);
      
    if ( hposition == gtk_adjustment_get_lower(hadj) ){
                
     if(current_page_num > 0){
      
      prev_page();
      scroll_count++;
     }
    }
   }
     
  }
  else if ( event->scroll.delta_x >  5 ){
   if( scroll_count == 0 ){
     
    GtkAdjustment *hadj = 
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

    gdouble hposition = gtk_adjustment_get_value (hadj);

    if ( hposition == ( gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj)) ){
     gint page_num = poppler_document_get_n_pages(doc);
                
     if(current_page_num < page_num){
     
      next_page();
      scroll_count++;
     }
    }
   }
  }
    
  return FALSE;
    
 }
 else{
  
  if ( event->scroll.direction == GDK_SCROLL_UP){
  
   direction = GTK_SCROLL_STEP_UP;
   horizontal = TRUE;
  
  }
  else if(  event->scroll.direction == GDK_SCROLL_DOWN){
  
   direction = GTK_SCROLL_STEP_DOWN;
   horizontal = TRUE;
  
  }
  else if ( event->scroll.direction == GDK_SCROLL_LEFT){
   
   if( event->scroll.time - scroll_time > 80 &&  scroll_time > 0 && scroll_zoom == -1){
    
    GtkAdjustment *hadj = 
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window)); 

    gdouble hposition = gtk_adjustment_get_value (hadj);

    if( hposition == gtk_adjustment_get_lower(hadj) ){
                
     if(current_page_num > 0){
      prev_page();
     }
                
    }
    else{
     direction = GTK_SCROLL_STEP_LEFT;
     horizontal = TRUE;
    }
    
   }
   
   if(scroll_zoom == 1 && event->scroll.time - scroll_time > 80 &&  scroll_time > 0){
   
    scroll_zoom = -1;
   
   }
   else
    scroll_zoom = -1;
   
   scroll_time = event->scroll.time;
  
  }
  else if ( event->scroll.direction == GDK_SCROLL_RIGHT){
   
   if( event->scroll.time - scroll_time > 80 &&  scroll_time > 0 && scroll_zoom == -1){
    
    GtkAdjustment *hadj = 
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

    gdouble hposition = gtk_adjustment_get_value (hadj);
    if ( hposition == ( gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj)) ){
     gint page_num = poppler_document_get_n_pages(doc);
                
     if(current_page_num < page_num){
       
      next_page();
     
     }
     else{
      
      horizontal = TRUE;
      direction = GTK_SCROLL_STEP_RIGHT;  
     }          
    }
    
   }
   
   if(scroll_zoom == 0 && event->scroll.time - scroll_time > 80 &&  scroll_time > 0 ){
   
    scroll_zoom = -1;
   
   }
   else
    scroll_zoom = -1;
  
   scroll_time = event->scroll.time;
   
   if( event->scroll.delta_x == -0.0 )
    scroll_count = 0;
    gdouble delta_x, delta_y; 

  }
 }
 
 if( direction != GTK_SCROLL_NONE)
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "scroll-child", direction, horizontal, &returnValue);
 
}

static gboolean
scrolled_window_keypress_cb(GtkWidget *widget, GdkEventKey *event, gpointer data){

 GtkScrollType direction = GTK_SCROLL_NONE;
 gboolean horizontal = FALSE;
 gboolean returnValue = TRUE;

 GtkAdjustment *hadj = 
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));

 gdouble hposition = gtk_adjustment_get_value (hadj);

 GtkAdjustment *vadj = 
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (widget));

 gdouble vposition = gtk_adjustment_get_value (vadj);

 GtkWidget *findbar = (GtkWidget *)data;

 switch ( event->keyval ){
  case GDK_KEY_Left:
  case GDK_KEY_KP_Left:
        
   if ( hposition == gtk_adjustment_get_lower(hadj) ){
                
    if(current_page_num > 0){
     gtk_adjustment_set_value(vadj, 
          gtk_adjustment_get_page_size(vadj) - gtk_adjustment_get_upper(vadj));
     prev_page();
    }

    return TRUE;
   }
   
   direction = GTK_SCROLL_STEP_LEFT;
   horizontal = TRUE;
   break;

  case GDK_KEY_Right:
  case GDK_KEY_KP_Right:
   
   {//check
   GtkAdjustment *hadj =
      gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
     
   gdouble hvalue = gtk_adjustment_get_value(hadj);
   
   }//check 
   
        
   if ( hposition == ( gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj)) ){
    
    gint page_num = poppler_document_get_n_pages(doc);
    if(current_page_num < page_num)
     next_page();
                
    return TRUE;
   }
            
   horizontal = TRUE;
   direction = GTK_SCROLL_STEP_RIGHT;
   break;

  case GDK_KEY_Up:
  case GDK_KEY_KP_Up:
  
   if ( vposition == gtk_adjustment_get_lower(vadj) ){
                
    if(current_page_num > 0){
     
     GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
     gtk_adjustment_set_value(vadj, 
        gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj) );
                 
     prev_page();
    }
    
    return TRUE;
   }
            
   direction = GTK_SCROLL_STEP_UP;
   break;

  case GDK_KEY_Down:
  case GDK_KEY_KP_Down:
  case GDK_KEY_j:
   
   if ( vposition == ( gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj)) ){
    
    gint page_num = poppler_document_get_n_pages(doc);
               
    if(current_page_num < page_num)
     next_page();
    return TRUE;
   }
   
   direction = GTK_SCROLL_STEP_DOWN;
   break;

  case GDK_KEY_Page_Up:
  case GDK_KEY_KP_Page_Up:
   
   if ( vposition == gtk_adjustment_get_lower(vadj) ){
                
    if(current_page_num > 0){
     
     GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
     gtk_adjustment_set_value(vadj, 
           gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj) );
                 
     prev_page();
    }
    
    return TRUE;
   }
            
   direction = GTK_SCROLL_STEP_BACKWARD;
   break;

  case GDK_KEY_space:
  case GDK_KEY_KP_Space:
  case GDK_KEY_Page_Down:
  case GDK_KEY_KP_Page_Down:
   
   if ( vposition == ( gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj)) ){
                
    gint page_num = poppler_document_get_n_pages(doc);
    
    if(current_page_num < page_num)
     next_page();
                
    return TRUE;
   }
            
   direction = GTK_SCROLL_STEP_FORWARD;
   break;
  case GDK_KEY_Home:
  case GDK_KEY_KP_Home:
   direction = GTK_SCROLL_START;
   break;

  case GDK_KEY_End:
  case GDK_KEY_KP_End:
   direction = GTK_SCROLL_END;
   break;

  case GDK_KEY_Return:
  case GDK_KEY_KP_Enter:
            
    g_message("sw return");
    gint page_num = poppler_document_get_n_pages(doc);
    if(current_page_num < page_num)
     next_page();
    
    direction = GTK_SCROLL_START;
    break;

   case GDK_KEY_BackSpace:
            
    page_num = poppler_document_get_n_pages(doc);
    if(current_page_num < page_num)
     next_page();
     
    direction = GTK_SCROLL_START;
    break;
   case GDK_KEY_F :
   case GDK_KEY_f :
    
    if (event->state & GDK_CONTROL_MASK){
         
     KEY_BUTTON_SEARCH = FALSE;
         
     pre_mode = mode = TEXT_SELECTION;
         
     gtk_widget_show(findPrev_button);
     gtk_widget_show(findNext_button);
     gtk_widget_show(find_exit_button);
         
     gtk_widget_show(findbar);
     
     GdkScreen *screen = gdk_screen_get_default ();
         
     if( gdk_screen_get_height(screen) > (int)(page_height*zoom_factor) )
      zoom_height();
         
     gtk_widget_grab_focus (findtext);
 
     if( find_ptr ){
         
      invert_search_region();
      g_list_free(find_ptr_head);
      find_ptr_head = NULL;
      find_ptr = NULL;
     }
 
     word_not_found = 1;
     
    }
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_question :
   case GDK_KEY_slash :

    if( find_ptr ){
         
     invert_search_region();
     g_list_free(find_ptr_head);
     find_ptr_head = NULL;
     find_ptr = NULL;
    }
        
    gtk_widget_hide(findPrev_button);
    gtk_widget_hide(findNext_button);
    gtk_widget_hide(find_exit_button);

    word_not_found = 1;

    if( event->keyval == GDK_KEY_question ){
         
     pre_mode = TEXT_SELECTION;
     mode = TEXT_SEARCH_PREV;
     
     if(mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SEARCH_NEXT){
          
      if(find_ptr_head){
       find_ptr_head = g_list_reverse(find_ptr_head);
      }
     }

     if( pre_keyval == GDK_KEY_slash )
      if(find_ptr_head)
       find_ptr_head = g_list_reverse(find_ptr_head);

     pre_keyval = GDK_KEY_question;

    }else if( event->keyval == GDK_KEY_slash ){
         
     pre_mode = TEXT_SELECTION;
     mode = TEXT_SEARCH_NEXT;
         
     if(mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SEARCH_PREV){
          
      if(find_ptr_head){
       find_ptr_head = g_list_reverse(find_ptr_head);
      }
     }

     if( pre_keyval == GDK_KEY_question )
      if(find_ptr_head)
       find_ptr_head = g_list_reverse(find_ptr_head);
    
     pre_keyval = GDK_KEY_slash;

    }
        
    gtk_widget_show(findbar);  
    gtk_widget_grab_focus (findtext);
        
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_equal :
       
    if (event->state & GDK_CONTROL_MASK){
     
      zoom_in();
        
      if(pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT)
       mode = pre_mode;
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_minus :
       
    if (event->state & GDK_CONTROL_MASK){
     
      zoom_out();
        
      if(pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT)
       mode = pre_mode;
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_w :
       
    if (event->state & GDK_CONTROL_MASK){
     
     zoom_width();    
    
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_F11 :
    
    if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) )
     gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), TRUE );
    else
     gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), FALSE );
    
    full_screen_cb();
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_v :
      
    if (event->state & GDK_CONTROL_MASK){
     
     if( !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(hide_toolbarMi)) ){  
      toggle_hide_toolbar();
     }
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   
   case GDK_KEY_d :
       
    if (event->state & GDK_CONTROL_MASK){
    
     if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(dual_pageMi) ) ){
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), TRUE );
      dual_page_mode = TRUE;
     
     }else{
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
      dual_page_mode = FALSE;
     }
     
    }
       
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_c :
      
    if (event->state & GDK_CONTROL_MASK){
        copy_text();
    }
       
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_C :
      
    if (event->state & GDK_SHIFT_MASK){
      
     GtkWidget *colorseldlg = gtk_color_chooser_dialog_new ("Select Highlight Color", NULL);

     gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
         
     if(response == GTK_RESPONSE_OK){
          
      text_highlight_mode_change();
        
      gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
          
      hc[0] = (guchar)(color.red*255);
      hc[1] = (guchar)(color.green*255);
      hc[2] = (guchar)(color.blue*255);
          
      char hc_color_name[9];
          
      sprintf(hc_color_name, "%02x%02x%02x00",hc[0],hc[1],hc[2]);
          
      unsigned long hc_color = strtoul(hc_color_name, NULL, 16);
      gdk_pixbuf_fill(hc_pixbuf, hc_color);
          
      gtk_image_set_from_pixbuf(GTK_IMAGE(hc_button_Image),hc_pixbuf);
     
      da_background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
      da_background_color.green = CLAMP ((guint) (color.green* 65535), 0, 65535);
      da_background_color.blue = CLAMP ((guint) (color.blue* 65535), 0, 65535);
          
     }
         
     gtk_widget_destroy (colorseldlg);
         
    }//end of if (event->state & GDK_CONTROL_MASK
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_x : 
    
     if (event->state & GDK_CONTROL_MASK){
      
      inverted_color_cb();
      
      if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
       gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(inverted_colorMi), TRUE);
      else
       gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(inverted_colorMi), FALSE);
       
      
     }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_B :
    if (event->state & GDK_SHIFT_MASK)
     if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) )
      color_set(2);
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_z :
    
    if (event->state & GDK_CONTROL_MASK){
        
     if( mode == TEXT_SELECTION ){
         
      text_highlight_mode_cb(NULL, NULL);
         
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     
      free(page_str);
     
     }
     else if( mode == TEXT_HIGHLIGHT ){
         
      erase_text_highlight_mode_cb(NULL, NULL);
         
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
      
      free(page_str);
      
     }
     else if( mode == ERASE_TEXT_HIGHLIGHT ){
         
      text_selection_mode_cb(NULL, NULL);
         
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     
      free(page_str);
      
     }
    }
       
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_q :
    
    if (event->state & GDK_CONTROL_MASK){
     on_destroy(NULL, NULL);
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_n :

    if (event->state & GDK_CONTROL_MASK){
     add_comment();
    }else{ 
     search_n(findbar); 
    }
       
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_N :
    
    search_N(findbar);
       
    pre_keyval = GDK_KEY_N;
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_s :
       
    if (event->state & GDK_CONTROL_MASK){ //ctrl-s  
     save_note();
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_P :
    if(mode ==  PAGE_CHANGE){
        
     mode = TEXT_SELECTION;
       
     int page_num = atoi(page_change_str)-1;
     
     if(page_num + 1 >poppler_document_get_n_pages(doc) ||
        page_num == -1 || page_num  == current_page_num){

      free(page_change_str);
      page_change_str = NULL;
      break;
     }
     
     current_page_num = page_num;
     page_change();
     free(page_change_str);
     page_change_str = NULL;
    } 
       
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_Escape :

    gtk_widget_hide(findbar);
    
    if( find_ptr ){
         
     invert_search_region();

     g_list_free(find_ptr_head);
     find_ptr_head = NULL;
     find_ptr = NULL;
    }
    else{
    
     if ( gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(full_screenMi))){
      gtk_window_unfullscreen (GTK_WINDOW (win));
      gtk_widget_show (menubar);
      
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), FALSE );
     }
     
    }
       
    char *page_str = (char*)malloc(100);
        
    sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
    gtk_window_set_title(GTK_WINDOW(win), page_str);
    
    free(page_str);
       
    word_not_found = 1;
        
    pre_mode = mode = TEXT_SELECTION;
        
    direction = GTK_SCROLL_NONE;
    cursor_enable = FALSE;
    
    break;
   case GDK_KEY_h :
        
    if (event->state & GDK_CONTROL_MASK){
     
     zoom_height();
        
    }else if(areas_ptr > areas){
   
     if(inverted){
          
      invertArea((int)((*areas_ptr).x1*zoom_factor),
                 (int)((*areas_ptr).y1*zoom_factor),
                 (int)((*areas_ptr).x2*zoom_factor),
                 (int)((*areas_ptr).y2*zoom_factor), 1);
     }
         
     areas_ptr--;
     line_offset--;
     
     g_message("lh (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
     g_message("lh (*areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
     
     
     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){
      areas_ptr--;
     }
     else if( (*areas_ptr).x2 - (*areas_ptr).x1 < 1.0 ){
      areas_ptr--;
     }
     
     g_message("lh (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
     g_message("lh (*areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
     
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
     
     GtkAdjustment *hadj =
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
     
     gdouble hvalue = gtk_adjustment_get_value(hadj);
     
     int cursor_x = (int)((*areas_ptr).x1*zoom_factor);
     
     g_message("lh cursor_x = %d", cursor_x);
     g_message("lh hvalue = %f", hvalue);
     
     g_message("lh sw_alloc.width = %d", sw_alloc.width);
     g_message("lh gtk_adjustment_get_upper(hadj) = %f", gtk_adjustment_get_upper(hadj));
     g_message("lh gtk_adjustment_get_step_increment(hadj) = %f", gtk_adjustment_get_step_increment(hadj));
     g_message("lh gtk_adjustment_get_page_size(hadj) = %f", gtk_adjustment_get_page_size(hadj));
     
     //if( cursor_x > sw_alloc.width ){
     if( cursor_x > hvalue ){
      
      g_message("lh (int)(cursor_y/sw_alloc.height)) = %d", (int)(cursor_x/sw_alloc.width) );
      g_message("lh (int)( gtk_adjustment_get_upper(hadj) /sw_alloc.width)) = %d", (int)(gtk_adjustment_get_upper(hadj)/sw_alloc.width) );
      //gtk_adjustment_set_value(vadj, sw_alloc.width*( (int)(cursor_y/sw_alloc.height)) );
      //gtk_adjustment_set_value(hadj, 500 );
      
      //gtk_adjustment_set_value(hadj, cursor_x - hvalue );
      
      
      int adj_n = (int)( gtk_adjustment_get_upper(hadj) /sw_alloc.width +0.5 );
      //int adj_n = (int)( gtk_adjustment_get_upper(hadj) /500 +0.5 );
      
      g_message("lh adj_n = %d", adj_n);
      
      /*
      if( adj_n == 1 )
       //gtk_adjustment_set_value(hadj, sw_alloc.width/2 );
       gtk_adjustment_set_value(hadj, 500 );
      else
       gtk_adjustment_set_value(hadj, 500*( adj_n ));
      // //gtk_adjustment_set_value(hadj, sw_alloc.width*( adj_n-1 ));
      */
      
      gtk_adjustment_set_value(hadj, sw_alloc.width*( adj_n*405 ));
      
      /*
      gdouble adj;
      
      if( cursor_x - sw_alloc.width > 500 )
      //if( cursor_x - sw_alloc.width > 500*adj_n )
       //adj = sw_alloc.width*( (int)(cursor_x/sw_alloc.width) );
       //adj = 580*( (int)(cursor_x/sw_alloc.width) );
       adj = 580*adj_n;
      else
       adj = 580;
      
      if(adj > gtk_adjustment_get_upper(hadj))
       adj =  1500;
       
      g_message("lh adj = %f", adj);
      
      gtk_adjustment_set_value(hadj, adj ); 
      */
      
     }
     else{
      
      gtk_adjustment_set_value(hadj, 0.0 );
      
     }
     

    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_i : 
    
    g_message("20170714 i line_count = %d", line_count);
    line_count--;
    if(line_count < 0){
     line_count = 0;
     areas_ptr = areas+line_offset;
    }else{
     //areas_ptr = areas_line[line_count];
     //areas_ptr++;
     if( inverted ){
        
      invertArea((int)((*areas_ptr).x1*zoom_factor),
                 (int)((*areas_ptr).y1*zoom_factor),
                 (int)((*areas_ptr).x2*zoom_factor),
                 (int)((*areas_ptr).y2*zoom_factor), 1);
     }
     
     if(line_count == 0)
      areas_ptr = areas+line_offset;
      //areas_ptr = areas_line[line_count]+line_offset+1;
     else
      areas_ptr = areas_line[line_count-1]+line_offset+1;
     //areas_ptr++;
     
     g_message("i line_count = %d", line_count);
     g_message("i (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
     g_message("i *areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
     
     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){ //end of line
      
      g_message("abc line_count = %d", line_count);
      if(line_count == 0){
       //areas_ptr = areas;
       g_message("i (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
       g_message("i (*areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
      }
      //else if( line_count == 1 ){
      // areas_ptr = areas;
      //}
      //else
      // areas_ptr = areas_line[line_count]+line_offset+1;
      
      
     }
     
     if( (*areas_line[line_count]).y1 == (*areas_ptr).y1 ){
     
      /*
      if(line_count == 0){
       areas_ptr = areas;
       g_message("i (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
       g_message("i (*areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
      }else
       areas_ptr = areas_line[line_count]+line_offset+1;
      */
      
      g_message("20170713");
      
      //areas_ptr = areas_line[line_count]+line_offset+1;
      
     }
     
     gint width, height;
     width = (gint)((page_width*zoom_factor)+0.5);
     height = (gint)((page_height*zoom_factor)+0.5);
     
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
     
     GtkAdjustment *vadj =
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
     
     gdouble vvalue = gtk_adjustment_get_value(vadj);
     
     g_message("i width = %d, height = %d", width, height);
     g_message("i sw_alloc.height = %d", sw_alloc.height);
     g_message("i (int)((*areas_ptr).y1*zoom_factor) = %d", (int)((*areas_ptr).y1*zoom_factor));
     g_message("i vvalue = %f", vvalue);
     
     int cursor_y = (int)((*areas_ptr).y1*zoom_factor);
     
     //if( cursor_y > sw_alloc.height ){
     if( cursor_y < vvalue ){
   
      g_message("i (int)(cursor_y/sw_alloc.height) = %d", (int)(cursor_y/sw_alloc.height));
      
      gtk_adjustment_set_value(vadj, sw_alloc.height*( (int)(cursor_y/sw_alloc.height)) );
      //gtk_adjustment_set_value(vadj, vvalue - sw_alloc.height);
      
     }
    
    }
    //line_count--;
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_k : 
  
    //line_count++;
    if(line_count == 50)
     line_count = 49;
    else{
     //areas_ptr = areas_line[line_count];
     //areas_ptr++;
     
     if( areas_line[line_count] == NULL  ){
         
         //next_page();
         direction = GTK_SCROLL_NONE;
         break;
     }
     
     if( inverted ){
          
      invertArea((int)((*areas_ptr).x1*zoom_factor),
                 (int)((*areas_ptr).y1*zoom_factor),
                 (int)((*areas_ptr).x2*zoom_factor),
                 (int)((*areas_ptr).y2*zoom_factor), 1);
     }
     g_message("20170711 line_count = %d", line_count);
     //int line_offset = areas_ptr - areas_line[line_count-1];
     g_message("line_offset = %d", line_offset);
     //areas_ptr = areas_line[line_count]+1+line_offset;
     areas_ptr = areas_line[line_count]+line_offset+1;
     
     g_message("k line_count = %d", line_count);
     g_message("k (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
     g_message("k *areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){ //end of line
      //areas_ptr++;
      //line_offset--;
     
      //line_count++;
      //areas_ptr = areas_line[line_count]+line_offset+1;
      //areas_ptr = areas_line[line_count]+line_offset;
      areas_ptr = areas_line[line_count]+1;
      
      g_message("k (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
      g_message("k *areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
      
     }
     //line_count++;
     //areas_ptr = areas_line[line_count];
     //areas_ptr++;
     
     gint width, height;
     width = (gint)((page_width*zoom_factor)+0.5);
     height = (gint)((page_height*zoom_factor)+0.5);
     
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
     
     GtkAdjustment *vadj =
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
     
     gdouble vvalue = gtk_adjustment_get_value(vadj);
     
     g_message("k width = %d, height = %d", width, height);
     g_message("k sw_alloc.height = %d", sw_alloc.height);
     g_message("k (int)((*areas_ptr).y1*zoom_factor) = %d", (int)((*areas_ptr).y1*zoom_factor));
     g_message("k vvalue = %f", vvalue);
     
     int cursor_y = (int)((*areas_ptr).y1*zoom_factor);
     
     if( cursor_y > sw_alloc.height ){
   
      g_message("k (int)(cursor_y/sw_alloc.height) = %d", (int)(cursor_y/sw_alloc.height));
      
      gtk_adjustment_set_value(vadj, sw_alloc.height*( (int)(cursor_y/sw_alloc.height) ) );
      
     }
     
    }
    
    line_count++;
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_l :
        
    if(areas_ptr <= areas+n_areas-1){
     
     if( inverted ){
          
      invertArea((int)((*areas_ptr).x1*zoom_factor),
                 (int)((*areas_ptr).y1*zoom_factor),
                 (int)((*areas_ptr).x2*zoom_factor),
                 (int)((*areas_ptr).y2*zoom_factor), 1);
     }
     
     areas_ptr++;
     line_offset++;
     
     if( areas_line[line_count] == areas_ptr ){
      line_offset = 0;
     }
     
     g_message("rl (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
     g_message("rl *areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
         
     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){ //end of line
      areas_ptr++;
      //line_offset--;
     }
     else if( (*areas_ptr).x2 - (*areas_ptr).x1 < 1.0 ){ //weird blank character
      areas_ptr++;
      //line_offset--;
     }
     
     g_message("rl line_offset = %d", line_offset);
     
     gint width, height;
     width = (gint)((page_width*zoom_factor)+0.5);
     height = (gint)((page_height*zoom_factor)+0.5);
     
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
     
     GtkAdjustment *hadj =
      gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
     
     gdouble hvalue = gtk_adjustment_get_value(hadj);
     
     g_message("rl width = %d, height = %d", width, height);
     g_message("rl sw_alloc.width = %d", sw_alloc.width);
     g_message("rl (int)((*areas_ptr).x1*zoom_factor) = %d", (int)((*areas_ptr).x1*zoom_factor));
     g_message("rl hvalue = %f", hvalue);
     
     int cursor_x = (int)((*areas_ptr).x2*zoom_factor);
     
     if( cursor_x > sw_alloc.width ){
      //g_message("rl (int)(cursor_x/sw_alloc.width) = %d", (int)(cursor_x/sw_alloc.width));
      
      g_message("rl cursor_x = %d", cursor_x);
      g_message("rl sw_alloc.width = %d", sw_alloc.width);
      
      g_message("rl gtk_adjustment_get_page_size(hadj) = %f", gtk_adjustment_get_page_size(hadj));
      g_message("rl gtk_adjustment_get_upper(hadj) = %f", gtk_adjustment_get_upper(hadj));
      g_message("rl gtk_adjustment_get_page_size(hadj) - gtk_adjustment_get_upper(hadj) = %f", gtk_adjustment_get_page_size(hadj) - gtk_adjustment_get_upper(hadj) );
      
      gdouble adj;
      
      //gtk_adjustment_set_value(vadj, gtk_adjustment_get_page_size(vadj) - gtk_adjustment_get_upper(vadj));
      
      //if( cursor_x - sw_alloc.width > sw_alloc.width )
      if( cursor_x - sw_alloc.width > 500 )
       adj = sw_alloc.width*( (int)(cursor_x/sw_alloc.width) );
      else
       adj = 580;
       //adj = 540;
       //adj = gtk_adjustment_get_upper(hadj);
      
      g_message("rl adj = %f", adj);
      
      gtk_adjustment_set_value(hadj, adj );  
      
      //gtk_adjustment_set_value(hadj, sw_alloc.width*( (int)(cursor_x/sw_alloc.width) ) );
     
     }
     else{
     
      g_message("rl 20170715 cursor_x <= sw_alloc.width");
      gtk_adjustment_set_value(hadj, 0.0 ); 
     }
     
     int cursor_y = (int)((*areas_ptr).y1*zoom_factor);
     
     if( cursor_y > sw_alloc.height ){
   
      g_message("rl (int)(cursor_y/sw_alloc.height) = %d", (int)(cursor_y/sw_alloc.height));
      
      gtk_adjustment_set_value(vadj, sw_alloc.height*( (int)(cursor_y/sw_alloc.height) ) );
      
     }

    }// end of if(areas_ptr <= areas+n_areas-1)
    
    //direction = GTK_SCROLL_HORIZONTAL_ENDS;
    //direction = GTK_SCROLL_PAGE_RIGHT;
    
    direction = GTK_SCROLL_NONE;
    break;
   default:

    if( strlen(event->string) == 1 ){
     if(  *(event->string) >= 48 && *(event->string) <=57 ){
      if( !page_change_str ){
            
       page_change_str = (gchar*)malloc(2);
            
       *page_change_str = *(event->string);
       *(page_change_str+1) = '\0';
      }
      else{
       
       gchar *str = (gchar *)malloc(strlen(page_change_str)+2);
            
       strcpy(str, page_change_str);
       strcat(str, event->string);
       page_change_str = str;
      }
      
      mode = PAGE_CHANGE;         
     }// end of if(  *(event->string) >= 48 && *(event->string) <=57 )
    }// end of if( strlen(event->string) == 1 )
         
    return FALSE;
    }// end of case

    if( direction != GTK_SCROLL_NONE)
     g_signal_emit_by_name(G_OBJECT(widget), "scroll-child", 
                             direction, horizontal, &returnValue);

}

void
on_destroy(GtkWidget* widget, gpointer data) {
   
 save_note();
 gtk_main_quit();

}

void
textbuffer_changed_cb(GtkTextBuffer *buffer, gpointer user_data){

 struct note* comment = (struct note *)user_data;
 
 GtkTextIter startIter;
 GtkTextIter endIter;
 gtk_text_buffer_get_start_iter(buffer, &startIter);
 gtk_text_buffer_get_end_iter(buffer, &endIter);
 
 GtkTextIter iter;
 
 char * pch, *tmp_pch;
 int count = 0;
   
 int max_len = 0;
 
 char *string = gtk_text_buffer_get_text(buffer, &startIter, &endIter, TRUE);
 size_t len = strlen(string);
 
 tmp_pch = string;
 
 pch=strchr(string,'\n');
   
 while (pch!=NULL){
   
  if(max_len == 0){
   max_len = pch-tmp_pch;
  }
  else{
    
   if( pch-tmp_pch > max_len )
    max_len = pch-tmp_pch;
    
  }
  
  count++;
  tmp_pch = pch;
  pch=strchr(pch+1,'\n');
  
  
 }
   
 free(comment->str);
 
 comment->str = (char*)malloc( strlen(string)+1 );
 strcpy(comment->str, string);
 
 gtk_widget_queue_draw (comment->comment);
 
}

void
layout_press(GtkWidget *widget, GdkEventButton *event, gpointer data){
  
  if (event->button == 1){
   
   GList *list, *iter;
         
   list = gtk_container_get_children (GTK_CONTAINER (layout));
   
   for (iter = list; iter; iter = iter->next){
    
    GtkWidget *child = GTK_WIDGET (iter->data);
    
    if(child == draw_area)
     continue;
   
    if(child == ldraw_area)
     continue;
           
    if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child) ){
     
     gtk_widget_get_allocation (child, &child_alloc);
     
     double obj_x, obj_y;
     
     if( width_offset >= (child_alloc.x + child_alloc.width) ){
      obj_x = event->x;
      obj_y = event->y;
     }else if( (event->x + width_offset >= child_alloc.x) &&
              (event->x + width_offset > (child_alloc.x + child_alloc.width) )){
      obj_x = event->x;
      obj_y = event->y;
     }

     if( (event->x + width_offset >= child_alloc.x) &&
         (event->x + width_offset < (child_alloc.x +child_alloc.width) )){

        obj_x = event->x+width_offset;
        
        GtkAllocation sw_alloc;
        gtk_widget_get_allocation (scrolled_window, &sw_alloc);
                 
        if( sw_alloc.height <= da_height )
         obj_y = event->y;
        else
         obj_y = event->y+height_offset;
        
     }
     
     if(lpage){ // it looks like it have no effect to this function
      
      GtkAllocation sw_alloc;
      gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
      if( sw_alloc.height > da_height )
       obj_y = obj_y+height_offset;
      
      int dp_width = (int)(zoom_factor*page_width);
      
      if( child_alloc.x+child_alloc.width > dp_width+1 ){
       
       if( child_alloc.x > dp_width+1)
        obj_x = event->x + dp_width+1;
       else{ //the comment is laid on between left and left page
      
        if(event->x > 500){ //get the left part
         obj_x = event->x;
        }else{ //get the right part
         obj_x = event->x + dp_width+1;
        }
       }
       
       if( sw_alloc.height <= da_height )
        obj_y = event->y;
       else
        obj_y = event->y+height_offset;
        
      }
      
     }
     
     if ( (obj_x >= child_alloc.x) &&
          (obj_x < (child_alloc.x + child_alloc.width)) &&
          (obj_y >= child_alloc.y) &&
          (obj_y < (child_alloc.y + child_alloc.height)) ){
             
      comment = child;
      
      lstart_x = event->x;
      lstart_y = event->y;
      layout_move = 1;
             
      break;
     }
            
    }
   
   }
   g_list_free (list);
    
  }//end of if (event->button == 1
  else if (event->button == 3){
   
   GtkWidget *label = get_layout_child(layout, event);

   if(label == NULL){
    return;
   }
   
   struct note *comment;
   
   struct list_head *tmp;

   list_for_each(tmp, &NOTE_HEAD){

    struct note *tmp1;
    tmp1= list_entry(tmp, struct note, list);
   
    if(label == tmp1->comment){
     comment = tmp1;
     break;
    }
   }
   
   GtkWidget *edit_win = 
           gtk_dialog_new_with_buttons ("Edit",
                                       NULL,
                                       GTK_DIALOG_MODAL,
                                       "_CANCEL",
                                       GTK_RESPONSE_CANCEL,
                                       "_OK",
                                       GTK_RESPONSE_OK,
                                       "Delete this comment",
                                       GTK_RESPONSE_APPLY,
                                       NULL);

   GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (edit_win));
      
   GtkWidget *view_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   gtk_container_add (GTK_CONTAINER (content_area), view_box);

   GtkWidget *textview = gtk_text_view_new ();
   GtkTextBuffer *buffer = gtk_text_buffer_new (NULL);
         
   const char *contents = comment->str;
   size_t len = strlen (contents);
   gtk_text_buffer_set_text (buffer, contents, len);
   gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), buffer);
         
   g_signal_connect(G_OBJECT( buffer ), "changed",
      G_CALLBACK(textbuffer_changed_cb), comment);
   
   gtk_box_pack_start(GTK_BOX(view_box), textview, TRUE, TRUE, 0);
         
   gtk_widget_show_all(edit_win);
         
   int result = gtk_dialog_run(GTK_DIALOG(edit_win));
         
   if( result == GTK_RESPONSE_OK ){
    
    GtkTextIter startIter;
    GtkTextIter endIter;
    gtk_text_buffer_get_start_iter(buffer, &startIter);
    gtk_text_buffer_get_end_iter(buffer, &endIter);

   }
   else if( result == GTK_RESPONSE_APPLY ){
    gtk_widget_hide(label);
       
    struct list_head *tmp, *q;

    list_for_each_safe(tmp, q, &NOTE_HEAD){

     struct note *tmp1;
     tmp1= list_entry(tmp, struct note, list);
         
     if( tmp1->comment == label ){
            
      list_del(&tmp1->list);
      free(tmp1->str);
      gtk_widget_destroy(tmp1->comment);
      free(tmp1);
      break;
     } 

    } // end of list_for_each_safe(tmp, q, &NOTE_HEAD)
   } // end of else if( result == GTK_RESPONSE_APPLY )

   gtk_widget_destroy (edit_win);
   
  }// end of else if (event->button == 3)

}

gboolean
layout_button_release_event (GtkWidget      *widget,
                             GdkEventButton *event,
                             gpointer        user_data){ 

 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);   
 
 struct list_head *tmp;

 list_for_each(tmp, &NOTE_HEAD){

  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  if(tmp1->comment == comment){
   
   if(dual_page_mode){
     
     int dp_width = (int)(zoom_factor*page_width);
     
     if( child_alloc.x > dp_width + 1 || event->x > dp_width + 1){
      
      left_right = 1;
     }else{
      
      left_right = 0;
     }
     
     if(left_right == 1 && event->x < 0)
      left_right = 0;
     
     if(left_right){ // right_page
      
      tmp1->page_num = current_page_num + 2;
     }else{ // left page
      
      tmp1->page_num = current_page_num + 1;
      
     }
    
   }
   
   if( da_height >= sw_alloc.height )
    tmp1->y = ((gint)(event->y - lstart_y) + child_alloc.y)/zoom_factor;
   else
    tmp1->y = ((gint)(event->y - lstart_y) + child_alloc.y)/zoom_factor - (gint)(height_offset/zoom_factor);
    
   if( da_width >= sw_alloc.width )
    tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x)/zoom_factor;
   else{

    if( dual_page_mode ){
    
     int dp_width = (int)(zoom_factor*page_width);
    
     if(left_right && event->x > dp_width+1){
      
      tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x - (dp_width+1 ))/zoom_factor;
     }
     else if( left_right  ){ 
     
      tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x - (dp_width+1 ))/zoom_factor;
     
     }
     else{
     
      tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x)/zoom_factor;
     }
    }else{
     
     tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x)/zoom_factor - (gint)(width_offset/zoom_factor);
    
    }
    
   }
    
    if(dual_page_mode){
     
     if(tmp1->y < 0 || event->y > (gint)((page_height*zoom_factor)+0.5)){
      
      tmp1->x = 100;
      tmp1->y = 100;
      
      gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, tmp1->x, tmp1->y);
     }
     
    }
    
  }
  
 }//end of list_for_each(tmp, &NOTE_HEAD
 
 if (event->button == 1 && layout_move == 1){
  comment = 0;
  layout_move = 0;
           
  return TRUE; // swallow event 
 }
 
 return FALSE;
}

gboolean
layout_motion_notify_event (GtkWidget      *widget,
                            GdkEventMotion *event,
                            gpointer        user_data){
 
 
 if ( comment ){
   
  gint newx = (gint)((event->x - lstart_x)) + child_alloc.x;
  gint newy = (gint)(event->y - lstart_y) + child_alloc.y;
  
  if (newx < 0)
   newx = 0;
  
  if (newy < 0)
   newy = 0;
  
  gtk_layout_move (GTK_LAYOUT (widget), comment, newx, newy);
  gdk_window_invalidate_rect (gtk_layout_get_bin_window (GTK_LAYOUT (widget)), NULL, FALSE);
                
  return TRUE; /* swallow event */
 }
        
 return FALSE;
}

GtkWidget * 
get_layout_child(GtkWidget      *layout, 
                 GdkEventButton *event){

 GList *list, *iter;
 list = gtk_container_get_children (GTK_CONTAINER (layout));
 
 for (iter = list; iter; iter = iter->next){

  GtkWidget *child = GTK_WIDGET (iter->data);
  
  if(child == draw_area)
   continue;
   
  if(child == ldraw_area)
   continue;
  
  if (gtk_widget_get_visible (child) && ! GTK_IS_CONTAINER(child) ){

    gtk_widget_get_allocation (child, &child_alloc);
    
    double obj_x, obj_y;
    if( width_offset >= (child_alloc.x + child_alloc.width) ){
     obj_x = event->x;
     obj_y = event->y;
    }else if( (event->x + width_offset >= child_alloc.x) &&
           (event->x + width_offset > (child_alloc.x + child_alloc.width) )){
     obj_x = event->x;
     obj_y = event->y;
    }

    if( (event->x + width_offset >= child_alloc.x) &&
        (event->x + width_offset < (child_alloc.x + child_alloc.width) )){

     obj_x = event->x+width_offset;
                 
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
                 
     if( sw_alloc.height <= da_height )
      obj_y = event->y;
     else
      obj_y = event->y+height_offset;
     
    }
    
    if( dual_page_mode ){
     
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
     
     int dp_width = (int)(zoom_factor*page_width);
     
     if( child_alloc.x+child_alloc.width > dp_width+1 ){
      
      if( child_alloc.x > dp_width+1)
       obj_x = event->x;
      else{ //the comment is laid on between left and left page
       
       if(event->x > 500){ //get the left part
        obj_x = event->x;
       }else{ //get the right part
        obj_x = event->x + dp_width+1;
       }
       
       
      }
      
      if( sw_alloc.height <= da_height )
       obj_y = event->y;
      else
       obj_y = event->y; 
     }
     
    }
    
    if ( (obj_x >= child_alloc.x) &&
         (obj_x < (child_alloc.x + child_alloc.width)) &&
         (obj_y >= child_alloc.y) &&
         (obj_y < (child_alloc.y + child_alloc.height)) ){
          
          return child;
 
    }
  }
 }

 return NULL;

}
