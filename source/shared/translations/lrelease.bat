set QTDIR=D:\Qt\5.6.3_vs2010\qtbase

for /f "tokens=*" %%i in ('dir/s/b *.ts') do "%QTDIR%\bin\lrelease.exe" "%%i"