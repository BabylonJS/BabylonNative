const os = require('os');
const shelljs = require('shelljs');
const chalk = require('chalk');

function selectRN(version) {
  if (version != '0.64' && version != '0.65' && version != '0.69') {
    console.error(chalk.black.bgRedBright(`Unsupported React Native ${version}`));
    process.exit(1);
  }
  
  console.log(chalk.black.bgCyan(`Using React Native ${version}`));

  shelljs.ln('-sf', `./${version}`, './Playground');
  shelljs.exec('npm install', {cwd: './Playground'});
}

// First arg will be 'node', second arg will be 'version.js'
const [command] = process.argv.slice(2);
const [version] = process.argv.slice(3);

if (command === 'selectRN') {
  selectRN(version);
} else {
  console.error(chalk.black.bgRedBright(`Unkown command: ${command}`));
  process.exit(1);
}