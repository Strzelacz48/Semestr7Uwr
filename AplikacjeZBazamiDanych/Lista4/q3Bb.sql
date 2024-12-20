USE AdventureWorksLT2022
BEGIN TRANSACTION;

-- Session 2 modifies the ListPrice
UPDATE SalesLT.Product
SET ListPrice = ListPrice * 1.20  -- 20% price increase
WHERE ProductID = 707;

COMMIT TRANSACTION;
