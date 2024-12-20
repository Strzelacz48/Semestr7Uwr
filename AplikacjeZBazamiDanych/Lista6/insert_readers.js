db.readers.insertMany([
    {
      _id: 1,
      PESEL: "55101011111",
      name: "Kowalski",
      city: "Wrocław",
      date_of_birth: "1955-10-10",
      borrowings: [
        { borrowing_id: 1, copy_id: 1, date: "2020-02-01", days: 12 },
        { borrowing_id: 2, copy_id: 3, date: "2020-01-21", days: 45 }
      ]
    },
    {
      _id: 2,
      PESEL: "60101033333",
      name: "Maliniak",
      city: "Wrocław",
      date_of_birth: "1960-10-10",
      borrowings: [
        { borrowing_id: 3, copy_id: 2, date: "2020-01-13", days: 7 },
        { borrowing_id: 4, copy_id: 3, date: "2020-02-12", days: 5 }
      ]
    }
  ]);
  