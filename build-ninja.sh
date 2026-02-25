#!/bin/bash
#
# Build script for Movie Thumbnailer (mtn) using Ninja
#
# Usage: ./build-ninja.sh [options]
# Options:
#   --clean     Clean build directory
#   --debug     Build with debug symbols
#   --static    Build static binary
#   --help      Show this help
#

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-ninja"
SRC_DIR="${SCRIPT_DIR}/src"
BIN_DIR="${SCRIPT_DIR}/bin"
NINJA_FILE="${BUILD_DIR}/build.ninja"

# Build options
BUILD_TYPE="release"
CLEAN_BUILD=false
STATIC_BUILD=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    cat << EOF
Build Movie Thumbnailer (mtn) using Ninja

Usage: $0 [options]

Options:
  --clean     Clean build directory before building
  --debug     Build with debug symbols and sanitizers
  --static    Build static binary (requires static libraries)
  --help      Show this help message

Examples:
  $0                    # Standard release build
  $0 --clean            # Clean build
  $0 --debug            # Debug build with symbols
  $0 --static           # Static binary build

Requirements:
  - Ninja build system
  - GCC or Clang
  - FFmpeg development libraries
  - libgd development libraries

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --debug)
            BUILD_TYPE="debug"
            shift
            ;;
        --static)
            STATIC_BUILD=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Check for required tools
check_requirements() {
    log_info "Checking build requirements..."
    
    local missing=()
    
    # Check for ninja
    if ! command -v ninja &> /dev/null; then
        missing+=("ninja")
    fi
    
    # Check for compiler
    if command -v gcc &> /dev/null; then
        CC=gcc
    elif command -v clang &> /dev/null; then
        CC=clang
    else
        missing+=("gcc or clang")
    fi
    
    # Check for pkg-config (optional but preferred)
    local HAS_PKG_CONFIG=false
    if command -v pkg-config &> /dev/null; then
        HAS_PKG_CONFIG=true
    fi
    
    # Check for FFmpeg (try pkg-config first, then fallback)
    local HAS_FFMPEG=false
    if [ "$HAS_PKG_CONFIG" = true ] && pkg-config --exists libavcodec libavformat libavutil 2>/dev/null; then
        HAS_FFMPEG=true
    elif [ -f "/usr/include/libavcodec/avcodec.h" ] || [ -f "/usr/local/include/libavcodec/avcodec.h" ]; then
        HAS_FFMPEG=true
    fi
    
    if [ "$HAS_FFMPEG" = false ]; then
        missing+=("FFmpeg development libraries")
    fi
    
    # Check for libgd (try pkg-config first, then fallback)
    local HAS_GD=false
    if [ "$HAS_PKG_CONFIG" = true ] && pkg-config --exists gd 2>/dev/null; then
        HAS_GD=true
    elif [ -f "/usr/include/gd.h" ] || [ -f "/usr/local/include/gd.h" ] || [ -f "/data/data/com.termux/files/usr/include/gd.h" ]; then
        HAS_GD=true
    fi
    
    if [ "$HAS_GD" = false ]; then
        log_warning "libgd not found via pkg-config, will try manual configuration"
    fi
    
    if [ ${#missing[@]} -ne 0 ]; then
        log_error "Missing required tools/libraries:"
        for item in "${missing[@]}"; do
            echo "  - $item"
        done
        echo ""
        log_info "Install on Ubuntu/Debian:"
        echo "  sudo apt-get install ninja-build gcc libavcodec-dev libavformat-dev libavutil-dev libavfilter-dev libswscale-dev libgd-dev pkg-config"
        echo ""
        log_info "Install on Fedora/RHEL:"
        echo "  sudo dnf install ninja-build gcc ffmpeg-devel gd-devel pkg-config"
        echo ""
        log_info "Install on Arch/Manjaro:"
        echo "  sudo pacman -S ninja gcc ffmpeg gd pkg-config"
        echo ""
        log_info "Install on Termux:"
        echo "  pkg install ninja clang ffmpeg libgd"
        exit 1
    fi
    
    log_success "All requirements found"
}

# Clean build directory
clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        log_info "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    if [ -d "$BIN_DIR" ] && [ "$CLEAN_BUILD" = true ]; then
        log_info "Cleaning bin directory..."
        rm -rf "$BIN_DIR"
    fi
}

# Generate Ninja build file
generate_ninja_file() {
    log_info "Generating Ninja build file..."
    
    mkdir -p "$BUILD_DIR"
    
    # Get compiler flags from pkg-config (with fallbacks)
    local CFLAGS=""
    local LIBS=""
    
    # FFmpeg flags
    if pkg-config --exists libavcodec 2>/dev/null; then
        CFLAGS+="$(pkg-config --cflags libavcodec libavformat libavutil libavfilter libswscale 2>/dev/null) "
        LIBS+="$(pkg-config --libs libavcodec libavformat libavutil libavfilter libswscale 2>/dev/null) "
    else
        # Fallback for standard paths
        CFLAGS+="-I/usr/include/ffmpeg -I/usr/local/include/ffmpeg "
        LIBS+="-lavcodec -lavformat -lavutil -lavfilter -lswscale "
    fi
    
    # libgd flags
    if pkg-config --exists gd 2>/dev/null; then
        CFLAGS+="$(pkg-config --cflags gd 2>/dev/null) "
        LIBS+="$(pkg-config --libs gd 2>/dev/null) "
    else
        # Fallback for standard paths
        CFLAGS+="-I/usr/include -I/usr/local/include -I/data/data/com.termux/files/usr/include "
        LIBS+="-lgd "
    fi
    
    # Set build-specific flags
    if [ "$BUILD_TYPE" = "debug" ]; then
        OPT_FLAGS="-g -O0 -DDEBUG -fsanitize=address"
        LDFLAGS="-fsanitize=address"
        log_info "Building in DEBUG mode"
    else
        OPT_FLAGS="-O3 -DNDEBUG"
        LDFLAGS=""
        log_info "Building in RELEASE mode"
    fi
    
    # Static build flags
    if [ "$STATIC_BUILD" = true ]; then
        OPT_FLAGS="$OPT_FLAGS -DMTN_STATIC"
        LDFLAGS="$LDFLAGS -static -static-libgcc"
        log_info "Building STATIC binary"
    fi
    
    # Add feature flags
    OPT_FLAGS="$OPT_FLAGS -DMTN_WITH_WEBP -DMTN_WITH_AVIF"
    
    # Create Ninja file
    cat > "$NINJA_FILE" << 'NINJA_EOF'
# Movie Thumbnailer (mtn) Ninja Build File
# Generated by build-ninja.sh

# Variables
NINJA_EOF

    # Append variables with proper expansion
    cat >> "$NINJA_FILE" << NINJA_EOF
cc = $CC
cflags = -Wall -Wextra $OPT_FLAGS $CFLAGS
ldflags = $LDFLAGS
libs = $LIBS -lm -lpthread

# Directories
builddir = $BUILD_DIR
bindir = $BIN_DIR
srcdir = $SRC_DIR

# Rule for compiling C files
rule cc
  command = \$cc -MMD -MT \$out -MF \$out.d \$cflags -c \$in -o \$out
  depfile = \$out.d
  deps = gcc

# Rule for linking
rule link
  command = \$cc \$ldflags \$in \$libs -o \$out

# Build object files
build \$builddir/mtn.o: cc \$srcdir/mtn.c
build \$builddir/mtn_context.o: cc \$srcdir/mtn_context.c
build \$builddir/mtn_thumbnail.o: cc \$srcdir/mtn_thumbnail.c
build \$builddir/mtn_error.o: cc \$srcdir/mtn_error.c

# Build final binary
build \$bindir/mtn: link \$builddir/mtn.o \$builddir/mtn_context.o \$builddir/mtn_thumbnail.o \$builddir/mtn_error.o

# Default target
default \$bindir/mtn

# Clean target
build clean: phony
  command = rm -rf \$builddir \$bindir/mtn
NINJA_EOF
    
    log_success "Ninja build file generated: $NINJA_FILE"
}

# Build with Ninja
build() {
    log_info "Building with Ninja..."
    
    cd "$BUILD_DIR"
    
    # Get number of CPU cores
    local JOBS=$(nproc 2>/dev/null || echo "4")
    
    log_info "Using $JOBS parallel jobs"
    
    if ninja -j "$JOBS"; then
        log_success "Build completed successfully!"
        log_info "Binary location: $BIN_DIR/mtn"
        
        # Show binary info
        if [ -f "$BIN_DIR/mtn" ]; then
            local SIZE=$(ls -lh "$BIN_DIR/mtn" | awk '{print $5}')
            log_info "Binary size: $SIZE"
            
            if [ "$BUILD_TYPE" = "debug" ]; then
                log_info "Debug symbols: included"
            fi
        fi
    else
        log_error "Build failed!"
        exit 1
    fi
}

# Verify build
verify() {
    if [ -f "$BIN_DIR/mtn" ]; then
        log_info "Verifying build..."
        
        # Check if binary is executable
        if [ -x "$BIN_DIR/mtn" ]; then
            log_success "Binary is executable"
        else
            log_warning "Binary is not executable, fixing permissions..."
            chmod +x "$BIN_DIR/mtn"
        fi
        
        # Get version
        local VERSION=$("$BIN_DIR/mtn" -v 2>&1 | head -1)
        log_info "Version: $VERSION"
        
        # Check dependencies
        if ldd "$BIN_DIR/mtn" &>/dev/null; then
            log_info "Linked libraries:"
            ldd "$BIN_DIR/mtn" | grep -E "(libav|libsw|libgd)" | while read line; do
                echo "  $line"
            done
        fi
        
        log_success "Build verification complete!"
    else
        log_error "Binary not found at $BIN_DIR/mtn"
        exit 1
    fi
}

# Main execution
main() {
    echo "========================================"
    echo "  Movie Thumbnailer (mtn) Build Script"
    echo "  Using Ninja Build System"
    echo "========================================"
    echo ""
    
    check_requirements
    clean_build
    generate_ninja_file
    build
    verify
    
    echo ""
    echo "========================================"
    log_success "Build completed successfully!"
    echo "========================================"
    echo ""
    echo "Binary: $BIN_DIR/mtn"
    echo ""
    echo "Quick test:"
    echo "  $BIN_DIR/mtn -v"
    echo "  $BIN_DIR/mtn -c 3 -r 2 your_video.mp4"
    echo ""
}

# Run main
main
