import Toast from "react-native-toast-message";

export const notify = {
  ok:   (msg: string) => Toast.show({ type: "success", text1: "✅ Success", text2: msg }),
  info: (msg: string) => Toast.show({ type: "info",    text1: "ℹ️ Info",    text2: msg }),
  warn: (msg: string) => Toast.show({ type: "info",    text1: "⚠️ Warning", text2: msg }),
  err:  (msg: string) => Toast.show({ type: "error",   text1: "❌ Error",    text2: msg }),
};
