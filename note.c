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

    GtkWidget *label = gtk_label_new("");
    
    char new_line = 1;
    char *pch_tmp = strchr(pch2+1, new_line);
    
    while( pch_tmp ){
     *pch_tmp = '\n';
      pch_tmp = strchr(pch_tmp+1, new_line);
    }
    
    const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";
    char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), pch2+1);
    
    gtk_label_set_markup(GTK_LABEL(label), markup);
    g_free(markup);

    gtk_widget_add_events (label,  GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
    
    gtk_layout_put (GTK_LAYOUT (layout), label, atoi(pch+1), atoi(pch1+1));

    struct note *note = (struct note*)malloc(sizeof(struct note));
    INIT_LIST_HEAD(&note->list);
    note->comment = label;
    note->x = atoi(pch+1);
    note->y = atoi(pch1+1);
    
    note->page_num = atoi(pch0+1);
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
    
    list_add(&(hg->list), &HR_HEAD);

    if( hg->page_num ==  current_page_num + 1 ){
     text_highlight_release(atoi(pch1+1), 
                            atoi(pch2+1), 
                            atoi(pch3+1), 
                            atoi(pch4+1), 
                            hg->color_name); 
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
   
   GtkWidget *label = tmp1->comment;
   const gchar *content = gtk_label_get_text(GTK_LABEL(label));
   
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

    GtkWidget *label = tmp1->comment;
    const gchar *content = gtk_label_get_text(GTK_LABEL(label));

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

    GtkWidget *label = tmp1->comment;
    const gchar *content = gtk_label_get_text(GTK_LABEL(label));
    
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

 if( mode == TEXT_SELECTION && selection_region && last_region != NULL){
  cairo_region_t *invert_region;
  invert_region = cairo_region_copy(selection_region);
  invertRegion(selection_region);
  cairo_region_destroy(selection_region);
  selection_region = NULL;
 }

 const char* format = "<span foreground=\"black\" font=\"%d\">%s</span>";

 char *markup = g_markup_printf_escaped(format, (gint)(FONT_SIZE*zoom_factor), "New Comment");

 GtkAdjustment *vadj =
  gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
 
 GtkWidget *new_label = gtk_label_new("");
 gtk_label_set_markup(GTK_LABEL(new_label), markup);
 gtk_layout_put (GTK_LAYOUT (layout), new_label, 100, (int)(gtk_adjustment_get_value (vadj)+100));
 gtk_widget_show(new_label);

 struct note *note = (struct note*)malloc(sizeof(struct note));
 INIT_LIST_HEAD(&note->list);
 note->comment = new_label;
 note->x = 100;
 note->y = 100;
 note->page_num = current_page_num + 1;
 
 list_add_tail(&(note->list), &NOTE_HEAD);
}

void 
add_comment_cb (GtkWidget* widget, gpointer data) {

 add_comment();
}
