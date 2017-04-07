Simple PDF Viewer
=================

It's a simple PDF viewer based on GTK3 and Poppler, and is available for Linux, Mac and Windows. Users can highlight text, remove text highlights and add comments on PDF files. These functionalities do not modify PDF files, and related information is saved in a note file.

這是一個使用GTK3以及Poppler編寫的簡單PDF閱讀器並且可以在Linux, Mac以及Windows下執行. 使用者可以在PDF檔案上標記文字, 移除文字標記以及新增註解. 上述功能並不會修改PDF檔案, 相關資訊會儲存在一個note檔案中.

Requirements 所需函式庫
-----

GTK+ version 3.12.2 or higher ( http://www.gtk.org/ )  
Poppler version 0.24.5 with glib bindings or higher ( http://poppler.freedesktop.org/ )  

Test Platforms 測試平台
-----
* ###Ubuntu 
* Ubuntu version : Ubuntu 14.04.5 LTS  
* gtk+-3.0 version : 3.12.2  
* poppler-glib version : 0.24.5  
```
      sudo apt-get install libgtk-3-dev libpoppler-glib-dev

```
* ###Mac 
* MacOS version : Sierra 10.12.1
* gtk+-3.0 version : 3.20.6
* poppler-glib version : 0.47.0
```
      brew install gtk+3 poppler gnome-icon-theme

```

* ###Windows (under Msys2 32bit)
* Windows version : Version 1607 Build 14393  
* gtk+-3.0 version : 3.22.10 
* poppler-glib version : 0.52.0 
```
      pacman -S mingw-w64-i686-gtk3 mingw-w64-i686-poppler

```

##Build

```
     make
```  
Usage 用法
-----
```
     ./spv [file name]  
```  
or  
``` 
     ./spv
```

Copy and paste text 文字複製以及貼上
-----------

###Ubuntu
After selecting text, users can paste with mouse middle button or use Ctrl+c to copy selected text and paste with Ctrl+v.  
在文字選取完後,利用滑鼠中鍵貼上文字, 或是利用Ctrl+c複製文字並以Ctrl+v貼上文字

###Windows

Use mouse right button or Ctrl+v to paste after selecting text.  
在文字選取完後,使用滑鼠右鍵或是Ctrl+v貼上文字

###Mac  

Use mouse right button or Command+v to paste after selecting text.  
在文字選取完後,使用滑鼠右鍵或是Command+v貼上文字

Key Bindings 鍵盤熱鍵
--------------------  

Key  |   Action
-----|---------
Ctrl+q | Quit 結束程式
Ctrl+s | Save note 儲存筆記
Ctrl+n | Add a comment 新增註解
Ctrl+f | Text Search 文字搜尋
/ | Search forward 向下搜尋
? | Search backword 向上搜尋
n | Repeat last search 重複上次搜尋
N | Repeat last search, but in opposite direction of original search 重複上次搜尋, 但是以相反方向搜尋
Ctrl+ | Zoom in 1x 放大一倍
Ctrl- | Zoom out 1x 縮小一倍
Ctrl+w | Zoom width 依視窗寬度縮放
Ctrl+z | Switch mode (text selection mode [S], text highlight mode [H] and erase text highlight mode [E]) 模式切換(文字選取模式[S], 文字標記模式[H]以及移除文字標記模式[E] )
Shift+c | Change highlight color 改變文字標記顏色
Ctrl+c | Copy selected text 複製選取文字
Ctrl+v | Show/Hide toolbar 顯示/隱藏工具列
Ctrl+mouse scroll up | zoom in 放大
Ctrl+mouse scroll down | zoom out 縮小
number+P | Go to page [number] (e.g., 11P, go to page 11) 前往指定頁數的頁面(如11P就是前往第11頁)
Right-arrow key | Go to next page 前往下一頁
Left-arrow key | Go to previous page 前往上一頁
ESC | Terminate text search or go to text selection mode 結束文字搜尋或是切換至文字選取模式


Touchpad Gesture 觸控板手勢
--------------

### Swipe 滑動

Swipe left or right with two fingers to go to previous or next page (Note: horizontal scrolling support is necessary).  
用兩指左右滑動以前往上一頁或是下一頁. 觸控板必須支援水平滾動.

### Pinch Zoom 放大或縮小

To zoom in or out, spread or pinch two fingers. This feature is only available in Windows and touchpad with multi-touch support is necessary.  
展開或是捏合兩指就可以放大或縮小頁面. 此功能目前只適用於Windows並且觸控板必須支援多點觸控.

Note & Comment 筆記與註解
------------

The note file is saved as [PDF filename].note (e.g., test.pdf.note).  
note檔案會被存為[PDF檔名].note (例如 test.pdf.note).

Press the left mouse button on comment to drag and drop it if you want to move it.  
如果你想移動註解, 利用滑鼠左鍵進行註解拖放

Click the right mouse button on a comment if you want to edit or delete it. A dialog will show up and you can edit or delete it.  
如果你想編輯或移除註解, 在註解上按滑鼠右鍵. 接著會有一個視窗出現, 便可以編輯或移除註解.

You can also save the content of all comments as [PDF filename].comment (e.g., test.pdf.comment).  
使用者可以將所有註解資訊存到[PDF檔名].comment (例如 test.pdf.comment).

