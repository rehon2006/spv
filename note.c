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

  while ( fgets(line, 1000, pfile) != NULL) {
  
   char *pch0 = strchr(line, ':');
   *pch0 = '\0';
   
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
    
    g_signal_connect (G_OBJECT (label), "draw", 
                   G_CALLBACK (comment_draw_cb), 
                   note);
    
    list_add_tail(&(note->list), &NOTE_HEAD);

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
   
    cairo_set_source_rgb (cr, color.red, color.green, color.blue);
    
    free(rgb);
    
    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
    
    list_add(&(hg->list), &HR_HEAD);

    if( hg->page_num ==  current_page_num + 1 ){
     
     if( zoom_factor == 1.0 ){
     
      cairo_rectangle (cr, atoi(pch1+1),
                           atoi(pch2+1), 
                           atoi(pch3+1),
                           atoi(pch4+1));
      
      
   
      cairo_fill (cr);
                            
     }
     else{
     
      cairo_rectangle (cr, (gint)(atoi(pch1+1)*zoom_factor),
                           (gint)(atoi(pch1+2)*zoom_factor), 
                           (gint)(atoi(pch1+3)*zoom_factor),
                           (gint)(atoi(pch1+4)*zoom_factor) );
      
      cairo_fill (cr);
      
     }
    }
   
   } //end of if(*line == '1')

  } // end of while((read = getline(&line, &len, pfile)) != -1)

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
  struct list_head *tmp;
  
  list_for_each(tmp, &NOTE_HEAD){
  
   struct note *tmp1;
   tmp1= list_entry(tmp, struct note, list);
   
   const gchar *content = tmp1->str;
   
   fprintf(pfile, "%s\n", content);
   
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
 
 struct stat buffer;  
 
 if( stat(full_path, &buffer) == -1 ){

  FILE *pfile;
  pfile = fopen(full_path,"w");
  if( pfile != NULL ){
 
   fprintf(pfile, "last_visited_page:%d\n", current_page_num+1);

   struct list_head *tmp;
 
   list_for_each(tmp, &NOTE_HEAD){

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

   }// end of list_for_each(tmp, &note_head.list)

   list_for_each(tmp, &HR_HEAD){

    struct highlight_region *tmp1;
    tmp1= list_entry(tmp, struct highlight_region, list);

    //1:1:00bfff:88:82:213:100
    fprintf(pfile, "%d:%d:%s:%d:%d:%d:%d\n", 1, tmp1->page_num, tmp1->color_name, tmp1->x, tmp1->y, tmp1->x + tmp1->width, tmp1->y + tmp1->height);
   }
 
   fclose(pfile);

  }// end of if( pfile != NULL )

 }
 else{ //file exists
  
  FILE *pfile;
  pfile = fopen(full_path, "w");
  if( pfile != NULL ){

   fprintf(pfile, "last_visited_page:%d\n", current_page_num+1);

   struct list_head *tmp;

   list_for_each(tmp, &NOTE_HEAD){

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
   
   } // end of list_for_each(tmp, &note_head.list)

   list_for_each(tmp, &HR_HEAD){

    struct highlight_region *tmp1;
    tmp1= list_entry(tmp, struct highlight_region, list);
   
    //1:1:00bfff:88:82:213:100
    fprintf(pfile, "%d:%d:%s:%d:%d:%d:%d\n", 1, tmp1->page_num, tmp1->color_name, tmp1->x, tmp1->y, tmp1->x + tmp1->width, tmp1->y + tmp1->height);
   }

   fclose(pfile);

  }
 
 }

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
 
 g_signal_connect (G_OBJECT (new_label), "draw", 
                   G_CALLBACK (comment_draw_cb), 
                   note);
 
 list_add_tail(&(note->list), &NOTE_HEAD);
}

void 
add_comment_cb (GtkWidget* widget, gpointer data) {

 add_comment();
}
