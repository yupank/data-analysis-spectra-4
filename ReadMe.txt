========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : Spectra4
========================================================================



Spectra4.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CSpectra4App application class.

Spectra4.cpp
    This is the main application source file that contains the application
    class CSpectra4App.

Spectra4.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.



res\Spectra4.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Spectra4.rc.

res\Spectra4.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.



/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CMDIFrameWnd and controls all MDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in Spectra4.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

For the child frame window:

ChildFrm.h, ChildFrm.cpp
    These files define and implement the CChildFrame class, which
    supports the child windows in an MDI application.

/////////////////////////////////////////////////////////////////////////////

Document - View - Controller:

Spectra4Doc.h, Spectra4Doc.cpp - the document
    These files contain your CSpectra4Doc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CSpectra4Doc::Serialize).

Spectra4View.h, Spectra4View.cpp - the view of the document
    These files contain your CSpectra4View class.
    CSpectra4View objects are used to view CSpectra4Doc objects.

res\Spectra4Doc.ico
    This is an icon file, which is used as the icon for MDI child windows
    for the CSpectra4Doc class.  This icon is included by the main
    resource file Spectra4.rc.

/////////////////////////////////////////////////////////////////////////////

Help Support:

hlp\Spectra4.hpj
    This file is the Help Project file used by the Help compiler to create
    your application's Help file.

hlp\*.bmp
    These are bitmap files required by the standard Help file topics for
    Microsoft Foundation Class Library standard commands.

hlp\*.rtf
    This file contains the standard help topics for standard MFC
    commands and screen objects.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Spectra4.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.


