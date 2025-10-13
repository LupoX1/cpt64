#!/bin/bash

# ============================================================================
# Script di build per progetto C generico
# Supporta: clean, configure, build, run per Linux/Windows/macOS
# ============================================================================

set -e

# Colori per output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configurazioni di default
BUILD_DIR="build"
BUILD_TYPE="Debug"
PLATFORM="linux"
USE_SDL="ON"
PROJECT_NAME=""

# Funzioni di utility
print_header() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Estrai il nome del progetto da project.conf
extract_project_name() {
    if [ -f "project.conf" ]; then
        PROJECT_NAME=$(grep "set(PROJECT_NAME" project.conf | sed 's/.*"\(.*\)".*/\1/')
    fi
}

# Rileva la piattaforma se non specificata
detect_platform() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PLATFORM="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        PLATFORM="macos"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        PLATFORM="windows"
    fi
}

# Pulisci i file di build
clean() {
    print_header "Cleaning build directory"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory removed (external dependencies preserved)"
    else
        print_warning "Build directory not found"
    fi
}

# Pulisci build e dipendenze esterne
clean_all() {
    print_header "Cleaning build directory and external dependencies"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory removed"
    fi
    
    if [ -d "external" ]; then
        rm -rf external
        print_success "External dependencies removed"
    fi
}

# Configura il progetto con CMake
configure() {
    print_header "Configuring project"
    
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "CMakeLists.txt not found in current directory"
        exit 1
    fi
    
    if [ ! -f "project.conf" ]; then
        print_error "project.conf not found in current directory"
        exit 1
    fi
    
    mkdir -p "$BUILD_DIR"
    
    local CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUSE_SDL=$USE_SDL"
    
    if [ "$PLATFORM" = "windows" ]; then
        CMAKE_FLAGS="$CMAKE_FLAGS -G 'MinGW Makefiles'"
    fi
    
    cd "$BUILD_DIR"
    cmake .. $CMAKE_FLAGS
    cd ..
    
    print_success "Configuration completed for $PLATFORM ($BUILD_TYPE)"
}

# Compila il progetto
build() {
    print_header "Building project"
    
    if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
        print_error "Project not configured. Run 'build.sh configure' first"
        exit 1
    fi
    
    cd "$BUILD_DIR"
    
    if [ "$PLATFORM" = "windows" ]; then
        cmake --build . --config "$BUILD_TYPE"
    else
        make -j$(nproc)
    fi
    
    cd ..
    print_success "Build completed successfully"
}

# Esegui l'applicazione
run() {
    print_header "Running application"
    
    extract_project_name
    
    if [ -z "$PROJECT_NAME" ]; then
        print_error "Could not extract project name"
        exit 1
    fi
    
    local exe_path=""
    
    case "$PLATFORM" in
        linux|macos)
            exe_path="$BUILD_DIR/$PROJECT_NAME"
            ;;
        windows)
            exe_path="$BUILD_DIR/$BUILD_TYPE/$PROJECT_NAME.exe"
            ;;
    esac
    
    if [ ! -f "$exe_path" ]; then
        print_error "Executable not found: $exe_path"
        print_warning "Make sure to build the project first with: build.sh build"
        exit 1
    fi
    
    print_success "Executing $exe_path"
    echo ""
    "$exe_path"
}

# Visualizza l'aiuto
show_help() {
    cat << EOF
${BLUE}=== Generic C Project Build Script ===${NC}

Usage: ./build.sh [command] [options]

Commands:
    clean           Remove build directory (keep external deps)
    clean-all       Remove build AND external dependencies
    configure       Configure project with CMake
    build           Compile the project
    run             Run the compiled application
    help            Show this help message

Options (use with commands):
    -t|--type       Build type: Debug (default) or Release
    -p|--platform   Target platform: linux (default), windows, or macos
    --sdl           Enable SDL3 support: ON (default) or OFF
    
Examples:
    ./build.sh clean                          # Clean build (keep SDL3)
    ./build.sh clean-all                      # Clean everything
    ./build.sh configure -t Release           # Configure for Release build
    ./build.sh configure -p windows           # Configure for Windows
    ./build.sh configure --sdl OFF            # Configure without SDL3
    ./build.sh build                          # Build the project
    ./build.sh run                            # Run the application
    
Full workflow:
    ./build.sh configure -t Release
    ./build.sh build
    ./build.sh run
    
After first build, you can just do:
    ./build.sh build   # Recompile (SDL3 already cached in external/)
    ./build.sh run
EOF
}

# Parse degli argomenti
parse_args() {
    while [ $# -gt 0 ]; do
        case "$1" in
            -t|--type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            -p|--platform)
                PLATFORM="$2"
                shift 2
                ;;
            --sdl)
                USE_SDL="$2"
                shift 2
                ;;
            *)
                shift
                ;;
        esac
    done
}

# Main
main() {
    if [ $# -eq 0 ]; then
        show_help
        exit 0
    fi
    
    local command=$1
    shift
    
    detect_platform
    parse_args "$@"
    
    print_header "Generic C Project Builder"
    echo "Platform: $PLATFORM | Build: $BUILD_TYPE | SDL: $USE_SDL"
    echo ""
    
    case $command in
        clean)
            clean
            ;;
        clean-all)
	    clean_all
	    ;;
        configure)
            configure
            ;;
        build)
            build
            ;;
        run)
            run
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_error "Unknown command: $command"
            echo "Use './build.sh help' to see available commands"
            exit 1
            ;;
    esac
}

main "$@"
