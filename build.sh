#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_usage() {
    cat << 'INNEREOF'
Usage: $0 [COMMAND] [OPTIONS]

Commands:
    config [debug|release]  Configure the project (default: release)
    build [target]          Build the project or specific target
    clean                   Clean build directory (keeps external/)
    clean-all               Clean build and external directories
    test                    Run tests
    run-cli                 Run CLI executable
    run-gui                 Run GUI executable
    all                     Config + build + test (default)
    help                    Show this help message
INNEREOF
}

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

configure() {
    local build_type="${1:-Release}"
    
    if [[ "$build_type" == "debug" ]]; then
        build_type="Debug"
    elif [[ "$build_type" == "release" ]]; then
        build_type="Release"
    fi
    
    print_info "Configuring project (${build_type})..."
    
    cmake -B "${BUILD_DIR}" \
          -DCMAKE_BUILD_TYPE="${build_type}" \
          -DBUILD_TESTING=ON \
          -DBUILD_CLI=ON \
          -DBUILD_GUI=ON
    
    print_info "Configuration complete!"
}

build() {
    local target="$1"
    
    if [[ ! -d "${BUILD_DIR}" ]]; then
        print_warn "Build directory not found, configuring first..."
        configure
    fi
    
    print_info "Building project..."
    
    if [[ -z "$target" ]]; then
        cmake --build "${BUILD_DIR}" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    else
        cmake --build "${BUILD_DIR}" --target "$target" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    fi
    
    print_info "Build complete!"
}

clean() {
    print_info "Cleaning build directory (keeping external/)..."
    rm -rf "${BUILD_DIR}"
    print_info "Clean complete!"
}

clean_all() {
    print_info "Cleaning build and external directories..."
    rm -rf "${BUILD_DIR}"
    rm -rf "${PROJECT_ROOT}/external"
    print_info "Clean all complete!"
}

run_tests() {
    if [[ ! -d "${BUILD_DIR}" ]]; then
        print_warn "Build directory not found, building first..."
        build
    fi
    
    print_info "Running tests..."
    cd "${BUILD_DIR}" && ctest --output-on-failure
    cd "${PROJECT_ROOT}"
    print_info "Tests complete!"
}

run_cli() {
    local exe="${BUILD_DIR}/bin/cpt64-cli"
    
    if [[ ! -f "$exe" ]]; then
        print_warn "CLI executable not found, building first..."
        build cpt64-cli
    fi
    
    print_info "Running CLI application..."
    "$exe"
}

run_gui() {
    local exe="${BUILD_DIR}/bin/cpt64-gui"
    
    if [[ ! -f "$exe" ]]; then
        print_warn "GUI executable not found, building first..."
        build cpt64-gui
    fi
    
    print_info "Running GUI application..."
    "$exe"
}

case "${1:-all}" in
    config)
        configure "$2"
        ;;
    build)
        build "$2"
        ;;
    clean)
        clean
        ;;
    clean-all)
        clean_all
        ;;
    test)
        run_tests
        ;;
    run-cli)
        run_cli
        ;;
    run-gui)
        run_gui
        ;;
    all)
        if [[ ! -d "" ]]; then
            configure "${2:-release}"
        fi
        build
        run_tests
        ;;
    help|--help|-h)
        print_usage
        ;;
    *)
        print_error "Unknown command: $1"
        print_usage
        exit 1
        ;;
esac
