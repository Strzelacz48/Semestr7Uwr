USE AdventureWorksLT2022;
GO

-- Drop the history table if it exists
IF OBJECT_ID('SalesLT.ProductPriceHistory', 'U') IS NOT NULL
    DROP TABLE SalesLT.ProductPriceHistory;
GO

-- Create the Product Price History table
CREATE TABLE SalesLT.ProductPriceHistory (
    HistoryID INT PRIMARY KEY IDENTITY(1,1),
    ProductID INT NOT NULL,
    StandardCost DECIMAL(18, 4) NOT NULL,
    ListPrice DECIMAL(18, 4) NOT NULL,
    ChangeDate DATETIME NOT NULL DEFAULT GETDATE(),
    EffectiveStartDate DATETIME NOT NULL,
    EffectiveEndDate DATETIME,
    FOREIGN KEY (ProductID) REFERENCES SalesLT.Product(ProductID) ON DELETE CASCADE
);
GO

-- Drop the trigger if it exists
IF OBJECT_ID('SalesLT.trg_RecordPriceHistory', 'TR') IS NOT NULL
    DROP TRIGGER SalesLT.trg_RecordPriceHistory;
GO

-- Create the trigger to log changes in StandardCost and ListPrice
CREATE TRIGGER SalesLT.trg_RecordPriceHistory
ON SalesLT.Product
AFTER UPDATE
AS
BEGIN
    -- Insert a new record into the ProductPriceHistory table for changes in StandardCost or ListPrice
    INSERT INTO SalesLT.ProductPriceHistory (ProductID, StandardCost, ListPrice, EffectiveStartDate)
    SELECT 
        p.ProductID,
        i.StandardCost,
        i.ListPrice,
        GETDATE()  -- Set the effective start date to the current date and time
    FROM 
        inserted i
    JOIN 
        SalesLT.Product p ON i.ProductID = p.ProductID
    WHERE 
        (i.StandardCost <> p.StandardCost OR i.ListPrice <> p.ListPrice);
END;
GO

-- Retrieve the price history with effective periods
WITH PriceHistory AS (
    SELECT 
        ProductID,
        StandardCost,
        ListPrice,
        ChangeDate,
        ROW_NUMBER() OVER (PARTITION BY ProductID ORDER BY ChangeDate) AS rn
    FROM 
        SalesLT.ProductPriceHistory
),
EffectivePeriods AS (
    SELECT 
        p1.ProductID,
        p1.StandardCost,
        p1.ListPrice,
        p1.ChangeDate AS EffectiveStartDate,
        COALESCE(p2.ChangeDate, GETDATE()) AS EffectiveEndDate
    FROM 
        PriceHistory p1
    LEFT JOIN 
        PriceHistory p2 ON p1.ProductID = p2.ProductID AND p1.rn = p2.rn - 1
)
SELECT 
    ProductID,
    StandardCost,
    ListPrice,
    EffectiveStartDate,
    EffectiveEndDate
FROM 
    EffectivePeriods
ORDER BY 
    ProductID, EffectiveStartDate;
