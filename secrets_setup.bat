@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul 2>&1
title Setup VMC Sabiana - Secrets Configuration

echo.
echo ========================================
echo    🏠 VMC SABIANA - HOME ASSISTANT
echo         Setup Configurazione
echo ========================================
echo.

REM Controlla se secrets.yaml esiste già
if exist "secrets.yaml" (
    echo ⚠️  Il file secrets.yaml esiste già!
    echo.
    echo Vuoi sovrascriverlo?
    echo [S] Si, sovrascrivi
    echo [N] No, esci
    echo.
    choice /c SN /n /m "Scegli (S/N): "
    if errorlevel 2 (
        echo.
        echo ❌ Setup annullato.
        goto :end
    )
    echo.
    echo ✅ Procedo con la sovrascrittura...
    echo.
)

echo 📡 CONFIGURAZIONE WIFI
echo ------------------------

REM Input WiFi SSID
:ask_wifi_ssid
echo.
echo Inserisci il nome della tua rete WiFi:
set "wifi_ssid="
set /p "wifi_ssid="
if "!wifi_ssid!"=="" (
    echo ❌ Il nome WiFi non può essere vuoto!
    goto :ask_wifi_ssid
)

REM Input WiFi Password
echo.
echo Inserisci la password WiFi (può essere vuota):
echo ATTENZIONE: La password sarà visibile sullo schermo
set "wifi_password="
set /p "wifi_password="

echo.
echo 🔒 PASSWORD DI SICUREZZA  
echo ------------------------
echo Queste password proteggono l'accesso al dispositivo ESP32
echo IMPORTANTE: Evita caratteri speciali come ^&, ^|, ^<, ^>, %%

REM Input Fallback Password
:ask_fallback
echo.
echo Inserisci password di fallback (minimo 8 caratteri):
echo Questa password serve per accedere all'ESP32 in emergenza
set "fallback_password="
set /p "fallback_password="

if "!fallback_password!"=="" (
    echo ❌ La password non può essere vuota!
    goto :ask_fallback
)

REM Controllo lunghezza password fallback
set "temp_pass=!fallback_password!"
set "count=0"
:count_fallback
if "!temp_pass:~%count%,1!"=="" (
    if !count! lss 8 (
        echo ❌ Password troppo corta! Minimo 8 caratteri, hai inserito !count!
        goto :ask_fallback
    )
    goto :fallback_ok
)
set /a count+=1
goto :count_fallback
:fallback_ok

REM Input OTA Password
:ask_ota
echo.
echo Inserisci password per aggiornamenti firmware (minimo 8 caratteri):
echo Questa password serve per aggiornare il software dell'ESP32
set "ota_password="
set /p "ota_password="

if "!ota_password!"=="" (
    echo ❌ La password non può essere vuota!
    goto :ask_ota
)

REM Controllo lunghezza password OTA
set "temp_pass=!ota_password!"
set "count=0"
:count_ota
if "!temp_pass:~%count%,1!"=="" (
    if !count! lss 8 (
        echo ❌ Password troppo corta! Minimo 8 caratteri, hai inserito !count!
        goto :ask_ota
    )
    goto :ota_ok
)
set /a count+=1
goto :count_ota
:ota_ok

echo.
echo 📋 RIEPILOGO CONFIGURAZIONE
echo ---------------------------
echo WiFi SSID: !wifi_ssid!
if "!wifi_password!"=="" (
    echo WiFi Password: ^(vuota^)
) else (
    echo WiFi Password: *** ^(nascosta^)
)
echo Password Fallback: *** ^(nascosta^)
echo Password OTA: *** ^(nascosta^)

echo.
echo Confermi questi dati?
choice /c SN /n /m "Vuoi creare il file con questi dati? (S/N): "
if errorlevel 2 (
    echo.
    echo ❌ Setup annullato dall'utente.
    goto :end
)

echo.
echo 🔑 Generazione chiave crittografia...

REM Genera timestamp per chiave univoca
set "timestamp=%date:~-4%%date:~3,2%%date:~0,2%_%time:~0,2%%time:~3,2%%time:~6,2%"
set "timestamp=!timestamp: =0!"
set "api_key=VMC_Sabiana_!timestamp!_ESPHome_API_Key"

echo ✅ Chiave generata: !api_key!
echo.
echo 📝 Creazione file secrets.yaml...

REM Crea il file secrets.yaml - versione sicura per caratteri speciali
(
echo # VMC Sabiana - Home Assistant Integration
echo # File generato automaticamente il %date% alle %time%
echo # 
echo # ⚠️  IMPORTANTE: Non condividere questo file!
echo # Contiene password e chiavi di sicurezza
echo.
echo # Configurazione WiFi
echo wifi_ssid: "!wifi_ssid!"
echo wifi_password: "!wifi_password!"
echo.
echo # Password di sicurezza del dispositivo
echo fallback_password: "!fallback_password!"
echo ota_password: "!ota_password!"
echo.
echo # Chiave crittografia API ^(generata automaticamente^)
echo api_encryption_key: "!api_key!"
echo.
echo # Note:
echo # - Mantieni questo file nella stessa cartella dei file .yaml
echo # - Non condividere mai questo file con altri
echo # - Fai un backup quando tutto funziona
) > secrets.yaml

REM Verifica creazione file
if exist "secrets.yaml" (
    echo.
    echo ✅ SUCCESS!
    echo ========================================
    echo 📁 File 'secrets.yaml' creato con successo!
    echo 🔒 File salvato nella cartella corrente
    echo.
    echo 🚀 PROSSIMI PASSI:
    echo 1. Apri ESPHome Dashboard
    echo    ^(http://localhost:6052 se usi Docker^)
    echo 2. Carica TUTTI i file .yaml del progetto
    echo 3. Compila il firmware ^(pulsante INSTALL^)
    echo 4. Carica il firmware sull'ESP32
    echo 5. Collega ESP32 alla VMC tramite cavo Modbus
    echo.
    echo 💡 SUGGERIMENTI IMPORTANTI:
    echo - Tieni secrets.yaml nella STESSA cartella degli altri .yaml
    echo - Se cambi password WiFi, modifica il file secrets.yaml
    echo - Annota le password in un posto sicuro
    echo - Testa prima la connessione WiFi dell'ESP32
    echo ========================================
) else (
    echo.
    echo ❌ ERRORE: Impossibile creare il file secrets.yaml
    echo.
    echo Possibili cause:
    echo - Permessi insufficienti nella cartella
    echo - Cartella protetta da antivirus
    echo - Spazio disco insufficiente
    echo.
    echo Soluzioni:
    echo 1. Esegui come amministratore ^(tasto destro → "Esegui come amministratore"^)
    echo 2. Controlla che la cartella non sia protetta
    echo 3. Prova a spostare i file in un'altra cartella
)

:end
echo.
echo ========================================
pause