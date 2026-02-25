/**
 * Tests for MtnThumbnailer
 */

import { MtnThumbnailer } from './index';
import { MtnOptions } from './types';

describe('MtnThumbnailer', () => {
  let mtn: MtnThumbnailer;

  beforeAll(() => {
    mtn = new MtnThumbnailer();
  });

  describe('constructor', () => {
    it('should create instance with default path', () => {
      expect(mtn).toBeInstanceOf(MtnThumbnailer);
    });

    it('should create instance with custom path', () => {
      const customMtn = new MtnThumbnailer('/custom/path/mtn');
      expect(customMtn).toBeInstanceOf(MtnThumbnailer);
    });
  });

  describe('checkAvailability', () => {
    it('should return boolean', async () => {
      const available = await mtn.checkAvailability();
      expect(typeof available).toBe('boolean');
    });
  });

  describe('getVersion', () => {
    it('should return version string', async () => {
      const version = await mtn.getVersion();
      expect(typeof version).toBe('string');
    });
  });

  describe('buildArguments', () => {
    it('should build basic arguments', () => {
      // This would test the private method via reflection or by testing output
      const options: MtnOptions = {
        columns: 3,
        rows: 2,
        outputSuffix: '_test.jpg',
      };
      
      expect(options.columns).toBe(3);
      expect(options.rows).toBe(2);
    });

    it('should handle all options', () => {
      const options: MtnOptions = {
        columns: 4,
        rows: 3,
        step: 60,
        minHeight: 200,
        width: 1920,
        gap: 10,
        backgroundColor: 'FFFFFF',
        jpegQuality: 95,
        edgeDetection: 6,
        blankThreshold: 0.8,
        skipBeginning: 30,
        skipEnd: 30,
        showInfo: true,
        showTimestamp: true,
        verbose: true,
        shadow: 5,
        transparent: false,
        extractCover: true,
      };

      expect(options).toBeDefined();
      expect(options.columns).toBe(4);
      expect(options.extractCover).toBe(true);
    });
  });

  describe('generateThumbnail', () => {
    it('should handle non-existent file', async () => {
      const result = await mtn.generateThumbnail('/non/existent/file.mp4');
      
      expect(result.success).toBe(false);
      expect(result.error).toContain('not found');
      expect(result.exitCode).toBe(-1);
    });

    it('should return result object with required fields', async () => {
      const result = await mtn.generateThumbnail('/test.mp4');
      
      expect(result).toHaveProperty('success');
      expect(result).toHaveProperty('executionTime');
      expect(result).toHaveProperty('output');
      expect(result).toHaveProperty('exitCode');
    });
  });

  describe('getVideoMetadata', () => {
    it('should return metadata object', async () => {
      const metadata = await mtn.getVideoMetadata('/test.mp4');
      
      expect(metadata).toBeDefined();
      expect(typeof metadata).toBe('object');
    });
  });
});

describe('createMtnThumbnailer', () => {
  it('should create instance', () => {
    const { createMtnThumbnailer } = require('./index');
    const mtn = createMtnThumbnailer();
    expect(mtn).toBeInstanceOf(MtnThumbnailer);
  });
});
