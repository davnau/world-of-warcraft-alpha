$ErrorActionPreference = 'Stop'

$projectPath = Join-Path $PSScriptRoot 'BattleNet.Server\BattleNet.Server.csproj'
$publishPath = 'E:\World of Warcraft'
$tablesPath = Join-Path $publishPath 'tables'
$stagingPath = Join-Path $publishPath 'out\battlenet-publish'

New-Item -ItemType Directory -Force -Path $tablesPath | Out-Null
if (Test-Path -LiteralPath $stagingPath) {
    Remove-Item -LiteralPath $stagingPath -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $stagingPath | Out-Null

$restoreArguments = @($projectPath, '--runtime', 'win-x64')
& dotnet restore @restoreArguments
if ($LASTEXITCODE -ne 0) {
    throw "dotnet restore failed with exit code $LASTEXITCODE"
}

$publishArguments = @(
    $projectPath,
    '--configuration', 'Release',
    '--runtime', 'win-x64',
    '--self-contained', 'true',
    '--output', $stagingPath,
    '-p:PublishSingleFile=true',
    '-p:IncludeNativeLibrariesForSelfExtract=true',
    '-p:EnableCompressionInSingleFile=true',
    '--no-restore')
& dotnet publish @publishArguments

if ($LASTEXITCODE -ne 0) {
    throw "dotnet publish failed with exit code $LASTEXITCODE"
}

Get-ChildItem -LiteralPath $stagingPath -File | Copy-Item -Destination $publishPath -Force
if (-not (Test-Path -LiteralPath (Join-Path $publishPath 'BattleNet.Server.exe'))) {
    throw 'Publish completed without creating BattleNet.Server.exe.'
}

Write-Host "Published BattleNet.Server.exe to $publishPath"
Write-Host "Local table directory: $tablesPath"
