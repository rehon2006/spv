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

#include "highlight.h"
#include "note.h"
#include "pdf.h"
#include "gui.h"
#include "search.h"
#include "page.h"
 
void erase_highlight(GtkWidget *widget, struct list_head *hr_HEAD, double zoom_factor){
 
 int selection_region_num;
 
 if( !dual_page_mode ){
  
  if(da_selection_region){
   selection_region_num = cairo_region_num_rectangles(da_selection_region);
  }
  
 }
 else{
 
  if( widget == draw_area ){
  
   if(da_selection_region){
    selection_region_num = cairo_region_num_rectangles(da_selection_region);
   }
  }
  else if( widget == ldraw_area ){
  
   if(lda_selection_region){
    selection_region_num = cairo_region_num_rectangles(lda_selection_region);
   }
  }
 
 }
 
  cairo_rectangle_int_t *tmp_ihr = ihr;
 
  if(!ihr){
  
   return;
  }
  
  if( !dual_page_mode ){
   
   if( !current_nc ){
    return;
   }
   
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

   list_for_each_safe(tmp, tmp_next, hr_HEAD){
     
    tmp1= list_entry(tmp, struct highlight_region, list);
    
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
        
        hg->ce = tmp1->ce;
        
        if( !dual_page_mode ){
         hg->page_num = current_page_num+1;
        }
        else{ //dual page mode
         
         if( widget == draw_area ){
          hg->page_num = current_page_num+2;
         }
         else if( widget == ldraw_area ){
          hg->page_num = current_page_num+1;
         }
         
        }
        
        list_add(&(hg->list), hr_HEAD);
        
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
        
        list_del(&tmp1->list);
        
        free(tmp1);
       }
       else{
        
        struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

        hg->x = b_x + b_width;
        hg->y = b_y;
        hg->width = a_x + a_width - b_x - b_width;
        hg->height = b_height;
        
        hg->ce = P_HR_COLOR;
        
        if( !dual_page_mode ){
         hg->page_num = current_page_num+1;
        }
        else{ //dual page mode
         
         if( widget == draw_area ){
          hg->page_num = current_page_num+2;
         }
         else if( widget == ldraw_area ){
          hg->page_num = current_page_num+1;
         }
         
        }
        
        list_add(&(hg->list), hr_HEAD);

        tmp1->width = b_x - a_x;
       }
       
      }
      else if( b_y < a_y ){
      
       if(selection_region_num > 1){
        if( b_width >= a_width && b_y + b_height>= a_y+a_height ){
        
         //for multiple-line highlight deletion
         //erasing highlight regions merges a large region when users try to delete many 
         //highlight regions which spreads over multiple lines
        
         list_del(&tmp1->list);
        
         free(tmp1);
        }
       }
       
      }// end of else if( b_y < a_y
      
     }
    } // end of list_for_each(tmp, &highlight_region_head.list)

    tmp_ihr++;
   } // end of while( tmp_ihr->x != -1 )
   
   //redraw the page
   
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

void save_highlight(GtkWidget *widget, struct list_head *hr_HEAD, double zoom_factor){
 
 int dup = 0; 
  
 struct list_head TMP_HR;
 INIT_LIST_HEAD(&TMP_HR);
 
 cairo_rectangle_int_t *tmp_hr = hr;
  
 while( tmp_hr->x != -1 ){
  tmp_hr++;
 }
 
 if( !list_empty(hr_HEAD) ){
   
  struct list_head *tmp, *q;
  struct highlight_region *tmp1;
  
  cairo_rectangle_int_t *tmp_hr = hr;

  int hr_a2_x, hr_a2_width;
 
  if(!hr){
   return;
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
   
   list_for_each_safe(tmp, q, hr_HEAD){

    tmp1= list_entry(tmp, struct highlight_region, list);
     
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
         
         tmp1->ce = P_HR_COLOR;
         
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
         
         tmp1->ce = P_HR_COLOR;
         
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
         
         // a_x     b_x
         //  -----  ---------
         // |  a |  |    b  |
         //  -----  ---------
         
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
          
          tmp1->ce = P_HR_COLOR;
          
         }
         else{
         
          if(  dup_x != b_x || dup_y != b_y  ){
          
           dup_x = b_x;
           dup_y = b_y;
           dup_width = b_width;
           dup_height = b_height;
           
           tmp1->x = b_x;
           tmp1->width = b_width;
           
           tmp1->ce = P_HR_COLOR;
           
          }
          else{
           
           if( dup_y == b_y ){
           //remove other highlight regions on the same line
            
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
          
          tmp1->ce = P_HR_COLOR;
          
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
     
    
     
     hg->ce = P_HR_COLOR;
     
     if( !dual_page_mode ){
      hg->page_num = current_page_num+1;
     }
     else{ //dual page mode
      
      if( widget == draw_area ){
       hg->page_num = current_page_num+2;
      }
      else if( widget == ldraw_area ){
       hg->page_num = current_page_num+1;
      }
      
     }
     
     list_add(&(hg->list), &TMP_HR);
     
     //draw highlight region
     
     cairo_t *cr;
     
     if( widget == draw_area ){
      cr = cairo_create(surface);
     }
     else if( widget == ldraw_area ){
      cr = cairo_create(lsurface);
     }
     
     if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
      cairo_set_source_rgb (cr, 1, 1 , 1);
      cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
      cairo_rectangle (cr, hg->x*zoom_factor,
                           hg->y*zoom_factor, 
                           hg->width*zoom_factor,
                           hg->height*zoom_factor);
      
      cairo_fill (cr);
      
     }
     
     cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
     
     cairo_set_source_rgb (cr, hg->ce->color.red, hg->ce->color.green, hg->ce->color.blue);
     
     cairo_rectangle (cr, hg->x*zoom_factor,
                          hg->y*zoom_factor, 
                          hg->width*zoom_factor,
                          hg->height*zoom_factor);
     
     cairo_fill (cr);
     
     cairo_destroy (cr);
     
     //draw highlight region
     
    } // end of if(!dup)
    else if( dup == 2 ){
     
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = b_x;
     hg->y = b_y;
     hg->width = b_width;
     hg->height = b_height;
     
     hg->ce = P_HR_COLOR;
     
     if( !dual_page_mode ){
      hg->page_num = current_page_num+1;
     }
     else{ //dual page mode
      
      if( widget == draw_area ){
       hg->page_num = current_page_num+2;
      }
      else if( widget == ldraw_area ){
       hg->page_num = current_page_num+1;
      }
      
     }
     
     list_add(&(hg->list), hr_HEAD);
     
     cairo_t *cr;
     
     if( widget == draw_area ){
      cr = cairo_create(surface);
     }
     else if( widget == ldraw_area ){
      cr = cairo_create(lsurface);
     }
     
     if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
      
      cairo_set_source_rgb (cr, 1, 1 , 1);
      cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
      
      cairo_rectangle (cr, hg->x*zoom_factor,
                           hg->y*zoom_factor, 
                           hg->width*zoom_factor,
                           hg->height*zoom_factor);
      
      cairo_fill (cr);
      
     }
     
     cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
     
     cairo_set_source_rgb (cr, hg->ce->color.red, hg->ce->color.green, hg->ce->color.blue);
     
     cairo_rectangle (cr, hg->x*zoom_factor,
                          hg->y*zoom_factor, 
                          hg->width*zoom_factor,
                          hg->height*zoom_factor);
     
     cairo_fill (cr);
     
     struct highlight_region *hg_a2 = (struct highlight_region *)malloc(sizeof(struct highlight_region));
     
     hg_a2->x = hr_a2_x;
     hg_a2->y = b_y;
     hg_a2->width = hr_a2_width;
     hg_a2->height = b_height;
     
     hg_a2->ce = P_HR_COLOR;
     
     if( !dual_page_mode ){
      hg_a2->page_num = current_page_num+1;
     }
     else{ //dual page mode
      
      if( widget == draw_area ){
       hg_a2->page_num = current_page_num+2;
      }
      else if( widget == ldraw_area ){
       hg_a2->page_num = current_page_num+1;
      }
      
     }
     
     list_add(&(hg_a2->list), hr_HEAD);
     
     if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
      
      cairo_set_source_rgb (cr, 1, 1 , 1);
      cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
      
      cairo_rectangle (cr, hg_a2->x*zoom_factor,
                           hg_a2->y*zoom_factor, 
                           hg_a2->width*zoom_factor,
                           hg_a2->height*zoom_factor);
      
      cairo_fill (cr);
      
     }
     
     cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
     
     cairo_set_source_rgb (cr, hg_a2->ce->color.red, hg_a2->ce->color.green, hg_a2->ce->color.blue);
     
     cairo_rectangle (cr, hg_a2->x*zoom_factor,
                          hg_a2->y*zoom_factor, 
                          hg_a2->width*zoom_factor,
                          hg_a2->height*zoom_factor);
     
     cairo_fill (cr);
     
     cairo_destroy (cr);
     
    }
   
    tmp_hr++;
   } // end of while( tmp_hr->x != -1 ) 

  } // end of if( !list_empty(hr_HEAD)) ) 
  else{ //empty hr_HEAD
   
   if( hr ){
    cairo_rectangle_int_t *tmp_hr = hr;
    
    while( tmp_hr->x != -1 ){
     
     struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

     hg->x = (int)(tmp_hr->x/zoom_factor+0.5);
     hg->y = (int)(tmp_hr->y/zoom_factor+0.5);
     hg->width = (int)(tmp_hr->width/zoom_factor+0.5);
     hg->height = (int)(tmp_hr->height/zoom_factor+0.5);
     
     hg->ce = P_HR_COLOR;
     
     if( !dual_page_mode ){
      hg->page_num = current_page_num+1;
     }
     else{ //dual page mode
      
      if( widget == draw_area ){ 
       hg->page_num = current_page_num+2;
      }
      else if( widget == ldraw_area ){ 
       hg->page_num = current_page_num+1;
      }
      
     }
     
     list_add(&(hg->list), hr_HEAD);
     
     cairo_t *cr;
     
     if( widget == draw_area ){
      cr = cairo_create(surface);
     }
     else if( widget == ldraw_area ){
      cr = cairo_create(lsurface);
     }
     
     if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
      
      cairo_set_source_rgb (cr, 1, 1 , 1);
      cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
      
      cairo_rectangle (cr, hg->x*zoom_factor,
                           hg->y*zoom_factor, 
                           hg->width*zoom_factor,
                           hg->height*zoom_factor);
      
      cairo_fill (cr);
      
     }
     
     cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
     
     cairo_set_source_rgb (cr, hg->ce->color.red, hg->ce->color.green, hg->ce->color.blue);
    
     cairo_rectangle (cr, hg->x*zoom_factor,
                          hg->y*zoom_factor, 
                          hg->width*zoom_factor,
                          hg->height*zoom_factor);
     
     cairo_fill (cr);
     
     cairo_destroy (cr);
     
     tmp_hr++;
    }
   }
    
  } //empty hr_HEAD
  
  if (selection_surface){
  
   cairo_surface_destroy (selection_surface);
   selection_surface = NULL;
  }
  
  gtk_widget_queue_draw (draw_area);
  
  if( dual_page_mode ){
   gtk_widget_queue_draw (ldraw_area);
  }
  
  struct list_head *tmp;
  struct list_head *q;
  struct highlight_region *hr_entry;
  
  list_for_each_safe(tmp, q, &TMP_HR){  
   
   hr_entry = list_entry(tmp, struct highlight_region, list);
   
   struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));
   
   hg->x = hr_entry->x;
   hg->y = hr_entry->y;
   hg->width = hr_entry->width;
   hg->height = hr_entry->height;
   hg->page_num = hr_entry->page_num;
    
   hg->ce = P_HR_COLOR;
   
   list_add(&(hg->list), hr_HEAD);
   
   list_del(&hr_entry->list);
   free(hr_entry);
   
  }//end of list_for_each_safe(tmp, q, &TMP_HR
 
}

void invertArea (gint x1, gint y1, gint x2, gint y2, int option){
 
 cairo_t* cr;
 if( option == 1 ){
  cr = cairo_create(surface);
 }
 else if( option == 2 ){
  cr = cairo_create(lsurface);
 }
 
 cairo_set_operator (cr, CAIRO_OPERATOR_DIFFERENCE); 
 cairo_set_source_rgb (cr, 1., 1., 1.); 
 
 cairo_rectangle (cr, x1, y1, (x2-x1), (y2-y1));
 
 cairo_fill (cr);
 
 cairo_destroy(cr);
 
 if( option == 1 ){
  gtk_widget_queue_draw (draw_area);
 }
 else if( option == 2 ){
  gtk_widget_queue_draw (ldraw_area);
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
 
 if( !dual_page_mode ){
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
     
     if( azoom_factor >= 0.0 ){
      invertArea( (gint)(rect->x1*zoom_factor), 
                  (gint)(rect->y2*zoom_factor),
                  (gint)(rect->x2*zoom_factor), 
                  (gint)(rect->y1*zoom_factor), 
                  2);
     }
     else if( azoom_factor< 0 ){
     
      invertArea( (gint)(rect->x1*(-azoom_factor)), 
                  (gint)(rect->y2*(-azoom_factor)),
                  (gint)(rect->x2*(-azoom_factor)), 
                  (gint)(rect->y1*(-azoom_factor)), 
                  2);
     }
       
    }
    else{ // right page
     
     if( azoom_factor <= 0.0 ){
      invertArea( (gint)(rect->x1*zoom_factor), 
                  (gint)(rect->y2*zoom_factor),
                  (gint)(rect->x2*zoom_factor), 
                  (gint)(rect->y1*zoom_factor), 
                  1);
     }
     else{
      invertArea( (gint)(rect->x1*azoom_factor), 
                  (gint)(rect->y2*azoom_factor),
                  (gint)(rect->x2*azoom_factor), 
                  (gint)(rect->y1*azoom_factor), 
                  1);
     }
     
    }
   }
   else if(mode == TEXT_SEARCH_PREV){
    if( pos <= g_list_position(find_ptr_head, rmatches) ){ // right page 
    
     if( azoom_factor <= 0.0 ){
      invertArea( (gint)(rect->x1*zoom_factor), 
                  (gint)(rect->y2*zoom_factor),
                  (gint)(rect->x2*zoom_factor), 
                  (gint)(rect->y1*zoom_factor), 
                  1);
     }
     else{
      invertArea( (gint)(rect->x1*azoom_factor), 
                  (gint)(rect->y2*azoom_factor),
                  (gint)(rect->x2*azoom_factor), 
                  (gint)(rect->y1*azoom_factor), 
                  1);
     }
     
    }
    else{ // left page
     
     if( azoom_factor >= 0.0 ){
      invertArea( (gint)(rect->x1*zoom_factor), 
                  (gint)(rect->y2*zoom_factor),
                  (gint)(rect->x2*zoom_factor), 
                  (gint)(rect->y1*zoom_factor), 
                  2);
     }
     else {
     
      invertArea( (gint)(rect->x1*(-azoom_factor)), 
                  (gint)(rect->y2*(-azoom_factor)),
                  (gint)(rect->x2*(-azoom_factor)), 
                  (gint)(rect->y1*(-azoom_factor)), 
                  2);
     }
     
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