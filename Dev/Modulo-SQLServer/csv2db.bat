REM sqlcmd -S localhost -d FutureHub_Svil -E -Q "DELETE FROM dbo.%2 WHERE SessionId=%1"
sqlcmd -S localhost -d FutureHub_Svil -E -Q " BULK INSERT dbo.%2 FROM '%cd%\..\output\ISTANZA%1_%2.csv' WITH (FIELDTERMINATOR = ';' , ROWTERMINATOR = '\n')"
