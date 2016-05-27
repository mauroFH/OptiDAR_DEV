sqlcmd -S localhost -d FutureHub_Svil -E -Q "SELECT * FROM dbo.%2 WHERE SessionID=%1" -W -s";" | findstr /V /C:"-" /B | findstr /V /C:"(" > ..\input\ISTANZA%1_%2.csv
..\Modulo-SQLServer\fart --remove ..\input\ISTANZA%1_%2.csv NULL
