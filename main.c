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

#include "main.h"

#include <sys/stat.h>

#if __linux__
#include <linux/limits.h>
#elif __APPLE_
#include <sys/param.h>
#endif

#include "gui.h"
#include "pdf.h"

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

 init_gui();
 
 init_note();
 
 gtk_main();
 
 return 0;
}
