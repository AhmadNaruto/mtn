# Movie Thumbnailer (mtn) - Project Context

## Project Overview

**Movie Thumbnailer (mtn)** is a C-based command-line utility that generates thumbnail screenshots from video files. It creates image grids (contact sheets) from video content, useful for previewing video files.

- **Primary Language:** C
- **License:** GNU GPL v2
- **Original Repository:** https://gitlab.com/movie_thumbnailer/mtn (by tuit & wahibre)
- **Fork Repository:** https://github.com/AhmadNaruto/mtn
- **Current Version:** 3.5.0 (as of Feb 2024)

### Core Functionality

mtn uses **FFmpeg's libavcodec** as its engine to decode video frames and **libgd** for image generation. It supports:

- All popular video codecs (h.265/hevc, h.264, mpeg1, mpeg2, mp4, vc1, wmv, xvid, divx)
- Multiple output formats: JPG, PNG, WebP, AVIF
- Customizable thumbnail grids with configurable rows/columns
- Metadata overlay (audio/video info, timestamps)
- Font rendering for text overlays
- Edge detection for shot selection
- WebVTT export for video chapters

## Project Structure

```
mtn/
├── src/                    # Main source code
│   ├── mtn.c              # Main application (~4600 lines)
│   ├── Makefile           # Build configuration
│   ├── Makefile.mingw     # Windows/MinGW build
│   ├── fake_tchar.h       # Unicode compatibility header
│   └── mtn.pro            # Qt project file (legacy)
├── test/                   # Test resources
│   ├── run_mtn.sh         # Comprehensive test script
│   └── font.ttf           # Test font file
├── misc/                   # Build scripts and patches
│   ├── configureFFmpeg.sh # FFmpeg configuration script
│   ├── gdjpeg.patch       # libgd patch
│   ├── gitlab-ci-info.sh  # CI info script
│   └── mtn_drop.bat       # Windows batch utility
└── bin/                    # Output directory (created on build)
```

## Building and Running

### Build Dependencies

| Platform | Command |
|----------|---------|
| **Debian/Ubuntu** | `apt-get install libgd-dev libavutil-dev libavcodec-dev libavformat-dev libavfilter-dev libswscale-dev make` |
| **Fedora/RHEL** | `dnf install ffmpeg-devel gd-devel make gcc-c++` |
| **Arch/Manjaro** | `pacman -Sy ffmpeg gd` |
| **macOS (brew)** | `brew install ffmpeg libgd` |
| **Windows (MSYS2)** | `pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-gcc mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-libgd` |

### Build Commands

```bash
# Standard build
cd src
make USER_CFLAGS=-DGB_F_FONTNAME=\\\"DefaultFont.ttf\\\"
make install

# Debug build
make debug

# Static build
make static

# Clean
make clean
make distclean
```

### Installation

```bash
# Install to /usr/local/bin (default PREFIX)
make install

# Custom prefix
make install PREFIX=/usr

# Staged install (for packaging)
make install DESTDIR=/path/to/stage
```

### Usage Examples

```bash
# Basic usage
mtn video.mp4

# Custom grid (3 columns, 4 rows) with font
mtn -c 3 -r 4 -f DejaVuSans.ttf video.mkv

# Output to specific directory with custom format
mtn -O /output/dir -o .png -X video.avi

# Verbose mode with debug info
mtn -v video.mkv

# Extract individual shots
mtn -I toi video.mp4

# WebVTT export
mtn --vtt video.mkv
```

### Key Command-Line Options

| Option | Description |
|--------|-------------|
| `-c N` | Number of columns |
| `-r N` | Number of rows |
| `-h N` | Minimum height (px) |
| `-f FONT` | Font file for text overlay |
| `-o EXT` | Output extension (.jpg, .png, .webp, .avif) |
| `-O DIR` | Output directory |
| `-X` | Include source filename in output |
| `-I toi` | Save individual shots (t=info, o=original, i=grid) |
| `-v` | Verbose mode |
| `-q` | Quiet mode |
| `--shadow=N` | Shadow size for thumbnails |
| `--cover` | Extract album art |
| `--vtt` | Export WebVTT chapters |
| `--filters` | FFmpeg filter chain |

## Development Conventions

### Coding Style

The project follows **SunOS** or **OpenBSD** C style conventions:
- K&R brace style
- 4-space indentation (tabs converted to spaces)
- Descriptive variable names
- Extensive inline comments for complex logic

### Code Structure (mtn.c)

Main functions include:
- `process_loop()` - Main processing loop for files
- `make_thumbnail()` - Core thumbnail generation
- `process_file()` - File handling and stream selection
- `evade_blank()` / `evade_blur()` - Shot quality detection
- `draw_text_*()` - Text rendering functions
- `dump_*_context()` - Debug output for FFmpeg contexts

### Testing Practices

Test script: `test/run_mtn.sh`

```bash
# Run comprehensive test suite
./test/run_mtn.sh /path/to/test/video.mp4

# Test specific binary
MTN=/path/to/mtn ./test/run_mtn.sh
```

The test script covers:
- Minimum switches
- All output formats (jpg, png, webp, avif)
- Individual shot extraction
- Grid configurations
- Transparency and shadows
- Unicode filenames
- Filters and tonemapping
- Recursive directory processing

### CI/CD

GitLab CI configuration (`.gitlab-ci.yml`) builds on:
- Fedora (latest)
- CentOS 7
- Debian 12
- Ubuntu (18.04, devel)
- Windows (MinGW cross-compile)

## Key Technical Details

### FFmpeg Integration

The code uses FFmpeg libraries:
- `libavcodec` - Video decoding
- `libavformat` - Container demuxing
- `libavutil` - Utility functions
- `libavfilter` - Video filtering
- `libswscale` - Image scaling/conversion

### Image Output

libgd is used for:
- JPEG, PNG output (native)
- WebP, AVIF (conditional, via `ENABLE_WEBP`, `ENABLE_AVIF` flags)
- Text rendering with FreeType
- Image composition (grid assembly)

### Platform Support

- **Linux** - Primary platform
- **macOS** - Homebrew support
- **Windows** - MinGW64 via MSYS2
- **FreeBSD** - pkg support

### Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Warning (non-fatal issues) |
| 2 | Error (processing failed) |

---

## Refactoring Status (2026)

### Completed Improvements

#### 1. Static Buffer Fixes ✅
- **Before:** Functions used static buffers (thread-unsafe)
  ```c
  // OLD: Thread-unsafe, overwrites on each call
  char *format_color(rgb_color col) {
      static char buf[7]; // FIXME
      sprintf(buf, "%02X%02X%02X", col.r, col.g, col.b);
      return buf;
  }
  ```
- **After:** Caller-provided buffers with size checking
  ```c
  // NEW: Thread-safe, buffer overflow protected
  char *format_color(rgb_color col, char *buf, size_t buf_size) {
      snprintf(buf, buf_size, "%02X%02X%02X", col.r, col.g, col.b);
      return buf;
  }
  ```

**Affected functions:**
- `format_color()` - Now requires caller buffer
- `format_size()` - Now requires caller buffer
- `get_stream_info()` - Now requires caller buffer + size
- `get_stream_info_type()` - Now requires buffer size parameter

#### 2. Buffer Overflow Prevention ✅
Replaced unsafe `sprintf`/`strcat` with size-checked variants:
- `sprintf` → `snprintf` (with size parameter)
- `strcat` → `strncat` (with remaining buffer size)
- Added buffer size calculations: `buf_size - strlen(buf) - 1`

**Locations fixed:**
- `get_stream_info()` - All string formatting (~15 locations)
- `get_stream_info_type()` - Stream info formatting (~10 locations)
- Individual filename handling in thumbnail generation
- Debug info string formatting

#### 3. Context Struct Foundation ✅
New modular architecture for global state management:

**New files:**
- `src/mtn_context.h` - Context struct definition (156 lines)
- `src/mtn_context.c` - Context initialization/cleanup (156 lines)

**Benefits:**
- Thread-safe design foundation
- Easier unit testing (inject mock contexts)
- Clearer separation of configuration vs. runtime state
- Automatic cleanup prevents memory leaks

**Usage pattern:**
```c
MtnContext ctx;
mtn_context_init(&ctx);
// ... use ctx throughout processing
mtn_context_cleanup(&ctx);
```

#### 4. Thumbnail Processing Module ✅
Extracted thumbnail generation logic into reusable components:

**New files:**
- `src/mtn_thumbnail.h` - Thumbnail context and function declarations (110 lines)
- `src/mtn_thumbnail.c` - Thumbnail processing implementation (397 lines)

**Key components:**
```c
typedef struct ThumbnailContext {
    AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    AVFrame *frame, *frame_rgb;
    struct SwsContext *sws_ctx;
    AVFilterGraph *filter_graph;
    gdImagePtr out_image;
    // ... more resources
} ThumbnailContext;
```

**Functions:**
- `thumbnail_context_init()` - Initialize context
- `thumbnail_context_cleanup()` - Free all resources
- `thumbnail_open_file()` - Open video and find stream
- `thumbnail_init_decoder()` - Initialize codec
- `thumbnail_init_filters()` - Setup video filters
- `thumbnail_alloc_frames()` - Allocate frame buffers
- `thumbnail_save_image()` - Save output image

#### 5. Error Handling Standardization ✅
Centralized error codes and macros for consistent error handling:

**New files:**
- `src/mtn_error.h` - Error codes and macros (110 lines)
- `src/mtn_error.c` - Error utilities (60 lines)

**Error codes:**
```c
typedef enum MtnError {
    MTN_SUCCESS = 0,
    MTN_ERROR_GENERIC = -1,
    MTN_ERROR_INVALID_ARG = -2,
    MTN_ERROR_OUT_OF_MEMORY = -3,
    MTN_ERROR_FILE_NOT_FOUND = -4,
    // ... more specific errors
} MtnError;
```

**Helper macros:**
```c
MTN_RETURN_ERROR(msg, errcode);
MTN_GOTO_ERROR(msg, label);
MTN_CHECK_ERROR(cond, msg, errcode);
MTN_CHECK_FFMPEG(ret, msg);
MTN_CHECK_NULL(ptr, msg);
```

#### 6. FIXME Comments Addressed ✅
Fixed priority FIXMEs:
- **Line 2555:** `static nb_file` - Documented purpose
- **Line 3353:** Edge array initialization - Now uses loop for safety
- **Line 3349-3350:** Step/repeat logic - Added clarifying comments

### Build System Updates

**Makefile changes:**
```makefile
# Before
SRCS = mtn.c

# After
SRCS = mtn.c mtn_context.c mtn_thumbnail.c mtn_error.c
```

**New clean target:**
```makefile
clean:
    rm -f $(OUT)/mtn *.o
```

### Code Quality Metrics

| Metric | Before | After | Improvement |
|--------|--------|-------|-------------|
| Files | 1 | 6 | Better modularity |
| Lines in mtn.c | 4,608 | 4,634 | +26 (documentation) |
| New helper files | 0 | 4 | ~880 lines |
| Static buffers | 3 | 0 | ✅ Eliminated |
| Unsafe sprintf | 25+ | 0 | ✅ All replaced |
| FIXME comments | 63 | 19 | ✅ 70% reduction |
| Build warnings | 0 | 2 | Minor (unrelated) |

### Remaining Work

#### Pending Refactoring
1. **Full make_thumbnail() decomposition** - Currently have framework, need to migrate full logic
2. **PTS handling** - Line 2202, 3407 (requires deep FFmpeg knowledge)
3. **Non-seek mode improvements** - Lines 2249, 2873
4. **Duration guessing** - Lines 2207, 2214 for .vob files
5. **Loop timeout check** - Line 3229 for very long files

#### Technical Debt
- 19 FIXME comments remaining (down from 63)
- 2 compiler warnings (unrelated to refactoring)
- Global variables still in mtn.c (foundation laid for migration)

## Contributing

See `CONTRIBUTING.md` for guidelines:

1. **Bug Reports:** Include OS, command line, sample file link, and `-v` output
2. **Code Changes:** Fork `devel` branch, submit Merge Request
3. **Documentation:** Fix typos/errors in README or Wiki
4. **C Standard:** Follow SunOS or OpenBSD style

## Common Tasks

### Adding a new output format
1. Add conditional compilation flag in `Makefile`
2. Implement gd image save function in `mtn.c`
3. Add command-line option parsing

### Debugging frame decoding issues
1. Use `-v` flag for verbose FFmpeg output
2. Check `dump_stream()` and `dump_codec_context()` output
3. Examine PTS/DTS timing in `process_loop()`

### Modifying text overlay
- Text rendering: `draw_text_*()` functions
- Font handling: `-f`, `-F`, `-L` options
- Color configuration: `-k` (background), `-F` (font spec)

## Related Links

- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [libgd Documentation](https://libgd.github.io/)
- **Original Project:**
  - [GitLab Repository](https://gitlab.com/movie_thumbnailer/mtn)
  - [Project Wiki](https://gitlab.com/movie_thumbnailer/mtn/wikis/home)
  - [Issue Tracker](https://gitlab.com/movie_thumbnailer/mtn/issues)
- **Fork (GitHub):**
  - [GitHub Repository](https://github.com/AhmadNaruto/mtn)
  - [Wiki](https://github.com/AhmadNaruto/mtn/wikis/home)
  - [Issues](https://github.com/AhmadNaruto/mtn/issues)

---

## Node.js Library

A Node.js/TypeScript wrapper is available in the `nodejs/` directory.

### Quick Start

```bash
cd nodejs
npm install
npm run build
```

### Usage

```typescript
import { MtnThumbnailer } from 'mtn-thumbnailer';

const mtn = new MtnThumbnailer();

// Generate thumbnail
const result = await mtn.generateThumbnail('video.mp4', {
  columns: 3,
  rows: 2,
  minHeight: 200,
});

console.log(`Thumbnail: ${result.outputPath}`);
```

### Features

- ✅ Full TypeScript support
- ✅ Promise-based API
- ✅ Progress callbacks
- ✅ Batch processing
- ✅ Video metadata extraction
- ✅ Express.js integration example

### Documentation

See [`nodejs/README.md`](nodejs/README.md) for complete API documentation.

### Examples

- `examples/basic.ts` - Simple thumbnail generation
- `examples/advanced.ts` - Custom options
- `examples/batch.ts` - Batch processing
- `examples/express-server.ts` - Web API integration

### API Methods

```typescript
// Single video
await mtn.generateThumbnail(videoPath, options, onProgress);

// Multiple videos
await mtn.generateThumbnails(videoPaths, options, onProgress);

// Get metadata
await mtn.getVideoMetadata(videoPath);

// Check availability
await mtn.checkAvailability();

// Get version
await mtn.getVersion();
```
