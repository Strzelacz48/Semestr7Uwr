USE AdventureWorksLT2022
BEGIN TRANSACTION;

-- Session 1 reads the ListPrice of a product
SELECT ProductID, ListPrice
FROM SalesLT.Product
WHERE ProductID = 707;

-- Waits to allow Session 2 to make changes
WAITFOR DELAY '00:00:10';

-- Session 1 reads the ListPrice again
SELECT ProductID, ListPrice
FROM SalesLT.Product
WHERE ProductID = 707;

COMMIT TRANSACTION;
