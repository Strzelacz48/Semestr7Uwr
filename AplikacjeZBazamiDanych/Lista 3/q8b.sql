
-- Czy dzia³aj¹ rekursywne triggery?
SELECT name AS 'Database name', is_recursive_triggers_on AS 'Recursive Triggers Enabled'
FROM sys.databases;

--ALTER DATABASE [Inventory]
--SET RECURSIVE_TRIGGERS ON;

--ALTER DATABASE [Orders]
--SET RECURSIVE_TRIGGERS ON;

--w³¹cz rekursywne triggery
--ALTER DATABASE [AdventureWorksLT2022]
--SET RECURSIVE_TRIGGERS ON;

--wy³¹cz rekursywne triggery
--ALTER DATABASE [AdventureWorks2022]
--SET RECURSIVE_TRIGGERS OFF;
-- Drop the history table if it exists
