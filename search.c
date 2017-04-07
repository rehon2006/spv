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
       
 word_not_found = TRUE;

 pre_mode = mode;
 mode = TEXT_SELECTION;
 
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
 
 word_not_found = TRUE;
 
}

void search_slash( void ){

 if( find_ptr ){
         
  invert_search_region();
  g_list_free(find_ptr_head);
  find_ptr_head = NULL;
  find_ptr = NULL;
 }
 
 word_not_found = TRUE;
 
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

 if(!find_ptr){
 
  if(page)
   g_object_unref (G_OBJECT (page));
 
  page = poppler_document_get_page(doc, current_page_num);
  GList *matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

  matches = g_list_reverse(matches);

  if( matches )
   word_not_found = FALSE;

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
   
    if(!page)
     page = poppler_document_get_page(doc, current_page_num);

    char *find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    if(find_text){
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch){
      
      break;
     }else{
      
      find_ptr = find_ptr->next;
     }
     
    } // end of if(find_text
    
    g_object_unref (G_OBJECT (page));
    page = NULL;
   
   }//end of while( find_ptr )
  }

 }else{

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
   
    if(!page)
     page = poppler_document_get_page(doc, current_page_num);
     
    char *find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    if(find_text){
    
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
    
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch ){
      
      break;
     }else{
      
      find_ptr = find_ptr->next;
     }
     
    }// end of if(find_text)
    
    g_object_unref (G_OBJECT (page));
    page = NULL;
   
   }//end of while( find_ptr )
  }

 } // end of if(!find_ptr) else

 if(find_ptr){
  
  if( pre_mode == TEXT_SEARCH_PREV ){
 
   invert_search_region();
   
  }
  else if( pre_mode == TEXT_SEARCH_NEXT ){
   
   invert_search_region();

  }// end of if( pre_mode == TEXT_SEARCH_NEXT )
  else{
	
   invert_search_region();

  }
 }else{

  if(find_ptr_head){
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
  }

  if(current_page_num  == 0)
   current_page_num = poppler_document_get_n_pages(doc);
  
   page = poppler_document_get_page(doc, current_page_num-1);
   
   GList *matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

   if( matches )
    word_not_found = FALSE;

   g_object_unref (G_OBJECT (page));
   page = NULL;

   while(!matches){
    current_page_num--;

    if(current_page_num == -1){
     current_page_num = 0;
     break;
    }
    

    if( current_page_num ==  find_current_page_num && word_not_found){
     break;
    }

    if( current_page_num == 0 ){
     current_page_num = poppler_document_get_n_pages(doc);
    }

    page = poppler_document_get_page(doc, current_page_num-1);
    matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

    g_object_unref (G_OBJECT (page));
    page = NULL;
    
    if(matches){
     find_ptr_head = matches;
     word_not_found = FALSE;
    }
   } // end of while(!matches)


   if( current_page_num != 0 ){
   
    if( current_page_num != find_current_page_num ){
     
     prev_page();
     find_ptr = NULL;
   
     find_prev(findbar);
    }
    else{
     
     if( !word_not_found ){
      prev_page();
      find_ptr = NULL;
     
      find_prev(findbar);
     }
    }
    
   }// end of if( current_page_num != 0 )
   else{
   
    if( !word_not_found ){
     
     current_page_num = poppler_document_get_n_pages(doc)-1;
     page_change();
     
     find_prev(findbar);
     
    }
    else{
     
     current_page_num = find_current_page_num;
     page_change();
     
    }

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

 if( !find_ptr ){

  if(page)
   g_object_unref (G_OBJECT (page));

  page = poppler_document_get_page(doc, current_page_num);

  GList *matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));
 
  if( matches )
   word_not_found = FALSE;
 
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
   
    if(!page)
     page = poppler_document_get_page(doc, current_page_num);
   
    char *find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    if( find_text ){
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) ) || pch ){
      
      break;
     }else{
      
      find_ptr = find_ptr->next;
     }
     
    } //end of if( find_text 
   
    g_object_unref (G_OBJECT (page));
    page = NULL;
   
   }//end of while( find_ptr )
  }
  
 }
 else{
  
  invert_search_region();
  
  find_ptr = find_ptr->next;
  
  if( find_ptr ){
   
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
   
    if(!page)
     page = poppler_document_get_page(doc, current_page_num);
    
    char *find_text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH, &tmp_rect);
    
    if( find_text ){ 
     
     char * pch;
     pch = strstr(find_text, gtk_entry_get_text(GTK_ENTRY(findtext)));
     
     if( ! strcmp( find_text, gtk_entry_get_text(GTK_ENTRY(findtext)) )  || pch ){
      
      break;
     }else{
     
      find_ptr = find_ptr->next;
     }
    } // end of if( find_text )
    
    g_object_unref (G_OBJECT (page));
    page = NULL;
   
   }// end of while(find_ptr)
  }
  
 }

 if(find_ptr){
  
  if( pre_mode == TEXT_SEARCH_PREV ){
	
   invert_search_region();

  }
  else if( pre_mode == TEXT_SEARCH_NEXT ){
   
   invert_search_region();

  }
  else{
	
   invert_search_region();
 
  }

 }else{

  if(find_ptr_head){
   g_list_free(find_ptr_head);
   find_ptr_head = NULL;
  }
  
  if(current_page_num == poppler_document_get_n_pages(doc)-1)
   current_page_num = -1;

  page = poppler_document_get_page(doc, current_page_num+1);
  GList *matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

  if( matches )
   word_not_found = FALSE;

  g_object_unref (G_OBJECT (page));
  page = NULL;

  while(!matches){
   current_page_num++;

   if(current_page_num == poppler_document_get_n_pages(doc)){
    current_page_num = -1;
    break;
   }

   if( current_page_num ==  find_current_page_num && word_not_found){
    break;
   }
   
   if(current_page_num == poppler_document_get_n_pages(doc) - 1 ){
    current_page_num = -1; 
    break;
   }

   page = poppler_document_get_page(doc, current_page_num+1);
   matches = poppler_page_find_text (page, gtk_entry_get_text(GTK_ENTRY(findtext)));

   g_object_unref (G_OBJECT (page));
   page = NULL;

   if(matches){
    find_ptr_head = matches;
    word_not_found = FALSE;
   }
  } // end of while(!matches)

  if(current_page_num != -1){
  
   if( current_page_num != find_current_page_num ){
   
    next_page();
    find_ptr = NULL;
   
    find_next(findbar);
   }
   else{ // if( current_page_num == find_current_page_num 
    
    if( !word_not_found ){
    
     next_page();
     find_ptr = NULL;
   
     find_next(findbar);
    } 
    
   }
  } // end of if(current_page_num != -1)
  else{
   
   if(!word_not_found){
    
    current_page_num = 0;   
    page_change();
    
    find_next(findbar);
   }
   else{
    
     current_page_num = find_current_page_num;  
     page_change();
     
   }
  }
  
 } // end of if(find_ptr) else

}