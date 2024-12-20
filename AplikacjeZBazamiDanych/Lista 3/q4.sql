USE AdventureWorksLT2022;
GO
-- Drop the trigger if it exists
IF OBJECT_ID('SalesLT.trg_UpdateCustomerModifiedDate', 'TR') IS NOT NULL
    DROP TRIGGER SalesLT.trg_UpdateCustomerModifiedDate;
GO

-- Create the trigger in the correct schema
CREATE TRIGGER SalesLT.trg_UpdateCustomerModifiedDate
ON SalesLT.Customer
AFTER UPDATE
AS
BEGIN
    -- Update the ModifiedDate to the current date and time for modified rows
    UPDATE SalesLT.Customer
    SET ModifiedDate = GETDATE()
    FROM inserted
    WHERE SalesLT.Customer.CustomerID = inserted.CustomerID;
END;
GO

-- Update a customer record
UPDATE SalesLT.Customer
SET FirstName = 'John', LastName = 'Doe'
WHERE CustomerID = 1; -- Replace with an existing CustomerID

-- Check the ModifiedDate
SELECT CustomerID, FirstName, LastName, ModifiedDate
FROM SalesLT.Customer
WHERE CustomerID = 1; -- Replace with the same CustomerID
