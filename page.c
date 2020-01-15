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
 
#include "page.h"
#include "gui.h"
#include "note.h"
#include "list.h"
#include "highlight.h"
#include "pdf.h"
#include "zoom.h"

static void dual_page(void){
 
 rcurrent_pe = current_pe->next;
 
 if( !rcurrent_pe ){
 
  rcurrent_pe = current_pe;
  current_pe = current_pe->prev;
  
 }
   
 current_nc = current_pe->curr_nc;
 rcurrent_nc = rcurrent_pe->curr_nc;
 
 current_bp = current_pe->curr_bp;
 rcurrent_bp = rcurrent_pe->curr_bp;
 
 if( current_bp || rcurrent_bp )
  blank_page = TRUE;
 
 char *title_str = (char*)malloc(100);
  
 if( !blank_page ){
  sprintf(title_str, "%s page %d/%d",file_name, current_pe->p_page+1,poppler_document_get_n_pages(doc));
 }
 else{
  
  if( !current_pe->curr_bp ){
   sprintf(title_str, "%s page %d/%d",file_name, current_pe->p_page+1,poppler_document_get_n_pages(doc));
  }
  else{
  
   if( doc ){
    sprintf(title_str, "%s page %d-%d/%d",file_name, current_pe->p_page+1,current_bp->page_num, poppler_document_get_n_pages(doc));
   }
   else{
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    if(file_name)
     sprintf(title_str, "%s page %d/%d",
      file_name, current_pe->curr_bp->page_num, page_num);
    else
     sprintf(title_str, "Untitled page %d/%d",
      current_pe->curr_bp->page_num, page_num);
        
   }
    
  }
  
 }
 
 gtk_window_set_title(GTK_WINDOW(win), title_str);
 
 free(title_str);
 
 gtk_widget_show(ldraw_area);
 
 if(lpage){
  g_object_unref (G_OBJECT (lpage));
  lpage = NULL;
 }

 char tmp_zf[10];
 
 #if GTK_CHECK_VERSION(3,18,10)
 sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ page_width));
 #else
 sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ page_width));
 #endif
 
 zoom_factor = atof(tmp_zf);
 
 if(lsurface){
  cairo_surface_destroy (lsurface);
  lsurface = NULL;
 }
 
 //left page
 page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);
 
 //right page
 page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc);
 
}

void dual_page_cb(void){
 
 if ( dual_page_mode ){
 
  if( doc ){
  
   gint page_num = poppler_document_get_n_pages(doc);
  
   if( page_num > 1 ){
   
    if(blank_page){
    
     if( current_bp ){
      if( current_bp->next ){
       if( current_nc->page_num < page_num)
        current_page_num--;
      }
     }
    
    }
   
    dual_page();
   
   }
  
  }
  else{
   
   //cannot enter dual-page mode with one page
   if( current_pe->prev || current_pe->next )
    dual_page();
  
  }
  
 }
 else{ // exit dual-page mode
  
  if( !current_bp && rcurrent_bp ){
   
   da_selection_region = lda_selection_region;
   lda_selection_region = NULL;
   
   if( blank_page ){
    blank_page = FALSE;
    current_page_num++;
   }
   
  }
  else if( current_bp && rcurrent_bp ){
   
   if(doc)
    current_page_num++;
  
  }
  else if( current_bp && !rcurrent_bp ){
   
   current_page_num = current_nc->page_num;
   
  }
  
  struct bp_comment *cmt;
  
  //hide comments from previous blank page
  if( current_bp ){
  
    cmt = current_bp->comment;
     
    while( cmt ){
    
     gtk_widget_hide(cmt->comment->comment);
     cmt = cmt->next;
      
    }
   }
   
   if( rcurrent_bp ){
  
    cmt = rcurrent_bp->comment;
     
    while( cmt ){
    
     gtk_widget_hide(cmt->comment->comment);
     cmt = cmt->next;
      
    }
    
    rcurrent_bp = NULL;
    
   }
   
   //hide comments from previous blank page
  
  if(lpage){
   g_object_unref (G_OBJECT (lpage));
   lpage = NULL;
  }
  
  if(page){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
  dual_page_mode = FALSE;
  
  zoom(ZOOM_WIDTH);
  
  gtk_widget_hide(ldraw_area);
  
  if(lsurface){
   cairo_surface_destroy (lsurface);
   lsurface = NULL;
  }
  
  if( rcurrent_nc ){
  
   struct list_head *tmp;
   struct note *cm_entry;
   
   list_for_each(tmp, &rcurrent_nc->CM_HEAD){
  
    cm_entry = list_entry(tmp, struct note, list);
   
    if( gtk_widget_is_visible(cm_entry->comment) ){
     gtk_widget_hide(cm_entry->comment);
    }
   
   }//list_for_each(tmp, &current_nc->CM_HEAD)
  
   rcurrent_nc = NULL;
  
  }
  
 }
 
}

void page_change( void ){
 
 char *page_str = (char*)malloc(100);
 
 if(!blank_page){
 
  sprintf(page_str, "%s page %d/%d",
   file_name, current_pe->p_page+1, poppler_document_get_n_pages(doc));
 
  gtk_window_set_title(GTK_WINDOW(win), page_str);
  
  if( dual_page_mode ){
   
   if( !rcurrent_pe ){
    rcurrent_pe = current_pe->next;
   }
   
   //left page

   current_nc = current_pe->curr_nc;
   
   page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);
   
   //right page
   
   rcurrent_nc = rcurrent_pe->curr_nc;
   
   page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc);
   
   display_comment(&rcurrent_nc->CM_HEAD);
  
  }
  else{
  
   current_nc = current_pe->curr_nc;
   
   page_setup(&surface, &da_selection_region, draw_area, current_nc);
   
  }
  
  display_comment(&current_nc->CM_HEAD);
  
 }
 else{
  
  //blank page
  
  if( !dual_page_mode ){
   
   if(doc)
    sprintf(page_str, "%s page %d-%d/%d",
    file_name, current_page_num+1, current_pe->curr_bp->page_num, poppler_document_get_n_pages(doc));
   else{
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    if(file_name)
     sprintf(page_str, "%s page %d/%d",
      file_name, current_pe->curr_bp->page_num, page_num);
    else
     sprintf(page_str, "Untitled page %d/%d",
      current_pe->curr_bp->page_num, page_num);
    
   }
   
   gtk_window_set_title(GTK_WINDOW(win), page_str);
   
   current_bp = current_pe->curr_bp;
   
   page_setup(&surface, NULL, draw_area, NULL);
   
   display_comment(NULL);
   
  }
  else{
   
   if( doc ){
    
    if(current_bp)
     sprintf(page_str, "%s page %d-%d/%d",
      file_name, current_pe->p_page+1, current_pe->curr_bp->page_num, poppler_document_get_n_pages(doc));
    else
     sprintf(page_str, "%s page %d/%d",
      file_name, current_pe->p_page+1, poppler_document_get_n_pages(doc));
    
   }
   else{
    
    gint page_num = current_pe->curr_nc->BP_TAIL->page_num;
    
    if(file_name)
     sprintf(page_str, "%s page %d/%d",
      file_name, current_pe->curr_bp->page_num, page_num);
    else
     sprintf(page_str, "Untitled page %d/%d",
      current_pe->curr_bp->page_num, page_num);
   
   }
   
   gtk_window_set_title(GTK_WINDOW(win), page_str);
   
   if(current_bp){
    page_setup(&lsurface, NULL, ldraw_area, NULL);
    display_comment(NULL);
   }
   else{
    page_setup(&lsurface, &lda_selection_region, ldraw_area, current_nc);
    display_comment(&current_nc->CM_HEAD);
   }
   
   if(rcurrent_bp){
    page_setup(&surface, NULL, draw_area, NULL);
    display_comment(NULL);
   }
   else{
    page_setup(&surface, &da_selection_region, draw_area, rcurrent_nc);
    display_comment(&rcurrent_nc->CM_HEAD);
   }
   
  }
  
 }
 
 free(page_str);

}

void
next_page_cb(GtkWidget* widget, gpointer data) {

 next_page();

}

void prev_page(void){
 
 if( current_pe->curr_nc ){
  
  struct list_head *tmp;
  struct note *cm_entry;
  
  list_for_each(tmp, &current_pe->curr_nc->CM_HEAD){

   cm_entry = list_entry(tmp, struct note, list);
   
   gtk_widget_hide(cm_entry->comment);
  
  }
  
 }
 
 if( blank_page ){
  
  if( current_pe->curr_bp ){
   
   struct bp_comment *cmt = current_pe->curr_bp->comment;
     
   while( cmt ){
      
    gtk_widget_hide(cmt->comment->comment);
    cmt = cmt->next;
      
   }
   
  }
  
 }
 
 if( !dual_page_mode ){
  
  if(current_pe->prev){
   
   current_pe = current_pe->prev;
   
   if( current_pe->curr_bp ){
   
    blank_page = TRUE;
    
    if( da_selection_region ){
     cairo_region_destroy(da_selection_region);
     da_selection_region = NULL;
    }
    
   }
   else{
    current_bp = NULL;
    blank_page = FALSE;
   }
   
  }
  
 }
 else{
  
  if( rcurrent_pe->curr_nc ){
  
   struct list_head *tmp;
   struct note *cm_entry;
  
   list_for_each(tmp, &rcurrent_pe->curr_nc->CM_HEAD){

    cm_entry = list_entry(tmp, struct note, list);
   
    gtk_widget_hide(cm_entry->comment);
  
   }
  
  }
  
  if( blank_page ){
  
   if( rcurrent_pe->curr_bp ){
   
    struct bp_comment *cmt = rcurrent_pe->curr_bp->comment;
     
    while( cmt ){
      
     gtk_widget_hide(cmt->comment->comment);
     cmt = cmt->next;
      
    }
   
   }
  
  }
  
  if(current_pe->prev){
   
   if( current_pe->prev->prev ){
    
    //  p10, p11, p12, p13
    //P            c    r
    //N  c    r
     
    rcurrent_pe = current_pe->prev;
    current_pe = rcurrent_pe->prev;
   }
   else{
     
    //  NULL, p1, p2, p3
    //P            c   r
    //N        c   r
     
    rcurrent_pe = current_pe;
    current_pe = current_pe->prev;
     
   }
    
  }
  
  if( current_pe->curr_bp || rcurrent_pe->curr_bp ){
     
   blank_page = TRUE;
     
   current_bp = current_pe->curr_bp;
   rcurrent_bp = rcurrent_pe->curr_bp;
     
  }
  else{
     
   blank_page = FALSE;
   current_bp = rcurrent_bp = NULL;
     
  }
  
 }
 
 current_page_num = current_pe->p_page;
 
 current_nc = current_pe->curr_nc;
 
 if( dual_page_mode )
  rcurrent_nc = rcurrent_pe->curr_nc;
 
 page_change();
 
}

void
prev_page_cb(GtkWidget* widget, gpointer data) {

 prev_page();

}

void next_page(void){
 
 if( current_pe->curr_nc ){
  
  struct list_head *tmp;
  struct note *cm_entry;
  
  list_for_each(tmp, &current_pe->curr_nc->CM_HEAD){

   cm_entry = list_entry(tmp, struct note, list);
   
   gtk_widget_hide(cm_entry->comment);
   
  }
  
 }
 
 if( blank_page ){
  
  if( current_pe->curr_bp ){
   
   struct bp_comment *cmt = current_pe->curr_bp->comment;
     
   while( cmt ){
      
    gtk_widget_hide(cmt->comment->comment);
    cmt = cmt->next;
      
   }
   
  }
  
 }
 
 
 if( !dual_page_mode ){
  
  if(current_pe->next){
   
   current_pe = current_pe->next;
   
   if( current_pe->curr_bp ){
   
    blank_page = TRUE;
    
    if( da_selection_region ){
     cairo_region_destroy(da_selection_region);
     da_selection_region = NULL;
    }
    
   }
   else{
    blank_page = FALSE;
    current_bp = NULL;
   }
   
  }
  
 }
 else{
  
  //dual page mode
  
  if( rcurrent_pe->curr_nc ){
  
   struct list_head *tmp;
   struct note *cm_entry;
  
   list_for_each(tmp, &rcurrent_pe->curr_nc->CM_HEAD){
    
    cm_entry = list_entry(tmp, struct note, list);
   
    gtk_widget_hide(cm_entry->comment);
   
   }
  
  }
  
  if( blank_page ){
   
   if( rcurrent_pe->curr_bp ){
   
    struct bp_comment *cmt = rcurrent_pe->curr_bp->comment;
     
    while( cmt ){
     
     gtk_widget_hide(cmt->comment->comment);
     cmt = cmt->next;
      
    }
   
   }
  
  }
  
  if(rcurrent_pe->next){
   
   if(rcurrent_pe->next->next){
     
    //  p1,p2,p3 p4
    //P  c  r
    //N        c  r
     
    current_pe = rcurrent_pe->next;
    rcurrent_pe = current_pe->next;
    
   }
   else{
      
    //  p11,p12,p13 NULL
    //P  c   r
    //N      c   r
      
    current_pe = rcurrent_pe;
    rcurrent_pe = rcurrent_pe->next;
      
   }
   
   if( current_pe->curr_bp || rcurrent_pe->curr_bp ){
     
    blank_page = TRUE;
     
    current_bp = current_pe->curr_bp;
    rcurrent_bp = rcurrent_pe->curr_bp;
     
   }
   else{
     
    blank_page = FALSE;
    current_bp = rcurrent_bp = NULL;
     
   }
    
  }
  
 }
 
 current_page_num = current_pe->p_page;
 
 current_nc = current_pe->curr_nc;
 
 if( dual_page_mode )
  rcurrent_nc = rcurrent_pe->curr_nc;
  
 page_change();
 
}

void page_setup(cairo_surface_t **surface, cairo_region_t **da_selection_region, GtkWidget *draw_area, struct note_cache *current_nc){
 
 struct list_head *tmp;
 
 if( selection_surface ){
 
  cairo_surface_destroy (selection_surface);
  selection_surface = NULL;
  
  gtk_widget_queue_draw (draw_area);
  
 }
 
 if( !dual_page_mode ){ //single page mode
  
  if( blank_page ){
   //set up blank page
   
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
  }
  else{
   
   if(page == NULL){
    
    current_page_num = current_pe->p_page;
    
    page = poppler_document_get_page(doc, current_page_num);
   
   }
  }
  
 }
 else{ //for dual page mode
  
  if( *surface == lsurface ){ //left page
   
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   if( !blank_page ){
    
    current_page_num = current_pe->p_page;
    
    page = poppler_document_get_page(doc, current_page_num);
    
   }
   else{
    
    if( !current_pe->curr_bp ){
     
     current_page_num = current_pe->p_page;
    
     page = poppler_document_get_page(doc, current_page_num);
     
    }
    
   }
   
  }
  else{ //right page
   
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   if( !blank_page ){
    
    page = poppler_document_get_page(doc, rcurrent_pe->p_page);
    
   }
   else{
    
    if( !rcurrent_pe->curr_bp ){
     page = poppler_document_get_page(doc, rcurrent_pe->p_page);
    }
    
   }
   
  }
  
 }
 
 pre_page_width = page_width;
 
 if(page)
  poppler_page_get_size(page, &page_width, &page_height);
 
 GtkAllocation sw_alloc;
 gtk_widget_get_allocation(scrolled_window, &sw_alloc);
 
 GtkAllocation da_alloc, lda_alloc;
 
 double awidth, aheight, bwidth, bheight;
  
 if( dual_page_mode && doc ){
  
  PopplerPage* apage;
  
  if( current_page_num >= 0 ){
   apage = poppler_document_get_page(doc, current_page_num);
  }
  else{
   apage = poppler_document_get_page(doc, 0);
  }
  
  poppler_page_get_size(apage, &awidth, &aheight); 
  
  gint page_num = poppler_document_get_n_pages(doc);
  
  if( current_page_num >= 0 ){
   if( current_page_num+1 == page_num )
    apage = poppler_document_get_page(doc, current_page_num);
   else
    apage = poppler_document_get_page(doc, current_page_num+1);
  }
  else
   apage = poppler_document_get_page(doc, 0);
  
  poppler_page_get_size(apage, &bwidth, &bheight); 
  
  if( awidth - bwidth < 1 )
   bwidth = awidth;
  else if( bwidth - awidth < 1 )
   awidth = bwidth;
  
  if( aheight - bheight < 1 )
   bheight = aheight;
  else if( bheight - aheight < 1 )
   aheight = bheight;
  
  g_object_unref (G_OBJECT (apage));
  apage = NULL;
  
 }
 else{
  
  if( !doc ){
   
   //a4 size
   awidth = bwidth = 595;
   aheight = bheight = 842;
   //a4 size
   
  }
  
 }
 
 
 if( !dual_page_mode ){
 
  if( pre_page_width != page_width && pre_page_width > 0 ){
  
   if( zoom_factor > 1 ){ 
   
    double width_zf;
 
    #if GTK_CHECK_VERSION(3,18,10)
    width_zf = (sw_alloc.width - 2)/page_width;
    #else
    width_zf = (sw_alloc.width - 2 - 12 )/page_width;
    #endif
   
    if( width_zf - zoom_factor < 1 ){
     zoom_factor = width_zf;
    }
   
   }
  
  }
  
 }
 else{ // dual-page mode
  
  if(pre_page_width > 0){
  
   int dp_width = (int)(zoom_factor*page_width);
   
   gtk_widget_get_allocation (draw_area, &da_alloc);
   gtk_widget_get_allocation (ldraw_area, &lda_alloc);
   
   if( awidth == bwidth && aheight == bheight ){
    
    azoom_factor = 0.0;
    
    char tmp_zf[10];
 
    if(page_width >= page_height)
     sprintf(tmp_zf, "%.3f", ( (screen_width+1 )/2.0/ page_width));
    else{
  
     #if GTK_CHECK_VERSION(3,18,10)
     sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ page_width));
     #else
     sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ page_width));
     #endif
    }
 
    zoom_factor = atof(tmp_zf);
   
   }
   else{ //pages with different size
    
    char tmp_zf[10];
   
    if( draw_area == ldraw_area ){ //left page
     
     if( aheight < bheight ){
     
      #if GTK_CHECK_VERSION(3,18,10)
      sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ awidth));
      #else
      sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ awidth));
      #endif
      
      azoom_factor = -atof(tmp_zf);
      
      if( atof(tmp_zf) == zoom_factor ){
       
       #if GTK_CHECK_VERSION(3,18,10)
       sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ bwidth));
       #else
       sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ bwidth));
       #endif
       
       zoom_factor = atof(tmp_zf);
       
      }
      
     }
     else if( aheight > bheight ){
     
      #if GTK_CHECK_VERSION(3,18,10)
      sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ bwidth));
      #else
      sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ bwidth));
      #endif
      
      azoom_factor = atof(tmp_zf);
      
     }
     
     #ifdef __APPLE__
     if( atof(tmp_zf) == zoom_factor ){
       
      #if GTK_CHECK_VERSION(3,18,10)
      sprintf(tmp_zf, "%.3f", ( (screen_width-4 )/2.0/ awidth));
      #else
      sprintf(tmp_zf, "%.3f", ( (screen_width-2 )/2.0/ awidth));
      #endif
       
      zoom_factor = atof(tmp_zf);
       
     }
     #endif
     
    }
   
   }
  
  }
  
 }
 
 gint width, height;
 
 if( !dual_page_mode ){
  
  width = (gint)((page_width*zoom_factor)+0.5);
  height = (gint)((page_height*zoom_factor)+0.5);

 }
 else{ //dual-page mode
  
  if( zoom_factor == azoom_factor || azoom_factor == 0.0 ){
   
   width = (gint)((page_width*zoom_factor)+0.5);
   height = (gint)((page_height*zoom_factor)+0.5);
 
  }
  else{
   
   if( azoom_factor < 0 ){ //left page
     
    if(draw_area == ldraw_area){
    
     width = (gint)((awidth*(-azoom_factor))+0.5);
     height = (gint)((aheight*(-azoom_factor))+0.5);
     
     ldaa.width = width;
     ldaa.height = height;
     
    }
    else{
     
     width = (gint)((bwidth*zoom_factor)+0.5);
     height = (gint)((bheight*zoom_factor)+0.5);
     
     daa.width = width;
     daa.height = height;
     
    }
     
   }
   else{ //right page
     
    if(draw_area == ldraw_area){
    
     width = (gint)((awidth*zoom_factor)+0.5);
     height = (gint)((aheight*zoom_factor)+0.5);
     
     ldaa.width = width;
     ldaa.height = height;
     
    }
    else{
    
     width = (gint)((bwidth*azoom_factor)+0.5);
     height = (gint)((bheight*azoom_factor)+0.5);
     
     daa.width = width;
     daa.height = height;
     
    }
     
   }

  }
  
 }
 
 if(*surface)
  cairo_surface_destroy (*surface);
 
 if(cr)
  cairo_destroy (cr);
 
 *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

 cr = cairo_create(*surface);
 
 if(!dual_page_mode)
  cairo_scale(cr, zoom_factor, zoom_factor);
 else{
 
  if( zoom_factor == azoom_factor || azoom_factor == 0.0){
   cairo_scale(cr, zoom_factor, zoom_factor);
  }
  else{
   
   if( azoom_factor < 0 ){ // left page
    
    if( draw_area == ldraw_area )
     cairo_scale(cr, -azoom_factor, -azoom_factor);
    else
     cairo_scale(cr, zoom_factor, zoom_factor);
     
   } 
   else{ //right page
   
    if( draw_area == ldraw_area )
     cairo_scale(cr, zoom_factor, zoom_factor);
    else
     cairo_scale(cr, azoom_factor, azoom_factor);
   
   }
   
  }
 
 }
 
 if(page)
  poppler_page_render(page, cr);
 
 if ( invert_color ){
   
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
 
 if( !blank_page ){
  if(da_selection_region){//
   
   if(*da_selection_region){
   
    cairo_region_destroy(*da_selection_region);
    
   }
   
  }
  
 }
 
 if( !dual_page_mode ){
  
  if(page){
   *da_selection_region = poppler_page_get_selected_region (
    page, zoom_factor, POPPLER_SELECTION_GLYPH, &area);
  }
 }
 else{
 
  if(page){
  
   if( zoom_factor == azoom_factor || azoom_factor == 0.0 ){
    if( da_selection_region ){
     *da_selection_region = poppler_page_get_selected_region (
      page, zoom_factor, POPPLER_SELECTION_GLYPH, &area);
    }
   }
   else{

    if( azoom_factor < 0 ){ //left page
    
     if( draw_area == ldraw_area ){
      *da_selection_region = poppler_page_get_selected_region (
       page, -azoom_factor, POPPLER_SELECTION_GLYPH, &area);
     }
     else{
      *da_selection_region = poppler_page_get_selected_region (
       page, zoom_factor, POPPLER_SELECTION_GLYPH, &area);
     }
    
    }//left page
    else{ // right page
    
     if( draw_area == ldraw_area ){
      *da_selection_region = poppler_page_get_selected_region (
       page, zoom_factor, POPPLER_SELECTION_GLYPH, &area);
     }
     else{
      *da_selection_region = poppler_page_get_selected_region (
       page, azoom_factor, POPPLER_SELECTION_GLYPH, &area);
     }
    
    }//right page
  
   }
  
  }//end of if(page)
  
 }
 
 if(!dual_page_mode)
  gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
 else{
  
  if( azoom_factor == 0.0 )
   gtk_layout_set_size (GTK_LAYOUT(layout), width, height);
  else if( azoom_factor < 0 ){// left page
   
   if( -azoom_factor > zoom_factor ){
    gtk_layout_set_size (GTK_LAYOUT(layout), 
       (gint)((awidth*(-azoom_factor))+0.5), (gint)((aheight*(-azoom_factor))+0.5));
   }
   else if( -azoom_factor < zoom_factor ){
    gtk_layout_set_size (GTK_LAYOUT(layout), 
       (gint)((bwidth*zoom_factor)+0.5), (gint)((bheight*zoom_factor)+0.5));
   }
   
  }
  else{ //right page
  
   if( azoom_factor > zoom_factor ){
    gtk_layout_set_size (GTK_LAYOUT(layout), 
       (gint)((bwidth*azoom_factor)+0.5), (gint)((bheight*azoom_factor)+0.5));
   }
   else if( azoom_factor < zoom_factor ){
    gtk_layout_set_size (GTK_LAYOUT(layout), 
       (gint)((awidth*zoom_factor)+0.5), (gint)((aheight*zoom_factor)+0.5));
   }
   
  }
  
 }
 
 gtk_widget_set_size_request (draw_area, width, height);
 
 if( dual_page_mode ){
  
  GtkAllocation da_alloc,lda_alloc;
  gtk_widget_get_allocation (draw_area, &da_alloc);
  gtk_widget_get_allocation (ldraw_area, &lda_alloc);
  
  if(awidth == bwidth && aheight == bheight){
   
   if( draw_area != ldraw_area){  //right page
     
    if( aheight > awidth ){
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,lda_alloc.width, 0);
    }
    else{
      
     int height = ( sw_alloc.height - 2 - (int)(aheight*(zoom_factor) ))/2;
      
     if(aheight_offset<0)
      height = 0;
      
     gtk_layout_move(GTK_LAYOUT(layout), draw_area,lda_alloc.width, height);
     
    }
     
   }
   else{ // left page
    
    int height = ( sw_alloc.height - 2 - (int)(aheight*(zoom_factor) ))/2;
      
    if(height < 0)
     height = 0;
      
    gtk_layout_move(GTK_LAYOUT(layout), draw_area,0, height);
    
   }

  } 
  else{  //page with different size
   
   if( draw_area == ldraw_area ){ //left page
    
    ldaa.x = 0;
    
    //for 1366*768 screen
    //full_screen = 0, sw_width*sw_height = 1364*666
    //full_screen = 1, sw_width*sw_height = 1364*766
    
    int height;
   
    if( azoom_factor < 0 ){
    
     height = ( sw_alloc.height - 2 - aheight*(-azoom_factor) )/2;
     
     if( full_screen ){
      ldaa.y = (screen_height  -2 - aheight*(-azoom_factor) )/2;
     }
     else{
      #ifdef __APPLE__
      ldaa.y = (screen_height - 70 -2 - aheight*(-azoom_factor) )/2;
      #else
      ldaa.y = (screen_height - 100 -2 - aheight*(-azoom_factor) )/2;
      #endif
     }
     
    }
    else{
    
     height = ( sw_alloc.height - 2 - aheight*(zoom_factor) )/2;
    
     ldaa.y = 0;
     
    }
    
    if( height < 0 )
     height = 0;
      
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, 0, height);
    
   }
   else{ //right page
    
    daa.x = ldaa.width;
    
    int height;
    
    if( azoom_factor < 0 ){
    
     height = ( sw_alloc.height - 2 - bheight*zoom_factor )/2;
    
     daa.y = 0;
     
    }
    else{
    
     height = ( sw_alloc.height - 2 - bheight*azoom_factor )/2;
    
     if( full_screen ){
      daa.y = (screen_height  -2 - bheight*(azoom_factor) )/2;
     }
     else{
     
      #ifdef __APPLE__
      daa.y = (screen_height - 70 -2 - bheight*(azoom_factor) )/2;
      #else
      daa.y = (screen_height - 100 -2 - bheight*(azoom_factor) )/2;
      #endif
    
     }
     
    }
   
    if( height < 0 )
     height = 0;
      
    gtk_layout_move(GTK_LAYOUT(layout), draw_area, lda_alloc.width, height);
    
   }
   
  }
 
 }
 
 da_width = width;
 da_height = height;
  
 gtk_widget_queue_draw (draw_area);
  
 if( page ){
  g_object_unref (G_OBJECT (page));
  page = NULL;
 }
 
 if( !blank_page ){
 
  struct highlight_region *hr_entry;
 
  if( current_nc ){
   
   list_for_each(tmp, &current_nc->HR_HEAD){
   
    hr_entry = list_entry(tmp, struct highlight_region, list);
  
    if( invert_color ){
     
     cairo_set_source_rgb (cr, 1, 1 , 1);
     cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
   
     cairo_rectangle (cr, hr_entry->x,
                          hr_entry->y, 
                          hr_entry->width,
                          hr_entry->height);
      
     cairo_fill (cr);
      
    }
   
    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
   
    cairo_set_source_rgb (cr, hr_entry->ce->color.red, 
                              hr_entry->ce->color.green, 
                              hr_entry->ce->color.blue);
   
    cairo_rectangle (cr, hr_entry->x,
                         hr_entry->y, 
                         hr_entry->width,
                         hr_entry->height);
   
    cairo_fill (cr);
    
   }//list_for_each(tmp, &HR_HEAD
  
  }
  
 }
 
 gtk_widget_queue_draw (draw_area);
 
 if( find_ptr ){
  invert_search_region();
 }
 
}
