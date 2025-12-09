@echo off
REM Batch script per eseguire i test su Windows (alternativa a PowerShell)

echo ===================================
echo ESPHome Schedule Tests
echo ===================================
echo.

REM Verifica Docker
docker ps >nul 2>&1
if errorlevel 1 (
    echo ERRORE: Docker non e' in esecuzione!
    echo Avvia Docker Desktop e riprova.
    exit /b 1
)

echo Building container con Google Test...
docker-compose -f ../docker-compose.yaml build esphome-test

if errorlevel 1 (
    echo ERRORE: Build fallita!
    exit /b 1
)

echo.
echo Running tests...
echo.

docker-compose -f ../docker-compose.yaml --profile test run --rm esphome-test

if errorlevel 1 (
    echo.
    echo ERRORE: Alcuni test sono falliti!
    exit /b 1
)

echo.
echo ===================================
echo Tutti i test sono passati!
echo ===================================