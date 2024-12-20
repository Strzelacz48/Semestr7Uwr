DECLARE @TableVariable TABLE (ID INT, Name VARCHAR(50));
INSERT INTO @TableVariable VALUES (1, 'TableVarRow1');
SELECT * FROM @TableVariable;


--Create local table
CREATE TABLE #LocalTempTable (ID INT, Name VARCHAR(50));
INSERT INTO #LocalTempTable VALUES (1, 'LocalTempRow1');
SELECT * FROM #LocalTempTable;
--Create global table
CREATE TABLE ##GlobalTempTable (ID INT, Name VARCHAR(50));
INSERT INTO ##GlobalTempTable VALUES (1, 'GlobalTempRow1');
SELECT * FROM ##GlobalTempTable;


-- Check tempdb for temporary tables
SELECT TABLE_NAME
FROM tempdb.INFORMATION_SCHEMA.TABLES
WHERE TABLE_NAME LIKE '%TempTable%';
