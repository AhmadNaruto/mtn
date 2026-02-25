import { spawn } from 'child_process';
import { join, dirname, resolve } from 'path';
import { existsSync, mkdirSync, rmSync } from 'fs';
import { MtnOptions, MtnResult, MtnProgress, VideoMetadata } from './types';

/**
 * Movie Thumbnailer (mtn) Node.js wrapper
 * 
 * Generates video thumbnails and contact sheets using mtn
 */
export class MtnThumbnailer {
  private mtnPath: string;
  private tempDir?: string;

  /**
   * Create MTN thumbnailer instance
   * @param mtnPath - Path to mtn binary (default: auto-detect)
   */
  constructor(mtnPath?: string) {
    this.mtnPath = mtnPath || this.detectMtnPath();
  }

  /**
   * Auto-detect mtn binary location
   */
  private detectMtnPath(): string {
    // Common paths
    const paths = [
      join(__dirname, '../../bin/mtn'),
      '/usr/local/bin/mtn',
      '/usr/bin/mtn',
      './bin/mtn',
      'mtn'
    ];

    for (const path of paths) {
      if (existsSync(path)) {
        return path;
      }
    }

    // Fallback to 'mtn' in PATH
    return 'mtn';
  }

  /**
   * Generate thumbnail for a video file
   * @param videoPath - Path to video file
   * @param options - MTN options
   * @param onProgress - Progress callback
   * @returns Promise with generation result
   */
  async generateThumbnail(
    videoPath: string,
    options: MtnOptions = {},
    onProgress?: (progress: MtnProgress) => void
  ): Promise<MtnResult> {
    const startTime = Date.now();

    // Validate video file exists
    if (!existsSync(videoPath)) {
      return {
        success: false,
        error: `Video file not found: ${videoPath}`,
        executionTime: Date.now() - startTime,
        output: '',
        exitCode: -1
      };
    }

    // Build command line arguments
    const args = this.buildArguments(videoPath, options);

    // Create output directory if needed
    if (options.outputDir && !existsSync(options.outputDir)) {
      mkdirSync(options.outputDir, { recursive: true });
    }

    return new Promise((resolve) => {
      const mtn = spawn(this.mtnPath, args);
      let output = '';
      let errorOutput = '';

      mtn.stdout.on('data', (data: Buffer) => {
        const text = data.toString();
        output += text;
        
        // Parse progress from output
        if (onProgress) {
          this.parseProgress(text, onProgress);
        }
      });

      mtn.stderr.on('data', (data: Buffer) => {
        errorOutput += data.toString();
      });

      mtn.on('close', (code) => {
        const executionTime = Date.now() - startTime;
        const success = code === 0 || code === 1; // 0 = success, 1 = warning

        // Parse output to find generated files
        const result: MtnResult = {
          success,
          executionTime,
          output: output + errorOutput,
          exitCode: code || 0
        };

        // Extract output paths from result
        this.parseOutputPaths(videoPath, options, result);

        resolve(result);
      });

      mtn.on('error', (err) => {
        resolve({
          success: false,
          error: `Failed to execute mtn: ${err.message}`,
          executionTime: Date.now() - startTime,
          output: '',
          exitCode: -1
        });
      });
    });
  }

  /**
   * Generate thumbnails for multiple videos
   * @param videoPaths - Array of video paths
   * @param options - MTN options
   * @param onProgress - Progress callback (per file)
   * @returns Promise with array of results
   */
  async generateThumbnails(
    videoPaths: string[],
    options: MtnOptions = {},
    onProgress?: (path: string, progress: MtnProgress) => void
  ): Promise<MtnResult[]> {
    const results: MtnResult[] = [];

    for (const videoPath of videoPaths) {
      const progressCallback = onProgress 
        ? (p: MtnProgress) => onProgress(videoPath, p)
        : undefined;

      const result = await this.generateThumbnail(videoPath, options, progressCallback);
      results.push(result);
    }

    return results;
  }

  /**
   * Get video metadata using mtn verbose output
   * @param videoPath - Path to video file
   * @returns Promise with video metadata
   */
  async getVideoMetadata(videoPath: string): Promise<VideoMetadata> {
    return new Promise((resolve) => {
      const args = ['-v', '-i', videoPath];
      const mtn = spawn(this.mtnPath, args);
      let output = '';

      mtn.stderr.on('data', (data: Buffer) => {
        output += data.toString();
      });

      mtn.on('close', () => {
        const metadata: VideoMetadata = this.parseMetadata(output);
        resolve(metadata);
      });

      mtn.on('error', () => {
        resolve({});
      });
    });
  }

  /**
   * Check if mtn binary is available
   */
  async checkAvailability(): Promise<boolean> {
    return new Promise((resolve) => {
      const mtn = spawn(this.mtnPath, ['-v']);
      
      mtn.on('close', (code) => {
        // Exit code 255 means mtn ran but no input file (expected)
        // Exit code 0 or 1 means success or warning
        resolve(code === 0 || code === 1 || code === 255);
      });

      mtn.on('error', () => {
        resolve(false);
      });
    });
  }

  /**
   * Get mtn version
   */
  async getVersion(): Promise<string> {
    return new Promise((resolve) => {
      const mtn = spawn(this.mtnPath, ['-v']);
      let version = '';

      mtn.stderr.on('data', (data: Buffer) => {
        version += data.toString();
      });

      mtn.on('close', () => {
        const match = version.match(/Movie Thumbnailer \(mtn\) ([\d.]+)/);
        resolve(match ? match[1] : 'unknown');
      });

      mtn.on('error', () => {
        resolve('unknown');
      });
    });
  }

  /**
   * Build command line arguments from options
   */
  private buildArguments(videoPath: string, options: MtnOptions): string[] {
    const args: string[] = [];

    // Output options
    if (options.outputDir) args.push('-O', options.outputDir);
    if (options.outputSuffix) args.push('-o', options.outputSuffix);
    if (options.useFullFilename) args.push('-X');
    if (options.customFilename) args.push('-x', options.customFilename);
    if (options.noOverwrite) args.push('-W');

    // Grid options
    if (options.columns) args.push('-c', options.columns.toString());
    if (options.rows !== undefined) args.push('-r', options.rows.toString());
    if (options.step) args.push('-s', options.step.toString());
    if (options.minHeight) args.push('-h', options.minHeight.toString());
    if (options.width !== undefined) args.push('-w', options.width.toString());
    if (options.gap) args.push('-g', options.gap.toString());

    // Appearance
    if (options.font) args.push('-f', options.font);
    if (options.backgroundColor) args.push('-k', options.backgroundColor);
    if (options.jpegQuality) args.push('-j', options.jpegQuality.toString());
    if (options.showInfo === false) args.push('-i');
    if (options.showTimestamp === false) args.push('-t');
    if (options.additionalText) args.push('-T', options.additionalText);

    // Detection
    if (options.edgeDetection) args.push('-D', options.edgeDetection.toString());
    if (options.blankThreshold) args.push('-b', options.blankThreshold.toString());

    // Timing
    if (options.skipBeginning) args.push('-B', options.skipBeginning.toString());
    if (options.skipEnd) args.push('-E', options.skipEnd.toString());
    if (options.cutDuration) args.push('-C', options.cutDuration.toString());

    // Streams
    if (options.videoStream !== undefined) args.push('-S', options.videoStream.toString());

    // Modes
    if (options.verbose) args.push('-v');
    if (options.quiet) args.push('-q');
    if (options.seekMode) args.push('-z');
    if (options.nonSeekMode) args.push('-Z');

    // Advanced
    if (options.aspectRatio) args.push('-a', options.aspectRatio.toString());
    if (options.shadow !== undefined) {
      args.push('--shadow', typeof options.shadow === 'number' ? options.shadow.toString() : '');
    }
    if (options.transparent) args.push('--transparent');
    if (options.extractCover) args.push('--cover');
    if (options.webVtt) args.push('--vtt', options.webVtt);
    if (options.filters) args.push('--filters', options.filters);
    if (options.tonemap) args.push('--tonemap');

    // Directory
    if (options.depth !== undefined) args.push('-d', options.depth.toString());
    if (options.extensions) args.push('-e', options.extensions.join(','));

    // Individual shots
    if (options.saveIndividual) {
      let individualArg = '';
      if (options.saveIndividual.thumbnail) individualArg += 't';
      if (options.saveIndividual.original) individualArg += 'o';
      if (options.saveIndividual.ignoreGrid) individualArg += 'i';
      if (individualArg) args.push('-I', individualArg);
    }

    // Info file
    if (options.infoSuffix) args.push('-N', options.infoSuffix);

    // Add video path
    args.push(videoPath);

    return args;
  }

  /**
   * Parse progress from mtn output
   */
  private parseProgress(output: string, onProgress: (progress: MtnProgress) => void): void {
    // Parse shot progress
    const shotMatch = output.match(/shot (\d+):/);
    if (shotMatch) {
      const currentShot = parseInt(shotMatch[1]);
      onProgress({
        currentShot,
        totalShots: currentShot + 1, // Estimate
        percentage: 0,
      });
    }

    // Parse time progress
    const timeMatch = output.match(/(\d+\.\d+) s.*?(\d+\.\d+) shots\/s/);
    if (timeMatch) {
      const currentTime = parseFloat(timeMatch[1]);
      onProgress({
        currentShot: 0,
        totalShots: 0,
        percentage: 0,
        currentTime,
      });
    }
  }

  /**
   * Parse output paths from mtn result
   */
  private parseOutputPaths(videoPath: string, options: MtnOptions, result: MtnResult): void {
    // Determine output filename
    let basePath = videoPath;
    if (options.outputDir) {
      basePath = join(options.outputDir, this.getFilename(videoPath));
    }

    // Remove extension if not using full filename
    if (!options.useFullFilename) {
      const lastDot = basePath.lastIndexOf('.');
      if (lastDot > 0) {
        basePath = basePath.substring(0, lastDot);
      }
    }

    // Set output path
    result.outputPath = basePath + (options.outputSuffix || '_s.jpg');

    // Check if files exist
    if (options.infoSuffix) {
      const infoPath = basePath + options.infoSuffix;
      if (existsSync(infoPath)) {
        result.infoPath = infoPath;
      }
    }

    if (options.extractCover) {
      const coverPath = basePath + '_cover.jpg';
      if (existsSync(coverPath)) {
        result.coverPath = coverPath;
      }
    }
  }

  /**
   * Parse video metadata from mtn output
   */
  private parseMetadata(output: string): VideoMetadata {
    const metadata: VideoMetadata = {};

    // Duration
    const durationMatch = output.match(/duration: (\d+\.\d+) s/);
    if (durationMatch) {
      metadata.duration = parseFloat(durationMatch[1]);
    }

    // Size
    const sizeMatch = output.match(/Size: (\d+) bytes/);
    if (sizeMatch) {
      metadata.size = parseInt(sizeMatch[1]);
    }

    // Video stream
    const videoMatch = output.match(/Video:.*?, (\d+)x(\d+)/);
    if (videoMatch) {
      metadata.width = parseInt(videoMatch[1]);
      metadata.height = parseInt(videoMatch[2]);
    }

    // Codec
    const codecMatch = output.match(/Video: ([^,]+)/);
    if (codecMatch) {
      metadata.codec = codecMatch[1].trim();
    }

    // Frame rate
    const fpsMatch = output.match(/(\d+\.\d+) fps/);
    if (fpsMatch) {
      metadata.frameRate = parseFloat(fpsMatch[1]);
    }

    // Bitrate
    const bitrateMatch = output.match(/bitrate: (\d+) kb\/s/);
    if (bitrateMatch) {
      metadata.bitrate = parseInt(bitrateMatch[1]);
    }

    return metadata;
  }

  /**
   * Get filename from path
   */
  private getFilename(path: string): string {
    return path.split('/').pop() || path.split('\\').pop() || path;
  }
}

/**
 * Create MTN thumbnailer instance (convenience function)
 * @param mtnPath - Path to mtn binary
 */
export function createMtnThumbnailer(mtnPath?: string): MtnThumbnailer {
  return new MtnThumbnailer(mtnPath);
}

export default MtnThumbnailer;
