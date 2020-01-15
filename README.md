Simple PDF Viewer
=================

SPV is a simple PDF viewer based on GTK3 and Poppler, and is available for Linux, Mac, ChromeOS (with Crostini support) and Windows. It supports text highlight, comments adding/removing, and so on.

Features
-----
* Text highlight and comments adding/removing
* Dual-page mode
* Change color of highlight regions
* Invert and change background color of all pages
* Change font, font color and background color of comments
* Zoom in, Zoom out, Zoom to height and Zoom to Width
* Add/Remove blank pages
* Create blank file
* Save as new PDF file

Requirements
-----

GTK+ version 3.12.2 or higher ( http://www.gtk.org/ )  
Poppler version 0.24.5 with glib bindings or higher ( http://poppler.freedesktop.org/ )  

Test Platforms
-----
* ### Ubuntu  
* Ubuntu version : Ubuntu 16.04.6 LTS  
* gtk+-3.0 version : 3.18.9  
* poppler-glib version : 0.41.0  
```
      sudo apt-get install libgtk-3-dev libpoppler-glib-dev
```
* ### Mac 
* MacOS version : Sierra 10.12.1
* gtk+-3.0 version : 3.20.6
* poppler-glib version : 0.47.0
```
      brew install gtk+3 poppler gnome-icon-theme
```

* ### Windows (under Msys2 64bit)
* Windows version : Version 1903 
* gtk+-3.0 version : 3.24.13 
* poppler-glib version : 0.83.0 
```
      pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-poppler
```
* ### ChromeOS
* version : 
* gtk+-3.0 version : 3.18.9  
* poppler-glib version : 0.41.0  
```
      sudo apt-get install libgtk-3-dev libpoppler-glib-dev
```

Build
-----
```
     make
```  
Usage
-----
Open PDF file or note file (for blank file)
```
     ./spv [file name]  
```  
or (Select PDF File) 
``` 
     ./spv
```
or (Create Blank File)
```
     ./spv -b
```

Copy and paste text
-----------

### Ubuntu

After selecting text, users can paste it with middle mouse button or use Ctrl+c to copy selected text and paste it with Ctrl+v.  

### Windows

Use right mouse button or Ctrl+v to paste after selecting text.  

### Mac  

Use right mouse button or Command+v to paste after selecting text.  

Key Bindings
--------------------  

Key  |   Action
-----|---------
Ctrl+q | Quit 
Ctrl+s | Save note file 
Ctrl+n | Add a comment 
Ctrl+f | Text search 
/ | Search forward 
? | Search backward 
n | Repeat last search 
N | Repeat last search, but in opposite direction of original search 
Ctrl+ | Zoom in 1x
Ctrl- | Zoom out 1x 
Ctrl+w | Zoom to width 
Ctrl+h | Zoom to height 
Ctrl+z | Switch mode (text selection mode [S], text highlight mode [H] and erase text highlight mode [E]) 
Ctrl+d | Enter/Leave dual-page mode 
Shift+d | Delete current blank page
F11 | Enter/Leave full-screen mode (Only for Linux and Windows) 
Shift+c | Change color of highlight regions
Ctrl+Shift+b | Change background color of all pages
Ctrl+b | Insert blank page after current page
Shift+b | Insert blank page before current page
Ctrl+c | Copy selected text 
Ctrl+x | Invert background color of all pages
number+P | Go to page [number] (e.g. 11P, go to page 11) 
Right-arrow key | Go to next page 
Left-arrow key | Go to previous page 
ESC | Terminate text search, go to text selection mode or leave full-screen mode 

Note File & Comment 
------------

A note file will be created after closing SPV or pressing Ctrl+s. It contains not only information about comments and highlight regions, but also information like last visited page, setting of inverting color and dual-page mode, comment property for this PDF file, and so on.

If you want to edit a comment, just click the right mouse button on a comment. A dialog will show up and you can edit the content of this comment, delete it or change the property which includes font, font color and background color.

SPV also allows user to save comments as a comment file, which is a plain text file containing only string part of all comments. 

Changing Background Color For All Pages
------------

There are three ways to change background color for all pages: invert background color, use default color #C7EDCC or select a new one.

