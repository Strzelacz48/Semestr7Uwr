-- Insert 5 specimens for BookID = 1, with unique SpecimenID values
INSERT INTO Specimens (SpecimenID, BookID) VALUES (1, 1);
INSERT INTO Specimens (SpecimenID, BookID) VALUES (2, 1);
INSERT INTO Specimens (SpecimenID, BookID) VALUES (3, 1);
INSERT INTO Specimens (SpecimenID, BookID) VALUES (4, 1);
INSERT INTO Specimens (SpecimenID, BookID) VALUES (5, 1);

-- Try inserting a 6th specimen for BookID = 1, which should trigger the error from the trigger
-- The SpecimenID here is 6, which is unique and should avoid the primary key error
--INSERT INTO Specimens (SpecimenID, BookID) VALUES (6, 1);  -- This will fail due to the trigger

SELECT * FROM Specimens