db.books.insertMany([
    {
      _id: 1,
      ISBN: "83-246-0279-8",
      title: "Microsoft Access. Podręcznik administratora",
      author: "Helen Feddema",
      year: 2006,
      price: 69.00,
      borrowed_last_month: false,
      copies: [
        { copy_id: 1, signature: "S0001" },
        { copy_id: 2, signature: "S0002" }
      ]
    },
    {
      _id: 2,
      ISBN: "978-83-246-2188-0",
      title: "Czysty kod. Podręcznik dobrego programisty",
      author: "Robert C. Martin",
      year: 2010,
      price: 67.00,
      borrowed_last_month: true,
      copies: [
        { copy_id: 3, signature: "S0011" }
      ]
    }
  ]);
  