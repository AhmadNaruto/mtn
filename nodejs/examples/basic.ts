/**
 * Example: Basic thumbnail generation
 * 
 * This example shows how to generate a simple thumbnail
 * for a video file using default settings.
 */

import { MtnThumbnailer } from './index';

async function main() {
  // Create thumbnailer instance
  const mtn = new MtnThumbnailer();

  // Check if mtn is available
  const available = await mtn.checkAvailability();
  if (!available) {
    console.error('mtn binary not found. Please install mtn first.');
    process.exit(1);
  }

  // Get version
  const version = await mtn.getVersion();
  console.log(`Using mtn version: ${version}`);

  // Generate thumbnail with default settings
  const videoPath = process.argv[2] || 'sample_video.mp4';
  console.log(`Generating thumbnail for: ${videoPath}`);

  const result = await mtn.generateThumbnail(videoPath);

  if (result.success) {
    console.log('✓ Thumbnail generated successfully!');
    console.log(`  Output: ${result.outputPath}`);
    console.log(`  Time: ${result.executionTime}ms`);
  } else {
    console.error('✗ Failed to generate thumbnail');
    console.error(`  Error: ${result.error}`);
    console.error(`  Output: ${result.output}`);
  }
}

main().catch(console.error);
