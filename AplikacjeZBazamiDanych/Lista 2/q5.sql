USE AdventureWorksLT2022;

-- Drop the procedure if it already exists
IF OBJECT_ID('SetDiscontinuedDate', 'P') IS NOT NULL
BEGIN
    DROP PROCEDURE SetDiscontinuedDate;
END
GO

-- Drop the table type if it already exists
IF EXISTS (SELECT * FROM sys.types WHERE is_table_type = 1 AND name = 'ProductIDTable')
BEGIN
    DROP TYPE ProductIDTable;
END
GO

-- Create a table type for the list of product IDs
CREATE TYPE ProductIDTable AS TABLE
(
    ProductID INT
);
GO

-- Create the stored procedure
CREATE PROCEDURE SetDiscontinuedDate
    @ProductIDs ProductIDTable READONLY,
    @DiscontinuedDate DATE
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @ProductID INT;

    -- Cursor to iterate through the product IDs in the table type
    DECLARE product_cursor CURSOR FOR
        SELECT ProductID FROM @ProductIDs;

    OPEN product_cursor;

    FETCH NEXT FROM product_cursor INTO @ProductID;

    WHILE @@FETCH_STATUS = 0
		BEGIN
        -- Check if the product has a NULL DiscontinuedDate
        IF EXISTS (SELECT 1 FROM [SalesLT].[Product] WHERE ProductID = @ProductID AND DiscontinuedDate IS NULL)
        BEGIN
            -- Update the DiscontinuedDate for products with a NULL value
            UPDATE SalesLT.Product
            SET DiscontinuedDate = @DiscontinuedDate
            WHERE ProductID = @ProductID AND DiscontinuedDate IS NULL;
        END
        ELSE
        BEGIN
            -- Print a message if the DiscontinuedDate is already set
            PRINT 'ProductID ' + CAST(@ProductID AS VARCHAR(10)) + ' already has a DiscontinuedDate.';
        END

        FETCH NEXT FROM product_cursor INTO @ProductID;
    END

    CLOSE product_cursor;
    DEALLOCATE product_cursor;
END
GO

-- Declare a variable of the ProductIDTable type
DECLARE @ProductList ProductIDTable;

-- Insert product IDs into the table variable
INSERT INTO @ProductList (ProductID)
VALUES (680), (706), (707), (708); -- Each product ID should be in its own set of parentheses

-- Execute the procedure with the list of product IDs and a specific discontinued date
EXEC SetDiscontinuedDate @ProductList, '2024-10-19';
