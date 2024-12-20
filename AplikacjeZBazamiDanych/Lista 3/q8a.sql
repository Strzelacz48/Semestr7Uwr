CREATE TRIGGER trg_ReorderIfLow
ON Inventory
AFTER UPDATE
AS
BEGIN
    DECLARE @ProductID INT, @Quantity INT;

    -- Capture the ProductID and new Quantity from the updated row
    SELECT @ProductID = ProductID, @Quantity = Quantity
    FROM inserted;

    -- Check if the new quantity is below threshold and if so, create a new order
    IF @Quantity < 10
    BEGIN
        -- Place an order for 20 more units (recursive call)
        INSERT INTO Orders (ProductID, OrderQuantity)
        VALUES (@ProductID, 20);

        -- Update Inventory to simulate the new stock arriving (recursive action)
        UPDATE Inventory
        SET Quantity = Quantity + 20
        WHERE ProductID = @ProductID;
    END
END;

DROP TABLE IF EXISTS Inventory;
DROP TABLE IF EXISTS Orders;

CREATE TABLE Inventory (
    ProductID INT PRIMARY KEY,
    ProductName VARCHAR(50),
    Quantity INT
);

CREATE TABLE Orders (
    OrderID INT IDENTITY PRIMARY KEY,
    ProductID INT,
    OrderQuantity INT,
    OrderDate DATETIME DEFAULT GETDATE()
);

INSERT INTO Inventory (ProductID, ProductName, Quantity)
VALUES (1, 'Sample Product', 15);

UPDATE Inventory
SET Quantity = 5
WHERE ProductID = 1;

SELECT * FROM Inventory