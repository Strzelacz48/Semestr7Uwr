SELECT 
    adresy.City, 
    COUNT(klienci_adresy.CustomerID) AS "Liczba klientow",
    COUNT(DISTINCT klienci.SalesPerson) AS "Liczba obslugujacych"
FROM 
    [SalesLT].[Address] AS adresy, 
    [SalesLT].[Customer] AS klienci,
    [SalesLT].[CustomerAddress] AS klienci_adresy
WHERE
    adresy.AddressID = klienci_adresy.AddressID 
    AND klienci.CustomerID = klienci_adresy.CustomerID
GROUP BY adresy.City