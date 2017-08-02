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

#include "search.h"

void find_exit_cb( GtkWidget *widget, gpointer user_data ){
  
 KEY_BUTTON_SEARCH = TRUE;
 
 gtk_widget_hide(findbar);
 gtk_entry_set_text(GTK_ENTRY(findtext), "");
 gtk_widget_grab_focus (scrolled_window);
 
 if( find_ptr ){
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
       
 char page_str[100];

 sprintf(page_str, "%s page %d/%d %s",file_name, current_page_num+1, poppler_document_get_n_pages(doc), "[S]");
 gtk_window_set_title(GTK_WINDOW(win), page_str);
       
 word_not_found = 1;

 pre_mode = mode;
 mode = TEXT_SELECTION;
 
 GdkScreen *screen = gdk_screen_get_default ();

 if( gdk_screen_get_height(screen) > (int)(page_height*zoom_factor) )
  zoom_height();
 
}

void find_text_cb( GtkWidget *widget, gpointer user_data ){

 KEY_BUTTON_SEARCH = FALSE;

 gtk_widget_show(findbar);
 gtk_widget_show(findPrev_button);
 gtk_widget_show(findNext_button);
 gtk_widget_show(find_exit_button);
 gtk_widget_grab_focus (findtext);
 
 if( find_ptr ){
         
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
 
 word_not_found = 1;
 
}

void search_slash( void ){

 if( find_ptr ){
         
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
 
 word_not_found = 1;
 
}

void search_n( GtkWidget *findbar ){
 
 if( mode == TEXT_HIGHLIGHT || mode == ERASE_TEXT_HIGHLIGHT ){
  mode = pre_mode;
  pre_mode = TEXT_SELECTION;
 }
 else if(mode == PAGE_CHANGE || mode == ZOOM_IN || mode == ZOOM_OUT || (mode == TEXT_SELECTION && pre_mode == TEXT_SEARCH_NEXT) || (mode == TEXT_SELECTION && pre_mode == TEXT_SEARCH_PREV)){
  mode = mode ^pre_mode;
  pre_mode = mode ^ pre_mode;  
  mode = mode ^ pre_mode;
 }
 
 if(mode == TEXT_SEARCH_NEXT){

  if(pre_keyval == GDK_KEY_N){ 
   
   mode = TEXT_SEARCH_PREV;
   pre_mode = TEXT_SELECTION;
   
   if(find_ptr_head)
    find_ptr_head = g_list_reverse(find_ptr_head);
         
   find_prev(findbar);
   pre_keyval = GDK_KEY_n;
  }
  else{

   if(pre_mode != ZOOM_IN &&  pre_mode != ZOOM_OUT)  
    pre_mode = TEXT_SELECTION;

   find_next(findbar);
  }

 }else if(mode == TEXT_SEARCH_PREV){
        
   if(pre_keyval == GDK_KEY_N){ 
     
    mode = TEXT_SEARCH_NEXT;
         
    pre_mode = TEXT_SELECTION;
     
    if(find_ptr_head)
     find_ptr_head = g_list_reverse(find_ptr_head);
     
     find_next(findbar);
     pre_keyval = GDK_KEY_n;
     
    }else{
       
      if(pre_mode != ZOOM_IN &&  pre_mode != ZOOM_OUT)
        pre_mode = TEXT_SELECTION;
       
      find_prev(findbar);
         
     }
 }// end of else if(mode == TEXT_SEARCH_PREV)
 
}

void search_N( GtkWidget *findbar ){
 
 if( (mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SELECTION) ||
     (mode == TEXT_SEARCH_NEXT && pre_mode == ZOOM_IN) ||
     (mode == TEXT_SEARCH_NEXT && pre_mode == ZOOM_OUT) ){
  
  pre_mode = mode;
  mode = TEXT_SEARCH_PREV;
 
  if(pre_mode == TEXT_SEARCH_NEXT && mode == TEXT_SEARCH_PREV){
   
   if(find_ptr_head && find_ptr)
    find_ptr_head = g_list_reverse(find_ptr_head);
   
   find_prev(findbar); 
  }
        
 }
 else if( (mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SELECTION) ||
          (mode == TEXT_SEARCH_PREV && pre_mode == ZOOM_IN) ||
          (mode == TEXT_SEARCH_PREV && pre_mode == ZOOM_OUT) ){
  
  pre_mode = mode;
  mode = TEXT_SEARCH_NEXT;
  
  if(pre_mode == TEXT_SEARCH_PREV && mode == TEXT_SEARCH_NEXT){
   
   if(find_ptr_head && find_ptr)
    find_ptr_head = g_list_reverse(find_ptr_head);
         
   find_next(findbar);
  }

 }
 else if( mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SEARCH_NEXT){
  //for consecutive N
        
  if( pre_keyval == GDK_KEY_slash ){
  
   pre_mode = mode;
   mode = TEXT_SEARCH_NEXT;
  }

  find_prev(findbar);
         
  } // end of if( mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SEARCH_NEXT )
  else if( mode == TEXT_SEARCH_PREV && pre_mode == TEXT_SEARCH_PREV ){
      
   pre_mode = mode;
   mode = TEXT_SEARCH_NEXT;

   find_next(findbar);
  } 
  else if( mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SEARCH_PREV ){
   //for consecutive N
   
   if( pre_keyval == GDK_KEY_question ){
    pre_mode = mode;
    mode = TEXT_SEARCH_PREV;
   }
   
   find_next(findbar);
         
   } 
   else if( mode == TEXT_SEARCH_NEXT && pre_mode == TEXT_SEARCH_NEXT){

    pre_mode = mode;
    mode = TEXT_SEARCH_PREV;

    find_prev(findbar);

   }

}

static int word_found(GList *matches){
 
 //return 0 for pattern matched, else pattern not matched
 
 if(!matches){
  return 1; //pattern not matched
 }
 
 GList *tmp;
 tmp = matches;
 
 if( mode == TEXT_SEARCH_NEXT ){
  
  if(current_page_num == poppler_document_get_n_pages(doc)){
   if(!lpage)
    current_page_num = -1;
   else
    current_page_num = -2;
  }
  
 }
 
 int not_found = 1;
 
   while( tmp ){
    PopplerRectangle *rect = tmp->data;
    PopplerRectangle tmp_rect;
    
    if( (gint)rect->y2 < (gint)rect->y1 ){
     
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
    
    }else if( (gint)rect->y2 > (gint)rect->y1 ) {
   
     rect->y1 = page_height - rect->y1;
     rect->y2 = page_height - rect->y2;
   
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
   
    }
    
    char *find_text;
    
    if( !lpage ){ //single page mode 
     
      if( mode == TEXT_SEARCH_NEXT ){
       
       if(current_page_num == -1){
        
        page = poppler_document_get_page(doc, 0);
        
       }else{
        
        if( current_page_num ==  poppler_document_get_n_pages(doc) - 1){
         page = poppler_document_get_page(doc, 0);
        }else{
         page = poppler_document_get_page(doc, current_page_num+1);//check
        }
        
       }
       
      }
      else if ( mode == TEXT_SEARCH_PREV ){
       
       if(current_page_num == -1){ 
        page = poppler_document_get_page(doc, poppler_document_get_n_pages(doc) -1);
       }else{
        
        if( current_page_num != 0 ){
         
         page = poppler_document_get_page(doc, current_page_num);
         
        }else{ //current_page_num == 0
         
         page = poppler_document_get_page(doc, 0);
         
        }
        
       }
      }
      
     find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    }
    else{ //dual-page mode
     
     gint pos = g_list_position(matches, tmp);
     
     gint lr_pos;
     
     if( mode == TEXT_SEARCH_PREV )
      if( lmatches )
       lr_pos = g_list_position(matches, lmatches);
      else
       lr_pos = g_list_position(matches, rmatches);
     else if ( mode == TEXT_SEARCH_NEXT )
      if( rmatches )
       lr_pos = g_list_position(matches, rmatches);
      else 
       lr_pos = g_list_position(matches, lmatches);
     
     if( lr_pos == -1 )
      lr_pos = 0;
     
     if(mode == TEXT_SEARCH_PREV){
     
      if( pos <= g_list_position(matches, rmatches) ){ // right page
      
       if(pos != -1){
       
        if(!page || lm_PageImage ){
        
         if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
          page = poppler_document_get_page(doc, current_page_num);
         }else{
         
          if( mode == TEXT_SEARCH_NEXT && current_page_num == -1 ){ //check
           
           page = poppler_document_get_page(doc, current_page_num+1);
           
          }
          else{
           page = poppler_document_get_page(doc, current_page_num+1);
          
          }
         
         } 
        
        }
        find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
       }
  
      }
      else{ //left page
      
       //check
       if(lpage){
        g_object_unref (G_OBJECT (lpage));
       }
       //check
       
       if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
        lpage = poppler_document_get_page(doc, current_page_num-1);
       }else{
       
        if( mode == TEXT_SEARCH_NEXT && current_page_num == -1 ){
         
         lpage = poppler_document_get_page(doc, current_page_num);
         
        }
        else{
         lpage = poppler_document_get_page(doc, current_page_num);
        
        }
       } 
      
       find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
      }
     }//end of if( mode == TEXT_SEARCH_PREV) 
     else if( mode == TEXT_SEARCH_NEXT){
     
      int lr_page = 0;
      
      gint temp_pos = 0;
      
      if( lmatches && rmatches )
       temp_pos = g_list_position(matches, lmatches);
      
      if( !lmatches && rmatches )
       temp_pos = lr_pos;
      
      if( pos >= g_list_position(matches, rmatches) ){ 
      
       if( !rmatches && lmatches ){ //rmatches is NULL, noting found in right page
        
        if(lpage){
         g_object_unref (G_OBJECT (lpage));
        }
      
        if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
         lpage = poppler_document_get_page(doc, current_page_num-1);
        }else{
       
         if( mode == TEXT_SEARCH_NEXT && current_page_num == -1 ){
          
          lpage = poppler_document_get_page(doc, current_page_num);
          
         }
         else{
          lpage = poppler_document_get_page(doc, current_page_num);
          
         }
        } 
      
        find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
       
       }
       else{ // right page
       
       
        if(pos != -1){
       
         if(!page || lm_PageImage ){
        
          if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
           page = poppler_document_get_page(doc, current_page_num);
          }else{
         
           if( mode == TEXT_SEARCH_NEXT && current_page_num == -1 ){ //check
            
            page = poppler_document_get_page(doc, current_page_num+1);
           
           }
           else{
            page = poppler_document_get_page(doc, current_page_num+1);
           
           }
         
          } 
        
         }
         find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
        }
       }
       
      }
      else{ //left page
      
       if(lpage){
        g_object_unref (G_OBJECT (lpage));
       }
      
       if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
        lpage = poppler_document_get_page(doc, current_page_num-1);
       }else{
       
        if( mode == TEXT_SEARCH_NEXT && current_page_num == -1 ){
         
         lpage = poppler_document_get_page(doc, current_page_num);
         
        }
        else{
         lpage = poppler_document_get_page(doc, current_page_num);
         
        }
       } 
      
       find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
       
      }
     
     }

    }
    
    if(page){
     g_object_unref (G_OBJECT (page));
     page = NULL;
    }
    
    if(find_text){
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch){
      
      not_found = 0;
      
      break;
     
     }else{
      tmp = tmp->next;
     }
     
    } // end of if(find_text
    
   }//end of while( find_ptr )
  
  return not_found;
   
}

void find_prev( GtkWidget *findbar ){

 GtkAdjustment *vadj =
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

 if( !strlen(gtk_entry_get_text(GTK_ENTRY(findtext))) ){

  if(find_ptr){
   
   invert_search_region();
   find_ptr = NULL;
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
  }

  return;
 }
 
 if(!find_ptr){ //find_ptr is null
  
  GList *matches;
  
  if(!lpage){ //single-page mode
   if(page)
    g_object_unref (G_OBJECT (page));
 
   page = poppler_document_get_page(doc, current_page_num);
   
   matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

   matches = g_list_reverse(matches);
  }
  else{ //dual-page mode
   
   if(page)
    g_object_unref (G_OBJECT (page));
    
   if(lpage)
    g_object_unref (G_OBJECT (lpage));
   
   if( current_page_num == poppler_document_get_n_pages(doc)-1 ){
    lpage = poppler_document_get_page(doc, current_page_num-1);
    page = poppler_document_get_page(doc, current_page_num);
   }else{
    lpage = poppler_document_get_page(doc, current_page_num);
    page = poppler_document_get_page(doc, current_page_num+1);
   }
   
   lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
   rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
   
   guint l_len = g_list_length(lmatches);
   guint r_len = g_list_length(rmatches);
   
   matches = g_list_concat(lmatches, rmatches);
   
   guint lpos = g_list_position(matches, lmatches);
   guint rpos = g_list_position(matches, rmatches);
   
   matches = g_list_reverse(matches);
   
   lpos = g_list_position(matches, lmatches);
   rpos = g_list_position(matches, rmatches);
   
   guint t_len = g_list_length(matches);
   
   g_object_unref (G_OBJECT (page));
   page = NULL;
   
  }
  
  if( !word_found(matches) )
   word_not_found = 0;
  else
   if(!matches)
    if(!lpage)
     word_not_found++;
    else
     word_not_found = word_not_found + 2;
  
  find_ptr = matches;
  find_ptr_head = matches;

  if(find_ptr){
   while( find_ptr ){
    PopplerRectangle *rect = find_ptr->data;
    PopplerRectangle tmp_rect;
   
    if( (gint)rect->y2 < (gint)rect->y1 ){
     
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
    
    }else if( (gint)rect->y2 > (gint)rect->y1 ) {
   
     rect->y1 = page_height - rect->y1;
     rect->y2 = page_height - rect->y2;
   
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
   
    }
    
    char *find_text= NULL;
    
    if( !lpage ){ //single page mode 
     
     if(!page)
      page = poppler_document_get_page(doc, current_page_num);
   
     find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    }
    else{ //dual-page mode
     
     gint pos = g_list_position(find_ptr_head, find_ptr);
     
     if( pos <= g_list_position(find_ptr_head, rmatches) ){ // right page 
      
      if(pos != -1){
       
       if(!page){
        
        if( current_page_num == poppler_document_get_n_pages(doc) - 1){
         page = poppler_document_get_page(doc, current_page_num);
        }else{
         page = poppler_document_get_page(doc, current_page_num+1);
        }
        
       }
       
       find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
      }
      
     }
     else{ //left page
     
      if(lpage)
       g_object_unref (G_OBJECT (lpage));
      
      if( current_page_num == poppler_document_get_n_pages(doc) - 1)
       lpage = poppler_document_get_page(doc, current_page_num-1);
      else
       lpage = poppler_document_get_page(doc, current_page_num);
      
      find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
     } 

    }
    
    if(page){
     g_object_unref (G_OBJECT (page));
     page = NULL;
    }
    
    if(find_text){
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch){
      
      break;
     
     }else{
      
      find_ptr = find_ptr->next;
     }
     
    } // end of if(find_text
   
   }//end of while( find_ptr )
  }

 }else{ // find_ptr is not null
 
 invert_search_region();

 find_ptr = find_ptr->next;
 
 if(find_ptr){
 
  while( find_ptr ){
   PopplerRectangle *rect = find_ptr->data;
   PopplerRectangle tmp_rect;
   
   if( (gint)rect->y2 < (gint)rect->y1 ){
    
    tmp_rect.x1 = rect->x1;
    tmp_rect.y1 = rect->y2;
    tmp_rect.x2 = rect->x2;
    tmp_rect.y2 = rect->y1;
    
   }else if( (gint)rect->y2 > (gint)rect->y1 ) {
   
    rect->y1 = page_height - rect->y1;
    rect->y2 = page_height - rect->y2;
   
    tmp_rect.x1 = rect->x1;
    tmp_rect.y1 = rect->y2;
    tmp_rect.x2 = rect->x2;
    tmp_rect.y2 = rect->y1;
   
   }
   
   char *find_text;
   
   if(!lpage){
    if(!page)
     page = poppler_document_get_page(doc, current_page_num);
    
    find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    
   }
   else{ //dual-page mode
   
    gint pos = g_list_position(find_ptr_head, find_ptr);
   
    if( pos <= g_list_position(find_ptr_head, rmatches) ){ // right page 
    
     if(pos != -1){
      if(!page)
       page = poppler_document_get_page(doc, current_page_num+1);
    
      find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
     }
     else
      find_text = NULL;
    }
    else{ //left page
     
     if(lpage)
      g_object_unref (G_OBJECT (lpage));
     
     lpage = poppler_document_get_page(doc, current_page_num);
     
     find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
    } 
   
   }
   
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   if(find_text){
    
    char * pch;
    pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
    
    if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch ){
      
     break;
    }else{
      
     find_ptr = find_ptr->next;
    }
     
   }// end of if(find_text)
   
  }//end of while( find_ptr )
 }

} // end of if(!find_ptr) else

 if(find_ptr){
  
  invert_search_region();
  
 }else{
  
  if(find_ptr_head){
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
  }
 
  GList *matches;
  char *find_text;
  
  if(!lpage){ // single-page mode
   
   if(current_page_num  == 0){
   
    current_page_num = poppler_document_get_n_pages(doc)-1;
    page = poppler_document_get_page(doc, current_page_num);
    
   }else{
    page = poppler_document_get_page(doc, current_page_num-1);
   }
   matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
  
  }
  else{ // dual-page mode
   if(current_page_num  == 0)
    current_page_num = poppler_document_get_n_pages(doc);
   
   if(page)
    g_object_unref (G_OBJECT (page));
    
   if(lpage)
    g_object_unref (G_OBJECT (lpage));
   
   if(current_page_num >= 2){
   
    lpage = poppler_document_get_page(doc, current_page_num-2);
     page = poppler_document_get_page(doc, current_page_num-1);
     
   }
   else{ //current_page_num == -1 or -2
    
    gint page_num = poppler_document_get_n_pages(doc);
    
    lpage = poppler_document_get_page(doc, page_num-2);
     page = poppler_document_get_page(doc, page_num-1);
    
   }
   
   lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
   rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
   
   matches = g_list_concat(lmatches, rmatches);
   
   //match for previous two pages
   matches = g_list_reverse(matches);
   
   if(current_page_num >= 2 )
     current_page_num = current_page_num -2;
   else
    current_page_num = poppler_document_get_n_pages(doc)-2;
  
  }
   
   if( !word_found(matches) ){ // check if previous page contains strings that match the searching string
    word_not_found = 0;
   
    if( !lpage ){
     current_page_num--;
    }
    
   }
   
   if( page ){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   while( word_found(matches) ){ // when there is no pattern matched
    //return 0 for pattern matched, else pattern not matched
    
    if(!lpage){
     if( current_page_num != -1 ){
      current_page_num--;
      
      if(current_page_num == -1)
       current_page_num = poppler_document_get_n_pages(doc)-1;
     }
     
    }
    else{
     
     if( current_page_num != -2 ){
      current_page_num = current_page_num-2;
     
      if(current_page_num == -2)
       current_page_num = poppler_document_get_n_pages(doc)-2;
     
     } 
    
    }
    
    if(!lpage)
     word_not_found++;
    else
     word_not_found = word_not_found +2;
    
    if(!lpage){
     if(current_page_num == -1) //check
      current_page_num = 0;
    }
    else{
     if( current_page_num == -2 || current_page_num == -1 )
      current_page_num = 0;
     
    }
    
    if( current_page_num ==  find_current_page_num )
     break;
    
    if(current_page_num == 0 && lm_PageImage ){
     
     if(page)
      g_object_unref (G_OBJECT (page));
    
     if(lpage)
      g_object_unref (G_OBJECT (lpage));

     lpage = poppler_document_get_page(doc, current_page_num);
     page = poppler_document_get_page(doc, current_page_num+1);
 
     lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
     rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

     matches = g_list_concat(lmatches, rmatches);

     matches = g_list_reverse(matches);
     
     if( !word_found(matches) )
      break;
     
    }
    
    if(page)
      g_object_unref (G_OBJECT (page));
    
    if(!lpage){ //single-page mode 
     page = poppler_document_get_page(doc, current_page_num);
    
     matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
    }
    else{ //dual-page mode
    
     if(page)
      g_object_unref (G_OBJECT (page));
    
     if(lpage)
      g_object_unref (G_OBJECT (lpage));

     lpage = poppler_document_get_page(doc, current_page_num);
     page = poppler_document_get_page(doc, current_page_num+1);
 
     lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
     rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

     matches = g_list_concat(lmatches, rmatches);

     matches = g_list_reverse(matches);
     
    }
    
    if(page){
     g_object_unref (G_OBJECT (page));
     page = NULL;
    }
    
    if(!lpage){ // single-page
     if(current_page_num == poppler_document_get_n_pages(doc)-1 && find_current_page_num == 0)
      break;
     else if( current_page_num  == find_current_page_num+1 ){
      if( !word_found(matches) )
       word_not_found = 0;
      break;
     }
    }
    else{ // dual-page
    
     if(current_page_num == poppler_document_get_n_pages(doc)-2 && find_current_page_num == 0){
      break;
     }
     else if( current_page_num  == find_current_page_num+2 ){
     
      if( !word_found(matches) )
       word_not_found = 0;
      else{
       
       if(!lpage)
        word_not_found++;
       else
        word_not_found = word_not_found +2;
      }
      
      break;
     }
     else if( current_page_num+1  == find_current_page_num ){
      break;
     }
     else if( current_page_num  == find_current_page_num ){
      break;
     }
     else if( word_not_found == poppler_document_get_n_pages(doc)-1 ){
      break;
     }
     
    }
    
    if(matches){
     
     find_ptr_head = matches;
     
     if( !word_found(matches) ){
      word_not_found = 0;
     }
     
    }
    
   } // end of while(!matches)
   
   if(word_not_found){
   
    if(word_not_found  == poppler_document_get_n_pages(doc)) {
    // there is no pattern matched in this pdf file
    
     current_page_num = find_current_page_num;
     page_change();
   
    }
    else{
     
     if(!lpage){
      
      if( current_page_num  == find_current_page_num+1 )
       current_page_num = find_current_page_num;
      
      page_change();
      
      if( word_not_found < poppler_document_get_n_pages(doc) - 1 )
       find_prev(findbar);
     
     }else{
      
      gint page_num = poppler_document_get_n_pages(doc);
      
      if( current_page_num  == find_current_page_num+2 || 
          current_page_num+1  == find_current_page_num || 
          word_not_found == page_num -1 || 
          word_not_found == page_num +1 ){
       
       current_page_num = find_current_page_num;
      
      }
      
      page_change();
      
      if( word_not_found != page_num && 
          word_not_found != page_num+2 && 
          word_not_found+2 != page_num && 
          word_not_found != page_num-1 && 
          word_not_found != page_num+1){
      
        find_prev(findbar);
        
      }
      
     }
     
    }
   }
   else{  //there are patterns matched in the previous page
    
    page_change();
    
    find_prev(findbar);
    
   }
   
 } // end of if(find_ptr) else 
 
}

void find_next( GtkWidget *findbar ){
 
 GtkAdjustment *vadj =
     gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));

 if( !strlen(gtk_entry_get_text(GTK_ENTRY(findtext))) ){
  
  if(find_ptr){ 
   
   invert_search_region();
   find_ptr = NULL;
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
  }

  return;
 }

 if( !find_ptr ){ //find_ptr is NULL
  
  GList *matches;
  
  if(!lpage){
   
   if(page)
    g_object_unref (G_OBJECT (page));

   page = poppler_document_get_page(doc, current_page_num);

   matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
  }
  else{ //dula-page mode
  
   if(page)
    g_object_unref (G_OBJECT (page));
    
   if(lpage)
    g_object_unref (G_OBJECT (lpage));
   
   if( current_page_num != -1 ){
    lpage = poppler_document_get_page(doc, current_page_num);
    page = poppler_document_get_page(doc, current_page_num+1);
   }
   else{
    lpage = poppler_document_get_page(doc, 0);
    page = poppler_document_get_page(doc, 1);
   }
   
   lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
   rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
   
   guint l_len = g_list_length(lmatches);
   guint r_len = g_list_length(rmatches);
   
   matches = g_list_concat(lmatches, rmatches);
   guint t_len = g_list_length(matches);
   
   g_object_unref (G_OBJECT (page));
   page = NULL;
   
  }
  
  word_not_found = word_found(matches);
  
  find_ptr = matches;
  find_ptr_head = matches;
  
   while( find_ptr ){
    PopplerRectangle *rect = find_ptr->data;
    PopplerRectangle tmp_rect;
   
    if( (gint)rect->y2 < (gint)rect->y1 ){

     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
    
    }else if( (gint)rect->y2 > (gint)rect->y1 ) {
     
     rect->y1 = page_height - rect->y1;
     rect->y2 = page_height - rect->y2;
   
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
   
    }
    
    char *find_text;
    
    if( !lpage ){ //single page mode 
     
     if(!page)
      page = poppler_document_get_page(doc, current_page_num);
   
     find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    }
    else{ //dual-page mode
    
     if(find_ptr != rmatches || rmatches == NULL){ //left page
      
      g_object_unref (G_OBJECT (lpage));
      
      if(current_page_num != -1)
       lpage = poppler_document_get_page(doc, current_page_num);
      else
       lpage = poppler_document_get_page(doc, 0);
      
      find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
      
     }
     else{ //right page, this block could not be reached
      
      if(!page)
       page = poppler_document_get_page(doc, current_page_num+1);
      
      find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
      
     }//this block could not be reached
    
    }
    
    if( find_text ){
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch ){
      //match the pattern and break
      break;
     }else{
      
      find_ptr = find_ptr->next;
     }
     
    } //end of if( find_text 
    
    if(page){
     g_object_unref (G_OBJECT (page));
     page = NULL;
    }
    
   }//end of while( find_ptr )
 
 }
 else{ //find_ptr is not NULL
  
  guint f_len = g_list_length(find_ptr);
 
  invert_search_region();
  
  find_ptr = find_ptr->next;
  
   while(find_ptr){
    PopplerRectangle *rect = find_ptr->data;
   
    PopplerRectangle tmp_rect;
   
    if( (gint)rect->y2 < (gint)rect->y1 ){

     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
    
    }else if( (gint)rect->y2 > (gint)rect->y1 ) {
   
     rect->y1 = page_height - rect->y1;
     rect->y2 = page_height - rect->y2;
   
     tmp_rect.x1 = rect->x1;
     tmp_rect.y1 = rect->y2;
     tmp_rect.x2 = rect->x2;
     tmp_rect.y2 = rect->y1;
   
    }
    
    gint pos = g_list_position(find_ptr_head, find_ptr);
    
    char *find_text;
    
    if(!lpage){
     
     //find_ptr is not null, so you cannot get next page's page
     if(!page)
      page = poppler_document_get_page(doc, current_page_num);
      
     find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    
    }
    else{
     
     if( pos < g_list_position(find_ptr_head, rmatches) || rmatches == NULL ){ // left page 
     
      g_object_unref (G_OBJECT (lpage));
      lpage = poppler_document_get_page(doc, current_page_num);
     
      find_text = poppler_page_get_selected_text(lpage, POPPLER_SELECTION_GLYPH, &tmp_rect);
     }
     else{ //right page
    
      if(!page)
       page = poppler_document_get_page(doc, current_page_num+1);
      
      find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
     
     }
    }
    
    if( find_text ){ 
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) )  || pch ){
      //match the pattern
      break;
     }else{
      
      find_ptr = find_ptr->next;
     }
    } // end of if( find_text )
    
    if(page){
     g_object_unref (G_OBJECT (page));
     page = NULL;
    }
    
   }// end of while(find_ptr)
  
 } // end of find_ptr is not NULL

 if(find_ptr){
 
  invert_search_region();
  
 }else{ //end of the find_ptr_head, so keep searching following pages
  
  if(find_ptr_head){
   
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
   
  }
  
  //if text-search goes to last page, go back to first page 
  if(!lpage){
   if(current_page_num == poppler_document_get_n_pages(doc)-1)
    current_page_num = -1;
  }
  else{ //dual-page mode
  
   if(current_page_num == poppler_document_get_n_pages(doc)-2)
    current_page_num = -1;
    
  }
  
  //get the information of next page
  
  GList *matches;
  
  if(!lpage){
  
   if(page){
    g_object_unref (G_OBJECT (page));
    page = NULL;
   }
   
   page = poppler_document_get_page(doc, current_page_num+1);
   
   matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
   
  }
  else{ 
   
   if( current_page_num == -1 )
    current_page_num = 0;
   else{
    current_page_num = current_page_num + 2;
    
    if( current_page_num == poppler_document_get_n_pages(doc) -1 )
     current_page_num = 0;
    
   }
   
   if(page)
    g_object_unref (G_OBJECT (page));
    
   if(lpage)
    g_object_unref (G_OBJECT (lpage));
   
   lpage = poppler_document_get_page(doc, current_page_num);
   page = poppler_document_get_page(doc, current_page_num+1);
   
   lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
   rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

   matches = g_list_concat(lmatches, rmatches);

  }
  
  if( !word_found(matches) )
   word_not_found = 0;
  else
   if(!matches)
    if(!lpage)
     word_not_found++;
    else
     word_not_found = word_not_found + 2;
 
  if(page){
   g_object_unref (G_OBJECT (page));
   page = NULL;
  }
  
  //if we cannot match any word in the next page
  //while(!matches ||  matches && word_not_found ){
  while( word_found(matches) ){ // when there is no pattern matched
   
   if(!lpage)
    word_not_found++;
   else
    word_not_found = word_not_found +2;
   
   if( word_not_found == poppler_document_get_n_pages(doc)+2){
    break;
   }
   
   if(!lm_PageImage)
    current_page_num++;
   else{
    current_page_num = current_page_num + 2;
    if( current_page_num == poppler_document_get_n_pages(doc) - 1 )
     current_page_num = 0;
   }
   
   //if we go to the last page, got back to page 1
   //if(current_page_num == poppler_document_get_n_pages(doc)){
   if(current_page_num == poppler_document_get_n_pages(doc)){
    current_page_num = 0;
   }
   
   //if we got back to the page that users start to search and cannot match any word
   if( current_page_num ==  find_current_page_num && word_not_found){
    break;
   }
   
   //if we go to the last page, got back to page 1
   if(current_page_num == poppler_document_get_n_pages(doc) - 1 ){
    
    current_page_num = -1; 
    
    if( word_not_found == 1 )
     break;
   }
   
   
   
   if(!lpage){
    page = poppler_document_get_page(doc, current_page_num+1);
    matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
   }
   else{
    
    if(page)
     g_object_unref (G_OBJECT (page));
    
    if(lpage)
     g_object_unref (G_OBJECT (lpage));

    lpage = poppler_document_get_page(doc, current_page_num);
    page = poppler_document_get_page(doc, current_page_num+1);
 
    lmatches = poppler_page_find_text (lpage, gtk_entry_get_text(GTK_ENTRY(findtext)));
    rmatches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

    matches = g_list_concat(lmatches, rmatches);

   }
   
   g_object_unref (G_OBJECT (page));
   page = NULL;
   
   if(matches){
    find_ptr_head = matches;
    
    if(lpage){
    
     if( !word_found(matches) ){
      word_not_found = 0;
      break;
     }else
      word_not_found = word_not_found +2;
     
     if( word_not_found == poppler_document_get_n_pages(doc)+2){
      break;
     }
     
    }
    
   }
   
   
  } // end of while(!matches)
  
  if(current_page_num != -1){
   
   if( current_page_num != find_current_page_num ){
    
    if( word_not_found ){
     
     if( word_not_found != poppler_document_get_n_pages(doc)+2){
      
      next_page();
      find_ptr = NULL;
      find_next(findbar);
      
     }
     else{
      current_page_num = find_current_page_num;
      page_change();
     }
     
    }else{ // word_not_found = 0
    
     if(lm_PageImage){
      if(current_page_num != poppler_document_get_n_pages(doc)-3)
       current_page_num = current_page_num - 2;
     }
     next_page();
      
     find_ptr = NULL;
     
     find_next(findbar);
      
    }
    
   }
   else{ //current_page_num == find_current_page_num
    
    if( !word_not_found ){
     
     if(!lm_PageImage)
      next_page();
     else
      page_change();
     
     find_ptr = NULL;
   
     find_next(findbar);
    }
    else{ //end of if( word_not_found)
     
     if( word_not_found == poppler_document_get_n_pages(doc)-1 || 
         word_not_found == poppler_document_get_n_pages(doc) ){
      
      page_change();
      
     }else{
      
      if( current_page_num != find_current_page_num ){
      
       next_page();
     
       find_ptr = NULL;
       find_next(findbar);
      }
      else{
       
       if( word_not_found == poppler_document_get_n_pages(doc)-1 ||
           word_not_found == poppler_document_get_n_pages(doc)+1 || 
           word_not_found == poppler_document_get_n_pages(doc)+2){
        
        page_change();
       }else{
       
        next_page();
     
        find_ptr = NULL;
        find_next(findbar);
       }
      
      }
      
     }
    
    }
    
   }
  } // end of if(current_page_num != -1)
  else{ //if(current_page_num == -1)
   
   if(current_page_num == -1)
    current_page_num = 0;   
   
   if( current_page_num == find_current_page_num )
    page_change();
   else
    find_next(findbar);
   
  }
  
 } // end of if(find_ptr) else

}