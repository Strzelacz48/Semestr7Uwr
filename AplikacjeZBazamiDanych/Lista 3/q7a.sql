-- Drop tables if they already exist
DROP TABLE IF EXISTS Specimens;
DROP TABLE IF EXISTS Books;

-- Create Books table
CREATE TABLE Books (
    BookID INT PRIMARY KEY,
    Title VARCHAR(100) NOT NULL
);

-- Create Specimens table with a foreign key to Books
CREATE TABLE Specimens (
    SpecimenID INT PRIMARY KEY,
    BookID INT,
    FOREIGN KEY (BookID) REFERENCES Books(BookID) ON DELETE CASCADE
);

-- Insert sample data into Books
INSERT INTO Books (BookID, Title)
VALUES (1, 'SQL Fundamentals'),
       (2, 'Advanced SQL Techniques');
