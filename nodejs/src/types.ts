/**
 * MTN Thumbnailer Options
 * Corresponds to mtn command-line options
 */
export interface MtnOptions {
  /** Output directory (-O) */
  outputDir?: string;
  
  /** Output filename suffix including extension (-o), e.g., '_preview.jpg' */
  outputSuffix?: string;
  
  /** Number of columns (-c), default: 3 */
  columns?: number;
  
  /** Number of rows (-r), 0 = auto-calculate */
  rows?: number;
  
  /** Time step between shots in seconds (-s) */
  step?: number;
  
  /** Minimum height of each shot in pixels (-h), default: 150 */
  minHeight?: number;
  
  /** Width of output image (-w), 0 = column * movie width */
  width?: number;
  
  /** Gap between shots in pixels (-g) */
  gap?: number;
  
  /** Font file for text overlay (-f) */
  font?: string;
  
  /** Background color in hex (-k), e.g., 'FFFFFF' */
  backgroundColor?: string;
  
  /** JPEG quality 1-100 (-j), default: 90 */
  jpegQuality?: number;
  
  /** Edge detection level (-D), 0 = off, >0 = on */
  edgeDetection?: number;
  
  /** Blank threshold (-b), 0-1, higher = more sensitive */
  blankThreshold?: number;
  
  /** Skip seconds from beginning (-B) */
  skipBeginning?: number;
  
  /** Skip seconds at end (-E) */
  skipEnd?: number;
  
  /** Cut movie duration to N seconds (-C) */
  cutDuration?: number;
  
  /** Show info text (-i to disable) */
  showInfo?: boolean;
  
  /** Show timestamp (-t to disable) */
  showTimestamp?: boolean;
  
  /** Additional text above image (-T) */
  additionalText?: string;
  
  /** Info text file suffix (-N) */
  infoSuffix?: string;
  
  /** Save individual shots (-I) */
  saveIndividual?: {
    thumbnail?: boolean;  // t: thumbnail size
    original?: boolean;   // o: original size
    ignoreGrid?: boolean; // i: don't create grid
  };
  
  /** Select video stream (-S) */
  videoStream?: number;
  
  /** Use full input filename including extension (-X) */
  useFullFilename?: boolean;
  
  /** Custom output filename base (-x) */
  customFilename?: string;
  
  /** Don't overwrite existing files (-W) */
  noOverwrite?: boolean;
  
  /** Verbose mode (-v) */
  verbose?: boolean;
  
  /** Quiet mode (-q) */
  quiet?: boolean;
  
  /** Always use seek mode (-z) */
  seekMode?: boolean;
  
  /** Always use non-seek mode (-Z) */
  nonSeekMode?: boolean;
  
  /** Override aspect ratio (-a), e.g., 1.3333 for 4:3 */
  aspectRatio?: number;
  
  /** Draw shadows beneath thumbnails (--shadow) */
  shadow?: number | true;
  
  /** Transparent background (--transparent), PNG only */
  transparent?: boolean;
  
  /** Extract album art (--cover) */
  extractCover?: boolean;
  
  /** Export WebVTT file (--vtt) */
  webVtt?: string;
  
  /** FFmpeg filter chain (--filters) */
  filters?: string;
  
  /** Tonemap HDR movies (--tonemap) */
  tonemap?: boolean;
  
  /** Recursion depth for directories (-d) */
  depth?: number;
  
  /** File extensions to process (-e) */
  extensions?: string[];
}

/**
 * Progress event during thumbnail generation
 */
export interface MtnProgress {
  /** Current shot index */
  currentShot: number;
  
  /** Total shots to generate */
  totalShots: number;
  
  /** Progress percentage (0-100) */
  percentage: number;
  
  /** Current timestamp in video (seconds) */
  currentTime?: number;
  
  /** Video duration (seconds) */
  duration?: number;
}

/**
 * Result from mtn execution
 */
export interface MtnResult {
  /** Success status */
  success: boolean;
  
  /** Output image path */
  outputPath?: string;
  
  /** Info file path (if generated) */
  infoPath?: string;
  
  /** Cover image path (if extracted) */
  coverPath?: string;
  
  /** Individual shot paths (if saved) */
  individualShots?: string[];
  
  /** WebVTT file path (if exported) */
  webVttPath?: string;
  
  /** Execution time in milliseconds */
  executionTime: number;
  
  /** mtn output log */
  output: string;
  
  /** Error message if failed */
  error?: string;
  
  /** Exit code */
  exitCode: number;
}

/**
 * Video metadata
 */
export interface VideoMetadata {
  /** Duration in seconds */
  duration?: number;
  
  /** Width in pixels */
  width?: number;
  
  /** Height in pixels */
  height?: number;
  
  /** Video codec */
  codec?: string;
  
  /** Audio codec */
  audioCodec?: string;
  
  /** Frame rate */
  frameRate?: number;
  
  /** Bitrate in kb/s */
  bitrate?: number;
  
  /** File size in bytes */
  size?: number;
}
