-- Create a trigger to enforce maximum 5 specimens per book
CREATE TRIGGER trg_EnforceMaxSpecimens
ON Specimens
AFTER INSERT
AS
BEGIN
    DECLARE @BookID INT;
    
    -- Get the BookID of the inserted row(s)
    SELECT @BookID = BookID FROM inserted;

    -- Check the count of specimens for this BookID
    IF (SELECT COUNT(*) FROM Specimens WHERE BookID = @BookID) > 5
    BEGIN
        -- Raise an error if more than 5 specimens are associated with the book
        RAISERROR ('A book can have a maximum of 5 specimens.', 16, 1);
        
        -- Rollback the transaction to prevent the insert
        ROLLBACK;
    END
END;
