USE AdventureWorksLT2022;  
GO  

IF EXISTS (SELECT name FROM sys.objects WHERE name = N'UpdateProductPricing')  
    DROP PROCEDURE UpdateProductPricing;  
GO  

CREATE PROCEDURE UpdateProductPricing  
    @ProductID INT,  
    @NewListPrice DECIMAL(18, 2)  
AS  
BEGIN  
    DECLARE @TranCounter INT;  
    SET @TranCounter = @@TRANCOUNT;  

    IF @TranCounter > 0  
        -- Set a savepoint for partial rollback if an active transaction exists  
        SAVE TRANSACTION ProductUpdateSave;  
    ELSE  
        -- Start a new transaction if no active transaction exists  
        BEGIN TRANSACTION;  

    BEGIN TRY  
        -- Check the StandardCost of the product before updating  
        DECLARE @CurrentCost DECIMAL(18, 2);  
        SELECT @CurrentCost = StandardCost  
        FROM SalesLT.Product  
        WHERE ProductID = @ProductID;  

        -- If the StandardCost is below a threshold, trigger an error  
        IF @CurrentCost < 50.00  
            THROW 50001, 'StandardCost is too low for price update.', 1;  

        -- Update the ListPrice if the StandardCost is acceptable  
        UPDATE SalesLT.Product  
        SET ListPrice = @NewListPrice  
        WHERE ProductID = @ProductID;  

        -- Commit the transaction if no errors occurred  
        IF @TranCounter = 0  
            COMMIT TRANSACTION;  
    END TRY  
    BEGIN CATCH  
        -- Error handling: Roll back only the work done in the procedure  
        IF @TranCounter = 0  
            -- Full rollback if transaction was started in the procedure  
            ROLLBACK TRANSACTION;  
        ELSE  
            -- Roll back to the savepoint if an active transaction exists  
            IF XACT_STATE() <> -1  
                ROLLBACK TRANSACTION ProductUpdateSave;  

        -- Return error information to the caller  
        DECLARE @ErrorMessage NVARCHAR(4000);  
        DECLARE @ErrorSeverity INT;  
        DECLARE @ErrorState INT;  

        SELECT @ErrorMessage = ERROR_MESSAGE(),  
               @ErrorSeverity = ERROR_SEVERITY(),  
               @ErrorState = ERROR_STATE();  

        RAISERROR (@ErrorMessage, @ErrorSeverity, @ErrorState);  
    END CATCH  
END;  
GO  

-- Test the procedure by running it with a product that has a low StandardCost  
EXEC UpdateProductPricing @ProductID = 707, @NewListPrice = 330.00;  
