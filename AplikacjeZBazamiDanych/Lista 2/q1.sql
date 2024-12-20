DROP FUNCTION IF EXISTS borrowed;
GO

CREATE FUNCTION borrowed (@days INT) 
RETURNS TABLE
AS
RETURN
    SELECT 
        Czytelnik.PESEL, 
        COUNT(Wypozyczenie.Wypozyczenie_ID) AS LiczbaEgzemplarzy
    FROM 
        Czytelnik
    INNER JOIN 
        Wypozyczenie ON Czytelnik.Czytelnik_ID = Wypozyczenie.Czytelnik_ID
    WHERE 
        Wypozyczenie.Liczba_Dni <= @days
    GROUP BY 
        Czytelnik.PESEL
    HAVING 
        COUNT(Wypozyczenie.Wypozyczenie_ID) > 0;
GO


SELECT * FROM borrowed(5)
SELECT * FROM borrowed(15)
SELECT * FROM borrowed(20)
SELECT * FROM borrowed(8)