using BattleNet.Core.Configuration;
using BattleNet.Persistence;
using BattleNet.Persistence.Models;
using BattleNet.Server;
using BattleNet.Server.Services;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System.Net;

var builder = WebApplication.CreateBuilder(args);
var options = ServerOptions.From(builder.Configuration, args);
var serviceMode = ServiceMode.Parse(args);
var realmConnectionString = options.RealmConnectionString ?? options.AuthConnectionString;
var localRepositories = new FileRepositories(options);
var worldContentRepository = new FileWorldContentRepository(options);

builder.WebHost.ConfigureKestrel(serverOptions =>
    serverOptions.Listen(IPAddress.Loopback, options.AccountWebPort));

builder.Services.AddSingleton(options);
builder.Services.AddSingleton<IAuthRepository>(_ =>
    string.IsNullOrWhiteSpace(options.AuthConnectionString)
        ? localRepositories
        : new MySqlAuthRepository(options.AuthConnectionString, options));
builder.Services.AddSingleton<IRealmRepository>(_ =>
    string.IsNullOrWhiteSpace(realmConnectionString)
        ? localRepositories
        : new MySqlRealmRepository(realmConnectionString, options.WorldConnectionString));
builder.Services.AddSingleton<IQuestRepository>(_ =>
    string.IsNullOrWhiteSpace(realmConnectionString)
        ? localRepositories
        : new InMemoryQuestRepository());
builder.Services.AddSingleton<ICharacterMutationRepository>(_ =>
    string.IsNullOrWhiteSpace(realmConnectionString)
        ? localRepositories
        : new InMemoryCharacterMutationRepository());
builder.Services.AddSingleton<ICharacterStateRepository>(_ =>
    string.IsNullOrWhiteSpace(realmConnectionString)
        ? localRepositories
        : new InMemoryCharacterStateRepository());
builder.Services.AddSingleton<IWorldRepository>(_ =>
    string.IsNullOrWhiteSpace(options.WorldConnectionString)
        ? localRepositories
        : new MySqlWorldRepository(options.WorldConnectionString));
builder.Services.AddSingleton<IDbcRepository>(_ =>
    string.IsNullOrWhiteSpace(options.DbcConnectionString)
        ? localRepositories
        : new MySqlDbcRepository(options.DbcConnectionString));
builder.Services.AddSingleton<IWorldContentRepository>(worldContentRepository);
builder.Services.AddSingleton<WorldSocialState>();

if (serviceMode.Includes("login"))
    builder.Services.AddHostedService<LoginServerService>();
if (serviceMode.Includes("realm"))
    builder.Services.AddHostedService<RealmServerService>();
if (serviceMode.Includes("proxy"))
    builder.Services.AddHostedService<ProxyServerService>();
if (serviceMode.Includes("world"))
    builder.Services.AddHostedService<WorldServerService>();
if (serviceMode.Includes("update"))
    builder.Services.AddHostedService<UpdateServerService>();

await using var host = builder.Build();
host.MapGet("/account/create", AccountCreationPage.Show);
host.MapPost("/account/create", AccountCreationPage.CreateAsync);

var logger = host.Services.GetRequiredService<ILoggerFactory>().CreateLogger("BattleNet");
logger.LogInformation("BattleNet server starting in {Mode} mode", serviceMode.Description);
logger.LogInformation("Client protocol target: {ClientVersion}", options.SupportedClient);
logger.LogInformation(
    "Account creation page: http://127.0.0.1:{AccountWebPort}/account/create",
    options.AccountWebPort);
if (string.IsNullOrWhiteSpace(options.AuthConnectionString)
    && string.IsNullOrWhiteSpace(options.RealmConnectionString)
    && string.IsNullOrWhiteSpace(options.WorldConnectionString)
    && string.IsNullOrWhiteSpace(options.DbcConnectionString))
    logger.LogInformation("Using self-contained JSON tables at {DataDirectory}; no MySQL installation is required.", options.DataDirectory);
else
    logger.LogInformation("Using the configured Alpha Core-compatible auth database.");
await host.RunAsync();

internal sealed class ServiceMode(HashSet<string> services)
{
    public string Description => string.Join(',', services.Order());

    public bool Includes(string service) => services.Contains(service);

    public static ServiceMode Parse(string[] args)
    {
        var requested = args.FirstOrDefault(arg => arg.StartsWith("--service=", StringComparison.OrdinalIgnoreCase));
        if (requested is not null)
            requested = requested["--service=".Length..];
        requested ??= args.SkipWhile(arg => !arg.Equals("--service", StringComparison.OrdinalIgnoreCase)).Skip(1).FirstOrDefault();
        var values = requested?.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        var selected = values is { Length: > 0 }
            ? values.ToHashSet(StringComparer.OrdinalIgnoreCase)
            : ["login", "realm", "proxy", "world", "update"];
        if (selected.Contains("all"))
            selected = ["login", "realm", "proxy", "world", "update"];
        return new ServiceMode(selected);
    }
}

internal static class AccountCreationPage
{
    public static IResult Show() => Results.Content(Render(), "text/html; charset=utf-8");

    public static async Task<IResult> CreateAsync(
        HttpRequest request,
        IAuthRepository repository,
        ILoggerFactory loggerFactory,
        CancellationToken cancellationToken)
    {
        if (!request.HasFormContentType)
            return Results.Content(
                Render("The account form was not submitted correctly."),
                "text/html; charset=utf-8",
                statusCode: StatusCodes.Status400BadRequest);

        var form = await request.ReadFormAsync(cancellationToken).ConfigureAwait(false);
        var username = form["accountName"].ToString();
        var password = form["password"].ToString();
        var confirmation = form["confirmPassword"].ToString();
        if (!string.Equals(password, confirmation, StringComparison.Ordinal))
            return Results.Content(
                Render("The passwords do not match.", username: username),
                "text/html; charset=utf-8",
                statusCode: StatusCodes.Status400BadRequest);

        try
        {
            var result = await repository
                .CreateAccountAsync(username, password, cancellationToken)
                .ConfigureAwait(false);
            return result switch
            {
                AccountCreateResult.Success => Results.Content(
                    Render(
                        $"Account {AccountRegistration.NormalizeUsername(username)} was created. Return to the game and log in with it.",
                        success: true),
                    "text/html; charset=utf-8",
                    statusCode: StatusCodes.Status201Created),
                AccountCreateResult.InvalidUsername => Results.Content(
                    Render(
                        $"Account names must be {AccountRegistration.MinimumUsernameLength}-{AccountRegistration.MaximumUsernameLength} letters or numbers.",
                        username: username),
                    "text/html; charset=utf-8",
                    statusCode: StatusCodes.Status400BadRequest),
                AccountCreateResult.InvalidPassword => Results.Content(
                    Render(
                        $"Passwords must be {AccountRegistration.MinimumPasswordLength}-{AccountRegistration.MaximumPasswordLength} printable characters.",
                        username: username),
                    "text/html; charset=utf-8",
                    statusCode: StatusCodes.Status400BadRequest),
                AccountCreateResult.AlreadyExists => Results.Content(
                    Render("That account name is already in use.", username: username),
                    "text/html; charset=utf-8",
                    statusCode: StatusCodes.Status409Conflict),
                _ => Results.Content(
                    Render("The account could not be created."),
                    "text/html; charset=utf-8",
                    statusCode: StatusCodes.Status500InternalServerError)
            };
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
            throw;
        }
        catch (Exception exception)
        {
            loggerFactory.CreateLogger("AccountCreation").LogError(
                exception,
                "Failed to create account {AccountName} from {RemoteAddress}",
                AccountRegistration.NormalizeUsername(username),
                request.HttpContext.Connection.RemoteIpAddress);
            return Results.Content(
                Render("The server could not create the account. Please try again."),
                "text/html; charset=utf-8",
                statusCode: StatusCodes.Status500InternalServerError);
        }
    }

    private static string Render(string message = "", bool success = false, string username = "")
    {
        var encodedUsername = WebUtility.HtmlEncode(username);
        var notice = string.IsNullOrEmpty(message)
            ? string.Empty
            : $"<p class=\"{(success ? "success" : "error")}\">{WebUtility.HtmlEncode(message)}</p>";
        return $$"""
            <!doctype html>
            <html lang="en">
            <head>
              <meta charset="utf-8">
              <meta name="viewport" content="width=device-width,initial-scale=1">
              <title>Create World of Warcraft Account</title>
              <style>
                :root { color-scheme: dark; font-family: Georgia, serif; }
                body { min-height: 100vh; margin: 0; display: grid; place-items: center; background: #080706; color: #d8c79a; }
                main { width: min(92vw, 430px); padding: 32px; border: 1px solid #66501d; background: #15120d; box-shadow: 0 0 32px #000; }
                h1 { margin: 0 0 8px; color: #f3c729; font-size: 28px; }
                p { line-height: 1.45; }
                label { display: block; margin-top: 18px; color: #f3c729; font-weight: bold; }
                input { box-sizing: border-box; width: 100%; margin-top: 6px; padding: 11px; border: 1px solid #7b642d; background: #050505; color: white; font: 16px sans-serif; }
                button { width: 100%; margin-top: 24px; padding: 12px; border: 1px solid #ad8d39; background: #31250c; color: #f3c729; font-weight: bold; cursor: pointer; }
                .help { color: #9e947d; font-size: 14px; }
                .error { color: #ff8b70; }
                .success { color: #8fe18f; }
              </style>
            </head>
            <body>
              <main>
                <h1>Create Account</h1>
                <p>Create an account for this preserved local Alpha realm.</p>
                {{notice}}
                <form method="post" action="/account/create">
                  <label for="accountName">Account name</label>
                  <input id="accountName" name="accountName" value="{{encodedUsername}}" minlength="3" maxlength="32" pattern="[A-Za-z0-9]+" autocomplete="username" required autofocus>
                  <label for="password">Password</label>
                  <input id="password" name="password" type="password" minlength="6" maxlength="64" autocomplete="new-password" required>
                  <label for="confirmPassword">Confirm password</label>
                  <input id="confirmPassword" name="confirmPassword" type="password" minlength="6" maxlength="64" autocomplete="new-password" required>
                  <button type="submit">Create Account</button>
                </form>
                <p class="help">Account names are not case-sensitive. Passwords are stored as an SRP6 verifier used by the original client login protocol.</p>
              </main>
            </body>
            </html>
            """;
    }
}
