#!/usr/bin/env node

const { execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

/**
 * Recursively copy files and directories
 * @param {string} src - Source directory
 * @param {string} dest - Destination directory
 */
function copyRecursive(src, dest) {
  if (!fs.existsSync(src)) {
    throw new Error(`Source directory ${src} does not exist`);
  }
  
  // Create destination directory if it doesn't exist
  if (!fs.existsSync(dest)) {
    fs.mkdirSync(dest, { recursive: true });
  }
  
  const entries = fs.readdirSync(src, { withFileTypes: true });
  
  for (const entry of entries) {
    const srcPath = path.join(src, entry.name);
    const destPath = path.join(dest, entry.name);
    
    if (entry.isDirectory()) {
      copyRecursive(srcPath, destPath);
    } else {
      fs.copyFileSync(srcPath, destPath);
    }
  }
}

/**
 * Execute a command in a specific directory
 * @param {string} command - The command to execute
 * @param {string} workingDir - The working directory
 * @param {string} stepName - Name of the step for logging
 */
function executeStep(command, workingDir, stepName) {
  console.log(`\n🔄 ${stepName}`);
  console.log(`Working directory: ${workingDir}`);
  console.log(`Command: ${command}`);
  
  // Check if directory exists
  if (!fs.existsSync(workingDir)) {
    console.error(`❌ Error: Directory ${workingDir} does not exist`);
    process.exit(1);
  }
  
  try {
    execSync(command, {
      cwd: workingDir,
      stdio: 'inherit', // This will show the output in real-time
      encoding: 'utf8'
    });
    console.log(`✅ ${stepName} completed successfully`);
  } catch (error) {
    console.error(`❌ ${stepName} failed with error:`);
    console.error(error.message);
    process.exit(1);
  }
}

async function main() {
  console.log('Setting up developer environment ...');
  
  const rootDir = path.join(process.cwd(), '..');
  
  // Step 1: NPM Install 
  const packageDir = path.join(rootDir, 'Package');
  executeStep('npm install', packageDir, 'NPM Install');
  
  // Step 2: Build BabylonNative source tree
  executeStep('npx gulp buildBabylonNativeSourceTree', packageDir, 'Build BabylonNative source tree');
  
  // Step 3: NPM Install (Playground)
  const playgroundDir = path.join(rootDir, 'Apps', 'Playground');
  executeStep('npm install', playgroundDir, 'NPM Install (Playground)');
  
  // Step 4: Install Module
  const moduleDir = path.join(rootDir, 'Modules', '@babylonjs', 'react-native');
  executeStep('npm install', moduleDir, 'Install Module');
  
  // Step 5: Build TypeScript and Copy Files
  console.log('\n🔄 Build Type script');
  console.log(`Working directory: ${packageDir}`);
  console.log('Command: npx gulp buildTypeScript');
  
  try {
    execSync('npx gulp buildTypeScript', {
      cwd: packageDir,
      stdio: 'inherit',
      encoding: 'utf8'
    });
    console.log('✅ TypeScript build completed successfully');
  } catch (error) {
    console.error('❌ TypeScript build failed with error:');
    console.error(error.message);
    process.exit(1);
  }
  
  // Copy files from Package/Assembled to Modules/@babylonjs/react-native
  console.log('\n📁 Copying files from Package/Assembled to Modules/@babylonjs/react-native');
  const sourceDir = path.join(packageDir, 'Assembled');
  const targetDir = path.join(rootDir, 'Modules', '@babylonjs', 'react-native');
  
  try {
    copyRecursive(sourceDir, targetDir);
    console.log('✅ Files copied successfully');
  } catch (error) {
    console.error('❌ File copy failed with error:');
    console.error(error.message);
    process.exit(1);
  }
  
  console.log('\n🎉 All steps completed successfully!');
}

// Handle unhandled promise rejections
process.on('unhandledRejection', (reason, promise) => {
  console.error('Unhandled Rejection at:', promise, 'reason:', reason);
  process.exit(1);
});

// Handle uncaught exceptions
process.on('uncaughtException', (error) => {
  console.error('Uncaught Exception:', error);
  process.exit(1);
});

// Run the main function
main().catch((error) => {
  console.error('Script failed:', error);
  process.exit(1);
});