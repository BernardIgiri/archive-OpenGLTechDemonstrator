# Microsoft Developer Studio Project File - Name="input" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=input - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "input.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "input.mak" CFG="input - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "input - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "input - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "input - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "input - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib dinput8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "input - Win32 Release"
# Name "input - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Data Systems Source"

# PROP Default_Filter ""
# Begin Group "Data Type Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bistree.cpp
# End Source File
# Begin Source File

SOURCE=.\bitree.cpp
# End Source File
# Begin Source File

SOURCE=.\clist.cpp
# End Source File
# Begin Source File

SOURCE=.\dlist.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\queue.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ConstStore.cpp
# End Source File
# Begin Source File

SOURCE=.\onddas.cpp
# End Source File
# End Group
# Begin Group "File Loader Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\DevILFileFormats.cpp
# End Source File
# Begin Source File

SOURCE=.\loadJPG.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadTGA.cpp
# End Source File
# End Group
# Begin Group "GUI Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIImageRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectionListRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SliderRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\TextFeildRenderer.cpp
# End Source File
# End Group
# Begin Group "Input Systems Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Consol.cpp
# End Source File
# Begin Source File

SOURCE=.\InputManager.cpp
# End Source File
# Begin Source File

SOURCE=.\InputSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\StringedInputProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\TextInputSystem.cpp
# End Source File
# End Group
# Begin Group "Utility Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\StringUtilities.cpp
# End Source File
# End Group
# Begin Group "Graphics SubSystems Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\texture manager.cpp"
# End Source File
# End Group
# Begin Group "Game Objects Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConversationSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerStateMachine.cpp
# End Source File
# End Group
# Begin Group "Renderer Source"

# PROP Default_Filter ""
# Begin Group "Polygon Class Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FaceClass.cpp
# End Source File
# Begin Source File

SOURCE=.\TexFaceClass.cpp
# End Source File
# End Group
# Begin Group "Model Class Source"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Data Systems Headers"

# PROP Default_Filter ""
# Begin Group "Data Type Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bistree.h
# End Source File
# Begin Source File

SOURCE=.\bitree.h
# End Source File
# Begin Source File

SOURCE=.\clist.h
# End Source File
# Begin Source File

SOURCE=.\dlist.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\queue.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ConstStore.h
# End Source File
# Begin Source File

SOURCE=.\onddas.h
# End Source File
# End Group
# Begin Group "File Loader Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\DevILFileFormats.h
# End Source File
# Begin Source File

SOURCE=.\loadJPG.h
# End Source File
# Begin Source File

SOURCE=.\LoadTGA.h
# End Source File
# End Group
# Begin Group "GUI Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonRenderer.h
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowGenerator.h
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowManager.h
# End Source File
# Begin Source File

SOURCE=.\GLGUIWindowRenderer.h
# End Source File
# Begin Source File

SOURCE=.\GUIImageRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SelectionListRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SliderRenderer.h
# End Source File
# Begin Source File

SOURCE=.\TextFeildRenderer.h
# End Source File
# End Group
# Begin Group "Input Systems Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Consol.h
# End Source File
# Begin Source File

SOURCE=.\InputManager.h
# End Source File
# Begin Source File

SOURCE=.\InputProcessor.h
# End Source File
# Begin Source File

SOURCE=.\InputSystem.h
# End Source File
# Begin Source File

SOURCE=.\StringedInputProcessor.h
# End Source File
# Begin Source File

SOURCE=.\TextInputSystem.h
# End Source File
# End Group
# Begin Group "Utility Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fonts.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\HiResTimer.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\StringUtilities.h
# End Source File
# Begin Source File

SOURCE=.\SystemTimer.h
# End Source File
# End Group
# Begin Group "Graphics SubSystems Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\texture manager.h"
# End Source File
# End Group
# Begin Group "Game Objects Headers"

# PROP Default_Filter ""
# Begin Group "ConversationSystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConversationSystem.h
# End Source File
# Begin Source File

SOURCE=.\ConversationSystemDefines.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\PlayerDefines.h
# End Source File
# Begin Source File

SOURCE=.\PlayerStateMachine.h
# End Source File
# End Group
# Begin Group "Renderer Headers"

# PROP Default_Filter ""
# Begin Group "Polygon Class Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FaceClass.h
# End Source File
# Begin Source File

SOURCE=.\TexFaceClass.h
# End Source File
# End Group
# Begin Group "Model Class Headers"

# PROP Default_Filter ""
# End Group
# End Group
# End Group
# Begin Group "Linked Libraries"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\..\..\Vc98\Lib\Opengl32.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Vc98\Lib\Glu32.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Vc98\Lib\Glaux.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Vc98\Lib\ijl15.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ImageLib\lib\ilu.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ImageLib\lib\ilut.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ImageLib\lib\il_wrap.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ImageLib\lib\DevIL.lib
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Text Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=".\INPUTPROC readme.txt"
# End Source File
# Begin Source File

SOURCE=".\old code dump.txt"
# End Source File
# Begin Source File

SOURCE=".\PlayerStateMachine notes.txt"
# End Source File
# End Group
# End Target
# End Project
