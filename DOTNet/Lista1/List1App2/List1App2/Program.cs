var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", () => "Witaj w mojej aplikacji ASP.NET Core!");  // Konfiguracja punktu koñcowego GET

app.Run();
