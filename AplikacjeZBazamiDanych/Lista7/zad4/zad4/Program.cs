using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Neo4j.Driver;

public class FetchPersonsExample : IDisposable
{
    private readonly IDriver _driver;

    public FetchPersonsExample(string uri, string user, string password)
    {
        _driver = GraphDatabase.Driver(uri, AuthTokens.Basic(user, password));
    }

    public async Task FetchAndPrintPersons()
    {

        using var session = _driver.AsyncSession();
        var persons = await session.ExecuteReadAsync(async tx =>
        {
            var result = await tx.RunAsync("MATCH (p:Person) RETURN id(p) AS id, p.name AS name");
            var personsList = new List<(int id, string name)>();

            await foreach (var record in result)
            {
                var id = record["id"].As<int>();
                var name = record["name"].As<string>();
                personsList.Add((id, name));
            }

            return personsList;
        });

        PrintAsTable(persons);
    }

    private void PrintAsTable(IEnumerable<(int id, string name)> persons)
    {
        Console.WriteLine("{0,-10} {1,-20}", "ID", "Name");
        Console.WriteLine(new string('-', 30));
        foreach (var person in persons)
        {
            Console.WriteLine("{0,-10} {1,-20}", person.id, person.name);
        }
    }

    public void Dispose()
    {
        _driver?.Dispose();
    }

    public static async Task Main()
    {
        using var app = new FetchPersonsExample("neo4j+s://f554519a.databases.neo4j.io:7687", "neo4j", "h2P5Vu22KBUjSsYJGF4rtQjcocYLFNt9BlXRDd_GlMw");

        await app.FetchAndPrintPersons();
    }
}
