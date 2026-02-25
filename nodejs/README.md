# mtn-thumbnailer

Node.js wrapper untuk **Movie Thumbnailer (mtn)** - generate video thumbnails dan contact sheets dengan mudah.

## Fitur

- ✅ TypeScript support
- ✅ Promise-based API
- ✅ Progress callbacks
- ✅ Batch processing
- ✅ Video metadata extraction
- ✅ Full mtn options support
- ✅ Cross-platform (Linux, macOS, Windows)

## Instalasi

### Prerequisites

Pastikan mtn sudah terinstall di sistem Anda:

```bash
# Ubuntu/Debian
sudo apt-get install mtn

# Atau build dari source
cd /path/to/mtn
cd src && make && sudo make install
```

### Install Package

```bash
npm install mtn-thumbnailer
```

## Quick Start

```typescript
import { MtnThumbnailer } from 'mtn-thumbnailer';

// Create instance
const mtn = new MtnThumbnailer();

// Generate thumbnail
const result = await mtn.generateThumbnail('video.mp4', {
  columns: 3,
  rows: 2,
  minHeight: 200,
});

console.log(`Thumbnail saved to: ${result.outputPath}`);
```

## Usage

### Basic Example

```typescript
import { MtnThumbnailer } from 'mtn-thumbnailer';

async function main() {
  const mtn = new MtnThumbnailer();
  
  // Check availability
  const available = await mtn.checkAvailability();
  if (!available) {
    console.error('mtn not found!');
    return;
  }
  
  // Generate with defaults
  const result = await mtn.generateThumbnail('movie.mp4');
  
  if (result.success) {
    console.log('✓ Generated:', result.outputPath);
  }
}

main();
```

### Advanced Options

```typescript
import { MtnThumbnailer } from 'mtn-thumbnailer';

const mtn = new MtnThumbnailer();

const result = await mtn.generateThumbnail('movie.mp4', {
  // Grid layout
  columns: 4,
  rows: 3,
  gap: 10,
  
  // Output
  outputSuffix: '_preview.jpg',
  jpegQuality: 95,
  
  // Appearance
  minHeight: 200,
  backgroundColor: '000000',
  showInfo: true,
  showTimestamp: true,
  
  // Detection
  edgeDetection: 6,
  blankThreshold: 0.8,
  
  // Timing
  skipBeginning: 30,
  skipEnd: 30,
  
  // Custom title
  additionalText: 'My Video Preview',
  
  // Advanced
  shadow: 5,
  extractCover: true,
  verbose: true,
});
```

### Progress Callback

```typescript
const result = await mtn.generateThumbnail(
  'movie.mp4',
  { columns: 3, rows: 2 },
  (progress) => {
    console.log(`Progress: ${progress.percentage.toFixed(0)}%`);
    console.log(`Current shot: ${progress.currentShot}`);
    if (progress.currentTime) {
      console.log(`Time: ${progress.currentTime.toFixed(1)}s`);
    }
  }
);
```

### Batch Processing

```typescript
import { MtnThumbnailer } from 'mtn-thumbnailer';

const mtn = new MtnThumbnailer();

const videos = [
  'video1.mp4',
  'video2.mp4',
  'video3.mp4',
];

const results = await mtn.generateThumbnails(
  videos,
  { columns: 3, outputDir: './thumbnails' },
  (videoPath, progress) => {
    console.log(`Processing ${videoPath}: ${progress.percentage}%`);
  }
);

// Summary
const successCount = results.filter(r => r.success).length;
console.log(`Success: ${successCount}/${results.length}`);
```

### Get Video Metadata

```typescript
const metadata = await mtn.getVideoMetadata('movie.mp4');

console.log('Duration:', metadata.duration, 'seconds');
console.log('Resolution:', metadata.width, 'x', metadata.height);
console.log('Codec:', metadata.codec);
console.log('FPS:', metadata.frameRate);
```

## API Reference

### Class: MtnThumbnailer

#### Constructor

```typescript
new MtnThumbnailer(mtnPath?: string)
```

- `mtnPath` - Optional path to mtn binary (auto-detected if not specified)

#### Methods

##### `generateThumbnail(videoPath, options?, onProgress?)`

Generate thumbnail for a single video.

```typescript
async generateThumbnail(
  videoPath: string,
  options?: MtnOptions,
  onProgress?: (progress: MtnProgress) => void
): Promise<MtnResult>
```

##### `generateThumbnails(videoPaths, options?, onProgress?)`

Generate thumbnails for multiple videos.

```typescript
async generateThumbnails(
  videoPaths: string[],
  options?: MtnOptions,
  onProgress?: (path: string, progress: MtnProgress) => void
): Promise<MtnResult[]>
```

##### `getVideoMetadata(videoPath)`

Extract video metadata.

```typescript
async getVideoMetadata(videoPath: string): Promise<VideoMetadata>
```

##### `checkAvailability()`

Check if mtn binary is available.

```typescript
async checkAvailability(): Promise<boolean>
```

##### `getVersion()`

Get mtn version string.

```typescript
async getVersion(): Promise<string>
```

### Options (MtnOptions)

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `columns` | number | 3 | Number of columns |
| `rows` | number | 0 (auto) | Number of rows |
| `step` | number | 120 | Time step in seconds |
| `minHeight` | number | 150 | Minimum height in pixels |
| `width` | number | 1024 | Output width (0 = auto) |
| `gap` | number | 0 | Gap between shots |
| `outputDir` | string | - | Output directory |
| `outputSuffix` | string | `_s.jpg` | Output filename suffix |
| `jpegQuality` | number | 90 | JPEG quality (1-100) |
| `backgroundColor` | string | `FFFFFF` | Background color (hex) |
| `font` | string | - | Font file path |
| `edgeDetection` | number | 0 | Edge detection level |
| `blankThreshold` | number | 0.8 | Blank frame threshold |
| `skipBeginning` | number | 0 | Skip seconds at start |
| `skipEnd` | number | 0 | Skip seconds at end |
| `showInfo` | boolean | true | Show metadata info |
| `showTimestamp` | boolean | true | Show timestamps |
| `verbose` | boolean | false | Verbose output |
| `shadow` | number/true | false | Shadow radius |
| `extractCover` | boolean | false | Extract album art |
| `filters` | string | - | FFmpeg filter chain |

### Result (MtnResult)

```typescript
interface MtnResult {
  success: boolean;
  outputPath?: string;
  infoPath?: string;
  coverPath?: string;
  executionTime: number;
  output: string;
  error?: string;
  exitCode: number;
}
```

### Progress (MtnProgress)

```typescript
interface MtnProgress {
  currentShot: number;
  totalShots: number;
  percentage: number;
  currentTime?: number;
  duration?: number;
}
```

### Metadata (VideoMetadata)

```typescript
interface VideoMetadata {
  duration?: number;      // seconds
  width?: number;         // pixels
  height?: number;        // pixels
  codec?: string;
  audioCodec?: string;
  frameRate?: number;
  bitrate?: number;       // kb/s
  size?: number;          // bytes
}
```

## Examples

Lihat folder `examples/` untuk contoh lengkap:

- [`basic.ts`](examples/basic.ts) - Simple thumbnail generation
- [`advanced.ts`](examples/advanced.ts) - Custom options
- [`batch.ts`](examples/batch.ts) - Batch processing

## Development

```bash
# Install dependencies
npm install

# Build
npm run build

# Test
npm test

# Lint
npm run lint
```

## License

GPL-2.0 (same as mtn)

## Links

- [mtn GitLab](https://gitlab.com/movie_thumbnailer/mtn)
- [mtn Wiki](https://gitlab.com/movie_thumbnailer/mtn/wikis/home)
