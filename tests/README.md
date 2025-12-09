# Testing Setup per ESPHome Schedule (Windows)

## Struttura del Progetto

```
./
├── Dockerfile.test
├── docker-compose.yml                  # <-- Contiene ENTRAMBI i container
└── tests/
    ├── example.json                    # <-- Un esempio di json usato per l'invio e la ricezione dei programmi giornalieri/settimanali
    ├── run-tests.ps1                   # <-- Script PowerShell per Windows
    ├── run-tests.bat                   # <-- Alternativa batch
    ├── build_and_test.sh               # <-- Gira dentro il container Linux
    ├── test_modbus_helpers.cpp         # <-- Test per le funzioni si supporto
    └── test_Blk4_UserTimerProgram.cpp  # <-- Test per le funzioni di conversione del json di comunicazione
```

## Due Container Separati

Il `docker-compose.yml` definisce:

1. **esphome** - Container di sviluppo normale (sempre attivo)
   - Dashboard ESPHome su porta [6052](http://localhost:6052/)
   - Compila i tuoi YAML
   - Rimane attivo ed in ascolto

2. **esphome-test** - Container per test (si chiude dopo i test)
   - Google Test installato
   - Esegue i test C++
   - Si chiude automaticamente

## Esecuzione dei Test su Windows

### 🎯 Opzione 1: VSCode (più comodo)
1. Premi `Ctrl+Shift+P`
2. Digita "Run Test Task"
3. Seleziona "Run ESPHome Tests"

Oppure:
- Premi `Ctrl+Shift+B` (se è il default test task)

### ⚡ Opzione 2: PowerShell (consigliato)
```powershell
.\run-tests.ps1
```

Se ricevi errore "script non firmato", esegui prima:
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\run-tests.ps1
```

### 📝 Opzione 3: Batch Script
```cmd
run-tests.bat
```

### 🐳 Opzione 4: Docker Compose diretto
```powershell
# Esegui SOLO i test (container esphome-test)
docker-compose -f docker-compose.yml --profile test run --rm esphome-test
```
---

## Test Coverage

La suite di test copre:

### 1. **Helper Functions**
- ✅ Validazione orari (00:00 - 23:59)
- ✅ Validazione speed (0-4, 255)

### 2. **JSON Parsing**
- ✅ Parsing corretto di JSON valido
- ✅ Rifiuto JSON senza `sb` (speed_before)
- ✅ Rifiuto speed_before invalido
- ✅ Rifiuto orari invalidi (>23:59)
- ✅ Rifiuto speed invalidi (>4 e ≠255)

### 3. **Write Complete Schedule**
- ✅ Rifiuto se giorni ≠ 7
- ✅ Scrittura esatta di 119 registri (7 giorni × 17 registri)
- ✅ Indirizzi corretti per giorno 0 e giorno 6
- ✅ Valori corretti scritti per primo intervallo
- ✅ Stop immediato su JSON invalido
- ✅ Sequenza corretta: time registers prima di speed registers
- ✅ Edge cases: 00:00, 23:59, 12:30
- ✅ Gestione speed speciale 255

## Troubleshooting

### Errore: `libgtest.so not found`
```bash
# Rebuilda il container
docker-compose -f docker-compose.test.yml build --no-cache
```

### Errore: `Permission denied` su build_and_test.sh
```bash
chmod +x tests/build_and_test.sh
```

## Note Importanti

1. **Docker avviato** - docker dev'essere in esecuzione
1. **I test NON scrivono su hardware reale** - tutto è mockato
1. **Ogni test è isolato** - il mock viene ricreato per ogni test
1. **Delay è stubbato** - i test girano istantaneamente
1. **Logging ESP è disabilitato** - nessun output di log nei test
