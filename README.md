# cpt64

Template base per progetto C con supporto CLI e GUI.

## Caratteristiche

- ✅ **Dual mode**: Eseguibili separati per modalità terminale (CLI) e grafica (GUI)
- ✅ **raylib**: Framework grafico semplice e potente
- ✅ **raygui**: GUI immediate-mode integrata con raylib (single header, C puro)
- ✅ **Build system**: CMake con configurazioni Debug/Release
- ✅ **Testing**: CTest integrato per unit testing
- ✅ **Struttura modulare**: Organizzazione in moduli (core, log, video)
- ✅ **Script automatici**: Build e run semplificati
- ✅ **Dipendenze persistenti**: Le librerie esterne vanno in `external/` e non vengono cancellate con clean

## Perché raylib + raygui?

**raylib** è perfetto per C:
- **API semplicissima**: Poche funzioni, chiare e intuitive
- **Cross-platform**: Windows, Linux, macOS out-of-the-box
- **C puro**: Niente C++, niente complicazioni
- **Leggero**: ~500KB compilato
- **Tutto incluso**: Grafica, audio, input, tutto in uno

**raygui** per GUI:
- **Single header**: Un solo `#include "raygui.h"`
- **Immediate mode**: Come Dear ImGui, ma per C
- **Integrazione perfetta**: Progettato per raylib
- **Semplice**: API minimalista e chiara

## Build rapido

```bash
./build.sh          # Build completo
./build.sh run-cli  # Esegui CLI
./build.sh run-gui  # Esegui GUI
```

## Requisiti

- CMake 3.20+
- GCC/Clang
- Git (per scaricare dipendenze)

## Comandi disponibili

- `./build.sh config [debug|release]` - Configura
- `./build.sh build` - Compila
- `./build.sh test` - Esegui test
- `./build.sh clean` - Pulisci build (mantiene external/)
- `./build.sh clean-all` - Pulisci tutto (build + external/)
- `./build.sh run-cli` - Esegui CLI
- `./build.sh run-gui` - Esegui GUI

## Struttura

```
cpt64/
├── src/            # Sorgenti
├── include/        # Header pubblici
├── tests/          # Unit tests
├── external/       # Dipendenze (raylib, raygui)
├── build/          # Build directory
└── assets/         # Risorse
```

## Note

- Il primo build scarica raylib e raygui in `external/`
- Le dipendenze non vengono riscarcate con `clean`
- Usa `clean-all` per eliminare anche `external/`

## Esempio GUI

La GUI di esempio include:
- Panel con titolo
- Label informativi
- Counter con bottoni Increment/Reset
- Slider RGB per colore background
- Anteprima colore in tempo reale
- Bottone Quit
- FPS counter

Tutto con API raylib/raygui semplicissime!
