USE AdventureWorksLT2022
BEGIN TRANSACTION;
--Dirty read
-- Session 1 updates a product price but doesn’t commit yet
UPDATE SalesLT.Product
SET ListPrice = ListPrice * 1.10  -- 10% price increase
WHERE ProductID = 707;

-- Waits to allow Session 2 to perform a read
WAITFOR DELAY '00:00:10';

-- Rolls back the change
ROLLBACK TRANSACTION;
