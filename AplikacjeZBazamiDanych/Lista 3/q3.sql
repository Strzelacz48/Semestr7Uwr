USE AdventureWorksLT2022;
GO
-- Drop tables if they already exist
DROP TABLE IF EXISTS Prices;
DROP TABLE IF EXISTS Rates;
DROP TABLE IF EXISTS Products;

-- Create Products table
CREATE TABLE Products (
    ID INT PRIMARY KEY IDENTITY(1,1),
    ProductName VARCHAR(100) NOT NULL
);

-- Create Rates table with Currency and Price in PLN
CREATE TABLE Rates (
    Currency VARCHAR(10) PRIMARY KEY,
    PricePLN DECIMAL(18, 2) NOT NULL
);

-- Create Prices table with reference to Products and Rates
CREATE TABLE Prices (
    ProductID INT,
    Currency VARCHAR(10),
    Price DECIMAL(18, 2),
    FOREIGN KEY (ProductID) REFERENCES Products(ID) ON DELETE CASCADE,
    FOREIGN KEY (Currency) REFERENCES Rates(Currency) ON DELETE CASCADE
);

-- Insert data into Products table
INSERT INTO Products (ProductName)
VALUES ('Laptop'), ('Smartphone'), ('Tablet'), ('Smartwatch');

-- Insert data into Rates table with currencies and exchange rates
INSERT INTO Rates (Currency, PricePLN)
VALUES ('USD', 4.50), ('EUR', 4.31), ('GBP', 5.20), ('JPY', 0.035);

-- Insert data into Prices table with various currencies
INSERT INTO Prices (ProductID, Currency, Price)
VALUES 
    (1, 'USD', 900),  -- Laptop price in USD
    (1, 'EUR', 850),  -- Laptop price in EUR
    (2, 'USD', 700),  -- Smartphone price in USD
    (2, 'GBP', 650),  -- Smartphone price in GBP
    (3, 'JPY', 120000),  -- Tablet price in JPY
    (3, 'EUR', 300);--,  -- Tablet price in EUR
    --(3, 'CHF', 400);  -- Smartwatch price in CHF, which is not in Rates

-- Declare variables for cursor operations
DECLARE @ProductID INT, @Currency VARCHAR(10), @Price DECIMAL(18, 2);
DECLARE @RatePLN DECIMAL(18, 2);

-- Declare and open the cursor
DECLARE price_cursor CURSOR DYNAMIC FOR
    SELECT ProductID, Currency, Price
    FROM Prices;

OPEN price_cursor;

-- Fetch the first row
FETCH NEXT FROM price_cursor INTO @ProductID, @Currency, @Price;

-- Iterate through each row in the cursor
WHILE @@FETCH_STATUS = 0
BEGIN
    -- Check if the currency exists in Rates table
    SELECT @RatePLN = PricePLN
    FROM Rates
    WHERE Currency = @Currency;

    IF @RatePLN IS NOT NULL
    BEGIN
        -- Currency exists, so update Price based on PricePLN in Rates
        UPDATE Prices
        SET Price = @Price * @RatePLN  -- Converts original price to PLN
        WHERE CURRENT OF price_cursor;
    END
    ELSE
    BEGIN
        -- Currency does not exist in Rates, so delete row from Prices
        DELETE FROM Prices
        WHERE CURRENT OF price_cursor;
    END

    -- Fetch the next row
    FETCH NEXT FROM price_cursor INTO @ProductID, @Currency, @Price;
END

-- Clean up
CLOSE price_cursor;
DEALLOCATE price_cursor;

-- Display the updated Prices table
SELECT * FROM Prices;
