/**
 * Example: Batch processing multiple videos
 * 
 * Process entire directories of videos and generate
 * thumbnails for each one.
 */

import { MtnThumbnailer } from './index';
import { readdirSync, statSync } from 'fs';
import { join } from 'path';

async function main() {
  const mtn = new MtnThumbnailer();
  
  // Directory to process
  const videoDir = process.argv[2] || './videos';
  
  // Supported video extensions
  const videoExtensions = ['.mp4', '.mkv', '.avi', '.mov', '.webm', '.wmv'];
  
  // Get all video files
  const videoFiles: string[] = [];
  
  try {
    const files = readdirSync(videoDir);
    for (const file of files) {
      const ext = file.toLowerCase().substring(file.lastIndexOf('.'));
      if (videoExtensions.includes(ext)) {
        videoFiles.push(join(videoDir, file));
      }
    }
  } catch (err) {
    console.error(`Error reading directory: ${err}`);
    process.exit(1);
  }
  
  if (videoFiles.length === 0) {
    console.log('No video files found in', videoDir);
    return;
  }
  
  console.log(`Found ${videoFiles.length} video files`);
  console.log('Starting batch processing...\n');
  
  // Process all videos
  const results = await mtn.generateThumbnails(
    videoFiles,
    {
      columns: 3,
      rows: 2,
      minHeight: 150,
      outputDir: './thumbnails',
      verbose: false,
    },
    (videoPath, progress) => {
      const filename = videoPath.split('/').pop();
      console.log(`[${progress.percentage.toFixed(0)}%] Processing: ${filename}`);
    }
  );
  
  // Summary
  const successCount = results.filter(r => r.success).length;
  const failCount = results.length - successCount;
  
  console.log('\n=== Batch Processing Complete ===');
  console.log(`Total: ${results.length}`);
  console.log(`Success: ${successCount}`);
  console.log(`Failed: ${failCount}`);
  
  if (failCount > 0) {
    console.log('\nFailed files:');
    results
      .filter(r => !r.success)
      .forEach((r, i) => {
        console.log(`  - ${videoFiles[i]}: ${r.error}`);
      });
  }
  
  console.log(`\nTotal time: ${results.reduce((sum, r) => sum + r.executionTime, 0)}ms`);
}

main().catch(console.error);
