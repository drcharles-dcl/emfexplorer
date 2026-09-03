===========
  EMFexplorer
===========

version 1.0
2004-sept-10

The EMFexplorer software and documentation are
copyright (C) 2004 Smith Charles..

Email: smith.charles@free.fr
www: http://frazmitic.free.fr/emfexplorer/

The EMF data structures, operators, and specification are
copyright 1995-2004 Microsoft Corp.
GDI+ is copyright 2000-2004 Microsoft Corp.


What is EMFexplorer ?
-------------------------------

EMFexplorer is an open source viewer for Windows Enhanced Metafiles (EMF)
files (standard Windows Metafiles (WMF) are also treated). The EMFexplorer
project also includes an ActiveX component to display such files on html pages.

EMFexplorer runs under Microsoft Windows XP/NT/98. Its source code can be
built with the Visual C++ compiler, version 6 or later.

The main challenge of EMFexplorer is to use GDI+ to render documents with
high quality.


Distribution and License
---------------------------------
MIT License: see LICENCE


Compatibility
-----------------

EMFexplorer is developed and tested on a Windows XP system.
It also runs on Windows NT Sp5 or higher, Windows 98 SE.

You'll need a Visual C++ compatible compiler to compile it.

In addition, you'll need a Platform SDK containing the GDI+ headers,
version 1.0  or later.

Getting EMFexplorer
----------------------------

The latest version is available from:

 http://frazmitic.free.fr/emfexplorer/

Source code and several precompiled executables are available.



Running EMFexplorer
-----------------------------

Just launch EMFexplorer.exe at least once so that it can register its
active document mime type: BGP (Bound Graphics Pages) files.

Once the application is launched, drag and drop one or more files on it,
and enjoy!

For the ActiveX compoment, SCEMFAx.ocx, you must use the installation file
(AXINSTALL.bat) before you can view EMG documents in an activex-compatible browser.
(EMG files are EMF,WMF,EMZ,WMZ files).

GDI+ 1.0 or later must be installed on the target system.

Compiling EMFexplorer
---------------------------------

See the separate file, BUILD.txt.


Supported File Formats
---------------------------------
- Bound Graphics Pages:.bgp (EMFexplorer's native document format)
- Windows Enhanced Metafiles:.emf
- Compressed Windows Enhanced Metafiles:.emz
- Windows Enhanced Metafiles Plus:.emf
- Windows Standard Metafiles:.wmf
- Compressed Windows Standard Metafiles:.wmz
- Joint Photographic Expert Group:.jpg, .jpeg
- Portable Network Graphics: .png
- Windows Bitmap: .bmp
- Graphics Interchange Format: .gif
- Tagged Image File Format: .tiff, .tif
- Rich Text Format:.rtf
- Text: .txt, .h, .c, .cpp
- Clipboard


Bugs
-------

If you find a bug in EMFexplorer, i.e., if it displays an error message,
crashes, or incorrectly displays a document, and you don't see that
bug listed here, please send me email (saying "It's full of stars!"),
with a pointer (URL, ftp site, etc.) to the EMF/WMF file.

But remember that EMFexplorer comes with NO WARRANTY and NO SUPPORT. 
Most likely, the information you provide will be used to feed the FAQ or Issues pages.

