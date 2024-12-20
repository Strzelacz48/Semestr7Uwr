USE AdventureWorksLT2022;

DROP TABLE IF EXISTS TestTable;


CREATE TABLE TestTable (
    ID INT PRIMARY KEY IDENTITY,
    Data VARCHAR(50)
);

-- Insert some sample data
INSERT INTO TestTable (Data) VALUES ('Row 1'), ('Row 2'), ('Row 3');


-- Set isolation level to SERIALIZABLE
SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;

BEGIN TRANSACTION;

-- Select data from TestTable without NOLOCK
SELECT * FROM [TestTable];

-- In another session, try to update or insert data to observe blocking behavior
-- Keep this transaction open for testing

-- COMMIT or ROLLBACK the transaction to release locks