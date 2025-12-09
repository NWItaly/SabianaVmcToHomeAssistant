# Script PowerShell per eseguire i test su Windows
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "ESPHome Schedule Tests" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

# Verifica che Docker sia in esecuzione
try {
    docker ps | Out-Null
} catch {
    Write-Host "ERRORE: Docker non è in esecuzione!" -ForegroundColor Red
    Write-Host "Avvia Docker Desktop e riprova." -ForegroundColor Yellow
    exit 1
}

Write-Host "Building container con Google Test..." -ForegroundColor Yellow
docker-compose -f ../docker-compose.yaml build esphome-test

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERRORE: Build fallita!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Running tests..." -ForegroundColor Yellow
Write-Host ""

# Usa --profile test per avviare SOLO il container di test
docker-compose -f ../docker-compose.yaml --profile test run --rm esphome-test

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERRORE: Alcuni test sono falliti!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "===================================" -ForegroundColor Green
Write-Host "Tutti i test sono passati!" -ForegroundColor Green
Write-Host "===================================" -ForegroundColor Green