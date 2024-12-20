CREATE TABLE Ksi��ka1 (
    BookID INT PRIMARY KEY, -- Primary key
    Title NVARCHAR(255),
    Author NVARCHAR(255),
    PublishedYear INT
);
CREATE TABLE Egzemplarz1 (
    CopyID INT PRIMARY KEY, -- Primary key
    BookID INT,             -- Foreign key referencing Ksi��ka(BookID)
    Condition NVARCHAR(50),
    LibraryBranch NVARCHAR(100),
    FOREIGN KEY (BookID) REFERENCES Ksi��ka1(BookID)
);


-- Oryginalny querry
SELECT 
    k.BookID, k.Title, e.CopyID, e.Condition
FROM 
    Ksi��ka1 k
JOIN 
    Egzemplarz1 e
ON 
    k.BookID = e.BookID
WHERE 
    k.PublishedYear > 2000 AND e.Condition = 'Good';


-- Opt.
CREATE CLUSTERED INDEX IX_Ksi��ka_BookID ON Ksi��ka1(BookID);
CREATE CLUSTERED INDEX IX_Egzemplarz_CopyID ON Egzemplarz(CopyID);

CREATE NONCLUSTERED INDEX IX_Ksi��ka_PublishedYear ON Ksi��ka1(PublishedYear);
CREATE NONCLUSTERED INDEX IX_Egzemplarz_Condition ON Egzemplarz1(Condition);

CREATE NONCLUSTERED INDEX IX_Egzemplarz_Covering
ON Egzemplarz1(BookID, Condition)
INCLUDE (CopyID, LibraryBranch);
