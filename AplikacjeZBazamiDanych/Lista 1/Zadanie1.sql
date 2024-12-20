SELECT DISTINCT City
FROM [SalesLT].[Address] JOIN [SalesLT].[SalesOrderHeader]
ON [SalesLT].[SalesOrderHeader].ShipToAddressID = [SalesLT].[Address].AddressID
WHERE ShipDate <= '2024-10-14'
ORDER BY City ASC