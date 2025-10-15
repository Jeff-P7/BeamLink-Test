import React, { useEffect, useRef, useState } from "react";
import { Animated, Easing, ScrollView, Text, View } from "react-native";
import { LogBus } from "../utils/logger";

type Entry = {
  t: string; level: "ok" | "info" | "warn" | "error" | "debug"; ns: string; msg: string; payload?: unknown;
};

const colorBadge: Record<Entry["level"], string> = {
  ok: "#16a34a",
  info: "#06b6d4",
  warn: "#eab308",
  error: "#ef4444",
  debug: "#6b7280",
};

export function DevOverlay({ visible = __DEV__ }) {
  const [lines, setLines] = useState<Entry[]>([]);
  const slide = useRef(new Animated.Value(0)).current;
  const scroller = useRef<ScrollView>(null);

  useEffect(() => {
    if (!visible) return;

    const onLog = (e: Entry) => {
      setLines((old) => [...old.slice(-49), e]);
      setTimeout(() => scroller.current?.scrollToEnd({ animated: true }), 0);

      slide.setValue(0);
      Animated.timing(slide, {
        toValue: 1,
        duration: 350,
        easing: Easing.out(Easing.cubic),
        useNativeDriver: true,
      }).start();
    };

    LogBus.on("log", onLog);
    return () => { LogBus.off("log", onLog); };
  }, [slide, visible]);

  if (!visible) return null;

  const translateY = slide.interpolate({ inputRange: [0, 1], outputRange: [30, 0] });
  const opacity = slide.interpolate({ inputRange: [0, 1], outputRange: [0, 1] });

  return (
    <Animated.View
      style={{
        position: "absolute",
        left: 12,
        right: 12,
        bottom: 12,
        opacity,
        transform: [{ translateY }],
      }}
    >
      <View
        style={{
          backgroundColor: "rgba(0,0,0,0.78)",
          borderRadius: 14,
          paddingVertical: 8,
          paddingHorizontal: 10,
          maxHeight: 220,
        }}
      >
        <ScrollView ref={scroller} keyboardShouldPersistTaps="always">
          {lines.map((l, i) => (
            <View key={i} style={{ flexDirection: "row", alignItems: "center", marginBottom: 6 }}>
              <View
                style={{
                  width: 8,
                  height: 8,
                  borderRadius: 8,
                  backgroundColor: colorBadge[l.level] || "#9ca3af",
                  marginRight: 8,
                }}
              />
              <Text style={{ color: "#cbd5e1", fontSize: 12 }}>
                [{l.t}] <Text style={{ color: "#a78bfa" }}>{l.ns}</Text>: {typeof l.msg === 'string' ? l.msg : JSON.stringify(l.msg)}
              </Text>
            </View>
          ))}
        </ScrollView>
      </View>
    </Animated.View>
  );
}
