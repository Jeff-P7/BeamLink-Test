#!/usr/bin/env node

const { program } = require('commander');
const fs = require('fs-extra');
const path = require('path');
const inquirer = require('inquirer');
const chalk = require('chalk');
const ora = require('ora');

const spinner = ora();

// Template paths
const TEMPLATES_DIR = path.join(__dirname, 'templates');
const FIRMWARE_TEMPLATE = path.join(TEMPLATES_DIR, 'firmware');
const MOBILE_TEMPLATE = path.join(TEMPLATES_DIR, 'mobile');

// Colors for output
const colors = {
  success: chalk.green,
  error: chalk.red,
  warning: chalk.yellow,
  info: chalk.blue,
  highlight: chalk.cyan
};

// Main CLI function
async function createBeamLinkApp() {
  console.log(colors.highlight.bold('\n⚡ BeamLink Project Scaffolder\n'));
  
  try {
    // Get project name
    const { projectName } = await inquirer.prompt([
      {
        type: 'input',
        name: 'projectName',
        message: 'What is your project name?',
        default: 'my-beamlink-project',
        validate: (input) => {
          if (!input.trim()) {
            return 'Project name cannot be empty';
          }
          if (!/^[a-zA-Z0-9-_]+$/.test(input)) {
            return 'Project name can only contain letters, numbers, hyphens, and underscores';
          }
          return true;
        }
      }
    ]);

    // Get project options
    const { includeMobile, includeFirmware, deviceType } = await inquirer.prompt([
      {
        type: 'confirm',
        name: 'includeFirmware',
        message: 'Include ESP32 firmware template?',
        default: true
      },
      {
        type: 'confirm',
        name: 'includeMobile',
        message: 'Include React Native mobile app template?',
        default: true
      },
      {
        type: 'list',
        name: 'deviceType',
        message: 'What type of ESP32 device are you using?',
        choices: [
          { name: 'ESP32 DevKitC (Generic)', value: 'esp32dev' },
          { name: 'ESP32-S2', value: 'esp32s2' },
          { name: 'ESP32-S3', value: 'esp32s3' },
          { name: 'ESP32-C3', value: 'esp32c3' },
          { name: 'Custom', value: 'custom' }
        ],
        default: 'esp32dev',
        when: (answers) => answers.includeFirmware
      }
    ]);

    // Create project directory
    const projectPath = path.resolve(projectName);
    
    spinner.start('Creating project structure...');
    
    if (await fs.pathExists(projectPath)) {
      spinner.fail();
      console.log(colors.error(`Directory ${projectName} already exists!`));
      process.exit(1);
    }

    await fs.ensureDir(projectPath);
    spinner.succeed('Project directory created');

    // Copy firmware template
    if (includeFirmware) {
      spinner.start('Setting up ESP32 firmware...');
      
      const firmwarePath = path.join(projectPath, 'firmware');
      await fs.copy(FIRMWARE_TEMPLATE, firmwarePath);
      
      // Update platformio.ini with selected device
      if (deviceType !== 'custom') {
        const platformioPath = path.join(firmwarePath, 'platformio.ini');
        let platformioContent = await fs.readFile(platformioPath, 'utf8');
        platformioContent = platformioContent.replace(/board = .*/, `board = ${deviceType}`);
        await fs.writeFile(platformioPath, platformioContent);
      }
      
      spinner.succeed('ESP32 firmware template ready');
    }

    // Copy mobile template
    if (includeMobile) {
      spinner.start('Setting up React Native mobile app...');
      
      const mobilePath = path.join(projectPath, 'app');
      await fs.copy(MOBILE_TEMPLATE, mobilePath);
      
      // Update package.json with project name
      const packageJsonPath = path.join(mobilePath, 'package.json');
      const packageJson = await fs.readJson(packageJsonPath);
      packageJson.name = projectName;
      await fs.writeJson(packageJsonPath, packageJson, { spaces: 2 });
      
      spinner.succeed('React Native mobile app template ready');
    }

    // Create project README
    spinner.start('Creating project documentation...');
    
    const readmeContent = generateProjectReadme(projectName, includeFirmware, includeMobile, deviceType);
    await fs.writeFile(path.join(projectPath, 'README.md'), readmeContent);
    
    spinner.succeed('Project documentation created');

    // Success message
    console.log(colors.success.bold('\n✅ Project created successfully!\n'));
    
    console.log(colors.info('Next steps:'));
    
    if (includeFirmware) {
      console.log(colors.highlight('📱 ESP32 Firmware:'));
      console.log(`   cd ${projectName}/firmware`);
      console.log('   pio run --target uploadfs  # Upload config');
      console.log('   pio run --target upload     # Upload firmware');
      console.log('   pio device monitor          # View output\n');
    }
    
    if (includeMobile) {
      console.log(colors.highlight('📱 Mobile App:'));
      console.log(`   cd ${projectName}/app`);
      console.log('   npm install');
      console.log('   npx expo run:android  # or npx expo run:ios\n');
    }
    
    console.log(colors.info('📚 Documentation:'));
    console.log(`   See ${projectName}/README.md for detailed instructions`);
    console.log('   Visit https://github.com/yourusername/BeamLink for more examples\n');
    
    console.log(colors.success('Happy coding! 🚀\n'));

  } catch (error) {
    spinner.fail();
    console.log(colors.error(`Error: ${error.message}`));
    process.exit(1);
  }
}

// Generate project-specific README
function generateProjectReadme(projectName, includeFirmware, includeMobile, deviceType) {
  let content = `# ${projectName}

A BeamLink IoT project with ESP32 firmware and React Native mobile app.

## 🚀 Quick Start

`;

  if (includeFirmware) {
    content += `### ESP32 Firmware

\`\`\`bash
cd firmware
pio run --target uploadfs  # Upload config file
pio run --target upload     # Upload firmware
pio device monitor          # View serial output
\`\`\`

**Device Type**: ${deviceType}
**Commands**: \`led:on\`, \`led:off\`, \`led:toggle\`, \`led:status\`, \`info\`, \`ping\`, \`stats\`

`;
  }

  if (includeMobile) {
    content += `### Mobile App

\`\`\`bash
cd app
npm install
npx expo run:android  # or npx expo run:ios
\`\`\`

**Features**: BLE scanning, ESP32 detection, LED control, real-time communication

`;
  }

  content += `## 🔄 Communication Flow

\`\`\`
[Mobile App] ← BLE → [ESP32 Firmware]
     ↓                    ↑
beam.send()          beam.notify()
beam.receive()       beam.onMessage()
\`\`\`

## 📚 Documentation

- **BeamLink Framework**: https://github.com/yourusername/BeamLink
- **ESP32 Library**: See firmware/README.md
- **Mobile App**: See app/README.md

## 🛠️ Development

This project was created with \`create-beamlink-app\` CLI tool.

For development setup and advanced configuration, see the main BeamLink documentation.

---

**Built with ❤️ using BeamLink Framework**
`;

  return content;
}

// CLI setup
program
  .name('create-beamlink-app')
  .description('Scaffold a complete BeamLink IoT project with ESP32 firmware and React Native app')
  .version('1.0.0')
  .action(createBeamLinkApp);

// Handle errors
process.on('unhandledRejection', (error) => {
  console.log(colors.error(`Error: ${error.message}`));
  process.exit(1);
});

// Run CLI
program.parse();
