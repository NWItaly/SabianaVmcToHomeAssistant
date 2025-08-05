#!/usr/bin/env python3
"""
Script di Setup Automatico per VMC Sabiana - Home Assistant
Genera automaticamente il file secrets.yaml
"""

import os
import secrets
import base64
import datetime

def generate_api_key():
    """Genera una chiave API sicura per ESPHome"""
    key_bytes = secrets.token_bytes(32)
    return base64.b64encode(key_bytes).decode('utf-8')

def validate_input(value, min_length=1, field_name="Campo"):
    """Valida un input generico"""
    if len(value.strip()) < min_length:
        return False, f"{field_name} deve essere di almeno {min_length} caratteri"
    return True, ""

def get_input_with_validation(prompt, min_length=1, field_name="Campo"):
    """Ottiene input con validazione semplice"""
    while True:
        value = input(prompt + ": ").strip()
        valid, error = validate_input(value, min_length, field_name)
        if valid:
            return value
        print(f"❌ {error}")

def main():
    print("🏠 Setup VMC Sabiana - Home Assistant")
    print("=" * 50)
    print("Questo script ti aiuterà a creare il file secrets.yaml")
    print("Prepara le credenziali della tua rete WiFi!")
    print()
    
    # Controlla se esiste già secrets.yaml
    if os.path.exists("secrets.yaml"):
        print("⚠️  Il file secrets.yaml esiste già!")
        response = input("Vuoi sovrascriverlo? (scrivi 'si' per confermare): ")
        if response.lower() not in ['si', 's', 'yes', 'y']:
            print("❌ Setup annullato.")
            return
        print()
    
    print("📋 INSERISCI I TUOI DATI")
    print("-" * 30)
    
    # Raccolta dati con input normale
    wifi_ssid = get_input_with_validation(
        "Nome rete WiFi", 1, "Nome WiFi"
    )
    
    wifi_password = input("Password WiFi (può essere vuota): ").strip()
    
    fallback_password = get_input_with_validation(
        "Password di fallback (min 8 caratteri)", 8, "Password fallback"
    )
    
    ota_password = get_input_with_validation(
        "Password aggiornamenti (min 8 caratteri)", 8, "Password OTA"
    )
    
    print("\n🔑 Generazione chiave API sicura...")
    api_key = generate_api_key()
    
    # Mostra riepilogo
    print("\n📋 RIEPILOGO CONFIGURAZIONE")
    print("-" * 30)
    print(f"WiFi SSID: {wifi_ssid}")
    print(f"WiFi Password: {'***nascosta***' if wifi_password else '(vuota)'}")
    print(f"Password Fallback: ***nascosta***")
    print(f"Password OTA: ***nascosta***")
    print(f"API Key: {api_key[:20]}... (generata)")
    
    print("\n❓ Confermi questi dati?")
    confirm = input("Scrivi 'si' per confermare e creare il file: ")
    
    if confirm.lower() not in ['si', 's', 'yes', 'y']:
        print("❌ Setup annullato.")
        return
    
    # Creazione file secrets.yaml
    secrets_content = f"""# VMC Sabiana - Home Assistant Integration
# File generato automaticamente il {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
# 
# ⚠️  IMPORTANTE: Mantieni questo file privato!

# Configurazione WiFi
wifi_ssid: "{wifi_ssid}"
wifi_password: "{wifi_password}"

# Password di sicurezza del dispositivo
fallback_password: "{fallback_password}"
ota_password: "{ota_password}"

# Chiave crittografia API (generata automaticamente)
api_encryption_key: "{api_key}"
"""
    
    try:
        with open("secrets.yaml", "w", encoding="utf-8") as f:
            f.write(secrets_content)
        
        print("\n✅ SETUP COMPLETATO!")
        print("=" * 50)
        print("📁 File 'secrets.yaml' creato correttamente!")
        print()
        print("🚀 PROSSIMI PASSI:")
        print("1. Apri ESPHome Dashboard")
        print("2. Carica tutti i file .yaml del progetto")
        print("3. Compila il firmware")
        print("4. Carica il firmware sull'ESP32")
        print("5. Collega ESP32 alla VMC via Modbus")
        print()
        print("💡 SUGGERIMENTI:")
        print("- Tieni il file secrets.yaml nella stessa cartella dei file yaml")
        print("- Non condividere secrets.yaml con nessuno")
        print("- Fai un backup della configurazione funzionante")
        
    except Exception as e:
        print(f"❌ ERRORE: Impossibile creare il file secrets.yaml")
        print(f"Dettaglio errore: {e}")
        print("Controlla i permessi della cartella e riprova")
        return 1
    
    return 0

if __name__ == "__main__":
    try:
        exit_code = main()
        print("\n" + "="*50)
        input("Premi INVIO per chiudere...")
        exit(exit_code)
    except KeyboardInterrupt:
        print("\n\n❌ Setup interrotto dall'utente")
        exit(1)
    except Exception as e:
        print(f"\n❌ Errore inaspettato: {e}")
        print("Segnala questo errore agli sviluppatori del progetto")
        input("Premi INVIO per chiudere...")
        exit(1)