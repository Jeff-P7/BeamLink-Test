import { logger as createLogger, configLoggerType, transportFunctionType } from "react-native-logs";

type LogLevel = 'ok' | 'info' | 'warn' | 'error' | 'debug';

type LogEntry = {
  t: string;
  level: LogLevel;
  ns: string;
  msg: string;
  payload?: any;
};

type LoggerOptions = {
  msg: string;
  rawMsg: string | null;
  level: {
    severity: number;
    text: LogLevel;
  };
  extension?: string | null;
};

// Simple EventEmitter implementation for React Native
class SimpleEventEmitter {
  private listeners: { [key: string]: Function[] } = {};

  on(event: string, callback: (entry: LogEntry) => void) {
    if (!this.listeners[event]) {
      this.listeners[event] = [];
    }
    this.listeners[event].push(callback);
    return this;
  }

  off(event: string, callback: Function) {
    if (!this.listeners[event]) return this;
    this.listeners[event] = this.listeners[event].filter(cb => cb !== callback);
    return this;
  }

  emit(event: string, ...args: any[]) {
    if (!this.listeners[event]) return false;
    this.listeners[event].forEach(callback => callback(...args));
    return true;
  }
}

// Small event bus so overlay can subscribe to logs
export const LogBus = new SimpleEventEmitter();
// Shape: { level, ns, msg, payload?, t }

const ANSI = {
  reset: "\x1b[0m",
  dim: "\x1b[2m",
  red: "\x1b[31m",
  green: "\x1b[32m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  magenta: "\x1b[35m",
  cyan: "\x1b[36m",
};

const EMOJI = {
  ok: "✅",
  info: "ℹ️",
  warn: "⚠️",
  error: "❌",
  debug: "🔧",
  ble: "📶",
  cfg: "🗂️",
  app: "🧭",
};

const levelStyle: Record<string, { color: string; emoji: string }> = {
  ok:    { color: ANSI.green,  emoji: EMOJI.ok },
  info:  { color: ANSI.cyan,   emoji: EMOJI.info },
  warn:  { color: ANSI.yellow, emoji: EMOJI.warn },
  error: { color: ANSI.red,    emoji: EMOJI.error },
  debug: { color: ANSI.dim,    emoji: EMOJI.debug },
};

const transport: transportFunctionType<any> = (props) => {
  const { msg, rawMsg } = props;
  const ts = new Date().toLocaleTimeString();
  const levelText = props.level.text as LogLevel;
  const style = levelStyle[levelText];
  const domain = props.extension || "";

  // Colored console (Metro supports ANSI in dev)
  if (__DEV__) {
    // eslint-disable-next-line no-console
    console.log(
      `${style?.color || ""}${style?.emoji || ""} [${ts}] ${domain}: ${rawMsg ?? msg}${ANSI.reset}`
    );
  } else {
    // eslint-disable-next-line no-console
    console.log(`[${ts}] ${domain}: ${rawMsg ?? msg}`);
  }

  // Notify overlay
  LogBus.emit("log", {
    t: ts,
    level: levelText,
    ns: domain,
    msg: rawMsg ?? msg
  });
};

// Extended config with custom fields
const config = {
  levels: { ok: 0, info: 1, warn: 2, error: 3, debug: 4 },
  severity: "debug",
  transport,
  dateFormat: "time",
  enabled: true
} as any;

// Create loggers with different extensions
const baseLogger = createLogger.createLogger(config);
export const logBLE = baseLogger.extend("BLE");
export const logCFG = baseLogger.extend("CFG");
export const logAPP = baseLogger.extend("APP");

// Enhanced error logging with file and function info
const getCallerInfo = (): { fileName: string; functionName: string } => {
  const stack = new Error().stack;
  if (!stack) return { fileName: 'unknown', functionName: 'unknown' };
  
  const lines = stack.split('\n');
  // Skip the first 3 lines (Error, getCallerInfo, and the logging function)
  const callerLine = lines[3];
  if (!callerLine) return { fileName: 'unknown', functionName: 'unknown' };
  
  // Extract file name and function from stack trace
  // Format: "at functionName (file:///path/to/file.tsx:line:column)"
  const match = callerLine.match(/at\s+(?:(\w+)\s+\()?.*?([^/\\]+\.(?:ts|tsx|js|jsx)):(\d+):(\d+)/);
  if (match) {
    const functionName = match[1] || 'anonymous';
    const fileName = match[2] || 'unknown';
    return { fileName, functionName };
  }
  
  return { fileName: 'unknown', functionName: 'unknown' };
};

// Enhanced error logging function
export const logError = (message: string, error?: any) => {
  const { fileName, functionName } = getCallerInfo();
  const location = `${fileName}:${functionName}`;
  const fullMessage = `${message} [${location}]`;
  
  if (error) {
    logBLE.error(fullMessage, error);
  } else {
    logBLE.error(fullMessage);
  }
};

// Convenience
export const L = {
  EMOJI,
};
