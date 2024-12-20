USE AdventureWorksLT2022;
GO

IF OBJECT_ID('SalesLT.CustomerBackup', 'U') IS NOT NULL
    DROP TABLE SalesLT.CustomerBackup;
GO

-- Create the backup table with the same structure as SalesLT.Customer
SELECT *
INTO SalesLT.CustomerBackup
FROM SalesLT.Customer
WHERE 1 = 0;  -- Copy structure only, no data
GO

-- Turn on IDENTITY_INSERT for the target table
SET IDENTITY_INSERT SalesLT.CustomerBackup ON;

-- Record the start time
DECLARE @StartTime1 DATETIME = GETDATE();

-- Insert all rows from Customer into CustomerBackup with a column list
INSERT INTO SalesLT.CustomerBackup (
    CustomerID, NameStyle, Title, FirstName, MiddleName, LastName, Suffix,
    CompanyName, SalesPerson, EmailAddress, Phone, PasswordHash, PasswordSalt,
    rowguid, ModifiedDate
)
SELECT 
    CustomerID, NameStyle, Title, FirstName, MiddleName, LastName, Suffix,
    CompanyName, SalesPerson, EmailAddress, Phone, PasswordHash, PasswordSalt,
    rowguid, ModifiedDate
FROM SalesLT.Customer;

-- Record the end time
DECLARE @EndTime1 DATETIME = GETDATE();

-- Turn off IDENTITY_INSERT
SET IDENTITY_INSERT SalesLT.CustomerBackup OFF;

-- Calculate and display the duration in milliseconds
SELECT DATEDIFF(MILLISECOND, @StartTime1, @EndTime1) AS BulkInsertDuration;
GO

-- Turn on IDENTITY_INSERT for the target table
SET IDENTITY_INSERT SalesLT.CustomerBackup ON;

-- Record the start time
DECLARE @StartTime2 DATETIME = GETDATE();

-- Declare a cursor for selecting data from SalesLT.Customer
DECLARE customer_cursor CURSOR FOR
SELECT CustomerID, NameStyle, Title, FirstName, MiddleName, LastName, Suffix,
       CompanyName, SalesPerson, EmailAddress, Phone, PasswordHash, PasswordSalt,
       rowguid, ModifiedDate
FROM SalesLT.Customer;

-- Open the cursor
OPEN customer_cursor;

-- Variables to hold column values
DECLARE @CustomerID INT;
DECLARE @NameStyle BIT;
DECLARE @Title NVARCHAR(8);
DECLARE @FirstName NVARCHAR(50);
DECLARE @MiddleName NVARCHAR(50);
DECLARE @LastName NVARCHAR(50);
DECLARE @Suffix NVARCHAR(10);
DECLARE @CompanyName NVARCHAR(128);
DECLARE @SalesPerson NVARCHAR(256);
DECLARE @EmailAddress NVARCHAR(50);
DECLARE @Phone NVARCHAR(25);
DECLARE @PasswordHash NVARCHAR(128);
DECLARE @PasswordSalt NVARCHAR(10);
DECLARE @rowguid UNIQUEIDENTIFIER;
DECLARE @ModifiedDate DATETIME;

-- Fetch the first row
FETCH NEXT FROM customer_cursor INTO 
    @CustomerID, @NameStyle, @Title, @FirstName, @MiddleName, 
    @LastName, @Suffix, @CompanyName, @SalesPerson, @EmailAddress, 
    @Phone, @PasswordHash, @PasswordSalt, @rowguid, @ModifiedDate;

-- Loop through each row in the cursor
WHILE @@FETCH_STATUS = 0
BEGIN
    -- Insert the current row into the backup table
    INSERT INTO SalesLT.CustomerBackup (
        CustomerID, NameStyle, Title, FirstName, MiddleName, LastName, Suffix,
        CompanyName, SalesPerson, EmailAddress, Phone, PasswordHash, PasswordSalt,
        rowguid, ModifiedDate
    )
    VALUES (
        @CustomerID, @NameStyle, @Title, @FirstName, @MiddleName, 
        @LastName, @Suffix, @CompanyName, @SalesPerson, @EmailAddress, 
        @Phone, @PasswordHash, @PasswordSalt, @rowguid, @ModifiedDate
    );
    
    -- Fetch the next row
    FETCH NEXT FROM customer_cursor INTO 
        @CustomerID, @NameStyle, @Title, @FirstName, @MiddleName, 
        @LastName, @Suffix, @CompanyName, @SalesPerson, @EmailAddress, 
        @Phone, @PasswordHash, @PasswordSalt, @rowguid, @ModifiedDate;
END;

-- Close and deallocate the cursor
CLOSE customer_cursor;
DEALLOCATE customer_cursor;

-- Record the end time
DECLARE @EndTime2 DATETIME = GETDATE();

-- Turn off IDENTITY_INSERT
SET IDENTITY_INSERT SalesLT.CustomerBackup OFF;

-- Calculate and display the duration in milliseconds
SELECT DATEDIFF(MILLISECOND, @StartTime2, @EndTime2) AS CursorInsertDuration;
GO
