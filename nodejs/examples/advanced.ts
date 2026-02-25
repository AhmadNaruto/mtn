/**
 * Example: Advanced thumbnail generation with custom options
 * 
 * This example demonstrates various mtn options
 * for customized thumbnail generation.
 */

import { MtnThumbnailer, MtnOptions } from '../src/index';

async function main() {
  const mtn = new MtnThumbnailer();

  const videoPath = process.argv[2] || 'sample_video.mp4';

  // Custom options for a professional-looking thumbnail
  const options: MtnOptions = {
    // Grid layout
    columns: 4,
    rows: 3,
    gap: 10,
    
    // Output settings
    outputSuffix: '_preview.jpg',
    jpegQuality: 95,
    
    // Appearance
    minHeight: 200,
    backgroundColor: '000000', // Black background
    showInfo: true,
    showTimestamp: true,
    
    // Font (if you have a custom font)
    // font: '/path/to/font.ttf',
    
    // Advanced detection
    edgeDetection: 6,
    blankThreshold: 0.8,
    
    // Skip first and last 30 seconds
    skipBeginning: 30,
    skipEnd: 30,
    
    // Add custom title
    additionalText: 'My Video Preview',
    
    // Verbose output for debugging
    verbose: true,
  };

  console.log('Generating advanced thumbnail...');
  console.log('Options:', JSON.stringify(options, null, 2));

  // Generate with progress callback
  const result = await mtn.generateThumbnail(
    videoPath,
    options,
    (progress) => {
      if (progress.currentTime) {
        console.log(`Processing: ${progress.currentTime.toFixed(1)}s`);
      }
    }
  );

  if (result.success) {
    console.log('\n✓ Success!');
    console.log(`Output: ${result.outputPath}`);
    console.log(`Execution time: ${result.executionTime}ms`);
    console.log(`Exit code: ${result.exitCode}`);
    
    if (result.infoPath) {
      console.log(`Info file: ${result.infoPath}`);
    }
  } else {
    console.error('\n✗ Failed!');
    console.error(`Error: ${result.error}`);
    console.error(`Output:\n${result.output}`);
  }
}

main().catch(console.error);
