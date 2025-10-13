# MyApp - Progetto C Generico

Applicazione C modulare con supporto multipiattaforma, CMake e SDL3 opzionale.

## 📋 Indice

- [Requisiti](#requisiti)
- [Struttura del Progetto](#struttura-del-progetto)
- [Compilazione Rapida](#compilazione-rapida)
- [Configurazione](#configurazione)
- [Compilazione Dettagliata](#compilazione-dettagliata)
- [Sviluppo](#sviluppo)
- [Aggiungere Moduli](#aggiungere-moduli)
- [Troubleshooting](#troubleshooting)

## 📦 Requisiti

### Dipendenze di Base

- **CMake**: >= 3.20
- **Compilatore C**: GCC, Clang o MSVC
- **Make** (Linux/macOS): `make` (preinstallato)
- **Git**: per il controllo versione

### Installazione Requisiti

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
```

#### Linux (Fedora/RHEL)
```bash
sudo dnf install gcc gcc-c++ cmake git make
```

#### macOS
```bash
# Installa Xcode Command Line Tools
xcode-select --install

# Installa CMake (via Homebrew)
brew install cmake
```

#### Windows (MinGW)
```bash
# Scarica da https://www.mingw-w64.org/
# Oppure via Chocolatey:
choco install cmake mingw-w64
```

### SDL3 (Opzionale)

Se abiliti `USE_SDL=ON` in `project.conf`, SDL3 verrà scaricata automaticamente durante la configurazione.

## 🏗️ Struttura del Progetto

```
MyApp/
├── CMakeLists.txt              # Configurazione CMake principale
├── project.conf                # Metadati progetto (nome, versione, descrizione)
├── build.sh                    # Script di build automatizzato
├── README.md                   # Questo file
│
├── include/                    # Header file comuni
│   ├── config.h               # (Generato) Configurazione applicazione
│   ├── types.h                # Definizioni comuni
│   └── utils.h                # Utility comuni
│
├── src/                        # Sorgenti organizzati per moduli
│   ├── main.c                 # Entry point (2 versioni: con/senza SDL)
│   │
│   ├── core/                  # Modulo core
│   │   ├── core.c
│   │   └── core.h
│   │
│   ├── graphics/              # Modulo grafica (SDL)
│   │   ├── graphics.c
│   │   └── graphics.h
│   │
│   └── utils/                 # Modulo utility
│       ├── utils.c
│       └── utils.h
│
└── build/                      # (Generato) Directory di compilazione
    ├── CMakeCache.txt
    ├── Makefile
    └── MyApp (eseguibile)
```

## 🚀 Compilazione Rapida

```bash
# 1. Pulisci build precedenti (opzionale)
./build.sh clean

# 2. Configura il progetto
./build.sh configure

# 3. Compila
./build.sh build

# 4. Esegui
./build.sh run
```

## ⚙️ Configurazione

### File `project.conf`

Questo file contiene i metadati del progetto e viene letto sia da CMake che dall'applicazione compilata.

```cmake
set(PROJECT_NAME "MyApp")              # Nome dell'applicazione
set(PROJECT_VERSION "1.0.0")           # Versione (SemVer)
set(PROJECT_DESCRIPTION "...")         # Descrizione breve
set(USE_SDL ON)                        # Abilita SDL3: ON oppure OFF
```

**Dopo aver modificato questo file, riconfigura il progetto:**
```bash
./build.sh configure
./build.sh build
```

### Accesso ai Metadati

I metadati sono disponibili nel codice tramite l'header `config.h`:

```c
#include "config.h"

printf("App: %s v%s\n", APP_NAME, APP_VERSION);
printf("Descrizione: %s\n", APP_DESCRIPTION);
```

## 🔨 Compilazione Dettagliata

### Opzioni di Compilazione

Lo script `build.sh` supporta diverse opzioni:

```bash
./build.sh [comando] [opzioni]

Comandi:
  clean              Rimuove la directory di build
  configure          Configura il progetto con CMake
  build              Compila il progetto
  run                Esegue l'applicazione compilata
  help               Mostra questo messaggio
```

### Opzioni di Build

```bash
# Build type
-t, --type       Debug (default) oppure Release
                 Debug: simboli, niente ottimizzazioni
                 Release: ottimizzazioni -O3, niente debug

# Piattaforma target
-p, --platform   linux (default), macos, windows
                 Rileva automaticamente, ma può essere forzata

# Support SDL3
--sdl            ON (default) oppure OFF
```

### Esempi di Build

#### Build Debug (locale)
```bash
./build.sh configure -t Debug
./build.sh build
./build.sh run
```

#### Build Release Ottimizzato
```bash
./build.sh configure -t Release
./build.sh build
./build.sh run
```

#### Build senza SDL3
```bash
./build.sh configure --sdl OFF
./build.sh build
```

#### Build per Windows (da Linux)
```bash
./build.sh configure -p windows -t Release
./build.sh build
```

#### Build Completo da Zero
```bash
./build.sh clean
./build.sh configure -t Release -p linux --sdl ON
./build.sh build
./build.sh run
```

### Build Manuale (senza script)

Se preferisci usare CMake direttamente:

```bash
# Crea directory di build
mkdir -p build
cd build

# Configura
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_SDL=ON

# Compila
make -j$(nproc)  # Linux/macOS
# oppure
cmake --build . --config Release  # Windows

# Esegui
./MyApp    # Linux/macOS
# oppure
./Release/MyApp.exe  # Windows

cd ..
```

## 💻 Sviluppo

### Workfloww di Sviluppo Consigliato

#### 1. Setup Iniziale
```bash
git clone <repo>
cd MyApp
chmod +x build.sh  # Rendi eseguibile (Linux/macOS)
./build.sh clean
./build.sh configure -t Debug  # Debug durante lo sviluppo
./build.sh build
./build.sh run
```

#### 2. Ciclo di Sviluppo
```bash
# Modifica il codice
vim src/main.c

# Ricompila
./build.sh build

# Testa
./build.sh run

# Ripeti finché non è ok
```

#### 3. Build Finale
```bash
./build.sh configure -t Release
./build.sh build
./build.sh run
```

### IDE Integration

#### VSCode
Installa l'estensione "CMake Tools" e il progetto verrà riconosciuto automaticamente.

#### CLion
Apri la cartella del progetto e CLion configurerà CMake automaticamente.

#### Qt Creator
Apri `CMakeLists.txt` come progetto.

### Debugger

#### Linux/macOS (GDB)
```bash
./build.sh configure -t Debug
./build.sh build
gdb ./build/MyApp
```

#### Windows (Visual Studio Debugger)
Compila con MSVC e usa Visual Studio per il debugging.

## 📚 Aggiungere Moduli

### Anatomia di un Modulo

Un modulo è composto da:
- **Header file** (`.h`): interfaccia pubblica
- **Source file** (`.c`): implementazione
- **Directory dedicata**: `src/[nome_modulo]/`

### Procedura: Aggiungere un Nuovo Modulo

#### Esempio: Aggiungere il modulo "logger"

##### 1. Crea la Directory del Modulo
```bash
mkdir -p src/logger
```

##### 2. Crea l'Header Pubblico
**File: `src/logger/logger.h`**
```c
#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void logger_init(const char* filename);
void logger_log(LogLevel level, const char* message, ...);
void logger_shutdown(void);

#endif /* LOGGER_H */
```

##### 3. Crea l'Implementazione
**File: `src/logger/logger.c`**
```c
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

static FILE* log_file = NULL;

void logger_init(const char* filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        fprintf(stderr, "Errore: impossibile aprire file di log\n");
        exit(1);
    }
}

void logger_log(LogLevel level, const char* message, ...) {
    if (!log_file) return;
    
    const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_buf[20];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_file, "[%s] [%s] ", time_buf, level_str[level]);
    
    va_list args;
    va_start(args, message);
    vfprintf(log_file, message, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fflush(log_file);
}

void logger_shutdown(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}
```

##### 4. Usa il Modulo in `main.c`
```c
#include <stdio.h>
#include "config.h"
#include "logger/logger.h"

int main(int argc, char* argv[]) {
    logger_init("app.log");
    
    logger_log(LOG_INFO, "Applicazione %s v%s avviata", APP_NAME, APP_VERSION);
    logger_log(LOG_DEBUG, "Debug mode attivo");
    
    printf("Scritto nel log\n");
    
    logger_shutdown();
    return 0;
}
```

##### 5. Compila
```bash
./build.sh build
./build.sh run
```

CMake troverà automaticamente i nuovi file `.c` tramite `GLOB_RECURSE`.

### Best Practice per i Moduli

#### 1. Naming Convention
```
src/[nome_modulo]/
    ├── [nome_modulo].h       # Header pubblico
    ├── [nome_modulo].c       # Implementazione
    ├── [nome_modulo]_internal.h  # Header privati (opzionale)
    └── [nome_modulo]_private.c   # Implementazione privata (opzionale)
```

#### 2. Header Guard
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

// ... contenuto ...

#endif /* MODULE_NAME_H */
```

#### 3. Dipendenze tra Moduli
Se il modulo B dipende dal modulo A, A deve essere compilato prima. CMake gestisce questo automaticamente tramite l'ordine di inclusione.

#### 4. Struttura di un Modulo Complesso
```
src/graphics/
├── graphics.h              # Interfaccia pubblica
├── graphics.c              # Implementazione pubblica
├── graphics_internal.h     # Interfaccia interna
├── graphics_renderer.c     # Sub-modulo privato
├── graphics_shader.c       # Sub-modulo privato
└── assets/                 # Risorse del modulo
    ├── shaders/
    └── textures/
```

## 🔗 Inclusioni e Path

### Path di Include Automatici

CMake configura automaticamente i seguenti path di ricerca per `#include`:

```c
// Cerca in: include/
#include "config.h"
#include "types.h"

// Cerca anche in: src/ (ogni cartella di modulo)
#include "logger/logger.h"      // Funziona
#include "logger.h"             // Potrebbe non funzionare (dipende da CMake)
```

### Best Practice
```c
// ✓ Usa il path relativo al modulo
#include "logger/logger.h"

// ✓ Per header comuni in include/
#include "config.h"

// ✗ Evita percorsi assoluti
#include "/home/user/project/src/logger/logger.h"
```

## 🐛 Troubleshooting

### Errore: "cmake: command not found"
**Soluzione**: Installa CMake
```bash
# Linux
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Scarica da https://cmake.org/download/
```

### Errore: "SDL3 not found"
**Soluzione**: SDL3 verrà scaricato automaticamente se `USE_SDL=ON` in `project.conf` è abilitato.
```bash
./build.sh configure --sdl ON
```

### Errore: "Executable not found"
**Soluzione**: Compila prima il progetto
```bash
./build.sh build
```

### Errore: "Permission denied" su build.sh
**Soluzione**: Rendi lo script eseguibile
```bash
chmod +x build.sh
```

### Build lento
**Soluzione**: Compila in Release e usa più core
```bash
./build.sh configure -t Release
make -j$(nproc)  # Usa tutti i core disponibili
```

### Moduli non trovati
**Soluzione**: Assicurati che i file `.c` siano in `src/` e CMake li troverà tramite GLOB:
```bash
# Verifica che i file esistano
find src -name "*.c"

# Riconfigura
./build.sh clean
./build.sh configure
./build.sh build
```

### Conflitto tra versioni di SDL
**Soluzione**: Usa la versione scaricata automaticamente
```bash
./build.sh clean
./build.sh configure --sdl ON
```

## 📖 Risorse Utili

- **CMake Documentation**: https://cmake.org/documentation/
- **SDL3 Documentation**: https://wiki.libsdl.org/
- **C Language Reference**: https://en.cppreference.com/w/c

---

**Ultima modifica**: Gennaio 2025
**Versione**: 1.0.0
