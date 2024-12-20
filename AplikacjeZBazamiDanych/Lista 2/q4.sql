-- Drop the table type if it already exists
DROP PROCEDURE IF EXISTS GetReaderBorrowingDays1;
GO

IF EXISTS (SELECT * FROM sys.types WHERE is_table_type = 1 AND name = 'ReaderIDsTable')
BEGIN
    DROP TYPE ReaderIDsTable;
END
GO


-- Create a table type to hold the list of reader IDs
CREATE TYPE ReaderIDsTable AS TABLE
(
    Czytelnik_ID INT
);
GO

-- Create the stored procedure that takes the table-valued parameter as input
CREATE PROCEDURE GetReaderBorrowingDays1
    @ReaderIDs ReaderIDsTable READONLY
AS
BEGIN
    SELECT 
        W.Czytelnik_ID,
        SUM(W.Liczba_Dni) AS TotalDaysBorrowed
    FROM 
        Wypozyczenie W
    INNER JOIN 
        @ReaderIDs R ON W.Czytelnik_ID = R.Czytelnik_ID
    GROUP BY 
        W.Czytelnik_ID
    ORDER BY 
        W.Czytelnik_ID;
END
GO
-- Declare a variable of the table type and populate it with data

DECLARE @Readers ReaderIDsTable;

INSERT INTO @Readers (Czytelnik_ID)
VALUES (1), (2), (3), (4);

-- Execute the procedure with the table of reader IDs
EXEC GetReaderBorrowingDays1 @Readers;
