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
#include <sys/types.h>
#include <sys/stat.h>

#include "note.h"

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
         
  const char *contents = note->str;
  size_t len = strlen (contents);
  gtk_text_buffer_set_text (buffer, contents, len);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), buffer);
         
  g_signal_connect(G_OBJECT( buffer ), "changed",
     G_CALLBACK(textbuffer_changed_cb), note);
   
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
   
   //delete this comment
   
   gtk_widget_hide(note->comment);
   
   note->list.next->prev = note->list.prev;
   note->list.prev->next = note->list.next;
   list_del(&note->list);
   free(note->str);
   gtk_widget_destroy(note->comment);
   free(note);
       
  } // end of else if( result == GTK_RESPONSE_APPLY )

  gtk_widget_destroy (edit_win);
  
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
     
   int dp_width = (int)(zoom_factor*page_width);
     
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
   
   if( pre_left_right == 0 && left_right == 1 ){ // comment being moved from left page to right page
    
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
    struct note *tmp1 = NULL;
    struct list_head *q;
  
    list_for_each_safe(tmp, q, &current_nc->CM_HEAD){  
  
     tmp1= list_entry(tmp, struct note, list);
     
     if( tmp1 == note ){
      
      struct list_head *prev, *next;
      prev = tmp1->list.prev;
      next = tmp1->list.next;
      
      list_add_tail(&(note->list), &rcurrent_nc->CM_HEAD);
      
      prev->next = next;
      next->prev = prev;
      
      break;
     }
     
    }
    
   }
   else if( pre_left_right == 1 && left_right == 0 ){ // comment being moved from right page to left page
   
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
    struct note *tmp1 = NULL;
    struct list_head *q;
  
    list_for_each_safe(tmp, q, &rcurrent_nc->CM_HEAD){  
  
     tmp1= list_entry(tmp, struct note, list);
     
     if( tmp1 == note ){
      
      struct list_head *prev, *next;
      prev = tmp1->list.prev;
      next = tmp1->list.next;
      
      list_add_tail(&(note->list), &current_nc->CM_HEAD);
      
      prev->next = next;
      next->prev = prev;
      
      break;
     }
     
    }//end of list_for_each_safe(tmp, q, &rcurrent_nc->CM_HEAD)
  
   }
   
  }// end of if(dual_page_mode)
  
  if( da_height >= sw_alloc.height )
   note->y = ((gint)(event->y - lstart_y) + comment_alloc.y)/zoom_factor;
  else
   note->y = (gint)((event->y - lstart_y + comment_alloc.y - height_offset)/zoom_factor);
 
  if( da_width >= sw_alloc.width )
   note->x = ((gint)((event->x - lstart_x)) + comment_alloc.x)/zoom_factor;
  else{
   
   if( dual_page_mode ){
    
    int dp_width = (int)(zoom_factor*page_width);
    
    if(left_right && event->x > dp_width+1){
      
     note->x = (gint)((event->x - lstart_x + comment_alloc.x - (dp_width+1 ))/zoom_factor);
    }
    else if( left_right  ){ 
     
     note->x = (gint)((event->x - lstart_x + comment_alloc.x - (dp_width+1 ))/zoom_factor);
     
    }
    else{
     
     note->x = (gint)((event->x - lstart_x + comment_alloc.x)/zoom_factor);
    }
    
   }
   else{
    note->x = (gint)((event->x - lstart_x + comment_alloc.x - width_offset )/zoom_factor);
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
 
 cairo_set_source_rgba (cr, 1.0, 1.0, 0.0, 0.5);
 cairo_paint(cr);
 
 struct note *comment = (struct note *)user_data;
 
 GtkAllocation cm_alloc;
 gtk_widget_get_allocation (comment->comment, &cm_alloc);
 
 PangoLayout *layout;
 PangoFontDescription *font_description;

 font_description = pango_font_description_new ();
 pango_font_description_set_family (font_description, "serif");
 pango_font_description_set_weight (font_description, PANGO_WEIGHT_BOLD);
 pango_font_description_set_absolute_size (font_description, 11 * PANGO_SCALE*zoom_factor);
 
 layout = pango_cairo_create_layout (cr);
 pango_layout_set_font_description (layout, font_description);
 
 pango_layout_set_text (layout, comment->str, -1);
 
 cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
 cairo_move_to (cr, 0.0, 0.0);
 pango_cairo_show_layout (cr, layout);
 
 int width, height;
 
 pango_layout_get_pixel_size(layout ,&width, &height);
 
 gtk_widget_set_size_request(comment->comment, width, height);
 
 g_object_unref (layout);
 pango_font_description_free (font_description);

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
 
 comment_click = 0;
 
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

  if( fgets(line, 1000, pfile) != NULL ){
   
   char *page_num = strchr(line, ':');
   current_page_num = atoi(page_num+1)-1;
  
   page_change();
  }
  
  note_num = -1;
  
  struct note_cache* nc_tmp = NULL;
  struct note_cache* nc_ptr = NULL;
  
  int nc_num = 0;
  
  while ( fgets(line, 1000, pfile) != NULL) {
  
   char *pch0 = strchr(line, ':');
   *pch0 = '\0';
 
   //comment
   if(*line == '0'){

    //0:15:305:561:test
    char *pch = strchr(pch0+1, ':');
    char *pch1 = strchr(pch+1, ':');
    char *pch2 = strchr(pch1+1, ':');
    
    char *tmp = line;
    *pch = *pch1 = *pch2 = '\0';

    char new_line = 1;
    char *pch_tmp = strchr(pch2+1, new_line);
    
    while( pch_tmp ){
     *pch_tmp = '\n';
      pch_tmp = strchr(pch_tmp+1, new_line);
    }
    
    GtkWidget *label = gtk_drawing_area_new();
    
    size_t len = strlen(pch2+1);
    char * npch;
    int count = 0;
    
    char *lpch = strrchr(pch2+1,'\n');
    if(lpch)
     *lpch = '\0';
    
    npch=strchr(pch2+1,'\n');
    
    while (npch!=NULL){
     count++;
     npch=strchr(npch+1,'\n');
    }
   
    if(count == 1){
     gtk_widget_set_size_request (label, len*8*zoom_factor,14*zoom_factor);
    }
    else{
     gtk_widget_set_size_request (label, len*5*zoom_factor,14*zoom_factor*(count));
    }
    
    const char* format;
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ) 
     format = "<span foreground=\"yellow\" font=\"%d\">%s</span>";
    else
     format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), pch2+1);
    
    g_free(markup);
    
    gtk_layout_put (GTK_LAYOUT (layout), label, atoi(pch+1), atoi(pch1+1));

    struct note *note = (struct note*)malloc(sizeof(struct note));
    INIT_LIST_HEAD(&note->list);
    note->comment = label;
    note->x = atoi(pch+1);
    note->y = atoi(pch1+1);
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
     
     if( !nc_tmp ){ //
      
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
    
    gtk_widget_hide(label);

    if(note->page_num == current_page_num+1){
     gtk_widget_show(label);
    }
   } // end of if(*line == '0')
   else if( *line == '1' ){ //highlight region
  
   //highlight region format :
   // <highlight region id>:<page number>:color:x:y:x+width:y+height
   //1:1:00bfff:88:82:213:100

    char *pch = strchr(pch0+1, ':');
    char *pch1 = strchr(pch+1, ':');
    char *pch2 = strchr(pch1+1, ':');
    char *pch3 = strchr(pch2+1, ':');
    char *pch4 = strchr(pch3+1, ':');
    
    *pch = *pch1 = *pch2  = *pch3 = *pch4 = '\0';

    struct highlight_region *hg = (struct highlight_region *)malloc(sizeof(struct highlight_region));

    hg->x = atoi(pch1+1);
    hg->y = atoi(pch2+1);
    hg->width = atoi(pch3+1) - atoi(pch1+1);
    hg->height = atoi(pch4+1) - atoi(pch2+1);
    hg->color_name = (char *)malloc(6+1);
    
    strcpy(hg->color_name, pch+1);
    
    hg->page_num = atoi(pch0+1);
    
    GdkRGBA color;

    char *rgb = (char*)malloc(8);
    sprintf(rgb, "#%s", hg->color_name);

    gdk_rgba_parse(&color, rgb);
    
    if ( gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(inverted_colorMi) ) ){
     
     cairo_set_source_rgb (cr, 1, 1 , 1);
     cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
     
     cairo_rectangle (cr, hg->x,
                          hg->y, 
                          hg->width,
                          hg->height);
      
     cairo_fill (cr);
      
    }
    
    cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
    free(rgb);
    
    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
    
    cairo_rectangle (cr, hg->x,
                         hg->y, 
                         hg->width,
                         hg->height);
      
    cairo_fill (cr);
    
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
    
   } //end of if(*line == '1')

  } // end of while((read = getline(&line, &len, pfile)) != -1)
  
  page_change();
  
  fclose(pfile);
  
 } // end of if( stat(full_path, &buffer) == 0 )
 else{
  current_page_num = 0;
  page_change();
 }

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
  
  while( nc_tmp ){
  
   list_for_each(tmp, &nc_tmp->CM_HEAD){
  
    struct note *tmp1;
    tmp1= list_entry(tmp, struct note, list);
   
    fprintf(pfile, "%s\n", tmp1->str);
   
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
 
  fprintf(pfile, "last_visited_page:%d\n", current_page_num+1);
  
  struct list_head *tmp;
  
  struct note_cache *nc_tmp = NULL;
  
  if( note_cache )
   nc_tmp = note_cache->next;
  
  while( nc_tmp ){
   
   list_for_each(tmp, &nc_tmp->CM_HEAD){
    struct note *tmp1;
    tmp1= list_entry(tmp, struct note, list);
    
    const gchar *content = tmp1->str;
    
    fprintf(pfile, "%d:%d:%d:%d:", 0, tmp1->page_num, tmp1->x, tmp1->y);

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
   
   //highlight region
   list_for_each(tmp, &nc_tmp->HR_HEAD){

    struct highlight_region *tmp1;
    tmp1= list_entry(tmp, struct highlight_region, list);
   
    //1:1:00bfff:88:82:213:100
    fprintf(pfile, "%d:%d:%s:%d:%d:%d:%d\n", 1, tmp1->page_num, tmp1->color_name, tmp1->x, tmp1->y, tmp1->x + tmp1->width, tmp1->y + tmp1->height);
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

void 
add_comment_cb (GtkWidget* widget, gpointer data) {

 add_comment();
}

void display_comment(struct list_head *cm_HEAD){
 
  struct list_head *tmp;
  
  list_for_each(tmp, cm_HEAD){

   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
  
   if(!dual_page_mode){
    
     gtk_widget_show(tmp1->comment);
     gtk_widget_queue_draw (tmp1->comment);
     
     gtk_layout_move(GTK_LAYOUT(layout),
                     tmp1->comment,
                     (gint)(tmp1->x*zoom_factor)+width_offset,
                     (gint)(tmp1->y*zoom_factor)+height_offset);
   
   }else{ //dual-page mode
   
    if( &(*cm_HEAD) == &current_nc->CM_HEAD ){
    
     gtk_layout_move( GTK_LAYOUT(layout), 
                      tmp1->comment, 
                      (gint)(tmp1->x*zoom_factor), 
                      (gint)(tmp1->y*zoom_factor)+height_offset);
    
     gtk_widget_show(tmp1->comment);
    
    }
    else if( &(*cm_HEAD) == &rcurrent_nc->CM_HEAD ){
     
     int dp_width = (int)(zoom_factor*page_width);
     
     gtk_layout_move( GTK_LAYOUT(layout),
                      tmp1->comment,
                      (gint)(tmp1->x*zoom_factor)+dp_width+1,
                      (gint)(tmp1->y*zoom_factor)+height_offset);
     
     gtk_widget_show(tmp1->comment);
  
   }
   
   gtk_widget_hide(tmp1->comment);
   gtk_widget_show(tmp1->comment);
  
  }
  
 }//list_for_each(tmp, cm_HEAD

}