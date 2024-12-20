--SELECT * FROM #LocalTempTable; -- This shouldn't

SELECT * FROM ##GlobalTempTable; -- This should work (I hope)