/**
 * Example: Express.js integration
 * 
 * Create a web API endpoint to generate
 * video thumbnails on demand.
 */

import express from 'express';
import { MtnThumbnailer } from '../src/index';
import { join } from 'path';

const app = express();
const mtn = new MtnThumbnailer();
const PORT = process.env.PORT || 3000;

app.use(express.json());

/**
 * POST /api/thumbnail
 * Generate thumbnail for a video
 */
app.post('/api/thumbnail', async (req, res) => {
  const { videoPath, options } = req.body;

  if (!videoPath) {
    return res.status(400).json({ error: 'videoPath is required' });
  }

  try {
    const result = await mtn.generateThumbnail(videoPath, options || {});

    if (result.success) {
      res.json({
        success: true,
        path: result.outputPath,
        executionTime: result.executionTime,
      });
    } else {
      res.status(500).json({
        success: false,
        error: result.error,
        output: result.output,
      });
    }
  } catch (error) {
    res.status(500).json({
      success: false,
      error: error.message,
    });
  }
});

/**
 * GET /api/metadata/:videoPath
 * Get video metadata
 */
app.get('/api/metadata/*', async (req, res) => {
  const videoPath = req.params[0];

  if (!videoPath) {
    return res.status(400).json({ error: 'videoPath is required' });
  }

  try {
    const metadata = await mtn.getVideoMetadata(videoPath);
    res.json({ success: true, metadata });
  } catch (error) {
    res.status(500).json({
      success: false,
      error: error.message,
    });
  }
});

/**
 * GET /health
 * Health check endpoint
 */
app.get('/health', async (req, res) => {
  const available = await mtn.checkAvailability();
  const version = await mtn.getVersion();

  res.json({
    status: 'ok',
    mtn: {
      available,
      version,
    },
  });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
  console.log('Endpoints:');
  console.log('  POST /api/thumbnail - Generate thumbnail');
  console.log('  GET  /api/metadata/* - Get video metadata');
  console.log('  GET  /health - Health check');
});
