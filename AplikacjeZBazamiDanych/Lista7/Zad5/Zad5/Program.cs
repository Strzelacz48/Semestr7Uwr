using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Neo4j.Driver;
using Zad5;

public class Neo4jExample : IDisposable
{
    private readonly IDriver _driver;

    public Neo4jExample(string uri, string user, string password)
    {
        _driver = GraphDatabase.Driver(uri, AuthTokens.Basic(user, password));
    }

    // Create a Person
    public async Task CreatePerson(string name)
    {
        using var session = _driver.AsyncSession();
        await session.ExecuteWriteAsync(async tx =>
        {
            await tx.RunAsync("CREATE (p:Person {name: $name}) RETURN id(p) AS id", new { name });
        });
    }

    // Read all Persons
    public async Task<List<Person>> GetAllPersons()
    {
        using var session = _driver.AsyncSession();
        return await session.ExecuteReadAsync(async tx =>
        {
            var result = await tx.RunAsync("MATCH (p:Person) RETURN id(p) AS id, p.name AS name");
            var persons = new List<Person>();

            await foreach (var record in result)
            {
                persons.Add(new Person
                {
                    Id = record["id"].As<int>(),
                    Name = record["name"].As<string>()
                });
            }

            return persons;
        });
    }

    // Update a Person's name
    public async Task UpdatePersonName(int id, string newName)
    {
        using var session = _driver.AsyncSession();
        await session.ExecuteWriteAsync(async tx =>
        {
            await tx.RunAsync("MATCH (p:Person) WHERE id(p) = $id SET p.name = $newName", new { id, newName });
        });
    }

    // Delete a Person
    public async Task DeletePerson(int id)
    {
        using var session = _driver.AsyncSession();
        await session.ExecuteWriteAsync(async tx =>
        {
            await tx.RunAsync("MATCH (p:Person) WHERE id(p) = $id DETACH DELETE p", new { id });
        });
    }

    // Get all relationships (Person ACTED_IN Movie)
    public async Task<List<ActedIn>> GetActedInRelationships()
    {
        using var session = _driver.AsyncSession();
        return await session.ExecuteReadAsync(async tx =>
        {
            var result = await tx.RunAsync(@"
                MATCH (p:Person)-[r:ACTED_IN]->(m:Movie)
                RETURN id(p) AS personId, p.name AS personName, id(m) AS movieId, m.title AS movieTitle, r.role AS role");

            var relationships = new List<ActedIn>();

            await foreach (var record in result)
            {
                relationships.Add(new ActedIn
                {
                    Actor = new Person
                    {
                        Id = record["personId"].As<int>(),
                        Name = record["personName"].As<string>()
                    },
                    Movie = new Movie
                    {
                        Id = record["movieId"].As<int>(),
                        Title = record["movieTitle"].As<string>()
                    },
                    Role = record["role"].As<string>()
                });
            }

            return relationships;
        });
    }

    public void Dispose()
    {
        _driver?.Dispose();
    }

    public static async Task Main()
    {
        // Connection parameters
        var uri = "neo4j+s://f554519a.databases.neo4j.io:7687";
        var user = "neo4j";
        var password = "h2P5Vu22KBUjSsYJGF4rtQjcocYLFNt9BlXRDd_GlMw";

        using var app = new Neo4jExample(uri, user, password);

        // Create a person
        await app.CreatePerson("Leonardo DiCaprio");

        // Fetch all persons
        var persons = await app.GetAllPersons();
        Console.WriteLine("Persons:");
        foreach (var person in persons)
        {
            Console.WriteLine($"ID: {person.Id}, Name: {person.Name}");
        }

        // Update a person's name
        if (persons.Count > 0)
        {
            await app.UpdatePersonName(persons[0].Id, "Leo DiCaprio");
        }

        // Fetch relationships
        var actedInRelationships = await app.GetActedInRelationships();
        Console.WriteLine("Acted In Relationships:");
        foreach (var relationship in actedInRelationships)
        {
            Console.WriteLine($"{relationship.Actor.Name} acted in {relationship.Movie.Title} as {relationship.Role}");
        }

        // Delete a person
        if (persons.Count > 0)
        {
            await app.DeletePerson(persons[0].Id);
        }
    }
}
