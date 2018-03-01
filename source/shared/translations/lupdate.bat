set QTDIR=D:\Qt\5.6.3_vs2010\qtbase

%QTDIR%\bin\lupdate %cd%\..\..\apps\label\label.pro
%QTDIR%\bin\lupdate %cd%\..\..\libs\styledui\styledui.pro
%QTDIR%\bin\lupdate %cd%\..\..\libs\projectexplorer\projectexplorer.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\core\core.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\devexplorer\devexplorer.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\phynetwork\phynetwork.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\sclmodel\sclmodel.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\devlibrary\devlibrary.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\baycopy\baycopy.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\baytemplate\baytemplate.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\ssdautoconn\ssdautoconn.pro
%QTDIR%\bin\lupdate %cd%\..\..\plugins\autoconn\autoconn.pro

call %QTDIR%\bin\linguist.exe