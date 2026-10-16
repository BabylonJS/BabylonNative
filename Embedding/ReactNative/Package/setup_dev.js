#!/usr/bin/env node

const { execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

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
  
  const rootDir = path.resolve(__dirname, '..');
  
  // Step 1: NPM Install 
  const packageDir = path.join(rootDir, 'Package');
  executeStep('npm install', packageDir, 'NPM Install');
  
  // Step 3: NPM Install (Playground)
  const playgroundDir = path.join(rootDir, 'Apps', 'Playground');
  executeStep('npm install', playgroundDir, 'NPM Install (Playground)');
  
  // Step 4: Install Module
  const moduleDir = path.join(rootDir, 'Modules', '@babylonjs', 'react-native');
  executeStep('npm install', moduleDir, 'Install Module');
  
  executeStep('npx gulp setupDev', packageDir, 'Build TypeScript and copy generated module files');
  
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