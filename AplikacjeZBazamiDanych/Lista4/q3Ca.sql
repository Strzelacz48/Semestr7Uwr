USE AdventureWorksLT2022; 
BEGIN TRANSACTION;

-- Session 1 reads products with ListPrice above 100
SELECT ProductID, ListPrice
FROM SalesLT.Product
WHERE ListPrice > 100;

-- Waits to allow Session 2 to insert a new row
WAITFOR DELAY '00:00:10';

-- Session 1 reads the same query again
SELECT ProductID, ListPrice
FROM SalesLT.Product
WHERE ListPrice > 100;

COMMIT TRANSACTION;
