USE AdventureWorksLT2022; 
BEGIN TRANSACTION;

-- Session 2 inserts a new product with ListPrice above 100
INSERT INTO SalesLT.Product (Name, ProductNumber, Color, ListPrice, StandardCost, SellStartDate)
VALUES ('New Product1', 'NP-101', 'Black', 150, 100, '2002-06-01 00:00:00.000');

COMMIT TRANSACTION;
