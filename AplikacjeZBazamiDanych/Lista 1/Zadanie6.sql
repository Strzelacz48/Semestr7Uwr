SELECT 
    klienci.LastName AS Nazwisko,
	klienci.FirstName AS Imie,
    SUM(sales_details.UnitPrice * sales_details.UnitPriceDiscount * sales_details.OrderQty) AS Zaoszczedzono
FROM 
    [SalesLT].[Customer] AS klienci,
    [SalesLT].[SalesOrderDetail] AS sales_details,
    [SalesLT].[SalesOrderHeader] AS sales_header
WHERE
	klienci.CustomerID = sales_header.CustomerID
	AND sales_details.SalesOrderID = sales_header.SalesOrderID
GROUP BY klienci.FirstName, klienci.LastName