sqlcmd -S localhost -d FutureHub_Svil -E -Q "DELETE FROM dbo.%3 WHERE SolutionId=%2"
sqlcmd -S localhost -d FutureHub_Svil -E -Q " BULK INSERT dbo.%3 FROM '%cd%\..\output\ISTANZA%1_%3.csv' WITH (FIELDTERMINATOR = ';' , ROWTERMINATOR = '\n')"
