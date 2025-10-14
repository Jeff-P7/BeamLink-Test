const { getDefaultConfig } = require('expo/metro-config');

const config = getDefaultConfig(__dirname);

// Bundle optimization
config.transformer.minifierConfig = {
  // Enable more aggressive minification
  mangle: true,
  keep_fnames: false,
  toplevel: true,
  compress: {
    drop_console: true, // Remove console.log in production
    drop_debugger: true,
    pure_funcs: ['console.log', 'console.info', 'console.debug'],
  },
};

// Tree shaking optimization
config.resolver.platforms = ['ios', 'android', 'native', 'web'];

// Enable Hermes for better performance
config.transformer.hermesParser = true;

// Optimize asset resolution
config.resolver.assetExts.push(
  // Add any additional asset extensions if needed
);

module.exports = config;