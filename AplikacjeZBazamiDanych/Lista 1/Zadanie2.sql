SELECT Name, COUNT(Name) AS Quantity
FROM [SalesLT].[ProductModel]
GROUP BY Name
--HAVING COUNT(Name) > 1