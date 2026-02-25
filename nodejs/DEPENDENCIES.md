# Node.js Dependencies Status

## Security Status ✅

**Vulnerabilities:** 0 found (as of Feb 2026)

All high-severity vulnerabilities have been fixed by updating:
- `@typescript-eslint/eslint-plugin` → v7.x
- `@typescript-eslint/parser` → v7.x
- `eslint` → v8.57.x

## Deprecation Warnings ℹ️

The following deprecation warnings appear during `npm install`:

### 1. `glob@7.2.3` (via Jest & ESLint)
```
npm warn deprecated glob@7.2.3: Old versions of glob are not supported
```

**Source:** Transitive dependency from:
- Jest → @jest/core → @jest/reporters → glob
- ESLint → file-entry-cache → flat-cache → rimraf → glob

**Impact:** None - This is used only for development/testing, not in production.

**Fix:** Will be resolved when Jest v30 is released with updated glob dependency.

### 2. `@humanwhocodes/*` packages (via ESLint)
```
npm warn deprecated @humanwhocodes/config-array@0.13.0
npm warn deprecated @humanwhocodes/object-schema@2.0.3
```

**Source:** ESLint v8.x internal dependencies

**Impact:** None - ESLint still functions correctly.

**Fix:** ESLint v9.x uses new config system, but requires migration.

### 3. `inflight@1.0.6` (via Jest)
```
npm warn deprecated inflight@1.0.6: This module is not supported, and leaks memory
```

**Source:** Jest → glob → inflight

**Impact:** None - Only affects development environment.

### 4. `rimraf@3.0.2` (via ESLint)
```
npm warn deprecated rimraf@3.0.2: Rimraf versions prior to v4 are no longer supported
```

**Source:** ESLint → flat-cache → rimraf

**Impact:** None - Used only for cache cleanup.

## Production Impact

**None.** All deprecations are in `devDependencies`:

| Package | Used In | Production Impact |
|---------|---------|-------------------|
| glob | Test coverage | ❌ None |
| @humanwhocodes/* | ESLint config | ❌ None |
| inflight | Jest internals | ❌ None |
| rimraf | ESLint cache | ❌ None |

The compiled library (`dist/`) has **zero dependencies** in production.

## Recommendations

### For Users (Installing the Library)
✅ **Safe to use** - No vulnerabilities, no production dependencies.

```bash
npm install mtn-thumbnailer
```

### For Developers (Contributing)

1. **Ignore deprecation warnings** - They don't affect functionality
2. **Run tests** - All tests pass despite warnings
3. **Focus on code** - Warnings are from tooling, not your code

### Future Updates

When these packages release major versions:
- **Jest v30** - Expected to update glob dependency
- **ESLint v9** - New config system (breaking changes)

We will update after verifying compatibility.

## Verification

To verify current status:

```bash
# Check for vulnerabilities
npm audit

# Check dependency tree
npm ls --depth=0

# Run tests
npm test

# Build library
npm run build
```

## Current Status (Feb 2026)

```
✓ 0 vulnerabilities
✓ 10/10 tests passing
✓ Build successful
✓ Production-ready
⚠ 5 deprecation warnings (dev-only, non-critical)
```

---

**Summary:** The library is production-ready. Deprecation warnings are from development tools and do not affect the compiled output or runtime behavior.
