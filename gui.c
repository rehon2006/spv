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
#include "gui.h"

void toggle_dual_page_cb(GtkWidget *widget, gpointer user_data){
 
 dual_page_cb();
 
 if( ! g_signal_handler_is_connected (event_box, event_box_motion_handler_id) ){
 
  event_box_motion_handler_id = g_signal_connect(event_box, "motion_notify_event", 
    G_CALLBACK(event_box_motion_event_cb), scrolled_window);
 }
 
 if( ! g_signal_handler_is_connected (levent_box, levent_box_motion_handler_id) ){
 
   levent_box_motion_handler_id = g_signal_connect(levent_box, "motion_notify_event", 
     G_CALLBACK(levent_box_motion_event_cb), scrolled_window);
 }
 
}

void full_screen_cb(void){

 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(full_screenMi) ) ){
  
  gtk_window_fullscreen (GTK_WINDOW (win));
  gtk_widget_hide (menubar);
  
  if(!lpage){
  
   GtkAllocation eb_alloc;
   gtk_widget_get_allocation (event_box, &eb_alloc);
   
   GtkAllocation win_alloc;
   gtk_widget_get_allocation (win, &win_alloc);
   
   if( win_alloc.height > eb_alloc.height )
    zoom_height();
   
  }
  
 }
 else{
 
  gtk_window_unfullscreen (GTK_WINDOW (win));
  gtk_widget_show (menubar);
  
  zoom_width();
  
 }
 
}

static void toggle_full_screen_cb(GtkWidget *widget, gpointer user_data){
 
 full_screen_cb();
 
}

static void inverted_color_cb(void){
 
  gint width = (gint)((page_width*zoom_factor)+0.5);
  gint height = (gint)((page_height*zoom_factor)+0.5);
  
  if(pixbuf)
   invertArea(0 ,0 ,width ,height, 1);
  
  if(lpage){
   if(lpixbuf)
    invertArea(0 ,0 ,width ,height, 2);
  }
  
  page_change();
 
}

static void toggle_inverted_color_cb(GtkWidget *widget, gpointer user_data){
 inverted_color_cb();
}

static void change_background_color_cb(GtkWidget *widget, gpointer user_data){
 
 if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
  if ( ! gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))){
   PDF_BACKGROUND_COLOR_CHANGED = FALSE;
  }else{ 
   PDF_BACKGROUND_COLOR_CHANGED = TRUE;
  }
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

 clipboard = gtk_widget_get_clipboard (
        GTK_WIDGET (selection_widget),
        GDK_SELECTION_CLIPBOARD);
   
  gtk_clipboard_set_text (clipboard, 
        selected_text->str, 
        selected_text->len);
        
}

static void copy_text_cb(GtkWidget *widget, gpointer user_data) {
 
 if( selection_region ){
  copy_text();
 }
 
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
  colorseldlg = gtk_color_chooser_dialog_new ("Select Highlight color", NULL);
 else if(option == 2)
  colorseldlg = gtk_color_chooser_dialog_new ("Select Background color", NULL);
 else
  return;
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){ 
 
  GdkRGBA color;
          
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
  
 }
 
 gtk_widget_destroy (colorseldlg);
    
}

static void color_set_cb(GtkWidget *widget, gpointer userdata){
 color_set(1);
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
 
 width_offset = 0;
 
 pre_motion_x = 0.0;
 
 newline_y = 0;
 
 left_right = 0;
 
 word_not_found = 1;

 find_ptr = find_ptr_head = NULL;
 lmatches = rmatches = NULL;
 page_change_str = NULL;

 FONT_SIZE = 11;

 INIT_LIST_HEAD(&NOTE_HEAD);
 INIT_LIST_HEAD(&HR_HEAD);
 
 have_selection = 0;
 selected_text = NULL;
 
 pressed = 0;
   
 layout_move = 0;

 last_region = NULL;
 selection_region = NULL;
 
 llast_region = NULL;
 lselection_region = NULL;
 
 start_x = start_y = 0.0;
 
 pre_keyval = GDK_KEY_Escape;
 
 sel_cursor = NULL;
    
 scroll_count = 0; 
 scroll_time = 0;
 scroll_zoom = -1;
  
 accel_group = gtk_accel_group_new();
 
 win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_window_add_accel_group(GTK_WINDOW(win), accel_group);
 
 //*****
 //g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) win);
 //time_handler(win);
 //cursor_enable = TRUE;
 //*****
 
 GdkScreen *screen = gdk_screen_get_default ();
 
 if( page_height > page_width ){ //portrait
  if(page_height >= gdk_screen_get_height(screen))
   gtk_widget_set_size_request(win, (int)(page_width/2)+30, gdk_screen_get_height(screen)/2 - 80);
  else
   gtk_widget_set_size_request(win, (int)page_width+30, gdk_screen_get_height(screen) - 80);
 }else{ //landscape
  if(page_height >= gdk_screen_get_height(screen))
   gtk_widget_set_size_request(win, (int)(page_width/2)+1, (int)page_height/2 + 80);
  else
   gtk_widget_set_size_request(win, (int)page_width+1, (int)page_height + 80);
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
 
 selection_widget = gtk_invisible_new ();
 gtk_selection_add_target (selection_widget,
                           GDK_SELECTION_PRIMARY,
                           GDK_SELECTION_TYPE_STRING,
                           1);
                           
 gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
 gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 1);
 
 g_signal_connect(G_OBJECT(scrolled_window), "size-allocate", G_CALLBACK(size_allocate_cb), NULL);
 
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
        G_CALLBACK(color_set), NULL);
 
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
 
 event_box = gtk_event_box_new();
 levent_box = gtk_event_box_new();
 
 gtk_widget_hide(levent_box);
 
 gtk_widget_add_events(GTK_WIDGET(event_box), GDK_POINTER_MOTION_MASK);
 gtk_widget_add_events(GTK_WIDGET(event_box), GDK_MOTION_NOTIFY);
 
 gtk_widget_add_events(GTK_WIDGET(levent_box), GDK_POINTER_MOTION_MASK);
 gtk_widget_add_events(GTK_WIDGET(levent_box), GDK_MOTION_NOTIFY);
    
 layout = gtk_layout_new(NULL, NULL);
 
 gint width, height;
 
 width = (gint)((page_width*zoom_factor)+0.5);
 height = (gint)((page_height*zoom_factor)+0.5);
 
 gtk_layout_set_size(GTK_LAYOUT(layout), width, height);
     
 gtk_widget_add_events (layout, GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

 g_signal_connect (scrolled_window, "button-press-event", G_CALLBACK (sw_button_press_cb), NULL);
    
 sw_motion_handler_id = g_signal_connect (scrolled_window, "motion-notify-event", G_CALLBACK (sw_button_motion_cb), NULL);

 gtk_container_add(GTK_CONTAINER(event_box), m_PageImage);
 gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
 
 lm_PageImage = NULL;
 
 gtk_event_box_set_visible_window(GTK_EVENT_BOX(levent_box), FALSE);

 gtk_layout_put (GTK_LAYOUT (layout), levent_box, 0, 0);
 gtk_layout_put (GTK_LAYOUT (layout), event_box, 0, 0);
    
 gtk_container_add(GTK_CONTAINER(scrolled_window), layout);
   
 g_signal_connect(scrolled_window, "key-press-event", G_CALLBACK(scrolled_window_keypress_cb), findbar);
 g_signal_connect(scrolled_window, "scroll-event", G_CALLBACK(scrolled_window_scroll_cb), NULL);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 pre_sw_width = sw_alloc.width;

 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);

 pre_eb_width = eb_alloc.width;
 
 gtk_widget_show_all(win);

 gtk_widget_hide(findbar);
 gtk_widget_hide(toolbar);
 
}

static void size_decrease_cb(GtkAllocation *allocation){
 
 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 GtkAllocation win_alloc;
 gtk_widget_get_allocation (win, &win_alloc);
 
 GtkAllocation tb_alloc;
 gtk_widget_get_allocation(toolbar, &tb_alloc);
 
 //event_box
 
 if( allocation->height <= eb_alloc.height ){
    
  if( allocation->width <= eb_alloc.width )
   gtk_layout_move(GTK_LAYOUT(layout), event_box, 2, 0);
  else{
  
   #if GTK_CHECK_VERSION(3,18,10)
   width_offset = (allocation->width - eb_alloc.width)/2;
   #else
   width_offset = (allocation->width - eb_alloc.width)/2-7;
   #endif 
   
   if( !lpage)
    gtk_layout_move(GTK_LAYOUT(layout), event_box, width_offset, 0);
   else{
    
    int dp_width = (int)(zoom_factor*page_width);
    
    if(page_width >= page_height){//landscape
     
     gtk_layout_move(GTK_LAYOUT(layout), event_box,dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, height_offset);
     
    }
    else{//portrait
     
     gtk_layout_move(GTK_LAYOUT(layout), event_box,dp_width+1, 0);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, 0);
     
    }
   
   }
     
  }
    
 }else{
   
   height_offset =  (allocation->height - eb_alloc.height)/2;
   
   width_offset = (allocation->width - eb_alloc.width)/2;
   
   if( !lpage) 
    gtk_layout_move(GTK_LAYOUT(layout), event_box, width_offset, height_offset);
   else{
    
    int dp_width = (int)(zoom_factor*page_width);
    
    if(page_width >= page_height){//landscape
     
     gtk_layout_move(GTK_LAYOUT(layout), event_box,dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, height_offset);
     
    }
    else{//portrait
     
     gtk_layout_move(GTK_LAYOUT(layout), event_box,dp_width+1, 0);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, 0);
     
    }
    
   } 
 }
 
 //event_box
 
 //comment
 struct list_head *tmp;

  list_for_each(tmp, &NOTE_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
    
    if( zoom_factor > 1.0 ){
     
     if( allocation->height <= eb_alloc.height ){
     
      if( allocation->width >= eb_alloc.width ){
      
       if(!lpage){
        gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
       }
       else{ // dual-page mode
         
         int dp_width = (int)(zoom_factor*page_width);
         
         height_offset = (allocation->height - eb_alloc.height)/2;
         width_offset = (allocation->width - eb_alloc.width)/2;
         
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
      
      if(!lpage){
       height_offset = (allocation->height - eb_alloc.height)/2;
      
       gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
      }
      else{ //dual-page mode
       
       height_offset = (allocation->height - eb_alloc.height)/2;
       
       int dp_width = (int)(zoom_factor*page_width);
       
       if(tmp1->page_num == current_page_num + 1){
        gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor),
         (gint)(tmp1->y*zoom_factor)+height_offset);
       }
       else if(tmp1->page_num == current_page_num + 2){
        gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)((tmp1->x)*zoom_factor)+dp_width+2,
         (gint)(tmp1->y*zoom_factor)+height_offset);
       }
       
      }
         
     }
     
    }
    else if( zoom_factor == 1.0 ){
     
     if( allocation->height > eb_alloc.height ){
     
      height_offset =  (allocation->height - eb_alloc.height)/2;
     
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
     
     height_offset =  (allocation->height - eb_alloc.height)/2;
    
     gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor)+height_offset);
    
    }
   
  }
 //comment
 
}

static void size_increase_cb(GtkAllocation *allocation){
 
 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);
 
 if( allocation->height <= eb_alloc.height ){
  
  if( allocation->width <= eb_alloc.width ){
   gtk_layout_move(GTK_LAYOUT(layout), event_box, 2, 0);
  }else{
   
   if(zoom_factor == 1.0){ //check
    
    if(page_width >= page_height){
     
     width_offset = (allocation->width - eb_alloc.width)/2;
    }else{
     
     #if GTK_CHECK_VERSION(3,12,3)
      width_offset = (allocation->width - eb_alloc.width)/2;
     #else
      width_offset = (allocation->width - eb_alloc.width)/2-7;
     #endif 
    }
   }else{
   
    #if GTK_CHECK_VERSION(3,18,10)
     width_offset = (allocation->width - eb_alloc.width)/2;
    #else
     width_offset = (allocation->width - eb_alloc.width)/2-7;
    #endif 
    
   }
   
   GtkAllocation eb_alloc;
   gtk_widget_get_allocation (event_box, &eb_alloc);
 
   GtkAllocation sw_alloc;
   gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
   GtkAllocation win_alloc;
   gtk_widget_get_allocation (win, &win_alloc);
   
   if(width_offset < 0)
    width_offset = 2;
   
   if(lpage){
   
    int dp_width = (int)(zoom_factor*page_width); 
     
    if(page_width >= page_height){
     
     height_offset =  (allocation->height - eb_alloc.height)/2;
     
     gtk_layout_move(GTK_LAYOUT(layout), event_box, dp_width+1, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, height_offset);
     
    }
    else{
    
     gtk_layout_move(GTK_LAYOUT(layout), event_box, dp_width+2, 0);
     gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, 0);
     
    }
    
   }else
    gtk_layout_move(GTK_LAYOUT(layout), event_box, width_offset, 0);
  
  }
    
 }else{
  
  GtkAllocation eb_alloc;
  gtk_widget_get_allocation (event_box, &eb_alloc);
 
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
  
  GdkScreen *screen = gdk_screen_get_default ();
  int screen_width = gdk_screen_get_width(screen);
    
  height_offset =  (allocation->height - eb_alloc.height)/2;
    
  width_offset = (allocation->width - eb_alloc.width)/2;
  
  if(lpage){ 
  
   int dp_width = (int)(zoom_factor*page_width);
   
   gtk_layout_move(GTK_LAYOUT(layout), event_box, dp_width+1, height_offset);
   gtk_layout_move(GTK_LAYOUT(layout), levent_box,0, height_offset);
   
  }else
   gtk_layout_move(GTK_LAYOUT(layout), event_box, width_offset, height_offset);
    
 }
  
 //comment
 
 struct list_head *tmp;
 
 list_for_each(tmp, &NOTE_HEAD){ 
  
  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  
  if( zoom_factor > 1.0 ){
     
   if( allocation->height <= eb_alloc.height ){
    
    if( allocation->width >= eb_alloc.width ){
     
     //pdf's height >= window's height
     //pdf's width <= windows' width 
       
     //event_box has higer height
     
     if(!lpage){  
      gtk_layout_move(GTK_LAYOUT(layout),
         tmp1->comment,
         (gint)(tmp1->x*zoom_factor)+width_offset,
         (gint)(tmp1->y*zoom_factor));
         
      if(tmp1->page_num != current_page_num+1)
       gtk_widget_hide(tmp1->comment);
     }
     else{ //dual-page mode
     
      if( page_width < page_height ){
      
       height_offset = (allocation->height - eb_alloc.height)/2;
       width_offset = (allocation->width - eb_alloc.width)/2;
       
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
    
    height_offset =  (allocation->height - eb_alloc.height)/2;
    
    if(!lpage){  
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
  else if( zoom_factor == 1.0 ){
     
   if( allocation->height > eb_alloc.height ){
    
    height_offset =  (allocation->height - eb_alloc.height)/2;
     
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
   
   height_offset =  (allocation->height - eb_alloc.height)/2;
    
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
 
 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);
 
 if( pre_sw_width == allocation->width+1 ){
  ;
 }else if(pre_sw_width > allocation->width){
  
  GtkAllocation eb_alloc;
  gtk_widget_get_allocation (event_box, &eb_alloc);
  
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
  
  GtkAllocation eb_alloc;
  gtk_widget_get_allocation (event_box, &eb_alloc);
  
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
 
 if(eb_alloc.width > pre_eb_width ){
  //1, zoom in
  
  size_increase_cb(allocation);
  
  pre_eb_width = eb_alloc.width;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 else if( eb_alloc.width < pre_eb_width){ 
  //2, zoom out
  
  size_decrease_cb(allocation);
    
  pre_eb_width = eb_alloc.width;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 
} // end of size_allocate_cb

gboolean time_handler(GtkWidget *widget) {
 
 if(cursor_enable){
 
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
   
   if( find_ptr ){
         
    invert_search_region();

    g_list_free(find_ptr_head);
    find_ptr_head = NULL;
    find_ptr = NULL;
    
    if( lpage ){
     lmatches = rmatches = NULL;
    }
    
   }
   
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

 if( mode == TEXT_SELECTION && selection_region){
  cairo_region_t *invert_region;
  invert_region = cairo_region_copy(selection_region);
  invertRegion(selection_region,1);
  
  cairo_region_destroy(selection_region);
  selection_region = NULL;
 }

 if( find_ptr ){
  
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
	
 if(mode != TEXT_HIGHLIGHT && mode != ERASE_TEXT_HIGHLIGHT)
  pre_mode = mode;

 mode = ERASE_TEXT_HIGHLIGHT;

 if(selection_region){
  cairo_region_destroy(selection_region);
  selection_region= NULL;
 }

 if(last_region){
  
  cairo_region_destroy(last_region);
  last_region= NULL;
 }
 
 char page_str[100];
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);
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

 if(selection_region){
  
  pre_mode = mode;
  mode = TEXT_SELECTION;
  
 }
 else{

  pre_mode = mode = TEXT_SELECTION;
  
 }

 char page_str[100];
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);

}

static void
text_selection_mode_cb(GtkWidget* widget, gpointer data) {

 text_selection_mode_change();

}

void
text_highlight_mode_change(void) {
 
 if( mode == TEXT_SELECTION && selection_region && last_region != NULL){
  cairo_region_t *invert_region;
  invert_region = cairo_region_copy(selection_region);
  invertRegion(selection_region,1);
  
  cairo_region_destroy(selection_region);
  selection_region = NULL;
 }
 
 if( find_ptr ){
   
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
 
 if(mode != TEXT_HIGHLIGHT)
  pre_mode = mode;

 mode = TEXT_HIGHLIGHT;

 char page_str[100];
 
 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);

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
scrolled_window_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer data){

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
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));

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
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));

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
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));

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
     gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));

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
     g_signal_emit_by_name(G_OBJECT(widget), "scroll-child", direction, horizontal, &returnValue);
 
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
  case GDK_KEY_k:
   
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
    
     if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(dual_pageMi) ) )
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), TRUE );
     else
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
     
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
        
      GdkRGBA color;
      gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
          
      hc[0] = (guchar)(color.red*255);
      hc[1] = (guchar)(color.green*255);
      hc[2] = (guchar)(color.blue*255);
          
      char hc_color_name[9];
          
      sprintf(hc_color_name, "%02x%02x%02x00",hc[0],hc[1],hc[2]);
          
      unsigned long hc_color = strtoul(hc_color_name, NULL, 16);
      gdk_pixbuf_fill(hc_pixbuf, hc_color);
          
      gtk_image_set_from_pixbuf(GTK_IMAGE(hc_button_Image),hc_pixbuf);
          
     }
         
     gtk_widget_destroy (colorseldlg);
         
    }//end of if (event->state & GDK_CONTROL_MASK
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_x : 
    
     if (event->state & GDK_CONTROL_MASK){
      if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
       gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(inverted_colorMi), TRUE);
      else
       gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(inverted_colorMi), FALSE);
       
      inverted_color_cb();
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
         
      char page_str[100];
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     }
     else if( mode == TEXT_HIGHLIGHT ){
         
      erase_text_highlight_mode_cb(NULL, NULL);
         
      char page_str[100];
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     }
     else if( mode == ERASE_TEXT_HIGHLIGHT ){
         
      text_selection_mode_cb(NULL, NULL);
         
      char page_str[100];
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
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
       
    char page_str[100];
        
    sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
    gtk_window_set_title(GTK_WINDOW(win), page_str);
       
    word_not_found = 1;
        
    pre_mode = mode = TEXT_SELECTION;
        
    direction = GTK_SCROLL_NONE;
    cursor_enable = FALSE;
    
    break;
   case GDK_KEY_h:
        
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

     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){
      areas_ptr--;
     }

    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_l:
        
    if(areas_ptr <= areas+n_areas-1){
         
     if( inverted ){
          
      invertArea((int)((*areas_ptr).x1*zoom_factor),
                 (int)((*areas_ptr).y1*zoom_factor),
                 (int)((*areas_ptr).x2*zoom_factor),
                 (int)((*areas_ptr).y2*zoom_factor), 1);
     }
     
     areas_ptr++;
         
     if( (*areas_ptr).y1 == (*areas_ptr).y2 ){
      areas_ptr++;
     }

    }
    
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

static void
levent_box_release_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data ){

 int selection_region_num;
 
 if(lselection_region){
  selection_region_num = cairo_region_num_rectangles(lselection_region);
 }
 
 if( mode == TEXT_HIGHLIGHT ){

  if(lselection_region){
   
   cairo_region_destroy(lselection_region);
   lselection_region = NULL;
   
  }
  //check
  cairo_region_t *tmp_last_region = NULL;
  if(llast_region){
   tmp_last_region = llast_region;
   llast_region = NULL;
  }
  //check
  
  int dup = 0;

  if( !list_empty(&HR_HEAD) ){

   struct list_head *tmp, *q;
  
   cairo_rectangle_int_t *tmp_hr = hr;

   int hr_a2_x, hr_a2_width;
   char hr_a2_color_name[7];

   if(!hr){
    goto highlight_click;
   }
  
   if(!pressed){
    goto highlight_click;
   }
  
   int a_x, a_y, a_width, a_height;
   int b_x, b_y, b_width, b_height;
  
   int dup_x, dup_y, dup_width, dup_height;
   dup_x = dup_y = dup_width = dup_height = -1;
  
   while( tmp_hr->x != -1 ){

    b_x = (int)(tmp_hr->x/zoom_factor+0.5);
    b_y = (int)(tmp_hr->y/zoom_factor+0.5);
    b_width = (int)(tmp_hr->width/zoom_factor+0.5);
    b_height = (int)(tmp_hr->height/zoom_factor+0.5);
    
    list_for_each_safe(tmp, q, &HR_HEAD){

     struct highlight_region *tmp1;
     tmp1= list_entry(tmp, struct highlight_region, list);
     
     if( tmp1->page_num !=  current_page_num+1 ){
      continue;
     }
     
     a_x = tmp1->x;
     a_y = tmp1->y;
     a_width = tmp1->width;
     a_height = tmp1->height;
     
     if( ( a_y == b_y ) && ( a_height == b_height )){
       
       if( a_x == b_x  ){
       
        if( a_width == b_width ){ //a == b
        
         //  ---------
         // |    a    |
         //  ---------
         
         char color_name[7];
         sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
         tmp1->color_name[0] = color_name[0];  
         tmp1->color_name[1] = color_name[1]; 
         tmp1->color_name[2] = color_name[2];
         tmp1->color_name[3] = color_name[3];
         tmp1->color_name[4] = color_name[4];
         tmp1->color_name[5] = color_name[5];
         
         dup = 1;     
        }
        else if( a_width > b_width ){ //b is within a
         
         //  ---------
         // |  b | a  |
         //  ---------
         
         tmp1->x = b_x + b_width;
         tmp1->width = a_x + a_width - b_x - b_width;
         
         dup = 0;
         
        }
        else{ //a is fully coverd by b
        
         //  ---------
         // |    b    |
         //  ---------
        
         tmp1->width = b_width;
         
         char color_name[7];
         sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
     
         tmp1->color_name[0] = color_name[0];  
         tmp1->color_name[1] = color_name[1]; 
         tmp1->color_name[2] = color_name[2];
         tmp1->color_name[3] = color_name[3];
         tmp1->color_name[4] = color_name[4];
         tmp1->color_name[5] = color_name[5];
        
         dup = 1;
        }
       
       }else if( a_x < b_x ){
        
        if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) ){

         // b is within a
         
         // a_x   b_x
         //  --------------
         // |  a1 | b  |  a2 |
         //  --------------
         
         tmp1->width = b_x - a_x; 
         
         hr_a2_x = b_x + b_width;
         hr_a2_width = a_x + a_width - b_x - b_width;
         strcpy(hr_a2_color_name, tmp1->color_name);
         
         dup = 2;
        }
        else if(( a_x + a_width ) <= ( b_x + b_width ) && b_x < ( a_x + a_width ) ){ 
         
         // a_x  b_x
         //  --------------
         // |  a | b       |
         //  --------------
        
         tmp1->width = b_x - a_x; 
         
         dup = 0;
        
        }
        else{ 
         // there is a gap between a and b
         
         if(dup != 2){
          dup = 0;
         }
        }
     
       }//if( a_x > b_x )
       else if( a_x > b_x ){

        if( (a_x + a_width) < (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
         // a is within b
         
         //  ---------
         // |    b    |
         //  ---------
         
         if(dup_x == -1){
          dup_x = b_x;
          dup_y = b_y;
          dup_width = b_width;
          dup_height = b_height;
          
          tmp1->x = b_x;
          tmp1->width = b_width;
         
          char color_name[7];
          sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
          tmp1->color_name[0] = color_name[0];  
          tmp1->color_name[1] = color_name[1]; 
          tmp1->color_name[2] = color_name[2];
          tmp1->color_name[3] = color_name[3];
          tmp1->color_name[4] = color_name[4];
          tmp1->color_name[5] = color_name[5];
         }
         else{
         
          if(  dup_x != b_x || dup_y != b_y  ){
          
           dup_x = b_x;
           dup_y = b_y;
           dup_width = b_width;
           dup_height = b_height;
           
           tmp1->x = b_x;
           tmp1->width = b_width;
         
           char color_name[7];
           sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
          
           tmp1->color_name[0] = color_name[0];  
           tmp1->color_name[1] = color_name[1]; 
           tmp1->color_name[2] = color_name[2];
           tmp1->color_name[3] = color_name[3];
           tmp1->color_name[4] = color_name[4];
           tmp1->color_name[5] = color_name[5];
          
          }
          else{
          
           if( dup_y == b_y ){
           //remove other highlight regions on the same line
           
            free(tmp1->color_name);
            
            list_del(&tmp1->list);
            free(tmp1);
           }
          }
         
         }
         dup = 1;
        }
        else if( (a_x + a_width) > (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
         
         // b_x  a_x
         //  --------------
         // |  b | a      |
         //  --------------
         
         dup = 0;
         
         tmp1->x = b_x + b_width;
         tmp1->width = a_x + a_width - b_x - b_width;
         
        }
        else{ 
        
         //if( a_x + a_width != b_x + b_width){
         // b_x     a_x
         //  -----  ---------
         // |  b |  |    a  |
         //  -----  --------
        
         if( a_x + a_width == b_x + b_width ){
          
          char color_name[7];
          sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
          tmp1->color_name[0] = color_name[0];  
          tmp1->color_name[1] = color_name[1]; 
          tmp1->color_name[2] = color_name[2];
          tmp1->color_name[3] = color_name[3];
          tmp1->color_name[4] = color_name[4];
          tmp1->color_name[5] = color_name[5];
          
          dup = 1;
         }
         else{
          if( dup != 2)
           dup = 0;
         }
        }
       } // end of else if( a_x > b_x )
       
     }
     else{ //differenct height
      
      if( a_x >= b_x && a_y+ a_height > b_y && a_y+ a_height <= b_y+b_height && b_width > a_width+1){
       //used to remove multiple lines
       //add  condition b_width > a_width +1 for preventing removing highlight regions
       //which locates different lines but have same width as new highlight region 
          
       free(tmp1->color_name);
          
       list_del(&tmp1->list);
       free(tmp1);
      }
        
      if( b_y >= a_y+a_height  ){
        
       if(dup != 2 || dup != 1)
        dup = 0;
        
      } 
     }
    }// end of list_for_each(tmp, &highlight_region_head.list)

    if(!dup){

     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));
     
     hg->x = b_x;
     hg->y = b_y;
     hg->width = b_width;
     hg->height = b_height;
     
     hg->color_name = (char *)malloc(6+1);
     
     char color_str[7];
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
     
     strcpy(hg->color_name, color_str);
     
     hg->page_num = current_page_num+1;
     
     list_add(&(hg->list), &HR_HEAD);
    } // end of if(!dup)
    else if( dup == 2 ){
    
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = b_x;
     hg->y = b_y;
     hg->width = b_width;
     hg->height = b_height;
     hg->color_name = (char *)malloc(6+1);
     
     char color_str[7];
     
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
    
     strcpy(hg->color_name, color_str);
     
     hg->page_num = current_page_num+1;
     
     list_add(&(hg->list), &HR_HEAD);
    
     struct highlight_region *hg_a2 = (struct highlight_region *)malloc(sizeof(struct highlight_region));
     
     hg_a2->x = hr_a2_x;
     hg_a2->y = b_y;
     hg_a2->width = hr_a2_width;
     hg_a2->height = b_height;
     
     hg_a2->color_name = (char *)malloc(6+1);
     
     strcpy(hg_a2->color_name, hr_a2_color_name);
     
     hg_a2->page_num = current_page_num+1;
     
     list_add(&(hg_a2->list), &HR_HEAD);
     
    }
   
    tmp_hr++;
   } // end of while( tmp_hr->x != -1 ) 

  } // end of if( !list_empty(highlight_region_head) )
  else{

   if( hr ){
    cairo_rectangle_int_t *tmp_hr = hr;
    
    while( tmp_hr->x != -1 ){
     
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = (int)(tmp_hr->x/zoom_factor+0.5);
     hg->y = (int)(tmp_hr->y/zoom_factor+0.5);
     hg->width = (int)(tmp_hr->width/zoom_factor+0.5);
     hg->height = (int)(tmp_hr->height/zoom_factor+0.5);

     hg->color_name = (char *)malloc(6+1);
      
     char color_str[7];
     
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
     
     strcpy(hg->color_name, color_str);
     
     hg->page_num = current_page_num+1;
      
     list_add(&(hg->list), &HR_HEAD);
    
     tmp_hr++;
    }
   }
   
  } // end of else
  
  page_change();
  
 } // end of if( mode == TEXT_HIGHLIGHT )
 else if( mode == ERASE_TEXT_HIGHLIGHT ){
 
  llast_region = NULL;

  cairo_rectangle_int_t *tmp_ihr = ihr;
 
  if(!ihr){
    
   goto highlight_click;
   
  }
   
  int a_x, a_y, a_width, a_height;
  int b_x, b_y, b_width, b_height;
   
  while( tmp_ihr->x != -1 ){

   b_x = (int)(tmp_ihr->x/zoom_factor+0.5);
   b_y = (int)(tmp_ihr->y/zoom_factor+0.5);
   b_width = (int)(tmp_ihr->width/zoom_factor+0.5);
   b_height = (int)(tmp_ihr->height/zoom_factor+0.5);


   struct list_head *tmp, *tmp_next;
   struct highlight_region *tmp1;

   list_for_each_safe(tmp, tmp_next, &HR_HEAD){
     
    tmp1= list_entry(tmp, struct highlight_region, list);
    
    if( tmp1->page_num !=  current_page_num+1 ){
      
     continue;
    }
     
    a_x = tmp1->x;
    a_y = tmp1->y;
    a_width = tmp1->width;
    a_height = tmp1->height;
     
     
     if( a_y == b_y && a_height == b_height ){
     
      if( a_x == b_x ){
      
       if( a_width <= b_width ){
       
        //  ---------
        // |    b    |
        //  ---------
        
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
        
       }
       else if( a_width > b_width ){ //b is within a
       
        //  ---------
        // |  b | a  |
        //  ---------
       
        tmp1->x = b_x + b_width;
        tmp1->width = a_x + a_width - b_x - b_width;
       
       }
      
      } // end of  if( a_x == b_x )
      else if( a_x < b_x ){
       
       if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) ){
        // b is within a
         
        // a_x   b_x
        //  --------------
        // |  a1 | b  |  a2 |
        //  --------------
        
        //a1
        tmp1->width = b_x - a_x; 
        
        //a2
        struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

        hg->x = b_x + b_width;
        hg->y = b_y;
        hg->width = a_x + a_width - b_x - b_width;
        hg->height = b_height;

        hg->color_name = (char *)malloc(6+1);

        strcpy(hg->color_name, tmp1->color_name);

        hg->page_num = current_page_num + 1;

        list_add(&(hg->list), &HR_HEAD);
        
       } // end of if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) 
       else if(( a_x + a_width ) <= ( b_x + b_width ) && b_x < ( a_x + a_width ) ){ 
       
        // a_x  b_x
        //  --------------
        // |  a | b       |
        //  --------------
      
        tmp1->width = b_x - a_x; 
         
       }
      
      }// end of else if( a_x < b_x )
      else if( a_x > b_x ){
       
       if( (a_x + a_width) < (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
        // a is within b 
         
        //  ---------
        // |    b    |
        //  ---------
       
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
        
       }
       else if( (a_x + a_width) > (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
       
        // b_x  a_x
        //  --------------
        // |  b | a      |
        //  --------------
        
        tmp1->x = b_x + b_width;
        tmp1->width = a_x + a_width - b_x - b_width;
       
       }
       else{
       
        //selection_region_num
        //for multiple lines deletion
        if(selection_region_num > 1){
        
         if( a_x > b_x && a_x < b_x+b_width ){
          free(tmp1->color_name);

          list_del(&tmp1->list);
        
          free(tmp1);
         }
        }
        
       }
      
      }// end of  else if( a_x > b_x 

     } // end of if( tmp_ihr->y == tmp1->y && tmp_ihr->height == tmp1->height )
     else{  // different height
       
      if( a_y == b_y ){
       
       if( b_width >= a_width ){
       
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
       }
       else{
        
        struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

        hg->x = b_x + b_width;
        hg->y = b_y;
        hg->width = a_x + a_width - b_x - b_width;
        hg->height = b_height;

        hg->color_name = (char *)malloc(6+1);

        strcpy(hg->color_name, tmp1->color_name);

        hg->page_num = current_page_num + 1;

        list_add(&(hg->list), &HR_HEAD);

        tmp1->width = b_x - a_x;
       }
       
      }
      else if( b_y < a_y ){
      
       if(selection_region_num > 1){
        if( b_width >= a_width && b_y + b_height>= a_y+a_height ){
        
         //for multiple-line highlight deletion
         //erasing highlight regions merges a large region when users try to delete many 
         //highlight regions which spreads over multiple lines
        
         free(tmp1->color_name);

         list_del(&tmp1->list);
        
         free(tmp1);
        }
       }
       
      }// end of else if( b_y < a_y
      
     }
    } // end of list_for_each(tmp, &highlight_region_head.list)

    tmp_ihr++;
   } // end of while( tmp_ihr->x != -1 )
  
   page_change();
   
 }// end of else if( mode == ERASE_TEXT_HIGHLIGHT )

 highlight_click:

 if( mode != TEXT_HIGHLIGHT && mode != ERASE_TEXT_HIGHLIGHT && pre_mode != TEXT_SEARCH_NEXT && pre_mode != TEXT_SEARCH_PREV && pre_mode != ERASE_TEXT_HIGHLIGHT )
  mode = pre_mode = TEXT_SELECTION;

 if(selected_text && selected_text->str){
 
  #if __linux__
  clipboard = gtk_widget_get_clipboard (
       GTK_WIDGET (selection_widget),
       GDK_SELECTION_PRIMARY);
  #else
  clipboard = gtk_widget_get_clipboard (
       GTK_WIDGET (selection_widget),
       GDK_SELECTION_CLIPBOARD);
  #endif
  
  gtk_clipboard_set_text (clipboard, 
                          selected_text->str, 
                          selected_text->len);
 }

 //for selected text 
 if(sel_cursor && layout_move == 0){
 
  gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
  pressed = 0;

 }

 //add this for avoidng last_region from highlighting again when clicking in text highlight mode
 if(hr){
  free(hr);
  hr = NULL;
 }
 
}

void
event_box_release_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data ){

 int selection_region_num;
 
 if(selection_region){
  selection_region_num = cairo_region_num_rectangles(selection_region);
 }
 
 if( mode == TEXT_HIGHLIGHT ){

  if(selection_region){
   
   cairo_region_destroy(selection_region);
   selection_region = NULL;
   
  }

  //check
  cairo_region_t *tmp_last_region = NULL;
  if(last_region){
   tmp_last_region = last_region;
   last_region = NULL;
  }
  //check
  
  int dup = 0;

  if( !list_empty(&HR_HEAD) ){

   struct list_head *tmp, *q;
  
   cairo_rectangle_int_t *tmp_hr = hr;

   int hr_a2_x, hr_a2_width;
   char hr_a2_color_name[7];

   if(!hr){
    goto highlight_click;
   }
  
   if(!pressed){
    goto highlight_click;
   }
  
   int a_x, a_y, a_width, a_height;
   int b_x, b_y, b_width, b_height;
  
   int dup_x, dup_y, dup_width, dup_height;
   dup_x = dup_y = dup_width = dup_height = -1;
  
   while( tmp_hr->x != -1 ){
    
    b_x = (int)(tmp_hr->x/zoom_factor+0.5);
    b_y = (int)(tmp_hr->y/zoom_factor+0.5);
    b_width = (int)(tmp_hr->width/zoom_factor+0.5);
    b_height = (int)(tmp_hr->height/zoom_factor+0.5);
    
    list_for_each_safe(tmp, q, &HR_HEAD){

     struct highlight_region *tmp1;
     tmp1= list_entry(tmp, struct highlight_region, list);
     
     if( tmp1->page_num !=  current_page_num+1 ){
      continue;
     }
     
     a_x = tmp1->x;
     a_y = tmp1->y;
     a_width = tmp1->width;
     a_height = tmp1->height;
     
     if( ( a_y == b_y ) && ( a_height == b_height )){
       
       if( a_x == b_x  ){
       
        if( a_width == b_width ){ //a == b
         
         //  ---------
         // |    a    |
         //  ---------
         
         char color_name[7];
         sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
         tmp1->color_name[0] = color_name[0];  
         tmp1->color_name[1] = color_name[1]; 
         tmp1->color_name[2] = color_name[2];
         tmp1->color_name[3] = color_name[3];
         tmp1->color_name[4] = color_name[4];
         tmp1->color_name[5] = color_name[5];
         
         dup = 1;     
        }
        else if( a_width > b_width ){ //b is within a
         
         //  ---------
         // |  b | a  |
         //  ---------
         
         tmp1->x = b_x + b_width;
         tmp1->width = a_x + a_width - b_x - b_width;
         
         dup = 0;
         
        }
        else{ //a is fully coverd by b
        
         //  ---------
         // |    b    |
         //  ---------
        
         tmp1->width = b_width;
         
         char color_name[7];
         sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
     
         tmp1->color_name[0] = color_name[0];  
         tmp1->color_name[1] = color_name[1]; 
         tmp1->color_name[2] = color_name[2];
         tmp1->color_name[3] = color_name[3];
         tmp1->color_name[4] = color_name[4];
         tmp1->color_name[5] = color_name[5];
        
         dup = 1;
        }
       
       }else if( a_x < b_x ){
        
        if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) ){

         // b is within a
         
         // a_x   b_x
         //  --------------
         // |  a1 | b  |  a2 |
         //  --------------
         
         tmp1->width = b_x - a_x; 
         
         hr_a2_x = b_x + b_width;
         hr_a2_width = a_x + a_width - b_x - b_width;
         strcpy(hr_a2_color_name, tmp1->color_name);
         
         dup = 2;
        }
        else if(( a_x + a_width ) <= ( b_x + b_width ) && b_x < ( a_x + a_width ) ){ 
         
         // a_x  b_x
         //  --------------
         // |  a | b       |
         //  --------------
        
         tmp1->width = b_x - a_x; 
         
         dup = 0;
        
        }
        else{ 
         // there is a gap between a and b
         
         if(dup != 2){
          dup = 0;
         }
        }
     
       }//if( a_x > b_x )
       else if( a_x > b_x ){

        if( (a_x + a_width) < (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
         // a is within b
         
         //  ---------
         // |    b    |
         //  ---------
         
         if(dup_x == -1){
          dup_x = b_x;
          dup_y = b_y;
          dup_width = b_width;
          dup_height = b_height;
          
          tmp1->x = b_x;
          tmp1->width = b_width;
         
          char color_name[7];
          sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
          tmp1->color_name[0] = color_name[0];  
          tmp1->color_name[1] = color_name[1]; 
          tmp1->color_name[2] = color_name[2];
          tmp1->color_name[3] = color_name[3];
          tmp1->color_name[4] = color_name[4];
          tmp1->color_name[5] = color_name[5];
         }
         else{
         
          if(  dup_x != b_x || dup_y != b_y  ){
          
           dup_x = b_x;
           dup_y = b_y;
           dup_width = b_width;
           dup_height = b_height;
           
           tmp1->x = b_x;
           tmp1->width = b_width;
         
           char color_name[7];
           sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
          
           tmp1->color_name[0] = color_name[0];  
           tmp1->color_name[1] = color_name[1]; 
           tmp1->color_name[2] = color_name[2];
           tmp1->color_name[3] = color_name[3];
           tmp1->color_name[4] = color_name[4];
           tmp1->color_name[5] = color_name[5];
          
          }
          else{
          
           if( dup_y == b_y ){
           //remove other highlight regions on the same line
           
            free(tmp1->color_name);
            
            list_del(&tmp1->list);
            free(tmp1);
           }
          }
         
         }
         dup = 1;
        }
        else if( (a_x + a_width) > (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
         
         // b_x  a_x
         //  --------------
         // |  b | a      |
         //  --------------
         
         dup = 0;
         
         tmp1->x = b_x + b_width;
         tmp1->width = a_x + a_width - b_x - b_width;
         
        }
        else{ 
        
         //if( a_x + a_width != b_x + b_width){
         // b_x     a_x
         //  -----  ---------
         // |  b |  |    a  |
         //  -----  --------
        
         if( a_x + a_width == b_x + b_width ){
          
          char color_name[7];
          sprintf(color_name, "%02x%02x%02x",hc[0],hc[1],hc[2]);
         
          tmp1->color_name[0] = color_name[0];  
          tmp1->color_name[1] = color_name[1]; 
          tmp1->color_name[2] = color_name[2];
          tmp1->color_name[3] = color_name[3];
          tmp1->color_name[4] = color_name[4];
          tmp1->color_name[5] = color_name[5];
          
          dup = 1;
         }
         else{
          if( dup != 2)
           dup = 0;
         }
        }
       } // end of else if( a_x > b_x )
       
     }
     else{ //differenct height
      
      if( a_x >= b_x && a_y+ a_height > b_y && a_y+ a_height <= b_y+b_height && b_width > a_width+1){
       //used to remove multiple lines
       //add  condition b_width > a_width +1 for preventing removing highlight regions
       //which locates different lines but have same width as new highlight region 
          
       free(tmp1->color_name);
          
       list_del(&tmp1->list);
       free(tmp1);
      }
        
      if( b_y >= a_y+a_height  ){
        
       if(dup != 2 || dup != 1)
        dup = 0;
        
      } 
     }
    }// end of list_for_each(tmp, &highlight_region_head.list)

    if(!dup){

     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));
     
     hg->x = b_x;
     hg->y = b_y;
     hg->width = b_width;
     hg->height = b_height;
     
     hg->color_name = (char *)malloc(6+1);
     
     char color_str[7];
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
     
     strcpy(hg->color_name, color_str);
     
     if(!lpage)
      hg->page_num = current_page_num+1;
     else //dual page mode
      hg->page_num = current_page_num+2;
     
     list_add(&(hg->list), &HR_HEAD);
    } // end of if(!dup)
    else if( dup == 2 ){
    
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = b_x;
     hg->y = b_y;
     hg->width = b_width;
     hg->height = b_height;
     hg->color_name = (char *)malloc(6+1);
     
     char color_str[7];
     
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
    
     strcpy(hg->color_name, color_str);
     
     if(!lpage)
      hg->page_num = current_page_num+1;
     else //dual page mode
      hg->page_num = current_page_num+2;
     
     list_add(&(hg->list), &HR_HEAD);
    
     struct highlight_region *hg_a2 = (struct highlight_region *)malloc(sizeof(struct highlight_region));
     
     hg_a2->x = hr_a2_x;
     hg_a2->y = b_y;
     hg_a2->width = hr_a2_width;
     hg_a2->height = b_height;
     hg_a2->color_name = (char *)malloc(6+1);
     
     strcpy(hg_a2->color_name, hr_a2_color_name);
     
     if(!lpage)
      hg_a2->page_num = current_page_num+1;
     else //dual page mode
      hg_a2->page_num = current_page_num+2;
     
     list_add(&(hg_a2->list), &HR_HEAD);
     
    }
   
    tmp_hr++;
   } // end of while( tmp_hr->x != -1 ) 

  } // end of if( !list_empty(highlight_region_head) )
  else{

   if( hr ){
    cairo_rectangle_int_t *tmp_hr = hr;
    
    while( tmp_hr->x != -1 ){
     
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = (int)(tmp_hr->x/zoom_factor+0.5);
     hg->y = (int)(tmp_hr->y/zoom_factor+0.5);
     hg->width = (int)(tmp_hr->width/zoom_factor+0.5);
     hg->height = (int)(tmp_hr->height/zoom_factor+0.5);
     
     hg->color_name = (char *)malloc(6+1);
      
     char color_str[7];
     
     sprintf(color_str, "%02x%02x%02x", hc[0], hc[1], hc[2]);
     
     strcpy(hg->color_name, color_str);
     
     if(!lpage)
      hg->page_num = current_page_num+1;
     else //dual page mode
      hg->page_num = current_page_num+2;
      
     list_add(&(hg->list), &HR_HEAD);
    
     tmp_hr++;
    }
   }
   
  } // end of else
  
  struct list_head *tmp;
  
  list_for_each(tmp, &HR_HEAD){

   struct highlight_region *tmp1;
   tmp1= list_entry(tmp, struct highlight_region, list);

   if(tmp1->page_num ==  current_page_num +2 ){
    
    text_highlight_release((int)(tmp1->x*zoom_factor), (int)(tmp1->y*zoom_factor), (int)((tmp1->x+tmp1->width)*zoom_factor), (int)((tmp1->y+tmp1->height)*zoom_factor), tmp1->color_name,1); 
   
   }

  }
  
  page_change();
  
 } // end of if( mode == TEXT_HIGHLIGHT )
 else if( mode == ERASE_TEXT_HIGHLIGHT ){
  
  last_region = NULL;

  cairo_rectangle_int_t *tmp_ihr = ihr;
 
  if(!ihr){
  
   goto highlight_click;
   
  }
   
  int a_x, a_y, a_width, a_height;
  int b_x, b_y, b_width, b_height;
   
  while( tmp_ihr->x != -1 ){

   b_x = (int)(tmp_ihr->x/zoom_factor+0.5);
   b_y = (int)(tmp_ihr->y/zoom_factor+0.5);
   b_width = (int)(tmp_ihr->width/zoom_factor+0.5);
   b_height = (int)(tmp_ihr->height/zoom_factor+0.5);

   struct list_head *tmp, *tmp_next;
   struct highlight_region *tmp1;

   list_for_each_safe(tmp, tmp_next, &HR_HEAD){
     
    tmp1= list_entry(tmp, struct highlight_region, list);
    
    if(!lpage){
     if( tmp1->page_num !=  current_page_num+1 )
      continue;
     
    }else{ //dual page mode
      if( tmp1->page_num !=  current_page_num+2 )
      continue;
    
    }
     
    a_x = tmp1->x;
    a_y = tmp1->y;
    a_width = tmp1->width;
    a_height = tmp1->height; 
     
     if( a_y == b_y && a_height == b_height ){
     
      if( a_x == b_x ){
      
       if( a_width <= b_width ){
        
        //  ---------
        // |    b    |
        //  ---------
        
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
        
       }
       else if( a_width > b_width ){ //b is within a
       
        //  ---------
        // |  b | a  |
        //  ---------
       
        tmp1->x = b_x + b_width;
        tmp1->width = a_x + a_width - b_x - b_width;
       
       }
      
      } // end of  if( a_x == b_x )
      else if( a_x < b_x ){
       
       if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) ){
        // b is within a
         
        // a_x   b_x
        //  --------------
        // |  a1 | b  |  a2 |
        //  --------------
        
        //a1
        tmp1->width = b_x - a_x; 
        
        //a2
        struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

        hg->x = b_x + b_width;
        hg->y = b_y;
        hg->width = a_x + a_width - b_x - b_width;
        hg->height = b_height;

        hg->color_name = (char *)malloc(6+1);

        strcpy(hg->color_name, tmp1->color_name);
        
        if(!lpage)
         hg->page_num = current_page_num+1;
        else //dual page mode
         hg->page_num = current_page_num+2;

        list_add(&(hg->list), &HR_HEAD);
        
       } // end of if( ( a_x + a_width ) > ( b_x + b_width ) && b_x < ( a_x + a_width ) 
       else if(( a_x + a_width ) <= ( b_x + b_width ) && b_x < ( a_x + a_width ) ){ 
       
        // a_x  b_x
        //  --------------
        // |  a | b       |
        //  --------------
      
        tmp1->width = b_x - a_x; 
         
       }
      
      }// end of else if( a_x < b_x )
      else if( a_x > b_x ){
       
       if( (a_x + a_width) < (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
        // a is within b 
         
        //  ---------
        // |    b    |
        //  ---------
       
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
        
       }
       else if( (a_x + a_width) > (b_x + b_width) &&  a_x < ( b_x + b_width ) ){
       
        // b_x  a_x
        //  --------------
        // |  b | a      |
        //  --------------
        
        tmp1->x = b_x + b_width;
        tmp1->width = a_x + a_width - b_x - b_width;
       
       }
       else{
       
        //selection_region_num
        //for multiple lines deletion
        if(selection_region_num > 1){
        
         if( a_x > b_x && a_x < b_x+b_width ){
          free(tmp1->color_name);

          list_del(&tmp1->list);
        
          free(tmp1);
         }
        }
        
       }
      
      }// end of  else if( a_x > b_x 

     } // end of if( tmp_ihr->y == tmp1->y && tmp_ihr->height == tmp1->height )
     else{  // different height
       
      if( a_y == b_y ){
       
       if( b_width >= a_width ){
       
        free(tmp1->color_name);

        list_del(&tmp1->list);
        
        free(tmp1);
       }
       else{
        
        struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

        hg->x = b_x + b_width;
        hg->y = b_y;
        hg->width = a_x + a_width - b_x - b_width;
        hg->height = b_height;

        hg->color_name = (char *)malloc(6+1);

        strcpy(hg->color_name, tmp1->color_name);

        if(!lpage)
         hg->page_num = current_page_num+1;
        else //dual page mode
         hg->page_num = current_page_num+2;

        list_add(&(hg->list), &HR_HEAD);

        tmp1->width = b_x - a_x;
       }
       
      }
      else if( b_y < a_y ){
      
       if(selection_region_num > 1){
        if( b_width >= a_width && b_y + b_height>= a_y+a_height ){
        
         //for multiple-line highlight deletion
         //erasing highlight regions merges a large region when users try to delete many 
         //highlight regions which spreads over multiple lines
        
         free(tmp1->color_name);

         list_del(&tmp1->list);
        
         free(tmp1);
        }
       }
       
      }// end of else if( b_y < a_y
      
     }
    } // end of list_for_each(tmp, &highlight_region_head.list)

    tmp_ihr++;
   } // end of while( tmp_ihr->x != -1 )
  
   page_change();
   
 }// end of else if( mode == ERASE_TEXT_HIGHLIGHT )

 highlight_click:

 if( mode != TEXT_HIGHLIGHT && mode != ERASE_TEXT_HIGHLIGHT && pre_mode != TEXT_SEARCH_NEXT && pre_mode != TEXT_SEARCH_PREV && pre_mode != ERASE_TEXT_HIGHLIGHT )
  mode = pre_mode = TEXT_SELECTION;

 if(selected_text && selected_text->str){
 
  #if __linux__
  clipboard = gtk_widget_get_clipboard (
       GTK_WIDGET (selection_widget),
       GDK_SELECTION_PRIMARY);
  #else
  clipboard = gtk_widget_get_clipboard (
       GTK_WIDGET (selection_widget),
       GDK_SELECTION_CLIPBOARD);
  #endif
  
  gtk_clipboard_set_text (clipboard, 
                          selected_text->str, 
                          selected_text->len);
 }

 //for selected text 
 if(sel_cursor && layout_move == 0){
 
  gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
  pressed = 0;

 }

 //add this for avoidng last_region from highlighting again when clicking in text highlight mode
 if(hr){
  free(hr);
  hr = NULL;
 }
 
}

void
textbuffer_changed_cb(GtkTextBuffer *buffer, gpointer user_data){

 GtkWidget *label = (GtkWidget *)user_data;
 
 GtkTextIter startIter;
 GtkTextIter endIter;
 gtk_text_buffer_get_start_iter(buffer, &startIter);
 gtk_text_buffer_get_end_iter(buffer, &endIter);
 
 char *markup;
 
 const char* format;
    
 if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
  format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
 else
  format = "<span foreground=\"black\" font=\"%d\">%s</span>";
 
 markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), gtk_text_buffer_get_text(buffer, &startIter, &endIter, TRUE));
     
 gtk_label_set_markup(GTK_LABEL(label), markup);
 g_free(markup);

}

static void
event_box_motion_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data ){
 
 if(lpage)
  left_right = 1;
 
 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  if(current_page_num > page_num-1) current_page_num = page_num-1; 
  
  if( !lpage )
   page = poppler_document_get_page(doc, current_page_num); 
  else
   page = poppler_document_get_page(doc, current_page_num+1); 
 }

 if( pressed && layout_move == 0 ){
  
  PopplerRectangle rect;
  
  rect.x1 = start_x/zoom_factor;
  rect.y1 = start_y/zoom_factor;
  rect.x2 = event->button.x/zoom_factor;
  rect.y2 = event->button.y/zoom_factor;
  
  if(rect.y1 == rect.y2)
        rect.y2++;
  if(rect.x1 == rect.x2)
        rect.x2++;
  
  cairo_region_t *res = NULL;

  GList *selections = poppler_page_get_selection_region(page, 1.0,
      POPPLER_SELECTION_GLYPH, &rect);
   
  res = cairo_region_create(); 
 
  if(selected_text->str){
   g_string_free(selected_text, FALSE);
  }
  
  selected_text = g_string_new (NULL);

  GList *selection = g_list_first (selections);
  for ( ; NULL != selection ; selection = g_list_next (selection)) {
        
   PopplerRectangle *rectangle = (PopplerRectangle *)selection->data;
   
   cairo_rectangle_int_t rect;
   
   rect.x = (gint) (rectangle->x1*zoom_factor+0.5);
   rect.y = (gint) (rectangle->y1*zoom_factor+0.5);
   rect.width  = (gint) ((rectangle->x2 - rectangle->x1)*zoom_factor+0.5);    
   rect.height = (gint) ((rectangle->y2  - rectangle->y1)*zoom_factor+0.5);
   
   gchar *temp_text;
    
   rectangle->y1 = rectangle->y1 +5;
   rectangle->y2 = rectangle->y2 -5;
   
   temp_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, rectangle);
   
   if(!newline_y)
    newline_y = rectangle->y1;
    
   if(rectangle->y1 > newline_y && newline_y ){
    newline_y = rectangle->y1;
   }
    
   g_string_append (selected_text, temp_text);
   g_string_append (selected_text, "\n");
   g_free (temp_text);

   rect.y = rect.y + (int)(2*zoom_factor);
   rect.height = rect.height - (int)(4*zoom_factor);
   
   if( rect.height < (int)(14*zoom_factor) ){
    rect.y = rect.y - (int)(2*zoom_factor);
    rect.height = rect.height + (int)(2*zoom_factor);
    
   }
   
   cairo_region_union_rectangle (res, &rect);
        
  }// end of for

  if( ! last_region || ! cairo_region_equal(last_region, res) ){
 
   if(last_region){
    cairo_region_destroy(last_region);
   }

   last_region = cairo_region_copy(res);

   if(NULL != selection_region){
    
    if(mode == TEXT_SELECTION){
    
     invertRegion(selection_region,1);
     
    }else if( mode == TEXT_HIGHLIGHT ){
     
     highlight_Region(selection_region, 1);
     
    }else if( mode == ERASE_TEXT_HIGHLIGHT ){
   
     invert_highlight_Region(selection_region, 1);
    
    }
    
    selection_region =  NULL;
   }
   
   if(mode == TEXT_SELECTION){
    
    invertRegion(last_region,1);
    
   }else if( mode == TEXT_HIGHLIGHT ){
   
    highlight_Region(last_region, 1);
   }
   else if( mode == ERASE_TEXT_HIGHLIGHT ){
    invert_highlight_Region(last_region, 1);
   
   }
   
   selection_region = cairo_region_copy(last_region);

  }
  
  gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
  
 } // end of if(pressed)
 
 PopplerRectangle points;
 GList *region;
 cairo_region_t *c_region;

 points.x1 = 0.0;
 points.y1 = 0.0;
 poppler_page_get_size (page, &(points.x2), &(points.y2));
 
 points.x2 = points.x2*zoom_factor;
 points.y2 = points.y2*zoom_factor;
 
 region = poppler_page_get_selection_region (page, 1.0,
   POPPLER_SELECTION_GLYPH,
   &points);

 c_region = create_region_from_poppler_region (region, zoom_factor);

 if (c_region){
  
  if( cairo_region_contains_point (c_region, event->motion.x, event->motion.y) )  {
   
   sel_cursor = gdk_cursor_new(GDK_XTERM);
   gdk_window_set_cursor(gtk_widget_get_window(widget), sel_cursor);
   
  }
  else{
   
   if(sel_cursor){
    gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
   }
  }
 } // end of if (c_region)

 g_list_free (region);
 cairo_region_destroy (c_region);
 
} // end of draw_motion_event_cb_2

static void
levent_box_motion_event_cb( GtkWidget* widget, GdkEvent *event, gpointer data ){
 
 if(lpage)
  left_right = 0;
 
 if(lpage == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  if(current_page_num > page_num-1) current_page_num = page_num-1; 
  lpage = poppler_document_get_page(doc, current_page_num); 
 }

 if( pressed && layout_move == 0 ){
  
  PopplerRectangle rect;
  
  rect.x1 = start_x/zoom_factor;
  rect.y1 = start_y/zoom_factor;
  rect.x2 = event->button.x/zoom_factor;
  rect.y2 = event->button.y/zoom_factor;
  
  if(rect.y1 == rect.y2)
        rect.y2++;
  if(rect.x1 == rect.x2)
        rect.x2++;
  
  cairo_region_t *res = NULL;

  GList *selections = poppler_page_get_selection_region(lpage, 1.0,
      POPPLER_SELECTION_GLYPH, &rect);
   
  res = cairo_region_create(); 
 
  if(selected_text->str){
   g_string_free(selected_text, FALSE);
  }
  
  selected_text = g_string_new (NULL);

  GList *selection = g_list_first (selections);
  for ( ; NULL != selection ; selection = g_list_next (selection)) {
        
   PopplerRectangle *rectangle = (PopplerRectangle *)selection->data;
   
   cairo_rectangle_int_t rect;
   
   rect.x = (gint) (rectangle->x1*zoom_factor+0.5);
   rect.y = (gint) (rectangle->y1*zoom_factor+0.5);
   rect.width  = (gint) ((rectangle->x2 - rectangle->x1)*zoom_factor+0.5);    
   rect.height = (gint) ((rectangle->y2  - rectangle->y1)*zoom_factor+0.5);
   
   gchar *temp_text;
    
   rectangle->y1 = rectangle->y1 +5;
   rectangle->y2 = rectangle->y2 -5;
   
   temp_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, rectangle);
   
   if(!newline_y)
    newline_y = rectangle->y1;
    
   if(rectangle->y1 > newline_y && newline_y ){
    newline_y = rectangle->y1;
   }
    
   g_string_append (selected_text, temp_text);
   g_string_append (selected_text, "\n");
   g_free (temp_text);

   rect.y = rect.y + (int)(2*zoom_factor);
   rect.height = rect.height - (int)(4*zoom_factor);
   
   if( rect.height < (int)(14*zoom_factor) ){
    rect.y = rect.y - (int)(2*zoom_factor);
    rect.height = rect.height + (int)(2*zoom_factor);
   }
   
   cairo_region_union_rectangle (res, &rect);
        
  }// end of for

  if( ! llast_region || ! cairo_region_equal(llast_region, res) ){
 
   if(llast_region){
    cairo_region_destroy(llast_region);
   }

   llast_region = cairo_region_copy(res);

   if(NULL != lselection_region){
    
    if(mode == TEXT_SELECTION){
     
     invertRegion(lselection_region,2);
     
    }else if( mode == TEXT_HIGHLIGHT ){
     
     highlight_Region(lselection_region, 2);
     
    }else if( mode == ERASE_TEXT_HIGHLIGHT ){
   
     invert_highlight_Region(lselection_region, 2);
    
    }
    
    lselection_region =  NULL;
   }
   
   if(mode == TEXT_SELECTION){
   
    invertRegion(llast_region,2);
    
   }else if( mode == TEXT_HIGHLIGHT ){
   
    highlight_Region(llast_region, 2);
   }
   else if( mode == ERASE_TEXT_HIGHLIGHT ){
    invert_highlight_Region(llast_region, 2);
   
   }
   
   lselection_region = cairo_region_copy(llast_region);

  }//end of if( ! llast_region || ! cairo_region_equal(llast_region, res) )
  
  gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
  
 } // end of if(pressed)
 
 PopplerRectangle points;
 GList *region;
 cairo_region_t *c_region;

 points.x1 = 0.0;
 points.y1 = 0.0;
 poppler_page_get_size (lpage, &(points.x2), &(points.y2));
 
 points.x2 = points.x2*zoom_factor;
 points.y2 = points.y2*zoom_factor;
 
 region = poppler_page_get_selection_region (lpage, 1.0,
   POPPLER_SELECTION_GLYPH,
   &points);

 c_region = create_region_from_poppler_region (region, zoom_factor);

 if (c_region){
  
  if( cairo_region_contains_point (c_region, event->motion.x, event->motion.y) )  {
   
   sel_cursor = gdk_cursor_new(GDK_XTERM);
   gdk_window_set_cursor(gtk_widget_get_window(widget), sel_cursor);
   
  }
  else{
   
   if(sel_cursor){
    gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
   }
  }
 } // end of if (c_region)

 g_list_free (region);
 cairo_region_destroy (c_region);
 
} // end of draw_motion_event_cb_2

void
sw_button_press_cb( GtkWidget* widget, GdkEventButton *event, gpointer data ){
 
 if ( !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dual_pageMi)) ){
  if(lpage){
   
   g_object_unref (G_OBJECT (lpage));
   lpage = NULL;
   
  }
 }
 
 if(gtk_widget_get_visible(findbar)){
 
  gtk_widget_hide(findbar);
  
  if( find_ptr ){
   invert_search_region();
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
   find_ptr = NULL;
  }
 
  gtk_entry_set_text(GTK_ENTRY(findtext), "");
  gtk_widget_grab_focus (scrolled_window);
  
  KEY_BUTTON_SEARCH = TRUE;
 
  word_not_found = 1;
  
  pre_mode = mode;
  mode = TEXT_SELECTION;
 }
 
 if(KEY_BUTTON_SEARCH && ( mode == TEXT_SEARCH_NEXT || mode == TEXT_SEARCH_PREV )){
  
  gtk_widget_hide(findbar);
  
  if( find_ptr ){
   invert_search_region();
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
   find_ptr = NULL;
  }
  
  char page_str[100];
  
  sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
  gtk_window_set_title(GTK_WINDOW(win), page_str);
       
  pre_mode = mode;
  mode = TEXT_SELECTION;
  
  word_not_found = 1;
 }
 
 if( inside_obj(event) ){
 
  if( g_signal_handler_is_connected (event_box, event_box_motion_handler_id) ){
   g_signal_handler_disconnect(event_box, event_box_motion_handler_id);
  }
  
  if( g_signal_handler_is_connected (event_box, event_box_release_handler_id) ){
   g_signal_handler_disconnect(event_box, event_box_release_handler_id);
  }
  
  if(lpage){ //dual-page mode
  
   if( g_signal_handler_is_connected (levent_box, levent_box_motion_handler_id) ){
    g_signal_handler_disconnect(levent_box, levent_box_motion_handler_id);
   }
  
   if( g_signal_handler_is_connected (levent_box, levent_box_release_handler_id) ){
    g_signal_handler_disconnect(levent_box, levent_box_release_handler_id);
   }
   
  }
  
  layout_press(event);
 }
 else{// !inside_obj(event)
 
  if( g_signal_handler_is_connected (layout, layout_motion_handler_id) ){
   g_signal_handler_disconnect(layout, layout_motion_handler_id);
  }
  
  if( g_signal_handler_is_connected (layout, layout_release_handler_id) ){
   g_signal_handler_disconnect(layout, layout_release_handler_id);
  }
  
  gint lwidth, lheight;
  gtk_layout_get_size(GTK_LAYOUT(layout), &lwidth, &lheight);
  
  event_box_press(event);
 
 } //end of !inside_obj(event)

}

void
sw_button_motion_cb( GtkWidget* widget, GdkEventMotion *event, gpointer data ){

 pre_motion_x = event->x;
 
 if(page == NULL){
  gint page_num = poppler_document_get_n_pages(doc);
  
  if(current_page_num > page_num-1) 
   current_page_num = page_num-1; 
   
  page = poppler_document_get_page(doc, current_page_num); 
 }

 PopplerRectangle points;
 GList *region;
 cairo_region_t *c_region;

 points.x1 = 0.0;
 points.y1 = 0.0;
 poppler_page_get_size (page, &(points.x2), &(points.y2));
 
 points.x2 = points.x2*zoom_factor;
 points.y2 = points.y2*zoom_factor;
 
 region = poppler_page_get_selection_region (page, 1.0,
   POPPLER_SELECTION_GLYPH,
   &points);

 c_region = create_region_from_poppler_region (region, zoom_factor);
 
 if (c_region){
  
  if( cairo_region_contains_point (c_region, event->x, event->y) )  {
   
   sel_cursor = gdk_cursor_new(GDK_XTERM);
   gdk_window_set_cursor(gtk_widget_get_window(event_box), sel_cursor);
   
  }
  else{
   
   if(sel_cursor){
    gdk_window_set_cursor(gtk_widget_get_window(event_box), NULL);
   }
  }
 } // end of if (c_region)

 g_list_free (region);
 cairo_region_destroy (c_region);
 
 g_object_unref (G_OBJECT (page));
 page = NULL;
}

void
event_box_press( GdkEventButton *event ){
 
 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);
 
 if( ! g_signal_handler_is_connected (event_box, event_box_motion_handler_id) ){
 
  event_box_motion_handler_id = g_signal_connect(event_box, "motion_notify_event", 
    G_CALLBACK(event_box_motion_event_cb), scrolled_window);
 }
 
 if( ! g_signal_handler_is_connected (event_box, event_box_release_handler_id) ){
  
  event_box_release_handler_id = g_signal_connect(event_box, "button-release-event", 
    G_CALLBACK(event_box_release_event_cb), areas);
 }
 
 if(lpage){ //dual-page mode
  if( ! g_signal_handler_is_connected (levent_box, levent_box_motion_handler_id) ){
 
   levent_box_motion_handler_id = g_signal_connect(levent_box, "motion_notify_event", 
     G_CALLBACK(levent_box_motion_event_cb), scrolled_window);
  }
 
  if( ! g_signal_handler_is_connected (levent_box, levent_box_release_handler_id) ){
  
   levent_box_release_handler_id = g_signal_connect(levent_box, "button-release-event", 
     G_CALLBACK(levent_box_release_event_cb), areas);
  }
 }
 
 selected_text = g_string_new (NULL);
 selected_text->str = NULL;

 if(mode == TEXT_SEARCH_NEXT || mode == TEXT_SEARCH_PREV){
  pre_mode = mode;
  mode = TEXT_SELECTION;
 }
 else if(mode == TEXT_HIGHLIGHT){
  
  pre_mode = mode;
 }
 else if( mode == ZOOM_IN || mode == ZOOM_OUT){
  mode = TEXT_SELECTION;
 }

 if( have_selection  ){
  GdkWindow *tmp_win = gdk_selection_owner_get(GDK_SELECTION_PRIMARY); 
  
  if(tmp_win != gtk_widget_get_window(selection_widget)){
    
    have_selection = gtk_selection_owner_set (selection_widget,
                                              GDK_SELECTION_PRIMARY,
                                              GDK_CURRENT_TIME);
  }
 }
 
 if(layout_move == 1)
   return;

 if(last_region || selection_region ){
  
   cairo_region_t *invert_region;
   invert_region = cairo_region_copy(selection_region);
   
   if(mode == TEXT_SELECTION){
    
    //check
    if(!page_changed){
     invertRegion(invert_region,1);
    }else{
     invertRegion(invert_region,1);
    }
    //check
     
    page_changed = FALSE;

    if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
     
     invertRegion(invert_region,1);
     
    }
   
    cairo_region_destroy(selection_region);
    selection_region = NULL;
    
    pre_mode = TEXT_SELECTION;
   
   }
   else if( mode == TEXT_HIGHLIGHT ){
    
    if(pre_mode == TEXT_SELECTION ){
     
     invertRegion(invert_region,1);
     
     pre_mode = mode;
    }else if( mode == TEXT_HIGHLIGHT && pre_mode == TEXT_HIGHLIGHT ){
     
     highlight_Region(invert_region, 1);
     
    }
    else if( pre_mode == ERASE_TEXT_HIGHLIGHT ){
    
     invertRegion(invert_region,1);
     invertRegion(invert_region,1);
     
    }
    
    if(invert_region)
     cairo_region_destroy(invert_region);
   }
   else if (mode == ERASE_TEXT_HIGHLIGHT){
   
    if( pre_mode ==  TEXT_HIGHLIGHT){
     if(invert_region)
      cairo_region_destroy(invert_region);
    }
    
   }
   
   gtk_image_set_from_pixbuf(GTK_IMAGE (m_PageImage), pixbuf);
 }
 
 if(llast_region || lselection_region ){
  
   cairo_region_t *invert_region;
   invert_region = cairo_region_copy(lselection_region);
   
   if(mode == TEXT_SELECTION){
    
    //check 
    if(!page_changed){
    
     invertRegion(invert_region,2);
  
    }else{
     
     invertRegion(invert_region,2);
    }
    //check
     
    page_changed = FALSE;

    if( pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT ){
     
     invertRegion(invert_region,2);
    
    }
   
    cairo_region_destroy(lselection_region);
    lselection_region = NULL;
    
    pre_mode = TEXT_SELECTION;
   
   }
   else if( mode == TEXT_HIGHLIGHT ){
    
    if(pre_mode == TEXT_SELECTION ){
    
     invertRegion(invert_region,2);
     pre_mode = mode;
     
    }else if( mode == TEXT_HIGHLIGHT && pre_mode == TEXT_HIGHLIGHT ){
     
     highlight_Region(invert_region, 2);
     
    }
    else if( pre_mode == ERASE_TEXT_HIGHLIGHT ){
    
     invertRegion(invert_region,2);
     invertRegion(invert_region,2);
     
    }
    
   
    if(invert_region)
     cairo_region_destroy(invert_region);
   }
   else if (mode == ERASE_TEXT_HIGHLIGHT){
   
    if( pre_mode ==  TEXT_HIGHLIGHT){
     if(invert_region)
      cairo_region_destroy(invert_region);
    }
    
   }
   
   if(lm_PageImage)
    gtk_image_set_from_pixbuf(GTK_IMAGE (lm_PageImage), lpixbuf);
 }
   
 if(sel_cursor){ 
  pressed = 1;
  
  start_x = event->x;
  start_y = event->y;
 
 }
 
}

void
layout_press(GdkEventButton *event){
  
  //layout motion      
  if( ! g_signal_handler_is_connected (layout, layout_motion_handler_id) ){
    layout_motion_handler_id = g_signal_connect (layout, "motion-notify-event", 
        G_CALLBACK (layout_motion_notify_event), event_box);
  }
  
  //layout release      
  if( ! g_signal_handler_is_connected (layout, layout_release_handler_id) ){
    layout_release_handler_id = g_signal_connect (layout, "button-release-event", 
        G_CALLBACK (layout_button_release_event), NULL);
  }
  
  if (event->button == 1){
   
   GList *list, *iter;
         
   list = gtk_container_get_children (GTK_CONTAINER (layout));
   
   for (iter = list; iter; iter = iter->next){
    
    GtkWidget *child = GTK_WIDGET (iter->data);
           
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
        
        GtkAllocation eb_alloc;
        gtk_widget_get_allocation (event_box, &eb_alloc);
                 
        GtkAllocation sw_alloc;
        gtk_widget_get_allocation (scrolled_window, &sw_alloc);
                 
        if( sw_alloc.height <= eb_alloc.height )
         obj_y = event->y;
        else
         obj_y = event->y+height_offset;
        
     }
     
     if(lpage){
      
      GtkAllocation eb_alloc;
      gtk_widget_get_allocation (event_box, &eb_alloc);
                 
      GtkAllocation sw_alloc;
      gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
      if( sw_alloc.height > eb_alloc.height )
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
       
       if( sw_alloc.height <= eb_alloc.height )
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
      pressed = 0;
             
      break;
     }
            
    }
   
   }
   g_list_free (list);
    
  }//end of if (event->button == 1
  else if (event->button == 3){
   
   pressed = 0;

   GtkWidget *label = get_layout_child(layout, event);

   if(label == NULL){
    return;
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
         
   const char *contents = gtk_label_get_text(GTK_LABEL(label));
   size_t len = strlen (contents);
   gtk_text_buffer_set_text (buffer, contents, len);
   gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), buffer);
         
   g_signal_connect(G_OBJECT( buffer ), "changed",
      G_CALLBACK(textbuffer_changed_cb), label);

   gtk_box_pack_start(GTK_BOX(view_box), textview, TRUE, TRUE, 0);
         
   gtk_widget_show_all(edit_win);
         
   int result = gtk_dialog_run(GTK_DIALOG(edit_win));
         
   if( result == GTK_RESPONSE_OK ){
    
    GtkTextIter startIter;
    GtkTextIter endIter;
    gtk_text_buffer_get_start_iter(buffer, &startIter);
    gtk_text_buffer_get_end_iter(buffer, &endIter);

    char *markup;
      
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    if( !strcmp (gtk_text_buffer_get_text(buffer, &startIter, &endIter, TRUE), ""))
     markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), "New Comment");
    else
     markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), 
                     gtk_text_buffer_get_text(buffer, &startIter, &endIter, TRUE));
          
    gtk_label_set_markup(GTK_LABEL(label), markup);
    g_free(markup);

   }
   else if( result == GTK_RESPONSE_APPLY ){
    gtk_widget_hide(label);
       
    struct list_head *tmp, *q;

    list_for_each_safe(tmp, q, &NOTE_HEAD){

     struct note *tmp1;
     tmp1= list_entry(tmp, struct note, list);
         
     if( tmp1->comment == label ){
            
      list_del(&tmp1->list);
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

 GtkAllocation eb_alloc;
 gtk_widget_get_allocation (event_box, &eb_alloc);
    
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);   
 
 struct list_head *tmp;

 list_for_each(tmp, &NOTE_HEAD){

  struct note *tmp1;
  tmp1= list_entry(tmp, struct note, list);
  if(tmp1->comment == comment){
   
   if(lpage){ //dual-page mode
     
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
   
   if( eb_alloc.height >= sw_alloc.height )
    tmp1->y = ((gint)(event->y - lstart_y) + child_alloc.y)/zoom_factor;
   else
    tmp1->y = ((gint)(event->y - lstart_y) + child_alloc.y)/zoom_factor - (gint)(height_offset/zoom_factor);
    
   if( eb_alloc.width >= sw_alloc.width )
    tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x)/zoom_factor;
   else{
    if(lpage){
    
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
    }else
     tmp1->x = ((gint)((event->x - lstart_x)) + child_alloc.x)/zoom_factor - (gint)(width_offset/zoom_factor);
   }
    
    if(lpage){
     
     if(tmp1->y < 0 || event->y > (gint)((page_height*zoom_factor)+0.5)){
      
      tmp1->x = 100;
      tmp1->y = 100;
      
      gtk_layout_move(GTK_LAYOUT(layout), tmp1->comment, tmp1->x, tmp1->y);
     }
     
    }
    
  }
  
 }//end of list_for_each(tmp, &NOTE_HEAD

 if(lpage){
  //diasable layout_motion and layout_release
  g_signal_handler_disconnect(layout, layout_motion_handler_id);
 
  g_signal_handler_disconnect(layout, layout_release_handler_id);
  //diasable layout_motion and layout_release
 
  //enable event_box motion and levent_motion
  
  if( ! g_signal_handler_is_connected (event_box, event_box_motion_handler_id) ){
 
   event_box_motion_handler_id = g_signal_connect(event_box, "motion_notify_event", 
    G_CALLBACK(event_box_motion_event_cb), scrolled_window);
  }
  
  if( ! g_signal_handler_is_connected (levent_box, levent_box_motion_handler_id) ){
 
   levent_box_motion_handler_id = g_signal_connect(levent_box, "motion_notify_event", 
     G_CALLBACK(levent_box_motion_event_cb), scrolled_window);
  }
  //enable event_box motion and levent_motion
 }
 
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

gboolean
inside_obj( GdkEventButton *event ){

 GList *list, *iter;

 GtkAllocation child_alloc;
 list = gtk_container_get_children (GTK_CONTAINER (layout));
 
 for (iter = list; iter; iter = iter->next){

  GtkWidget *child = GTK_WIDGET (iter->data);
  
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
     
    GtkAllocation eb_alloc;
    gtk_widget_get_allocation (event_box, &eb_alloc);
                 
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation (scrolled_window, &sw_alloc);
                 
    if( sw_alloc.height <= eb_alloc.height )
     obj_y = event->y;
    else
     obj_y = event->y+height_offset;
     
   }
    
   if(lpage){
    
    GtkAllocation eb_alloc;
    gtk_widget_get_allocation (event_box, &eb_alloc);
                 
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
    if( sw_alloc.height > eb_alloc.height )
     obj_y = obj_y+height_offset;
    
    int dp_width = (int)(zoom_factor*page_width);
    
    if( child_alloc.x+child_alloc.width > dp_width+1 ){
     
     if( child_alloc.x > dp_width+1) //the comment is completely on the right page
      obj_x = event->x + dp_width+1;
     else{ //the comment is laid on between left and left page
      
      if(event->x > 500){ //get the left part
       obj_x = event->x;
      }else{ //get the right part
       obj_x = event->x + dp_width+1;
      }
     }
     
     if( sw_alloc.height <= eb_alloc.height )
      obj_y = event->y;
     else
      obj_y = event->y+height_offset;
     
    }
     
   }
   
   if ( (obj_x >= child_alloc.x) &&
        (obj_x < (child_alloc.x + child_alloc.width)) &&
        (obj_y >= child_alloc.y) &&
        (obj_y < (child_alloc.y + child_alloc.height)) ){
    
    if(lpage){ //dual-page mode      
     
     int dp_width = (int)(zoom_factor*page_width);
     
     if( left_right && ( child_alloc.x > dp_width+1 ) ){
      
      return TRUE; 
      
     }
     else if( !left_right && ( child_alloc.x <= dp_width+1 ) ){
      
      return TRUE;
      
     }
     else if( left_right && ( obj_x > dp_width+1 ) ){
      
      return TRUE;
      
     }
     else{
      
      return FALSE;
      
     }
     
    }
    else
     return TRUE; 

   }
  }
  
 }// end of for (iter = list; iter; iter = iter->next)

 return FALSE;

}

GtkWidget * 
get_layout_child(GtkWidget      *layout, 
                 GdkEventButton *event){

 GList *list, *iter;
 list = gtk_container_get_children (GTK_CONTAINER (layout));
 
 for (iter = list; iter; iter = iter->next){

  GtkWidget *child = GTK_WIDGET (iter->data);
  
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
     
     GtkAllocation eb_alloc;
     gtk_widget_get_allocation (event_box, &eb_alloc);
                 
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
                 
     if( sw_alloc.height <= eb_alloc.height )
      obj_y = event->y;
     else
      obj_y = event->y+height_offset;
     
    }
    
    if(lpage){
     
     GtkAllocation eb_alloc;
     gtk_widget_get_allocation (event_box, &eb_alloc);
                 
     GtkAllocation sw_alloc;
     gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
     if( sw_alloc.height > eb_alloc.height )
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
      
      if( sw_alloc.height <= eb_alloc.height )
       obj_y = event->y;
      else
       obj_y = event->y+height_offset;     
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
