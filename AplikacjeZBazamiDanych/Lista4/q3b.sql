USE AdventureWorksLT2022
-- Session 2 reads the uncommitted price change (dirty read)
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
SELECT ProductID, ListPrice
FROM SalesLT.Product
WHERE ProductID = 707;
