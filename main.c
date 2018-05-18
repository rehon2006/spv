/*
 * Copyright (C) 2017-2018 rehon2006, rehon2006@gmail.com
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

#include "main.h"

#include <sys/stat.h>

#ifdef __linux__
#include <linux/limits.h>
#elif __APPLE_
#include <sys/param.h>
#elif _WIN32
#include <dirent.h>
#include <direct.h>
#endif

#include <sys/types.h>

#include "gui.h"
#include "pdf.h"
#include "note.h"

void save_pref( void ){
 
 char *homedir = getenv("HOME");

 char* pref_home = (char*) malloc(strlen(homedir)+strlen("/.spv/")+1); // 9 = 1+length("/.spv")
 
 strcpy(pref_home,homedir);
 strcat(pref_home, "/.spv/");
 
 DIR *dp = NULL;
 
 if(dp = opendir(pref_home)){
  closedir(dp);
 }else{
  
  #ifdef _WIN32
  _mkdir(pref_home);
  #else
  mkdir(pref_home, S_IRWXU|S_IRWXG|S_IRWXO);
  #endif
  
  closedir(dp);
  
 }
 
 struct stat file_stat;
 
 char* pref_name = (char*) malloc(strlen( pref_home )+strlen("spv_pref")+1 );
 strcpy(pref_name, pref_home);
 strcat(pref_name, "spv_pref");

 FILE *pfile;
 pfile = fopen(pref_name,"w");
  
 if( pfile != NULL ){
  
  char *hr_color_str, *font_color_str, *bg_color_str;
  
  hr_color_str = (char*)malloc(10);
  
  sprintf(hr_color_str, "#%02X%02X%02X%02X", (int)(G_HR_COLOR->color.red*255),  
                                             (int)(G_HR_COLOR->color.green*255), 
                                             (int)(G_HR_COLOR->color.blue*255),
                                             (int)(G_HR_COLOR->color.alpha*255)); 
  
  
  font_color_str = (char*)malloc(10);
  
  sprintf(font_color_str, "#%02X%02X%02X%02X", (int)(G_CM_FONT_COLOR->color.red*255),  
                                               (int)(G_CM_FONT_COLOR->color.green*255), 
                                               (int)(G_CM_FONT_COLOR->color.blue*255),
                                               (int)(G_CM_FONT_COLOR->color.alpha*255));
                                                    
  bg_color_str = (char*)malloc(10);
  
  sprintf(bg_color_str, "#%02X%02X%02X%02X", (int)(G_CM_BG_COLOR->color.red*255),  
                                             (int)(G_CM_BG_COLOR->color.green*255), 
                                             (int)(G_CM_BG_COLOR->color.blue*255),
                                             (int)(G_CM_BG_COLOR->color.alpha*255));
  
  fprintf(pfile, "G:%s:%s:%s:%s:\n",
   pango_font_description_to_string(G_CM_FONT_DESC),
   hr_color_str, font_color_str, bg_color_str);

  fclose(pfile);
  
 }
  
 free(pref_home);
 free(pref_name);
 
}

static void init_pref( void ){
 
 G_HR_COLOR = NULL;
 
 G_CM_FONT_COLOR = G_CM_BG_COLOR = NULL;
 
 COL_TAB = NULL;
 
 char *homedir = getenv("HOME");

 char* pref_home = (char*) malloc(strlen(homedir)+strlen("/.spv/")+1); // 9 = 1+length("/.spv")
 strcpy(pref_home,homedir);
 strcat(pref_home, "/.spv/");
 
 DIR *dp = NULL;
 
 if(dp = opendir(pref_home)){
  closedir(dp);
 }else{
  
  #ifdef _WIN32
  _mkdir(pref_home);
  #else
  mkdir(pref_home, S_IRWXU|S_IRWXG|S_IRWXO);
  #endif
  
  closedir(dp);
  
 }
 
 struct stat file_stat;
 
 char* pref_name = (char*) malloc(strlen( pref_home )+strlen("spv_pref")+1 );
 strcpy(pref_name, pref_home);
 strcat(pref_name, "spv_pref");
 
 if( stat(pref_name, &file_stat) == -1 ){
  
  char *hr_color_str, *font_color_str, *bg_color_str;
  
  G_CM_FONT_DESC = pango_font_description_from_string ("Serif Bold 11");
 
  FILE * pfile = fopen(pref_name,"a");
  
  //build color table
  COL_TAB = (struct color_table*)malloc(sizeof(struct color_table));
  COL_TAB->rc = -1;
  
  COL_TAB->next = (struct color_table*)malloc(sizeof(struct color_table));
  COL_TAB->next->rc = 1;
  
  hr_color_str = (char*)malloc(10);
  strcpy(hr_color_str, "#FFFF00FF");
  
  char ctemp = *(hr_color_str+7);
  *(hr_color_str+7) = '\0';
  
  gdk_rgba_parse(&COL_TAB->next->color, hr_color_str);
  *(hr_color_str+7) = ctemp;
  COL_TAB->next->color.alpha = strtol (hr_color_str+7,NULL,16)/255.0;
  
  G_HR_COLOR = COL_TAB->next;
  G_HR_COLOR->rc++;
  
  struct color_table *ct_tmp = COL_TAB->next;
  ct_tmp->next = (struct color_table*)malloc(sizeof(struct color_table));
  ct_tmp->next->rc = 1;
  
  font_color_str = (char*)malloc(10);
  strcpy(font_color_str, "#000000FF");
  
  ctemp = *(font_color_str+7);
  *(font_color_str+7) = '\0';
  
  gdk_rgba_parse(&ct_tmp->next->color, font_color_str);
  *(font_color_str+7) = ctemp;
  ct_tmp->next->color.alpha = strtol (font_color_str+7,NULL,16)/255.0;
  
  G_CM_FONT_COLOR = ct_tmp->next;
  G_CM_FONT_COLOR->rc++;
  
  ct_tmp = ct_tmp->next;
  ct_tmp->next = (struct color_table*)malloc(sizeof(struct color_table));
  ct_tmp->next->rc = 1;
  
  bg_color_str = (char*)malloc(10);
  strcpy(bg_color_str, "#FFFF0080");
  ct_tmp->next->next = NULL;
  
  ctemp = *(bg_color_str+7);
  *(bg_color_str+7) = '\0';
  
  gdk_rgba_parse(&ct_tmp->next->color, bg_color_str);
  *(bg_color_str+7) = ctemp;
  ct_tmp->next->color.alpha = strtol (bg_color_str+7,NULL,16)/255.0;
  
  G_CM_BG_COLOR = ct_tmp->next;
  G_CM_BG_COLOR->rc++;
  //build color table
  
  fprintf(pfile, "G:%s:%s:%s:%s:\n",
   pango_font_description_to_string(G_CM_FONT_DESC),
   hr_color_str, font_color_str, bg_color_str);
  
  free(hr_color_str);
  free(font_color_str);
  free(bg_color_str);
   
  fclose (pfile);
   
 }
 else{
 
  FILE *pfile;
  
  char line[1000];
  size_t len = 0;
  ssize_t read;
  
  pfile = fopen(pref_name, "r");
  
  if(pfile == NULL)
   exit(EXIT_FAILURE);
  
  char *pch0 = NULL;
  
  char *pch, *pch1, *pch2, *pch3,*pch4, *pch5, *pch6;
  
  pch = pch1 = pch2 = pch3 = pch4 = pch5 = pch6 = NULL;
 
  while( fgets(line, 1000, pfile) != NULL ){
   
   pch0 = strchr(line, ':');
   
   if(pch0){
    
    pch = strchr(pch0+1, ':');
    
   }
   
   if( pch ){
    *pch = '\0';
    G_CM_FONT_DESC = pango_font_description_from_string (pch0+1);
    pch1 = strchr(pch+1, ':');
    
   }
   
   COL_TAB = (struct color_table*)malloc(sizeof(struct color_table));
   COL_TAB->rc = -1;
  
   struct color_table *ct_tmp = COL_TAB;   
   
   char *color_str = NULL;
   
   char ctemp;
   
   while( pch1 ){
    
    *pch1 = '\0';
    
    ct_tmp->next = (struct color_table*)malloc(sizeof(struct color_table));
      
    ct_tmp->next->rc = 1;
    ct_tmp->next->next = NULL;
    
    color_str = (char*)malloc(10);
    strcpy(color_str, pch+1);
    
    ctemp = *(color_str+7);
    *(color_str+7) = '\0';
    
    gdk_rgba_parse(&ct_tmp->next->color, color_str);
    *(color_str+7) = ctemp;
    ct_tmp->next->color.alpha = strtol (color_str+7,NULL,16)/255.0;
    
    if( !strcmp(color_str, "#FFFF00FF") ){
     G_HR_COLOR = ct_tmp->next;
     G_HR_COLOR->rc++;
    }
    else if( !strcmp(color_str, "#000000FF") ){
     G_CM_FONT_COLOR = ct_tmp->next;
     G_CM_FONT_COLOR->rc++;
    }
    else if( !strcmp(color_str, "#FFFF0080") ){
     G_CM_BG_COLOR = ct_tmp->next;
     G_CM_BG_COLOR->rc++;
    }
    
    free(color_str);
    color_str = NULL;
    
    ct_tmp = ct_tmp->next;
    
    pch = pch1;
    
    pch1 = strchr(pch1+1, ':');
    
   }
   
  }// end while( fgets(line, 1000, pfile) != NULL 
  
  //existing file without content
  if( !G_HR_COLOR && !G_CM_FONT_COLOR && !G_CM_BG_COLOR ){
  
   char *hr_color_str, *font_color_str, *bg_color_str;
  
   G_CM_FONT_DESC = pango_font_description_from_string ("Serif Bold 11");

   COL_TAB = (struct color_table*)malloc(sizeof(struct color_table));
   COL_TAB->rc = -1;
   COL_TAB->next = (struct color_table*)malloc(sizeof(struct color_table));
   COL_TAB->next->rc = 1;

   hr_color_str = (char*)malloc(10);
   strcpy(hr_color_str, "#FFFF00FF");
  
   char ctemp = *(hr_color_str+7);
   *(hr_color_str+7) = '\0';
   
   gdk_rgba_parse(&COL_TAB->next->color, hr_color_str);
   *(hr_color_str+7) = ctemp;
   COL_TAB->next->color.alpha = strtol (hr_color_str+7,NULL,16)/255.0;
   
   free(hr_color_str);
   
   G_HR_COLOR = COL_TAB->next;
   G_HR_COLOR->rc++;
  
   struct color_table *ct_tmp = COL_TAB->next;
   ct_tmp->next = (struct color_table*)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;

   font_color_str = (char*)malloc(10);
   strcpy(font_color_str, "#000000FF");
  
   ctemp = *(font_color_str+7);
   *(font_color_str+7) = '\0';
   
   gdk_rgba_parse(&ct_tmp->next->color, font_color_str);
   *(font_color_str+7) = ctemp;
   ct_tmp->next->color.alpha = strtol (font_color_str+7,NULL,16)/255.0;
   
   free(font_color_str);
   
   G_CM_FONT_COLOR = ct_tmp->next;
   G_CM_FONT_COLOR->rc++;
  
   ct_tmp = ct_tmp->next;
   
   ct_tmp->next = (struct color_table*)malloc(sizeof(struct color_table));
   ct_tmp->next->rc = 1;

   bg_color_str = (char*)malloc(10);
   strcpy(bg_color_str, "#FFFF0080");
   ct_tmp->next->next = NULL;
  
   ctemp = *(bg_color_str+7);
   *(bg_color_str+7) = '\0';
   
   gdk_rgba_parse(&ct_tmp->next->color, bg_color_str);
   *(bg_color_str+7) = ctemp;
   ct_tmp->next->color.alpha = strtol (bg_color_str+7,NULL,16)/255.0;
 
   G_CM_BG_COLOR = ct_tmp->next;
   G_CM_BG_COLOR->rc++;
   
   free(bg_color_str);
   
  }//end of if( !G_HR_COLOR && !G_CM_FONT_COLOR1 && !G_CM_BG_COLOR1 )
  
  fclose(pfile);
  
 }
 
 free(pref_home);
 free(pref_name);
}

int main(int argc, char* argv[]) {
 
 gtk_init(&argc, &argv);
    
 if( argc == 1 ){
  GtkWidget *dialog;
  
  dialog = gtk_file_chooser_dialog_new("Open file", NULL,
             GTK_FILE_CHOOSER_ACTION_OPEN, 
	     "_Cancel", GTK_RESPONSE_CANCEL,
	     "_Open", GTK_RESPONSE_ACCEPT, 
	     NULL);
  
  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if(res == GTK_RESPONSE_ACCEPT){
   GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
   init_pdf(gtk_file_chooser_get_filename(chooser));
  }
  else if ( res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT)
   exit(1);
   
  gtk_widget_destroy(dialog);
  
 }else if(argc == 2){
  
 char abs_path[100];
 #ifdef _WIN32
  _fullpath(abs_path, argv[1], 100);
 #else
  realpath(argv[1], abs_path);
 #endif
 
 init_pdf(abs_path);
 
 }else if (argc != 2) {
  printf("Useage: ./spv [file name] or ./spv\n");
  return 1;
 }     
 
 init_pref();
 
 init_gui();
 
 gtk_main();
 
 return 0;
}
