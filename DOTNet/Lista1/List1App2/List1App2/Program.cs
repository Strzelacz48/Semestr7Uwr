var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", () => "Witaj w mojej aplikacji ASP.NET Core!");  // Konfiguracja punktu ko�cowego GET

app.Run();
