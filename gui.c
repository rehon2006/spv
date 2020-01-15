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

#include <unistd.h>
 
#include <pango/pangocairo.h>

#include "list.h"
#include "gui.h"
#include "note.h"
#include "highlight.h"
#include "search.h"
#include "zoom.h"
#include "page.h"
#include "pdf.h"
#include "main.h"

static void ic_cbutton_toggled_cb(GtkWidget *widget, gpointer data){
 
 G_INVERT_COLORS = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
 
}

static void g_cm_bg_color_set_cb(GtkWidget *widget, gpointer user_data){
 
 GtkWidget *colorseldlg = gtk_color_chooser_dialog_new ("Select Color for Comment's Background", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  if( G_CM_BG_COLOR == P_CM_BG_COLOR ){
   
   gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &G_CM_BG_COLOR->color);
   P_CM_BG_COLOR = G_CM_BG_COLOR;
   
  }
  else{
   gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &G_CM_BG_COLOR->color);
  }
  
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
  
  cairo_set_source_rgba (cr, G_CM_BG_COLOR->color.red, 
                             G_CM_BG_COLOR->color.green, 
                             G_CM_BG_COLOR->color.blue, 
                             G_CM_BG_COLOR->color.alpha);
  
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
   
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
 }
 
 gtk_widget_destroy (colorseldlg);
 
}

static void p_cm_bg_color_set_cb(GtkWidget *widget, gpointer user_data){
 
 GtkWidget *colorseldlg = gtk_color_chooser_dialog_new ("Select Color for Comment's Background", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  GdkRGBA color;
  
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
  
  if( gdk_rgba_equal(&color,&P_CM_FONT_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }
  else if( gdk_rgba_equal(&color,&P_CM_BG_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }
  else if( gdk_rgba_equal(&color,&P_HR_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }

  struct color_table *ct_tmp;
  
  if( G_HR_COLOR == P_HR_COLOR ){
   
   if( G_CM_FONT_COLOR == P_CM_FONT_COLOR ){
    ct_tmp = G_CM_BG_COLOR;
   }
   else{
    ct_tmp = P_CM_FONT_COLOR;
   }
   
  }
  else{
   
   if( G_CM_FONT_COLOR == P_CM_FONT_COLOR ){
    ct_tmp = P_HR_COLOR;
   }
   else{
    ct_tmp = P_CM_FONT_COLOR;
   }
   
  }
  
  if( !ct_tmp->next ){
   
   ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;
   ct_tmp->next->color = color;
   ct_tmp->next->next = NULL;
  }
  else{
  
   struct color_table *tmp = ct_tmp->next;
   
   ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;
   ct_tmp->next->color = color;
   ct_tmp->next->next = tmp->next;
   
  }
  
  P_CM_BG_COLOR = ct_tmp->next;
   
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
  
  cairo_set_source_rgba (cr, P_CM_BG_COLOR->color.red, 
                             P_CM_BG_COLOR->color.green, 
                             P_CM_BG_COLOR->color.blue, 
                             P_CM_BG_COLOR->color.alpha);
  
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
   
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
 }
 
 gtk_widget_destroy (colorseldlg);
 
}

static void g_cm_font_color_set_cb(GtkWidget *widget, gpointer user_data){
 
 GtkWidget *colorseldlg;
 
 colorseldlg = gtk_color_chooser_dialog_new ("Select Font Color for Comment", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &G_CM_FONT_COLOR->color);
  
  if( G_CM_FONT_COLOR == P_CM_FONT_COLOR ){
   P_CM_FONT_COLOR = G_CM_FONT_COLOR;
  }
 
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
  
  cairo_set_source_rgba (cr, G_CM_FONT_COLOR->color.red, 
                             G_CM_FONT_COLOR->color.green, 
                             G_CM_FONT_COLOR->color.blue, 
                             G_CM_FONT_COLOR->color.alpha);
   
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
  
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
 }
 
 gtk_widget_destroy (colorseldlg);
 
}

static void p_cm_font_color_set_cb(GtkWidget *widget, gpointer user_data){
 
 GtkWidget *colorseldlg;
 
 colorseldlg = gtk_color_chooser_dialog_new ("Select Font Color for Comment", NULL);
 
 gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorseldlg), TRUE);
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){
  
  GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
  
  GdkRGBA color;
  
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
  
  if( gdk_rgba_equal(&color,&P_CM_FONT_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }
  else if( gdk_rgba_equal(&color,&P_CM_BG_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }
  else if( gdk_rgba_equal(&color,&P_HR_COLOR->color ) ){
   gtk_widget_destroy (colorseldlg);
   return;
  }
  
  struct color_table *ct_tmp;
  
  if( G_HR_COLOR == P_HR_COLOR ){
   ct_tmp = G_CM_BG_COLOR;
  }
  else{
   ct_tmp = P_HR_COLOR;
  }
  
  if( !ct_tmp->next ){
   
   ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;
   ct_tmp->next->color = color;
   ct_tmp->next->next = NULL;
   
  }
  else{
  
   struct color_table *tmp = ct_tmp->next;
   
   ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;
   ct_tmp->next->color = color;
   ct_tmp->next->next = tmp;
   
  }
  
  P_CM_FONT_COLOR = ct_tmp->next;
  
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
  cairo_t *cr = cairo_create(surface);
  
  cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                             P_CM_FONT_COLOR->color.green, 
                             P_CM_FONT_COLOR->color.blue, 
                             P_CM_FONT_COLOR->color.alpha);
   
  cairo_paint(cr);
   
  gtk_image_set_from_surface (GTK_IMAGE(image), surface);
  
  if(surface)
   cairo_surface_destroy (surface);
  
  if(cr)
   cairo_destroy (cr);
  
 }
 
 gtk_widget_destroy (colorseldlg);
 
}

static void g_cm_font_changed(GtkFontButton *font_button, gpointer user_data) {
 
 if( pango_font_description_equal( G_CM_FONT_DESC, P_CM_FONT_DESC ) ){
  
  G_CM_FONT_DESC = pango_font_description_from_string (gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button)));
  
  P_CM_FONT_DESC = G_CM_FONT_DESC;
  
 }
 else{
  G_CM_FONT_DESC = pango_font_description_from_string (gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button)));
 }
 
}

static void p_cm_font_changed(GtkFontButton *font_button, gpointer user_data) {
 
 P_CM_FONT_DESC = pango_font_description_from_string (gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button)));
 
 struct list_head *tmp;
 struct note *tmp1;
 
 if( current_nc ){
  
  list_for_each(tmp, &current_nc->CM_HEAD){

   tmp1= list_entry(tmp, struct note, list);
   
   gtk_widget_queue_draw(tmp1->comment);
   
  }//list_for_each(tmp, &current_nc->CM_HEAD)
  
 }
 
 if( rcurrent_nc ){
  
  list_for_each(tmp, &rcurrent_nc->CM_HEAD){

   tmp1= list_entry(tmp, struct note, list);
   
   gtk_widget_queue_draw(tmp1->comment);
   
  }//list_for_each(tmp, &current_nc->CM_HEAD)
  
 }

}

static void pref_cb( GtkWidget *widget, gpointer user_data ){
 
 GtkWidget *pref_win = 
           gtk_dialog_new_with_buttons ("Preferences",
                                        NULL,
                                        GTK_DIALOG_MODAL,
                                        "_CANCEL",
                                        GTK_RESPONSE_CANCEL,
                                        "_OK",
                                        GTK_RESPONSE_OK,
                                        NULL);
 
 GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (pref_win));
      
 GtkWidget *pref_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
 gtk_container_add (GTK_CONTAINER (content_area), pref_box);
 
 //Global Setting
 
 GtkWidget *gframe;
 gframe = gtk_frame_new("Global Setting");
 gtk_container_set_border_width( GTK_CONTAINER(gframe), 5 );

 //font family
 
 GtkWidget *g_cm_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
 
 GtkWidget *g_cm_font_button = gtk_font_button_new_with_font(pango_font_description_to_string(G_CM_FONT_DESC));
 
 GtkWidget *g_cm_font_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);

 GtkWidget *g_cm_font_label = gtk_label_new("Font:");
  
 g_signal_connect(G_OBJECT(g_cm_font_button), "font_set",
                    G_CALLBACK(g_cm_font_changed), NULL);

 gtk_box_pack_start(GTK_BOX(g_cm_font_box), g_cm_font_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_font_box), g_cm_font_button, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_vbox), g_cm_font_box, TRUE, TRUE, 0);
 
 //font color
 
 GtkWidget *g_cm_font_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
   
 GtkWidget *g_cm_font_color_label = gtk_label_new("Font Color:");

 cairo_surface_t * surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
 cairo_t *cr = cairo_create(surface);

 cairo_set_source_rgba (cr, G_CM_FONT_COLOR->color.red, 
                            G_CM_FONT_COLOR->color.green, 
                            G_CM_FONT_COLOR->color.blue, 
                            G_CM_FONT_COLOR->color.alpha);
 
 cairo_paint(cr);
   
 GtkWidget *g_cm_font_color_button = gtk_button_new();
 
 GtkWidget *g_cm_font_color_button_Image = gtk_image_new_from_surface(surface);
 gtk_button_set_image(GTK_BUTTON(g_cm_font_color_button),g_cm_font_color_button_Image); 
   
 gtk_widget_set_can_focus(g_cm_font_color_button, FALSE);
   
 g_signal_connect(G_OBJECT(g_cm_font_color_button), "clicked",
                  G_CALLBACK(g_cm_font_color_set_cb), NULL); 
   
 gtk_box_pack_start(GTK_BOX(g_cm_font_color_box), g_cm_font_color_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_font_color_box), g_cm_font_color_button, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_vbox), g_cm_font_color_box, TRUE, TRUE, 0);
 
 //background color
 
 GtkWidget *g_cm_bg_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
 GtkWidget *g_cm_bg_color_label = gtk_label_new("Background Color:");

 if(surface){
  cairo_surface_destroy (surface);
  surface = NULL;
 }
  
 if(cr){
  cairo_destroy (cr);
  cr = NULL;
 }
   
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
 cr = cairo_create(surface);

 cairo_set_source_rgba (cr, G_CM_BG_COLOR->color.red, 
                            G_CM_BG_COLOR->color.green, 
                            G_CM_BG_COLOR->color.blue, 
                            G_CM_BG_COLOR->color.alpha);

 cairo_paint(cr);
   
 GtkWidget *g_cm_bg_color_button = gtk_button_new();
   
 GtkWidget *g_cm_bg_color_button_Image = gtk_image_new_from_surface(surface);
   
 gtk_button_set_image(GTK_BUTTON(g_cm_bg_color_button),g_cm_bg_color_button_Image); 
   
 if(surface){
  cairo_surface_destroy (surface);
  surface = NULL;
 }
  
 if(cr){
  cairo_destroy (cr);
  cr = NULL;
 }
  
 gtk_widget_set_can_focus(g_cm_bg_color_button, FALSE);
   
 g_signal_connect(G_OBJECT(g_cm_bg_color_button), "clicked",
                  G_CALLBACK(g_cm_bg_color_set_cb), NULL);
   
 gtk_box_pack_start(GTK_BOX(g_cm_bg_color_box), g_cm_bg_color_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_bg_color_box), g_cm_bg_color_button, TRUE, TRUE, 0);
   
 gtk_box_pack_start(GTK_BOX(g_cm_vbox), g_cm_bg_color_box, TRUE, TRUE, 0);
 
 //invert color
 GtkWidget *g_invert_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
 GtkWidget *ic_cbutton = gtk_check_button_new_with_label("Invert Colors");
 
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ic_cbutton),G_INVERT_COLORS);
 
 gtk_widget_set_can_focus(ic_cbutton, FALSE);
 
 g_signal_connect(G_OBJECT(ic_cbutton),"toggled",G_CALLBACK(ic_cbutton_toggled_cb),NULL);

 gtk_box_pack_start(GTK_BOX(g_invert_color_box), ic_cbutton, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(g_cm_vbox), g_invert_color_box, TRUE, TRUE, 0);
 //invert color
 
 gtk_container_add( GTK_CONTAINER(gframe), g_cm_vbox );
 
 //Global Setting
 
 GtkWidget *pframe;
 pframe = gtk_frame_new("Current PDF Setting");
 
 //font family
 
 GtkWidget *p_cm_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
 
 GtkWidget *p_cm_font_button = gtk_font_button_new_with_font(pango_font_description_to_string(P_CM_FONT_DESC));
 
 GtkWidget *p_cm_font_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);

 GtkWidget *p_cm_font_label = gtk_label_new("Font:");
  
 g_signal_connect(G_OBJECT(p_cm_font_button), "font_set",
                    G_CALLBACK(p_cm_font_changed), NULL);

 gtk_box_pack_start(GTK_BOX(p_cm_font_box), p_cm_font_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(p_cm_font_box), p_cm_font_button, TRUE, TRUE, 0);
   
 gtk_box_pack_start(GTK_BOX(p_cm_vbox), p_cm_font_box, TRUE, TRUE, 0);
 
 //font color
 
 GtkWidget *p_cm_font_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
   
 GtkWidget *p_cm_font_color_label = gtk_label_new("Font Color:");

 if(surface){
  cairo_surface_destroy (surface);
  surface = NULL;
 }
  
 if(cr){
  cairo_destroy (cr);
  cr = NULL;
 }
 
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
 cr = cairo_create(surface);
 
 cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                            P_CM_FONT_COLOR->color.green, 
                            P_CM_FONT_COLOR->color.blue, 
                            P_CM_FONT_COLOR->color.alpha);

 cairo_paint(cr);
   
 GtkWidget *p_cm_font_color_button = gtk_button_new();
 
 GtkWidget *p_cm_font_color_button_Image = gtk_image_new_from_surface(surface);
 gtk_button_set_image(GTK_BUTTON(p_cm_font_color_button),p_cm_font_color_button_Image); 
   
 gtk_widget_set_can_focus(p_cm_font_color_button, FALSE);
   
 g_signal_connect(G_OBJECT(p_cm_font_color_button), "clicked",
                  G_CALLBACK(p_cm_font_color_set_cb), NULL); 
   
 gtk_box_pack_start(GTK_BOX(p_cm_font_color_box), p_cm_font_color_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(p_cm_font_color_box), p_cm_font_color_button, TRUE, TRUE, 0);
   
 gtk_box_pack_start(GTK_BOX(p_cm_vbox), p_cm_font_color_box, TRUE, TRUE, 0);
 
 //background color
 
 GtkWidget *p_cm_bg_color_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
   
 GtkWidget *p_cm_bg_color_label = gtk_label_new("Background Color:");

 if(surface){
  cairo_surface_destroy (surface);
  surface = NULL;
 }
  
 if(cr){
  cairo_destroy (cr);
  cr = NULL;
 }
   
 surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 10);
 
 cr = cairo_create(surface);

 cairo_set_source_rgba (cr, P_CM_BG_COLOR->color.red, 
                            P_CM_BG_COLOR->color.green, 
                            P_CM_BG_COLOR->color.blue, 
                            P_CM_BG_COLOR->color.alpha);

 cairo_paint(cr);
   
 GtkWidget *p_cm_bg_color_button = gtk_button_new();
   
 GtkWidget *p_cm_bg_color_button_Image = gtk_image_new_from_surface(surface);
   
 gtk_button_set_image(GTK_BUTTON(p_cm_bg_color_button),p_cm_bg_color_button_Image); 
   
 if(surface){
  cairo_surface_destroy (surface);
  surface = NULL;
 }
  
 if(cr){
  cairo_destroy (cr);
  cr = NULL;
 }
   
 gtk_widget_set_can_focus(p_cm_bg_color_button, FALSE);
   
 g_signal_connect(G_OBJECT(p_cm_bg_color_button), "clicked",
                  G_CALLBACK(p_cm_bg_color_set_cb), NULL);
   
 gtk_box_pack_start(GTK_BOX(p_cm_bg_color_box), p_cm_bg_color_label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(p_cm_bg_color_box), p_cm_bg_color_button, TRUE, TRUE, 0);
   
 gtk_box_pack_start(GTK_BOX(p_cm_vbox), p_cm_bg_color_box, TRUE, TRUE, 0);
 
 gtk_container_add( GTK_CONTAINER(pframe), p_cm_vbox );
 
 gtk_box_pack_start(GTK_BOX(pref_box), gframe, TRUE, TRUE, 0);
 
 gtk_box_pack_start(GTK_BOX(pref_box), pframe, TRUE, TRUE, 0);
 
 gtk_widget_show_all(pref_win);

 int result = gtk_dialog_run(GTK_DIALOG(pref_win));
 
 if( result == GTK_RESPONSE_CANCEL || result == GTK_RESPONSE_DELETE_EVENT ){
  gtk_widget_destroy (pref_win);
 }
 else if( result == GTK_RESPONSE_OK ){
  gtk_widget_destroy (pref_win);
 }
 
}

static void toggle_cursor_mode_cb( GtkWidget *widget, gpointer user_data ){

 if( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(cursor_modeMi) ) ){
 
  g_timeout_add(500, (GSourceFunc) time_handler, (gpointer) win);
  time_handler(win);
 
 }

}

void toggle_dual_page_cb(GtkWidget *widget, gpointer user_data){
 
 if( doc ){
      
  gint page_num = poppler_document_get_n_pages(doc);
      
  if( page_num == 1 ){
   dual_page_mode = FALSE;
   gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
   return;
  }
      
 }
 else{
      
  if( !current_pe->prev && !current_pe->next ){
   dual_page_mode = FALSE;
   gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
   return;
  }
      
 }
 
 if(dual_page_mode)
  dual_page_mode = FALSE;
 else
  dual_page_mode = TRUE;
  
 dual_page_cb();

}

void full_screen_cb(void){
 
 if ( full_screen ){
  
  GtkAllocation sw_alloc;
  gtk_widget_get_allocation (scrolled_window, &sw_alloc);
  
  pre_sw_height = sw_alloc.height;
  
  gtk_window_fullscreen (GTK_WINDOW (win));
  gtk_widget_hide (menubar);
  
  GtkAllocation win_alloc;
  gtk_widget_get_allocation (win, &win_alloc);
  
  pre_sw_height = sw_alloc.height;
  
  if(!dual_page_mode){
   if( win_alloc.height > da_height ){
    zoom(ZOOM_HEIGHT);
   }
  } 

  full_screen = TRUE;
  
  if( dual_page_mode && dual_page_mode != 0 ){
  
   double awidth, aheight, bwidth, bheight;
  
   PopplerPage* apage = poppler_document_get_page(doc, current_page_num);
 
   poppler_page_get_size(apage, &awidth, &aheight); 
 
   apage = poppler_document_get_page(doc, current_page_num+1);
  
   poppler_page_get_size(apage, &bwidth, &bheight); 
  
   g_object_unref (G_OBJECT (apage));
   apage = NULL;
  
   if( azoom_factor < 0 ){
    ldaa.y = (screen_height  - 2 - aheight*(-azoom_factor) )/2;
    daa.y = 0;
   
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, ldaa.y);
   }
   else if( azoom_factor > 0 ){
    ldaa.y = 0;
    daa.y = (screen_height  - 2 - bheight*(azoom_factor) )/2;
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, daa.x, daa.y);
   
   }
   
  }
  
  if(current_nc)
   display_comment(&current_nc->CM_HEAD);
  
  if(dual_page_mode) 
   if( rcurrent_nc )
    display_comment(&rcurrent_nc->CM_HEAD);
 
 }
 else{ 
  
  gtk_window_unfullscreen (GTK_WINDOW (win));
  gtk_widget_show (menubar);
  
  if( dual_page_mode ){
   
   if( page_width >= page_height ){
    
    GtkAllocation da_alloc,lda_alloc;
    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
   
    GtkAllocation win_alloc;
    gtk_widget_get_allocation (win, &win_alloc);
   
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation (scrolled_window, &sw_alloc);
   
    GtkAllocation mb_alloc;
    gtk_widget_get_allocation (menubar, &mb_alloc);
   
    height_offset = (sw_alloc.height - da_height -100)/2;
    
    gint width, height;
    
    gtk_layout_get_size(GTK_LAYOUT(layout), &width, &height);
    
    if( azoom_factor == 0.0 ){
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,da_alloc.width, height_offset);
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, height_offset);
    }
    else if( azoom_factor > 0 ){
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, 0);
    }
    else{ 
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, (sw_alloc.height - lda_alloc.height -100)/2);
    }
    
   }
   
   if( azoom_factor < 0 ){
   
    GtkAllocation da_alloc,lda_alloc;
    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
    
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area,0, (sw_alloc.height - lda_alloc.height -100)/2);
    
    ldaa.y = (screen_height  - 2 - 100 - lda_alloc.height )/2;
    daa.y = 0;
     
   }
   else if ( azoom_factor > 0 ){
    
    GtkAllocation da_alloc,lda_alloc;
    gtk_widget_get_allocation (draw_area, &da_alloc);
    gtk_widget_get_allocation (ldraw_area, &lda_alloc);
    
    GtkAllocation sw_alloc;
    gtk_widget_get_allocation (scrolled_window, &sw_alloc);
    
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,lda_alloc.width, (sw_alloc.height - da_alloc.height -100)/2);
    
    ldaa.y = 0;
    daa.y = (screen_height - 100 - 2 - da_alloc.height )/2;
    
   }
   
   if( rcurrent_nc )
    display_comment(&rcurrent_nc->CM_HEAD);
   
  }else if( !dual_page_mode ){
   zoom(ZOOM_WIDTH);
  }
  
  if(current_nc)
   display_comment(&current_nc->CM_HEAD);
  
  full_screen = FALSE;
  
  pre_sw_height = 0;
  
 }
 
}

static gboolean toggle_full_screen_cb(GtkWidget *widget, gpointer user_data){
 
 if(!full_screen){
  full_screen = TRUE;
 }
 else{
  full_screen = FALSE;
 }
 
 full_screen_cb();
 
 return TRUE;
 
}

static void inverted_color_cb(void){
 
 if( mode == TEXT_SELECTION ){
  
  if( invert_color ){ 
  
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

 if( dual_page_mode ){
  
  //left page
  page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);
  
  //right page
  page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc); 
  
 }  
 else{
 
  page_setup(&surface, &da_selection_region, draw_area, current_nc);
  
 }
 
}

static void toggle_page_comment_cb(GtkWidget *widget, gpointer user_data){

 gboolean pc = gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(page_commentMi) );
 
 if ( pc ){
  
  GdkScreen *screen = gdk_screen_get_default ();
 
  gtk_paned_set_position (GTK_PANED(paned), gdk_screen_get_height(screen)*0.8);
  
  gtk_widget_show(tview);
  gtk_widget_grab_focus(tview);
  gtk_widget_show(tsw);
  
  add_page_comment();
 
 }
 else{
  gtk_widget_hide(tview);
  gtk_widget_hide(tsw);
  gtk_widget_grab_focus(scrolled_window);
 }
 
}

static void toggle_inverted_color_cb(GtkWidget *widget, gpointer user_data){
 
 if(invert_color)
  invert_color = FALSE;
 else
  invert_color = TRUE;
  
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

/*
///////////////////////////////
void toggle_hide_toolbar(void){
 
 if( TOOL_BAR_VISIBLE ){
  
  gtk_widget_hide(toolbar);
  TOOL_BAR_VISIBLE = FALSE;
    
 } else {
    
  gtk_widget_show(toolbar);
  TOOL_BAR_VISIBLE = TRUE;
    
 }
 
 if( screen_height > (int)(page_height*zoom_factor) ){
  zoom(ZOOM_HEIGHT);
 }
    
}
///////////////////////////////
*/

void copy_text(void){

 if (selection_surface){
  
  GtkClipboard *clipboard = 
     gtk_clipboard_get_for_display(gdk_display_get_default(),
                                   GDK_SELECTION_CLIPBOARD);
     
  gtk_clipboard_set_text (clipboard, selected_text->str, -1);
 
 }
        
}

static void on_saveas_cb(GtkWidget *widget, gpointer user_data) {
 
 save_as();
 
}

static void on_open_file_cb(GtkWidget *widget, gpointer user_data) {
 
 /*
 //////////////////////////////////////////////////
  //for opening new file
  GtkWidget *dialog;
  
  dialog = gtk_file_chooser_dialog_new("Open file", NULL,
             GTK_FILE_CHOOSER_ACTION_OPEN, 
	     "_Cancel", GTK_RESPONSE_CANCEL,
	     "_Open", GTK_RESPONSE_ACCEPT, 
	     NULL);
  
  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if(res == GTK_RESPONSE_ACCEPT){
   GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
   gchar *file_name = gtk_file_chooser_get_filename(chooser);
   init_pdf(file_name);
   g_free(file_name);
  }
  else if ( res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT)
   exit(1);
   
  gtk_widget_destroy(dialog);
  //for opening new file
  //////////////////////////////////////////////////
  */
 
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

/*
/////////////////////
static void toggle_hide_toolbar_cb(GtkWidget *widget, gpointer user_data) {
 
 if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
 
  TOOL_BAR_VISIBLE = TRUE;
  toggle_hide_toolbar();
  
 }
 else{
  
  TOOL_BAR_VISIBLE = FALSE;
  
 }
 
}
/////////////////////
*/

static void color_set(GtkWidget *widget, int option){
 
 GtkWidget *colorseldlg;
 
 if(option == 1)
  colorseldlg = gtk_color_chooser_dialog_new ("Select Highlight Color", NULL);
 else if(option == 2)
  colorseldlg = gtk_color_chooser_dialog_new ("Select Background Color", NULL);
 else
  return;
 
 gint response = gtk_dialog_run (GTK_DIALOG (colorseldlg));
 
 if(response == GTK_RESPONSE_OK){ 
  
  GdkRGBA color;
  
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (colorseldlg), &color);
  
  if(option == 1){ //change highlight color
  
   text_highlight_mode_change();
   
   cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 20);
 
   cairo_t *cr = cairo_create(surface);

   cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha);
   
   cairo_paint(cr);
   
   if( widget ){
    GtkWidget *image = gtk_button_get_image (GTK_BUTTON(widget));
    gtk_image_set_from_surface (GTK_IMAGE(image), surface);
   }
   
   if(surface)
    cairo_surface_destroy (surface);
  
   if(cr)
    cairo_destroy (cr);
   
   if( gdk_rgba_equal(&color,&P_CM_FONT_COLOR->color ) ){
    gtk_widget_destroy (colorseldlg);
    return;
   }
   else if( gdk_rgba_equal(&color,&P_CM_BG_COLOR->color ) ){
    gtk_widget_destroy (colorseldlg);
    return;
   }
   else if( gdk_rgba_equal(&color,&P_HR_COLOR->color ) ){
    gtk_widget_destroy (colorseldlg);
    return;
   }
   
   struct color_table *ct_tmp;
   
   ct_tmp = G_CM_BG_COLOR;
   
   if( !ct_tmp->next ){
    
    ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
    ct_tmp->next->rc = 1;
    ct_tmp->next->color = color;
    ct_tmp->next->next = NULL;
    
   }
   else{
   
    struct color_table *tmp = ct_tmp->next;
   
    ct_tmp->next =  (struct color_table *)malloc(sizeof(struct color_table));
    ct_tmp->next->rc = 1;
    ct_tmp->next->color = color;
    ct_tmp->next->next = tmp;
    
   }
  
   P_HR_COLOR = ct_tmp->next;
   
  }
  else if(option == 2){ //change background color
   
   background_color[0] = color.red;
   background_color[1] = color.green;
   background_color[2] = color.blue;
   
   PDF_BACKGROUND_COLOR_CHANGED = TRUE;
   gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(change_background_colorMi), TRUE);
   page_change();
   
  }
  
  /*
  da_background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (color.green* 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (color.blue* 65535), 0, 65535);
  
  da_background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
  da_background_color.green = CLAMP ((guint) (color.green* 65535), 0, 65535);
  da_background_color.blue = CLAMP ((guint) (color.blue* 65535), 0, 65535);
  */
  
 }
 
 gtk_widget_destroy (colorseldlg);
    
}

static void color_set_cb(GtkWidget *widget, gpointer user_data){
 color_set(widget, 1);
}

gboolean draw_area_draw( GtkWidget *widget, cairo_t *cr, gpointer data ){

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
 
 if( widget == draw_area ){
  
  cairo_set_line_width(cr, 2.0);

  cairo_set_source_rgb (cr, 0.827, 0.827, 0.827);

  cairo_move_to(cr, 0, 0);
  
  cairo_line_to(cr, 0, page_height*zoom_factor);
  
  cairo_stroke(cr);
  
 }
 
 /*
 ////////////////
 {
 //print page number for blank page
 if( current_bp || rcurrent_bp ){
  
  cairo_move_to(cr, 100, 100);
  
  PangoLayout *layout;
 
  layout = pango_cairo_create_layout (cr);


  PangoFontDescription *tmp_font_desc = pango_font_description_copy(P_CM_FONT_DESC); 
  
  pango_font_description_set_absolute_size (tmp_font_desc, pango_font_description_get_size(tmp_font_desc)*zoom_factor);

  pango_layout_set_font_description (layout, tmp_font_desc);
  pango_font_description_free (tmp_font_desc);
  
  char pn[1];
  
  if(widget == draw_area)
   sprintf(pn, "%d",rcurrent_bp->page_num);
  else
   sprintf(pn, "%d",current_bp->page_num);
   
  pango_layout_set_text (layout, pn, -1);

  cairo_set_source_rgba (cr, P_CM_FONT_COLOR->color.red, 
                             P_CM_FONT_COLOR->color.green, 
                             P_CM_FONT_COLOR->color.blue, 
                             P_CM_FONT_COLOR->color.alpha);

  
  cairo_move_to (cr, 0.0, 0.0);
  pango_cairo_show_layout (cr, layout); 
  
 }
 //print page number for blank page
 }
 //////////////////
 */
 
 return TRUE;
 
}

static gboolean
selections_render( GtkWidget *widget , double zoom_factor){

 PopplerRectangle doc_area;
 
 if (selection_surface)
  cairo_surface_destroy (selection_surface);
 
 cairo_t *cr;
 
 doc_area.x1 = start_x/zoom_factor;
 doc_area.y1 = start_y/zoom_factor;
 doc_area.x2 = stop_x/zoom_factor;
 doc_area.y2 = stop_y/zoom_factor;
 
 selection_surface = cairo_image_surface_create ( CAIRO_FORMAT_ARGB32,
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
tsw_button_press (GtkWidget *widget, GdkEventButton *event, gpointer data){
 
 if( !gtk_widget_is_focus(tsw) ){
  
  gtk_widget_set_can_focus(tsw, TRUE);
  gtk_widget_set_can_focus(tview, TRUE);
  
  gtk_widget_grab_focus(tview);
  
 }
 
}

static gboolean
da_button_press (GtkWidget *widget, GdkEventButton *event, gpointer data){
 
 if( !blank_page ){
  start_x = event->x;
  start_y = event->y;
 
  stop_x = event->x;
  stop_y = event->y;
 }
 else{
  start_x = start_y = stop_x = stop_y= -1.0;
 }
 
 if(selection_surface){
 
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
 
 if(!gtk_widget_is_focus(scrolled_window)){
  gtk_widget_grab_focus(scrolled_window);
 }
 
 //-----
 //if( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(page_commentMi) ) ){
 // gtk_widget_set_can_focus(tsw, FALSE);
 // gtk_widget_set_can_focus(tview, FALSE);
 //}
 //-----
 
 return TRUE;

}

static gboolean
da_motion_notify (GtkWidget *widget, GdkEventMotion *event, gpointer data){
 
 if( start_x != -1.0){
  
  gdouble page_width, page_height;
  
  stop_x = event->x;
  stop_y = event->y;
  
  if(!dual_page_mode)
   selections_render( widget, zoom_factor );
  else{
   
   if( widget == ldraw_area ){
    
    if( azoom_factor == 0.0 )
     selections_render( widget, zoom_factor );
    else if ( azoom_factor < 0 )// left page
     selections_render( widget, -azoom_factor );
    else
     selections_render( widget, zoom_factor );
     
   }
   else{
    
    if( azoom_factor == 0.0 )
     selections_render( widget, zoom_factor );
    else if ( azoom_factor < 0 )// left page
     selections_render( widget, zoom_factor );
    else
     selections_render( widget, azoom_factor );
    
   }
    
  }
   
 }
 else{
  
  if( !blank_page ){
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
  
 }
 
 return TRUE;
 
}

static gboolean
da_button_release (GtkWidget *widget, GdkEventButton *event, gpointer data){
 
 if(start_y == -1.0)
  return FALSE;
 else if( start_y == stop_y && start_x == stop_x ){
  start_x = start_y = stop_x = stop_y= -1.0;
  return FALSE;
 }
 
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
 
  double tmp_zf;
 
  if( !dual_page_mode )
   tmp_zf = zoom_factor;
  else{
  
   if( azoom_factor == 0.0 ){
    tmp_zf = zoom_factor;
   }
   else{
   
    if( widget == ldraw_area ){
    
     if( azoom_factor < 0 ){ //left page
      tmp_zf = -azoom_factor;
     }
     else{
      tmp_zf = zoom_factor;
     }
    
    }
    else{
    
     if( azoom_factor < 0 ){ //left page
      tmp_zf = zoom_factor;
     }
     else{
      tmp_zf = azoom_factor;
     }
    
    }
   
   }
  
  }
  
  for ( ; NULL != selection ; selection = g_list_next (selection)) {
  
   rectangle = (PopplerRectangle *)selection->data;
  
   tmp_hr->x = (int)(rectangle->x1*tmp_zf+0.5);
   tmp_hr->y = (int)(rectangle->y1*tmp_zf+0.5);
   tmp_hr->width = (int)((rectangle->x2-rectangle->x1)*tmp_zf+0.5);
   tmp_hr->height = (int)((rectangle->y2-rectangle->y1)*tmp_zf+0.5);
  
   tmp_hr++;
   
  }
 
  if( mode == TEXT_HIGHLIGHT ){
   
   //i054 begin
   
   //get current_nc
   if( !current_nc ){
   
    struct note_cache *tmp = note_cache->next;
    
    struct note_cache *min = note_cache;

    while( tmp ){
    
     if( current_page_num+1 < tmp->page_num ){
    
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
    tmp->page_num = current_page_num + 1;
   
    tmp->BP_HEAD = NULL;
    tmp->BP_TAIL = NULL;
   
    current_nc = tmp;
    prev_nc = current_nc->prev;
    current_pe->curr_nc = tmp;
    
   }
   //get current_nc
   
   if( !dual_page_mode ){ 
    
    save_highlight( widget, &current_nc->HR_HEAD, zoom_factor);
   }
   else{
   
    if( widget == ldraw_area ){
    
     if( azoom_factor == 0.0 )
      save_highlight( widget, &current_nc->HR_HEAD, zoom_factor);
     else if( azoom_factor < 0 )//left page
      save_highlight( widget, &current_nc->HR_HEAD, -azoom_factor);
     else
      save_highlight( widget, &current_nc->HR_HEAD, zoom_factor);
    }
    else if( widget == draw_area ){
   
     //get rcurrent_nc
     if( !rcurrent_nc ){
   
      struct note_cache *tmp = note_cache->next;
      
      struct note_cache *min = note_cache;

      while( tmp ){
    
       if( current_page_num+2 < tmp->page_num ){
    
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
      tmp->page_num = current_page_num + 2;
     
      tmp->BP_HEAD = NULL;
      tmp->BP_TAIL = NULL;
     
      rcurrent_nc = tmp;
      
      rcurrent_pe->curr_nc = tmp;
      
     }
     //get rcurrent_nc
     
     if( azoom_factor == 0.0 )
      save_highlight( widget, &rcurrent_nc->HR_HEAD, zoom_factor);
     else if( azoom_factor < 0 )//left page
      save_highlight( widget, &rcurrent_nc->HR_HEAD, zoom_factor);
     else
      save_highlight( widget, &rcurrent_nc->HR_HEAD, azoom_factor);
    }
   
   }
  
   if(hr){
    free(hr);
    hr = NULL;
   }
  
  }
  else if( mode == ERASE_TEXT_HIGHLIGHT ){
  
   if( !dual_page_mode ){ 
   
    if( current_nc )
     erase_highlight( widget, &current_nc->HR_HEAD, tmp_zf );
  
   }
   else{
   
    if( widget == ldraw_area ){
    
     if( current_nc )
      erase_highlight( widget, &current_nc->HR_HEAD, tmp_zf );
    
    }
    else if( widget == draw_area ){
    
     if( rcurrent_nc )
      erase_highlight( widget, &rcurrent_nc->HR_HEAD, tmp_zf );
    
    }
   
   }
  
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
  
  #ifdef __linux__
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

#ifdef __APPLE__
static gboolean win_state_event_cb( GtkWidget *widget, GdkEventWindowState *event, gpointer user_data ){
 
 if( event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN ){
  
  if(!full_screen)
   full_screen = TRUE;
  else
   full_screen = FALSE;
  
  full_screen_cb();
  
 }
 else if( (event->new_window_state & GDK_WINDOW_STATE_FOCUSED) && full_screen ){
  
  full_screen = FALSE;
  
  full_screen_cb();
  
 }
 
 return TRUE;
}
#endif

static void tview_draw(){
 
 cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(tview));
 cairo_set_source_rgb (cr,0,1,1);
 cairo_paint (cr);
 
 cairo_destroy(cr);
 
}

static void
tbuffer_changed_cb(GtkTextBuffer *buffer, gpointer user_data){
 
 GtkTextIter start,end;
 
 gtk_text_buffer_get_bounds (buffer, &start, &end);
 
 gtk_text_buffer_apply_tag_by_name(buffer, "white_fg", &start, &end);
 
}

static gboolean tview_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data){
 
 cairo_set_source_rgb (cr,0,1,1);
 cairo_paint (cr);
 
 cairo_set_source_rgb(cr, 1, 1, 0);
 cairo_set_line_width(cr, 5);
 
 cairo_move_to(cr,50,50);
 cairo_line_to(cr,80,80);
 cairo_stroke(cr);
 
 return FALSE;
 
}

gboolean
tbuffer_cursor_position_changed_cb(GtkTextBuffer *buffer){
 
 gint pos;
 g_object_get (buffer, "cursor-position", &pos, NULL);
 
 if(pos>0){
  
  GtkTextIter siter,eiter;
  
  gtk_text_buffer_get_iter_at_offset (buffer,&eiter,pos);
 
 
  gtk_text_buffer_get_iter_at_offset (buffer,&siter,pos-1);
  
  if( sp != ep ){
   
   GtkTextIter tp_siter, tp_eiter;
   
   gtk_text_buffer_get_iter_at_offset (buffer, &tp_siter,sp);
   gtk_text_buffer_get_iter_at_offset (buffer, &tp_eiter,ep);
   
   gtk_text_buffer_remove_tag_by_name (buffer,
                                    "black_fg",
                                    &tp_siter, &tp_eiter);
   
   gtk_text_buffer_remove_tag_by_name (buffer,
                                    "white_bg",
                                    &tp_siter, &tp_eiter);
   
  }
   
  sp = pos-1;
  ep = pos;
  
  gtk_text_buffer_apply_tag_by_name (buffer,"black_fg", &siter,&eiter);
  gtk_text_buffer_apply_tag_by_name (buffer,"white_bg", &siter,&eiter);
  
 }
 else{
  
  if( sp == 0 && ep == 1 ){
   
   GtkTextIter tp_siter, tp_eiter;
   
   gtk_text_buffer_get_iter_at_offset (buffer, &tp_siter,sp);
   gtk_text_buffer_get_iter_at_offset (buffer, &tp_eiter,ep);
   
   gtk_text_buffer_remove_tag_by_name (buffer,
                                    "black_fg",
                                    &tp_siter, &tp_eiter);
   
   gtk_text_buffer_remove_tag_by_name (buffer,
                                    "white_bg",
                                    &tp_siter, &tp_eiter);
   
  }
  
 }
 
 return FALSE;
 
}

static void on_swipe_cb(GtkGestureSwipe *gesture,
               gdouble          velocity_x,
               gdouble          velocity_y,
               gpointer         user_data){
 
 g_message("swipe");
 
}

void init_gui(void){
 
 GdkScreen *screen = gdk_screen_get_default ();
 
 screen_height = gdk_screen_get_height(screen);
 screen_width = gdk_screen_get_width(screen);
 
 current_page_num = -1;
 
 mode = pre_mode = TEXT_SELECTION;
 
 KEY_BUTTON_SEARCH = TRUE;
 
 hr = ihr = NULL;

 cursor_pos =  0;
 pre_cursor_pos = -1;
 
 pre_sw_height = 0;
 
 bindex = 0;
 
 full_screen = FALSE;
  
 current_cm = NULL;
 
 width_offset = 0;
 
 left_right = 0;
 
 word_not_found = 1;

 find_ptr = find_ptr_head = NULL;
 lmatches = rmatches = NULL;
 page_change_str = NULL;

 selected_text = NULL;
 
 start_x = start_y = stop_x = stop_y= -1.0;
 
 pre_keyval = GDK_KEY_Escape;
 
 da_cursor = GDK_LAST_CURSOR;
    
 scroll_count = 0; 
 scroll_time = 0;
 scroll_zoom = -1;
 
 delta_t = 0;
 delta_y = 0.0;
 
 tview = NULL;
 
 layout = NULL;
 
 azoom_factor = 0.0;
 
 comment_buffer_changed = FALSE;
 
 sp = ep = 0;
 
 accel_group = gtk_accel_group_new();
 
 win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 
 gtk_window_add_accel_group(GTK_WINDOW(win), accel_group);
 
 //*****
 //cursor_enable = TRUE;
 //*****
 
 g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(on_destroy), NULL);
 //g_signal_connect(G_OBJECT(win), "swipe", G_CALLBACK(on_swipe_cb), NULL);
 
 #ifdef __APPLE__
 g_signal_connect(G_OBJECT(win), "window-state-event", G_CALLBACK(win_state_event_cb), NULL);
 #endif
 
 paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
 
 tview = gtk_text_view_new();
 
 /*****reserved
 g_signal_connect (G_OBJECT (tview), "draw", 
                   G_CALLBACK (tview_draw_cb), 
                   NULL);
 *****reserved*/
 
 GtkCssProvider* Provider = gtk_css_provider_new();
 GdkDisplay* Display = gdk_display_get_default();
 GdkScreen* Screen = gdk_display_get_default_screen(Display);

 gtk_style_context_add_provider_for_screen(Screen, GTK_STYLE_PROVIDER(Provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
 
 gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(Provider), dark_theme, -1, NULL);

 
 /*reserved
 GtkStyleContext *Context;
 Context = gtk_widget_get_style_context(textview);
 gtk_style_context_add_class(Context, "textview");
 reserved*/
 
 tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tview));
 
 /*****reserved
 g_signal_connect(G_OBJECT( tbuffer ), "changed",
     G_CALLBACK(tbuffer_changed_cb), NULL);
 
 g_signal_connect (G_OBJECT( tbuffer ), "notify::cursor-position", 
     G_CALLBACK (tbuffer_cursor_position_changed_cb), NULL);
 ******reserved*/
 
 vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
 
 tsw = gtk_scrolled_window_new(NULL,NULL);
 
 g_signal_connect (G_OBJECT (tsw), "button-press-event",
	           G_CALLBACK (tsw_button_press),
		   NULL);
 
 gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tsw),
                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
 gtk_container_set_border_width(GTK_CONTAINER(tsw), 1);

 gtk_container_add(GTK_CONTAINER(tsw), tview);
 
 gtk_paned_add1 (GTK_PANED (paned), vbox);
 gtk_paned_add2 (GTK_PANED (paned), tsw);

 gtk_container_add(GTK_CONTAINER(win), paned);
 
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
 
 //////////Menubar Initialization//////////
 
 menubar = gtk_menu_bar_new();
 
 fileMenu = gtk_menu_new();
 editMenu = gtk_menu_new();
 viewMenu = gtk_menu_new();
    
 goMenu = gtk_menu_new();
 modeMenu = gtk_menu_new();
 noteMenu = gtk_menu_new();

 fileMi = gtk_menu_item_new_with_label("File");
 
 saveasMi = gtk_menu_item_new_with_label("Save as");
 g_signal_connect(G_OBJECT(saveasMi), "activate",
     G_CALLBACK(on_saveas_cb), NULL);
 
 //////////////
 //openMi = gtk_menu_item_new_with_label("Open File");
 //g_signal_connect(G_OBJECT(openMi), "activate",
 //    G_CALLBACK(on_open_file_cb), NULL);
 /////////////
 
 quitMi = gtk_menu_item_new_with_label("Quit");
 g_signal_connect(G_OBJECT(quitMi), "activate",
     G_CALLBACK(on_destroy), NULL);
 
 gtk_widget_add_accelerator(quitMi, "activate", accel_group,
                     GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
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
 gtk_widget_add_accelerator(zoominMi, "activate", accel_group,
                     GDK_KEY_equal, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 
 zoomoutMi = gtk_menu_item_new_with_label("Zoom Out");
 g_signal_connect(G_OBJECT(zoomoutMi), "activate",
     G_CALLBACK(zoom_out_cb), NULL);
 gtk_widget_add_accelerator(zoomoutMi, "activate", accel_group,
                     GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   
 zoomwidthMi = gtk_menu_item_new_with_label("Zoom Width");
 g_signal_connect(G_OBJECT(zoomwidthMi), "activate",
     G_CALLBACK(zoom_width_cb), NULL);
 gtk_widget_add_accelerator(zoomwidthMi, "activate", accel_group,
                     GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
 zoomheightMi = gtk_menu_item_new_with_label("Zoom Height");
 g_signal_connect(G_OBJECT(zoomheightMi), "activate",
     G_CALLBACK(zoom_height_cb), NULL);
 gtk_widget_add_accelerator(zoomheightMi, "activate", accel_group,
                     GDK_KEY_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 
 //hide_toolbarMi = gtk_check_menu_item_new_with_label("Hide Toolbar");
 //g_signal_connect(G_OBJECT(hide_toolbarMi), "activate", 
 //       G_CALLBACK(toggle_hide_toolbar_cb), NULL);
 
 change_background_colorMi = gtk_check_menu_item_new_with_label("Change Background Color");
 g_signal_connect(G_OBJECT(change_background_colorMi), "activate", 
        G_CALLBACK(change_background_color_cb), NULL);

 dual_pageMi = gtk_check_menu_item_new_with_label("Dual Page");
 g_signal_connect(G_OBJECT(dual_pageMi), "activate", 
        G_CALLBACK(toggle_dual_page_cb), NULL);
 gtk_widget_add_accelerator(dual_pageMi, "activate", accel_group,
                     GDK_KEY_d, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 full_screenMi = gtk_check_menu_item_new_with_label("Full Screen");
 g_signal_connect(G_OBJECT(full_screenMi), "activate", 
        G_CALLBACK(toggle_full_screen_cb), NULL);
 gtk_widget_add_accelerator(full_screenMi, "activate", accel_group, 
GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
 
 inverted_colorMi = gtk_check_menu_item_new_with_label("Invert Colors");
 g_signal_connect(G_OBJECT(inverted_colorMi), "activate", 
        G_CALLBACK(toggle_inverted_color_cb), NULL);
 gtk_widget_add_accelerator(inverted_colorMi, "activate", accel_group,
                     GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 //-----
 //page_commentMi = gtk_check_menu_item_new_with_label("Page Comment");
 //g_signal_connect(G_OBJECT(page_commentMi), "activate", 
 //       G_CALLBACK(toggle_page_comment_cb), NULL);
 //gtk_widget_add_accelerator(page_commentMi, "activate", accel_group,
 //                    GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 //-----
 
 /////////////////      
 //cursor_modeMi = gtk_check_menu_item_new_with_label("Cursor mode");
 //g_signal_connect(G_OBJECT(cursor_modeMi), "activate", 
 //       G_CALLBACK(toggle_cursor_mode_cb), NULL);
 
 //continuous_modeMi = gtk_check_menu_item_new_with_label("Continuous mode");
 //g_signal_connect(G_OBJECT(continuous_modeMi), "activate", 
 //       G_CALLBACK(toggle_continuous_mode_cb), NULL);
 ////////////////
 
 goMi = gtk_menu_item_new_with_label("Go");
 
 nextpageMi = gtk_menu_item_new_with_label("Next Page");
 g_signal_connect( G_OBJECT(nextpageMi), "activate",
                   G_CALLBACK(next_page_cb), NULL);   
 gtk_widget_add_accelerator(nextpageMi, "activate", accel_group,
                     GDK_KEY_Page_Down, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 prepageMi = gtk_menu_item_new_with_label("Previous Page");
 g_signal_connect( G_OBJECT(prepageMi), "activate",
                   G_CALLBACK(prev_page_cb), NULL);
 gtk_widget_add_accelerator(prepageMi, "activate", accel_group,
                     GDK_KEY_Page_Up, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 modeMi = gtk_menu_item_new_with_label("Mode");
    
 text_selectionMi = gtk_menu_item_new_with_label("Text Selection Mode");
 g_signal_connect( G_OBJECT(text_selectionMi), "activate",
                   G_CALLBACK(text_selection_mode_cb), NULL);

 text_highlightMi = gtk_menu_item_new_with_label("Text Highlight Mode");
 g_signal_connect( G_OBJECT(text_highlightMi ), "activate",
                   G_CALLBACK(text_highlight_mode_cb), NULL);
 
 erase_text_highlightMi = gtk_menu_item_new_with_label("Erase Text Highlight Mode");
 g_signal_connect( G_OBJECT( erase_text_highlightMi ), "activate",
                   G_CALLBACK(erase_text_highlight_mode_cb), NULL);
 
 noteMi = gtk_menu_item_new_with_label("Note");
 
 add_commentMi = gtk_menu_item_new_with_label("Add A Comment");
 g_signal_connect( G_OBJECT( add_commentMi ), "activate",
                   G_CALLBACK(add_comment_cb), NULL);
 gtk_widget_add_accelerator(add_commentMi, "activate", accel_group,
                     GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 insert_bp_beforeMi = gtk_menu_item_new_with_label("Insert A Blank Page Before Current Page");
 g_signal_connect( G_OBJECT( insert_bp_beforeMi ), "activate",
                   G_CALLBACK(insert_bp_before_cb), NULL);
 gtk_widget_add_accelerator(insert_bp_beforeMi, "activate", accel_group,
                     GDK_KEY_b, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
 
 insert_bp_afterMi = gtk_menu_item_new_with_label("Insert A Blank Page After Current Page");
 g_signal_connect( G_OBJECT( insert_bp_afterMi ), "activate",
                   G_CALLBACK(insert_bp_after_cb), NULL);
 gtk_widget_add_accelerator(insert_bp_afterMi, "activate", accel_group,
                     GDK_KEY_b, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
        
 del_blank_pageMi = gtk_menu_item_new_with_label("Delete Current Blank Page");
 g_signal_connect( G_OBJECT( del_blank_pageMi ), "activate",
                   G_CALLBACK(del_blank_page_cb), NULL);
 gtk_widget_add_accelerator(del_blank_pageMi, "activate", accel_group,
                     GDK_KEY_d, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
 
 save_noteMi = gtk_menu_item_new_with_label("Save Notes");
 g_signal_connect( G_OBJECT( save_noteMi ), "activate",
                   G_CALLBACK(save_note_cb), NULL);
 gtk_widget_add_accelerator(save_noteMi, "activate", accel_group,
                     GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
 save_commentMi = gtk_menu_item_new_with_label("Save Comments");
 g_signal_connect( G_OBJECT( save_commentMi ), "activate",
                   G_CALLBACK(save_comment_cb), NULL);
 
 prefMi = gtk_menu_item_new_with_label("Preferences");
 g_signal_connect( G_OBJECT(prefMi), "activate",
                   G_CALLBACK(pref_cb), NULL);
 
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
 //gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveasMi);
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
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), hide_toolbarMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), change_background_colorMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), dual_pageMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), full_screenMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), inverted_colorMi);
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), page_commentMi);
 
 //////////
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), cursor_modeMi);
 //gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), continuous_modeMi);
 /////////
 
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), viewMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(goMi), goMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(goMenu), prepageMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(goMenu), nextpageMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), goMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(modeMi), modeMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), text_selectionMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), text_highlightMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(modeMenu), erase_text_highlightMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), modeMi);

 gtk_menu_item_set_submenu(GTK_MENU_ITEM(noteMi), noteMenu);
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), add_commentMi);
 //gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), add_blank_pageMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), insert_bp_beforeMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), insert_bp_afterMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), del_blank_pageMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), save_noteMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), save_commentMi);
 
 gtk_menu_shell_append(GTK_MENU_SHELL(noteMenu), prefMi);
 gtk_menu_shell_append(GTK_MENU_SHELL(menubar), noteMi);
 
 gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
 
 //////////Menubar Initialization//////////
 
 toolbar = gtk_toolbar_new();
 gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
 
 TOOL_BAR_VISIBLE = FALSE;
 
 GtkWidget *toolbar_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
 
 gtk_box_set_spacing(GTK_BOX(toolbar_box), 1);
 
 hc_button = gtk_button_new();
 
 cairo_surface_t * hc_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 20);
 
 cairo_t *hc_cr = cairo_create(hc_surface);
 
 cairo_set_source_rgba (hc_cr, G_HR_COLOR->color.red, 
                               G_HR_COLOR->color.green, 
                               G_HR_COLOR->color.blue, 
                               G_HR_COLOR->color.alpha);

 cairo_paint(hc_cr);
 
 hc_button_Image = gtk_image_new_from_surface(hc_surface);
 
 gtk_button_set_image(GTK_BUTTON(hc_button),hc_button_Image); 
 
 gtk_widget_set_can_focus(hc_button, FALSE);
 
 g_signal_connect(G_OBJECT(hc_button), "clicked",
                  G_CALLBACK(color_set_cb), NULL); 
 
 gtk_widget_set_can_focus(hc_button, FALSE);
 
 gtk_container_add (GTK_CONTAINER (toolbar_box), hc_button);
 
 if(hc_surface)
  cairo_surface_destroy (hc_surface);
  
 if(hc_cr)
  cairo_destroy (hc_cr);
 
 //hc_color
 
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
        G_CALLBACK(zoom_in_cb), NULL);
 
 zoom_out_button = gtk_button_new_from_icon_name ("zoom-out", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(zoom_out_button, FALSE);
 g_signal_connect(G_OBJECT(zoom_out_button), "clicked", 
        G_CALLBACK(zoom_out_cb), NULL);
 
 zoom_width_button = gtk_button_new_from_icon_name ("zoom-fit-best", GTK_ICON_SIZE_BUTTON);
 gtk_widget_set_can_focus(zoom_width_button, FALSE);
 g_signal_connect(G_OBJECT(zoom_width_button), "clicked", 
        G_CALLBACK(zoom_width_cb), NULL);
 
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
        G_CALLBACK(add_comment_cb), NULL);
 
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
 
 if(!layout){
 
  draw_area = gtk_drawing_area_new();
  ldraw_area = gtk_drawing_area_new();
  
  da_width = (gint)((page_width*zoom_factor)+0.5);
  da_height = (gint)((page_height*zoom_factor)+0.5);
 
  pre_da_width = da_width;
 
  gtk_widget_set_size_request (draw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));
  
  gtk_widget_set_size_request (ldraw_area, (gint)((page_width*zoom_factor)+0.5), (gint)((page_height*zoom_factor)+0.5));
 
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
  
  gtk_layout_set_size(GTK_LAYOUT(layout), width, height);
 
  gtk_layout_put (GTK_LAYOUT (layout), ldraw_area, 0, 0);
  gtk_layout_put (GTK_LAYOUT (layout), draw_area, 0, 0);
  
 }
 
 gtk_widget_add_events (draw_area, GDK_POINTER_MOTION_MASK | 
                                   GDK_BUTTON_MOTION_MASK | 
                                   GDK_BUTTON_PRESS_MASK | 
                                   GDK_BUTTON_RELEASE_MASK |
                                   GDK_SCROLL_MASK |
                                   GDK_SMOOTH_SCROLL_MASK |
                                   GDK_TOUCHPAD_GESTURE_MASK );
 
 gtk_widget_add_events (ldraw_area, GDK_POINTER_MOTION_MASK | 
                                    GDK_BUTTON_MOTION_MASK | 
                                    GDK_BUTTON_PRESS_MASK | 
                                    GDK_BUTTON_RELEASE_MASK | 
                                    GDK_SCROLL_MASK );
 
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
 
 //-----
 //g_signal_connect(G_OBJECT (draw_area), "scroll-event", 
 //                 G_CALLBACK(da_scroll_cb),
 //                 NULL);
 //                 
 //g_signal_connect(G_OBJECT (ldraw_area), "scroll-event", 
 //                 G_CALLBACK(da_scroll_cb),
 //                 NULL);
 //-----
                  
 g_signal_connect(G_OBJECT (draw_area), "touch-event", 
                  G_CALLBACK(da_touch_cb),
                  NULL);
 
 gtk_container_add(GTK_CONTAINER(scrolled_window), layout);
   
 g_signal_connect(scrolled_window, "key-press-event", G_CALLBACK(scrolled_window_keypress_cb), findbar);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 pre_sw_width = sw_alloc.width;

 gtk_widget_show_all(win);
 
 init_note();
 
 if(!blank_page)
  page_change();
 else{
  page_setup(&surface, NULL, draw_area, NULL);
  display_comment(NULL);
 }
 
 if( dual_page_mode && invert_color ){
  
  if(dual_page_mode){
   dual_page_mode = FALSE;
  }else
   dual_page_mode = TRUE;
    
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), TRUE );
  
  if( mode == TEXT_SELECTION ){
  
    da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
    da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
    da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
  
    da_background_color.red = CLAMP ((guint) (1* 65535), 0, 65535);
    da_background_color.green = CLAMP ((guint) (1 * 65535), 0, 65535);
    da_background_color.blue = CLAMP ((guint) (1 * 65535), 0, 65535);
   
  }
 
  if(invert_color)
   invert_color = FALSE;
  else
   invert_color = TRUE;
  
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(inverted_colorMi), TRUE);
  
 }
 else{
 
  if(dual_page_mode){
   
   if(dual_page_mode)
    dual_page_mode = FALSE;
   else
    dual_page_mode = TRUE;
    
   gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), TRUE );
   
  }
  else{
   gtk_widget_hide(ldraw_area);
  }
  
  if(invert_color){
   
   if(invert_color)
    invert_color = FALSE;
   else
    invert_color = TRUE;
   
   gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(inverted_colorMi), TRUE);
   
  }
  
 }
 
 gtk_widget_hide(findbar);
 gtk_widget_hide(toolbar);
 
 gtk_widget_hide(tview);
 gtk_widget_hide(tsw);
 
 gtk_widget_grab_focus(scrolled_window);
 
}

static void size_adjust_cb(GtkAllocation *allocation){
 
 double width_zf;
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation (scrolled_window, &sw_alloc);
 
 #if GTK_CHECK_VERSION(3,18,10)
 width_zf = (sw_alloc.width - 2)/page_width;
 #else
 width_zf = (sw_alloc.width - 2 - 12 )/page_width;
 #endif
 
 if( zoom_factor >= width_zf){
 
  width_offset = 0;
  height_offset = 0;
  
  gtk_layout_move(GTK_LAYOUT(layout), draw_area, 0, 0);
  
 }
 else{
  
  #if GTK_CHECK_VERSION(3,12,3)
  width_offset = (allocation->width - da_width)/2;
  #else
  width_offset = (allocation->width - da_width)/2-7;
  #endif
  
  height_offset = (allocation->height - da_height)/2;
  width_offset = (allocation->width - da_width)/2;
  
  if( height_offset < 0  ){
   height_offset = 0;
  }
  
  if( width_offset < 0  ){
   width_offset = 0;
  }
  
  if( !dual_page_mode ){
   gtk_layout_move(GTK_LAYOUT(layout), draw_area, width_offset, height_offset);
  }
  else{
   
   int dp_width = (int)(zoom_factor*page_width);
   
   GtkAllocation da_alloc,lda_alloc;
   gtk_widget_get_allocation (draw_area, &da_alloc);
   gtk_widget_get_allocation (ldraw_area, &lda_alloc);
   
   if( dp_width > lda_alloc.width+1 ){
   
    char tmp_zf[10];
   
    if(page_width >= page_height){
    
     sprintf(tmp_zf, "%.3f", ( (screen_width+12 )/2.0/ page_width));
     
    }
    else{
  
     #if GTK_CHECK_VERSION(3,18,10)
     sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ page_width));
     #else
     sprintf(tmp_zf, "%.3f", ( (screen_width-18 )/2.0/ page_width));
     #endif
    }
   
    dp_width = (int)(atof(tmp_zf)*page_width)-15;
    
   }
   
   if( da_alloc.width == lda_alloc.width && da_alloc.height == lda_alloc.height ){
   
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width, height_offset);
    gtk_layout_move(GTK_LAYOUT(layout), ldraw_area, 0, height_offset);
   }
   else{
   
    dp_width = lda_alloc.width;
   
    if( da_alloc.height > screen_height )
     gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width, 0);
    else
     gtk_layout_move(GTK_LAYOUT(layout), draw_area, dp_width, height_offset);
    
    if( lda_alloc.height > screen_height )
     gtk_layout_move(GTK_LAYOUT(layout), ldraw_area, 0, 0);
    else{
     
     double awidth, aheight, bwidth, bheight;
   
     PopplerPage* apage = poppler_document_get_page(doc, current_page_num);
 
     poppler_page_get_size(apage, &awidth, &aheight); 
 
     apage = poppler_document_get_page(doc, current_page_num+1);
  
     poppler_page_get_size(apage, &bwidth, &bheight); 
 
     if( awidth == bwidth && aheight == bheight )
      gtk_layout_move(GTK_LAYOUT(layout), ldraw_area, 0, height_offset);
     else{
     
     
      GtkAllocation sw_alloc;
      gtk_widget_get_allocation(scrolled_window, &sw_alloc);
      
      gtk_layout_move(GTK_LAYOUT(layout), ldraw_area, 
                                          0, 
                                          ( sw_alloc.height - 2 - lda_alloc.height )/2);
     
     }
     
    }
    
   }
   
  }
  
 }
 
 if( !blank_page ){
  if( current_nc )
   display_comment(&current_nc->CM_HEAD);
 }
 else{
  
  if( !dual_page_mode )
   display_comment(NULL);
  else{
   
   if( !current_bp && rcurrent_bp ) 
    display_comment(&current_nc->CM_HEAD);
   else if( current_bp && rcurrent_bp )
    display_comment(NULL);
   else if( current_bp && !rcurrent_bp )
    display_comment(&rcurrent_nc->CM_HEAD);
    
  }
  
 }
 
 if( dual_page_mode ){
 
  if( rcurrent_nc ){
   
   if( !rcurrent_bp )
    display_comment(&rcurrent_nc->CM_HEAD);
   else
    display_comment(NULL);
  }
  
 }
 
}

static void
size_allocate_cb( GtkWidget *win, GtkAllocation *allocation, gpointer data){
 
 if( pre_sw_width == allocation->width+1 ){
  ;
 }else if(pre_sw_width > allocation->width){
  
  //window gets smaller
  //zoom out
  
  if(allocation->width >= (gint)(page_width*(zoom_factor-1.0)) && allocation->width <= (gint)(page_width*(zoom_factor)) ){
   zoom(ZOOM_OUT);
  }
  
  int z_count = pre_sw_width/allocation->width;
  
  if( z_count > 1 ){
   while(z_count>0){
    zoom(ZOOM_OUT);
    z_count--;
   }
  }
  
  size_adjust_cb(allocation);
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
 }
 else if(pre_sw_width < allocation->width){
  
  GtkAllocation tview_alloc;
  gtk_widget_get_allocation (tview, &tview_alloc);
  
  //window gets bigger
  //zoom in
  
  GtkAllocation w_alloc;
  gtk_widget_get_allocation (win, &w_alloc);
  
  if(screen_width == w_alloc.width+2){
   zoom(ZOOM_WIDTH);
  }
  
  if(allocation->width >= (gint)(page_width*(zoom_factor+1.0)) && allocation->width <= (gint)(page_width*(zoom_factor+2.0)) ){
   zoom(ZOOM_IN);
  }
  
  size_adjust_cb(allocation);
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 else{  //pre_sw_width == allocation->width
  
  pre_sw_width = allocation->width + 1;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 
 if(da_width > pre_da_width ){
 
  //1, zoom in
  
  size_adjust_cb(allocation);
  
  pre_da_width = da_width;
  g_signal_emit_by_name(G_OBJECT(scrolled_window), "size-allocate", allocation, data);
  
 }
 else if( da_width < pre_da_width){ 
  //2, zoom out
  
  size_adjust_cb(allocation);
  
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
   
   if( screen_height > (int)(page_height*zoom_factor) ){
    zoom(ZOOM_HEIGHT);
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
 
 sprintf(page_str, "%s page %d/%d %s",
                    file_name, 
                    current_page_num+1, 
                    poppler_document_get_n_pages(doc), 
                    "[E]");
                    
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
 
 sprintf(page_str, "%s page %d/%d %s",
                    file_name, 
                    current_page_num+1, 
                    poppler_document_get_n_pages(doc), 
                    "[H]");
 
 gtk_window_set_title(GTK_WINDOW(win), page_str);
 
 free(page_str);
 
 da_glyph_color.red = CLAMP ((guint) (0 * 65535), 0, 65535);
 da_glyph_color.green = CLAMP ((guint) (0* 65535), 0, 65535);
 da_glyph_color.blue = CLAMP ((guint) (0* 65535), 0, 65535);
 
 da_background_color.red = CLAMP ((guint) (P_HR_COLOR->color.red* 65535), 0, 65535);
 da_background_color.green = CLAMP ((guint) (P_HR_COLOR->color.green * 65535), 0, 65535);
 da_background_color.blue = CLAMP ((guint) (P_HR_COLOR->color.blue * 65535), 0, 65535);
 
}

static void
text_highlight_mode_cb(GtkWidget* widget, gpointer data) {
 
 text_highlight_mode_change();
 
}

gboolean
touchpad_cb(GtkWidget *widget, GdkEvent *event, gpointer data){
 
 g_message("touchpad swipe");
 
 return FALSE;
}

static gboolean
da_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer data){
 
 GtkScrollType direction = GTK_SCROLL_NONE;
 gboolean horizontal = FALSE;
 gboolean returnValue = TRUE;

 if( event->scroll.state & GDK_CONTROL_MASK ){
  
  if( event->scroll.direction == GDK_SCROLL_UP ) {
   zoom(ZOOM_IN); 
  }else if ( event->scroll.direction == GDK_SCROLL_DOWN ) {
   zoom(ZOOM_OUT);
  }
  /*
  ////////////
  else if( event->scroll.direction == GDK_SCROLL_SMOOTH ){
   
   g_message("20190425 scroll_smooth");
   
   g_message("20190424 da_scroll event->scroll.delta_x = %f", event->scroll.delta_x);
   g_message("20190424 da_scroll event->scroll.delta_y = %f", event->scroll.delta_y);
   
   if( event->scroll.delta_y < 0 ){
    
    if(delta_y > 0.0)
     delta_y = 0.0;
     
    delta_y = delta_y + event->scroll.delta_y;
    
    g_message("20190424 da_scroll delta_y = %f", delta_y);
    
    if( delta_y < -2.5 ){
     zoom(ZOOM_IN);
     delta_y = 0.0;
    }
    
   }
   else if( event->scroll.delta_y > 0 ){
    
    if(delta_y < 0.0)
     delta_y = 0.0;
    
    delta_y = delta_y + event->scroll.delta_y;
    
    if( delta_y > 2.5 ){
     zoom(ZOOM_OUT);
     delta_y = 0.0;
    }
    
   }
   
   
   //if(event->scroll.delta_y < 0)
   // delta_t = delta_t + event->scroll.time - scroll_time;
   //else
   // delta_t = delta_t - event->scroll.time + scroll_time;
   //
   //g_message("smooth delta_t = %d", delta_t);
   //
   //if( delta_t <= 0 ){
   // delta_t = 1;
   //}
   //
   //g_message("smooth delta_t = %d, delta_t/1000.0 = %f", delta_t, delta_t/200.0);
   //
   //
   ////zoom_factor = delta_t/200.0;
   ////
   ////if( event->scroll.delta_y < 0 )
   //// zoom(ZOOM_IN);
   ////else
   //// zoom(ZOOM_OUT);
   //
   //
   //g_message("smooth page = %p", page);
   //
   //if( !page ){
   // page = poppler_document_get_page(doc, current_page_num+1);
   // poppler_page_get_size(page, &page_width, &page_height);
   //}
   //
   ////if(page)
   //// poppler_page_get_size(page, &page_width, &page_height);
   //
   //zoom_factor = delta_t/2000.0+1;
   //
   //g_message("smooth zoom_factor = %f", zoom_factor);
   //
   //if( zoom_factor > 5  ){
   // delta_t = 0;
   // return FALSE;
   //}
   //
   //
   //
   //gint width, height;
   //
   //width = (gint)((page_width*zoom_factor)+0.5);
   //height = (gint)((page_height*zoom_factor)+0.5);
   //
   ////if(surface)
   //// cairo_surface_destroy (surface);
   //
   ////if(cr)
   //// cairo_destroy (cr);
   //
   ////surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
   //
   ////cr = cairo_create(surface);
   //
   ////cairo_matrix_t matrix;
   //
   ////cairo_get_matrix (cr, &matrix);
   //
   ////cairo_matrix_translate (&matrix,
   ////                           width / 2,
   ////                           height / 2);
   //
   //cairo_save (cr);
   //
   //cairo_translate(cr, 5, 5);
   //cairo_scale(cr, zoom_factor, zoom_factor);
   //cairo_set_source_surface(cr, surface, 0, 0);
   //cairo_paint(cr);
   //
   ////cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
   ////cairo_fill (cr);
   //
   ////cairo_matrix_scale (&matrix, zoom_factor, zoom_factor);
   //
   ////cairo_set_matrix (cr, &matrix);
   //
   ////if(page)
   //// poppler_page_render(page, cr);
   //
   ////cairo_set_operator (cr, CAIRO_OPERATOR_DARKEN);
   ////cairo_set_source_rgb (cr, 1., 1., 1.);
   //
   ////cairo_paint (cr);
   //
   ////cairo_restore (cr);
   //
   ////cairo_destroy (cr);
   //
   ////gtk_widget_set_size_request(draw_area, width, height);
   //
   ////gtk_layout_set_size(GTK_LAYOUT(layout), width, height);
   //
   ////gtk_widget_queue_draw(draw_area);
   //
   //if(delta_t > 10000){
   // g_message("smooth ctrl delta_t = %d", delta_t);
   // delta_t = 0;
   //}
   //
   //scroll_time = event->scroll.time;
   
   
  }
  ////////////
  */
  
  
  return FALSE;
   
 }
 else if( event->scroll.direction == GDK_SCROLL_SMOOTH ){
  
  //remove this condition
  if( event->scroll.state & GDK_CONTROL_MASK ){
  
   delta_t = delta_t + event->scroll.time - scroll_time;
   
   if(delta_t > 1000){
    delta_t = 0;
   }
   
  }
  else{
  
   gdouble delta_x, delta_y; 
    
   scroll_zoom = -1;
  
   if( event->scroll.delta_x > 0.3 ){
    next_page();
   }
   else if( event->scroll.delta_x < -0.3 ){
    prev_page();
   }
  
   //new
  
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
  
   scroll_time = event->scroll.time;
  
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
    else{
      
      horizontal = TRUE;
      direction = GTK_SCROLL_STEP_RIGHT;  
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
    
    if( current_pe->prev ){
    
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
    
    if (current_pe->next)
     next_page();
             
    return TRUE;
   }
            
   horizontal = TRUE;
   direction = GTK_SCROLL_STEP_RIGHT;
   break;

  case GDK_KEY_Up:
  case GDK_KEY_KP_Up:
  
   if ( vposition == gtk_adjustment_get_lower(vadj) ){
                
    if (current_pe->prev) {
     
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
    
    struct page_entry *np;
    
    if ( !dual_page_mode )
     np = current_pe->next;
    else
     np = rcurrent_pe->next;
    
    if (np){
     
     gtk_adjustment_set_value( vadj, gtk_adjustment_get_lower(vadj) );
     
     next_page();
     
    }
  
    return TRUE;
    
   }
   
   direction = GTK_SCROLL_STEP_DOWN;
   break;

  case GDK_KEY_Page_Up:
  case GDK_KEY_KP_Page_Up:
   
   if ( vposition == gtk_adjustment_get_lower(vadj) ){
                
    if( current_pe->prev ){
     
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
   
   if ( vposition == ( gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj) ) ){
    
    struct page_entry *np;
    
    if ( !dual_page_mode )
     np = current_pe->next;
    else
     np = rcurrent_pe->next;
    
    if (np){
     
     gtk_adjustment_set_value( vadj, gtk_adjustment_get_lower(vadj) );
     
     next_page();
     
    }
               
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
    
    struct page_entry *np;
    
    if ( !dual_page_mode )
     np = current_pe->next;
    else
     np = rcurrent_pe->next;
    
    if (np){
   
     next_page();
     
    }
    
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
     
     if( screen_height > (int)(page_height*zoom_factor) ){
      zoom(ZOOM_HEIGHT);
     }
         
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
     
     zoom(ZOOM_IN);
        
     if(pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT)
      mode = pre_mode;
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_minus :
       
    if (event->state & GDK_CONTROL_MASK){
     
     zoom(ZOOM_OUT);
        
     if(pre_mode == TEXT_HIGHLIGHT || pre_mode == ERASE_TEXT_HIGHLIGHT)
      mode = pre_mode;
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_w :
       
    if (event->state & GDK_CONTROL_MASK){
     
     zoom(ZOOM_WIDTH);
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_g :
    if (event->state & GDK_CONTROL_MASK){
     add_page_comment();
    }
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_F11 :
    
    if(!full_screen){
     gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), TRUE);
    }
    else{
     gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), FALSE);
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   /*
   ////////////
   case GDK_KEY_v :
      
    if (event->state & GDK_CONTROL_MASK){
     
     if( !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(hide_toolbarMi)) ){  
      toggle_hide_toolbar();
     }
    }
    
    direction = GTK_SCROLL_NONE;
    break;
    ////////////
   */
   case GDK_KEY_d :
    
    if (event->state & GDK_CONTROL_MASK){
     
     if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(dual_pageMi) ) ){
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), TRUE );
      dual_page_mode = TRUE;
     
     }else{
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
      dual_page_mode = FALSE;
     }
      
     if( doc ){
      
      gint page_num = poppler_document_get_n_pages(doc);
      
      if( page_num == 1 ){
       dual_page_mode = FALSE;
       gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
      }
      
     }
     else{
      
      if( !current_pe->prev && !current_pe->next ){
       dual_page_mode = FALSE;
       gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(dual_pageMi), FALSE );
      }
      
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
     
     color_set(NULL, 1);
     
    }//end of if (event->state & GDK_CONTROL_MASK
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_x : 
     
     if (event->state & GDK_CONTROL_MASK){
      
      if(invert_color)
       invert_color = FALSE;
      else
       invert_color = TRUE;
      
      inverted_color_cb();
      
     }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_B :
    
    //ctrl+b -> append blank page
    //shift+b -> prepend blank page
    //ctrl+shift+b -> change background color
    
    if ( (event->state & GDK_SHIFT_MASK) && (event->state & GDK_CONTROL_MASK)){
     if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) )
      color_set(widget, 2);
    }
    else if( event->state & GDK_SHIFT_MASK ){
     
     insert_bp_cb(FALSE);
     
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_b :
    
    //ctrl+b -> append blank page
    //shift+b -> prepend blank page
    //ctrl+shift+b -> change background color
    
    if(event->state & GDK_CONTROL_MASK){
     insert_bp_cb(TRUE);
    }
    
    direction = GTK_SCROLL_NONE;
    break;
   case GDK_KEY_z :
    
    if (event->state & GDK_CONTROL_MASK){
        
     if( mode == TEXT_SELECTION ){
      
      text_highlight_mode_change();
     
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[H]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     
      free(page_str);
     
     }
     else if( mode == TEXT_HIGHLIGHT ){
      
      erase_text_highlight_mode_change();
      
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[E]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
      
      free(page_str);
      
     }
     else if( mode == ERASE_TEXT_HIGHLIGHT ){
      
      text_selection_mode_change();
      
      char *page_str = (char*)malloc(100);
         
      sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
      gtk_window_set_title(GTK_WINDOW(win), page_str);
     
      free(page_str);
      
     }
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
     
     struct page_entry *tmp_pe = current_pe;
     
     int page_num;
     
     if(doc){
      
      page_num = atoi(page_change_str)-1;
      
      current_page_num = current_pe->p_page;
     
      if(page_num + 1 >poppler_document_get_n_pages(doc) ||
        page_num == -1 || ( page_num  == current_page_num && !blank_page )){

       free(page_change_str);
       page_change_str = NULL;
       break;
      }
     
      struct page_entry *tmp_pe = NULL;
     
      if( page_num < current_pe->p_page ){
      
       tmp_pe = current_pe->prev;
      
       while( tmp_pe ){
       
        if( page_num == tmp_pe->p_page && !tmp_pe->curr_bp ){
         break;
        }
       
        tmp_pe = tmp_pe->prev;
       }
      
      }
      else if ( page_num > current_pe->p_page ){
      
       tmp_pe = current_pe->next;
      
       while( tmp_pe ){
       
        if( page_num == tmp_pe->p_page && !tmp_pe->curr_bp ){
         break;
        }
       
        tmp_pe = tmp_pe->next;
       }
      
      }
      else{
      
       if( current_pe->curr_bp ){
       
        tmp_pe = current_pe->prev;
       
        while( tmp_pe ){
        
         if( !tmp_pe->curr_bp ){
          //switch from blank page to pdf pages
          blank_page = FALSE;
          break;
         }
        
         tmp_pe = tmp_pe->prev;
        
        }
       
       }
       else
        tmp_pe = current_pe;
     
      }
     
      current_pe = tmp_pe;
      
      current_nc = current_pe->curr_nc;
      current_bp = current_pe->curr_bp;
      
      if(dual_page_mode){
      
       rcurrent_pe = current_pe->next;
       rcurrent_nc = rcurrent_pe->curr_nc;
      
       rcurrent_bp = rcurrent_pe->curr_bp;
      
      }
     
      if(current_bp)
       blank_page = TRUE;
      else{
      
       if( dual_page_mode ){
      
        if(rcurrent_bp)
         blank_page = TRUE;
        else
         blank_page = FALSE;
       }
       else{
        blank_page = FALSE;
       }
       
      }
     
      current_page_num = current_pe->p_page;
      page_change();
     
      free(page_change_str);
      page_change_str = NULL;
     
     }//end of if(doc)
     else{
      
      page_num = atoi(page_change_str);
      
      if( page_num != current_bp->page_num){
       
       if( page_num >= 0 && page_num <= current_pe->curr_nc->BP_TAIL->page_num ){
       
        int diff = page_num - current_bp->page_num;
       
        if( diff > 0 ){ //move forward
         
         while( diff > 0 ){
          
          current_pe = current_pe->next;
          
          diff--;
          
         }
         
        }
        else{ // move backward
         
         while( diff < 0 ){
          
          current_pe = current_pe->prev;
          
          diff++;
          
         }
         
        }
        
        current_page_num = current_pe->p_page;
        current_bp = current_pe->curr_bp;
        
        if(dual_page_mode){
         
         rcurrent_pe = current_pe->next;
         rcurrent_bp = current_bp->next;
         
        }
        
        page_change();
        
       }
       else{
        free(page_change_str);
        page_change_str = NULL;
        break;
       }
       
       free(page_change_str);
       page_change_str = NULL;
       
      }
      else{
       
       free(page_change_str);
       page_change_str = NULL;
       
       direction = GTK_SCROLL_NONE;
       break;
       
      }
      
     }
     
     //hide comment
     if( tmp_pe->curr_nc ){
  
      struct list_head *tmp;
      struct note *cm_entry;
  
      list_for_each(tmp, &tmp_pe->curr_nc->CM_HEAD){

       cm_entry = list_entry(tmp, struct note, list);
   
       gtk_widget_hide(cm_entry->comment);
  
      }
  
     }
 
     if( blank_page ){
  
      if( tmp_pe->curr_bp ){
   
       struct bp_comment *cmt = tmp_pe->curr_bp->comment;
     
       while( cmt ){
      
        gtk_widget_hide(cmt->comment->comment);
        cmt = cmt->next;
      
       }
    
      }
  
     }
     
     if( dual_page_mode ){
      
      tmp_pe = tmp_pe->next;
      
      if( tmp_pe != current_pe ){
      
       if( tmp_pe->curr_nc ){
  
        struct list_head *tmp;
        struct note *cm_entry;
  
        list_for_each(tmp, &tmp_pe->curr_nc->CM_HEAD){

         cm_entry = list_entry(tmp, struct note, list);
         gtk_widget_hide(cm_entry->comment);
  
        }
  
       }
  
       if( blank_page ){
  
        if( tmp_pe->curr_bp ){
   
         struct bp_comment *cmt = tmp_pe->curr_bp->comment;
     
          while( cmt ){
         
           gtk_widget_hide(cmt->comment->comment);
           cmt = cmt->next;
      
         }
   
        }
  
       }
      
      }//end of if( tmp_pe != current_pe 
     
     }
     
     mode = TEXT_SELECTION;
     //hide comment
     
    } //end of if(mode ==  PAGE_CHANGE)
       
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
     
     if( full_screen ){
      
      gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(full_screenMi), FALSE );
      full_screen = FALSE;
      full_screen_cb();
      
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
     
     zoom(ZOOM_HEIGHT);
        
    }
    /*
    //////////
    else if(areas_ptr > areas){
   
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
      
      
      //if( adj_n == 1 )
      // //gtk_adjustment_set_value(hadj, sw_alloc.width/2 );
      // gtk_adjustment_set_value(hadj, 500 );
      //else
      // gtk_adjustment_set_value(hadj, 500*( adj_n ));
      //// //gtk_adjustment_set_value(hadj, sw_alloc.width*( adj_n-1 ));
      
      
      gtk_adjustment_set_value(hadj, sw_alloc.width*( adj_n*405 ));
      
      
      //gdouble adj;
      //
      //if( cursor_x - sw_alloc.width > 500 )
      ////if( cursor_x - sw_alloc.width > 500*adj_n )
      // //adj = sw_alloc.width*( (int)(cursor_x/sw_alloc.width) );
      // //adj = 580*( (int)(cursor_x/sw_alloc.width) );
      // adj = 580*adj_n;
      //else
      // adj = 580;
      //
      //if(adj > gtk_adjustment_get_upper(hadj))
      // adj =  1500;
      // 
      //g_message("lh adj = %f", adj);
      //
      //gtk_adjustment_set_value(hadj, adj ); 
   
      
     }
     else{
      
      gtk_adjustment_set_value(hadj, 0.0 );
      
     }
     

    }
    //////////
    */
    
    direction = GTK_SCROLL_NONE;
    break;
   /* 
   //////////
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
     
      
      //if(line_count == 0){
      // areas_ptr = areas;
      // g_message("i (*areas_ptr).x1 = %f, (*areas_ptr).x2 = %f", (*areas_ptr).x1, (*areas_ptr).x2);
      // g_message("i (*areas_ptr).y1 = %f, (*areas_ptr).y2 = %f", (*areas_ptr).y1, (*areas_ptr).y2);
      //}else
      // areas_ptr = areas_line[line_count]+line_offset+1;
      
      
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
    
    direction = GTK_SCROLL_NONE;
    break;
    //////////
   */ 
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
      
      if( mode != PAGE_CHANGE )
       pre_mode = mode;
      
      mode = PAGE_CHANGE;         
     }// end of if(  *(event->string) >= 48 && *(event->string) <=57 )
     else{
      
      if( mode == PAGE_CHANGE ){
       mode = pre_mode;
       free(page_change_str);
       page_change_str = NULL;
      }
      
     }
    
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
 save_pref();  
 
 gtk_main_quit();

}

gboolean
cursor_position_changed_cb(GtkTextBuffer *buffer){
 
 gint pos;
 g_object_get (buffer, "cursor-position", &pos, NULL);
 
 GtkTextIter start, end;

 pre_str_pos = str_pos;
 str_pos = pos;
 
 GtkTextIter piter;
 
 gtk_text_buffer_get_iter_at_mark(buffer,
      &cur_cursor, gtk_text_buffer_get_insert(buffer));
 
 gint row = gtk_text_iter_get_line(&cur_cursor);
 gint col = gtk_text_iter_get_line_offset(&cur_cursor);
 
 gtk_text_buffer_get_start_iter (buffer, &start);
 gtk_text_buffer_get_end_iter (buffer, &end);
 char *text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
 
 bindex = gtk_text_iter_get_offset(&cur_cursor);
 
 if(gtk_text_iter_is_end(&cur_cursor)){//last character
  
  bindex = gtk_text_iter_get_bytes_in_line(&cur_cursor);
  
  if(row > 0){
   
   gint line_count = gtk_text_buffer_get_line_count(buffer);
   gint current_line = gtk_text_iter_get_line(&cur_cursor);
   
   GtkTextIter liter;
    
   while( current_line != 0 ){
     
    gtk_text_buffer_get_iter_at_line ( buffer,
                                       &liter,
                                       current_line-1);
    
    bindex = bindex + gtk_text_iter_get_bytes_in_line(&liter);
    
    current_line--;
     
   }

  }
   
 }
 else{
  
  bindex = gtk_text_iter_get_line_index(&cur_cursor);
   
  if( col == 0 ){
    
   if( bindex > 0)
    bindex--;
   
  }
   
  if(row > 0){
   
   gint line_count = gtk_text_buffer_get_line_count(buffer);
   gint current_line = gtk_text_iter_get_line(&cur_cursor);
   
   GtkTextIter liter;
    
   while( current_line != 0 ){
     
    gtk_text_buffer_get_iter_at_line ( buffer,
                                       &liter,
                                       current_line-1);
    
    bindex = bindex + gtk_text_iter_get_bytes_in_line(&liter);
     
    current_line--;
     
   }
    
  }
   
 }

 
 gtk_widget_queue_draw (current_cm->comment);
 
 return FALSE;
 
}



void
textbuffer_changed_cb(GtkTextBuffer *buffer, gpointer user_data){

 struct note* comment = (struct note *)user_data;
 
 GtkTextIter startIter;
 GtkTextIter endIter;
 gtk_text_buffer_get_start_iter(buffer, &startIter);
 gtk_text_buffer_get_end_iter(buffer, &endIter);
 
 char * pch, *tmp_pch;
 int count = 0;
   
 int max_len = 0;
 
 char *string = gtk_text_buffer_get_text(buffer, &startIter, &endIter, TRUE);
 
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
 
 comment_buffer_changed = TRUE;
 
 gtk_widget_queue_draw (comment->comment);
 
}
