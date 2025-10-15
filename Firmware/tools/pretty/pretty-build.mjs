#!/usr/bin/env node
import { spawn } from "node:child_process";
import pc from "picocolors";

// ---- config (adjust env and example paths as needed) ----
const ENV = process.env.PIO_ENV || "esp32dev";
const EXAMPLE = process.env.PIO_EXAMPLE || "examples/led_toggle";
const MONITOR_BAUD = process.env.MONITOR_BAUD || "115200";
// ---------------------------------------------------------

const args = new Set(process.argv.slice(2));
const task = (args.has("--task") && process.argv[process.argv.indexOf("--task")+1]) || "";
const showBanner = args.has("--banner");

const emoji = {
  bolt: "⚡",
  ok: "✅",
  info: "ℹ️",
  warn: "⚠️",
  err: "❌",
  upload: "📤",
  chip: "🧠",
  file: "🗂️",
  plug: "🔌",
  magnify: "🔎",
  test: "🧪",
  build: "🔨",
  flash: "💾",
  monitor: "📺",
  clean: "🧹",
  config: "⚙️",
  wifi: "📶",
  ble: "📡",
  sensor: "🌡️",
  led: "💡",
  rocket: "🚀",
  gear: "⚙️",
  check: "✔️",
  cross: "✖️",
  star: "⭐",
  heart: "❤️",
  fire: "🔥",
  zap: "⚡",
  shield: "🛡️",
  key: "🔑",
  lock: "🔒",
  unlock: "🔓"
};

function step(title) {
  const line = `${pc.bold(pc.cyan(emoji.bolt + " " + title))}`;
  console.log(line);
}

function info(msg) {
  console.log(pc.dim(emoji.info + " " + msg));
}

function ok(msg) {
  console.log(pc.green(emoji.ok + " " + msg));
}

function warn(msg) {
  console.log(pc.yellow(emoji.warn + " " + msg));
}

function err(msg) {
  console.error(pc.red(emoji.err + " " + msg));
}

function success(msg) {
  console.log(pc.bold(pc.green(emoji.star + " " + msg)));
}

function run(cmd, cmdArgs, cwd = EXAMPLE) {
  return new Promise((resolve, reject) => {
    const child = spawn(cmd, cmdArgs, { cwd, stdio: "inherit", shell: true });
    child.on("exit", (code) => code === 0 ? resolve() : reject(new Error(`${cmd} exited ${code}`)));
  });
}

async function doClean() {
  step("Clean build artifacts");
  info(`ENV=${ENV}  DIR=${EXAMPLE}`);
  await run("pio", ["run", "--environment", ENV, "--target", "clean"]);
  ok("Build cleaned");
}

async function doUploadFS() {
  step("Upload filesystem (beam.config.h)");
  info(`ENV=${ENV}  DIR=${EXAMPLE}`);
  await run("pio", ["run", "--environment", ENV, "--target", "uploadfs"]);
  ok("Filesystem uploaded");
}

async function doBuild() {
  step("Build firmware");
  info(`ENV=${ENV}  DIR=${EXAMPLE}`);
  await run("pio", ["run", "--environment", ENV]);
  success("Build succeeded");
}

async function doUploadFW() {
  step("Upload firmware to ESP32");
  info(`ENV=${ENV}  DIR=${EXAMPLE}`);
  await run("pio", ["run", "--environment", ENV, "--target", "upload"]);
  success("Firmware uploaded");
}

async function doMonitor() {
  step("Open serial monitor");
  info(`BAUD=${MONITOR_BAUD}  DIR=${EXAMPLE}`);
  await run("pio", ["device", "monitor", "--baud", MONITOR_BAUD]);
}

async function doTest() {
  step("Run Unity tests");
  info(`ENV=${ENV}  DIR=${EXAMPLE}`);
  await run("pio", ["test", "--environment", ENV]);
  success("Tests passed");
}

async function doAll() {
  console.log(pc.bold(pc.magenta(emoji.rocket + " Starting full deployment pipeline...")));
  console.log("");
  
  await doClean();
  console.log("");
  
  await doBuild();
  console.log("");
  
  await doUploadFW();
  console.log("");
  
  console.log(pc.bold(pc.green(emoji.heart + " Deployment complete! Opening monitor...")));
  console.log("");
  
  await doMonitor();
}

function banner() {
  console.log("");
  console.log(pc.bold(pc.magenta(`${emoji.bolt} BeamLink Firmware Runner`)));
  console.log(pc.gray(`ENV=${ENV}  EXAMPLE=${EXAMPLE}  BAUD=${MONITOR_BAUD}`));
  console.log("");
  console.log(pc.bold("Available commands:"));
  console.log(pc.cyan(`  ${emoji.build}  npm run build      - Build firmware`));
  console.log(pc.cyan(`  ${emoji.flash}  npm run flash:fw    - Upload firmware`));
  console.log(pc.cyan(`  ${emoji.file}  npm run flash:fs    - Upload filesystem`));
  console.log(pc.cyan(`  ${emoji.rocket} npm run flash:all   - Full deployment`));
  console.log(pc.cyan(`  ${emoji.monitor} npm run monitor     - Serial monitor`));
  console.log(pc.cyan(`  ${emoji.test}  npm run test       - Run tests`));
  console.log(pc.cyan(`  ${emoji.clean} npm run clean      - Clean build`));
  console.log("");
  console.log(pc.dim("Override defaults:"));
  console.log(pc.gray(`  PIO_ENV=esp32dev PIO_EXAMPLE=examples/sensor_monitor npm run flash:all`));
  console.log("");
}

(async () => {
  try {
    if (showBanner) { banner(); return; }
    switch (task) {
      case "clean":    await doClean(); break;
      case "uploadfs": await doUploadFS(); break;
      case "build":    await doBuild(); break;
      case "upload":   await doUploadFW(); break;
      case "monitor":  await doMonitor(); break;
      case "test":     await doTest(); break;
      case "all":      await doAll(); break;
      default:
        banner();
        warn("No task provided. Example: `npm run flash:all` or `npm run build`");
    }
  } catch (e) {
    err(e.message);
    process.exit(1);
  }
})();
