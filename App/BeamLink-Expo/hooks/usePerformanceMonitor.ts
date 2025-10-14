import { useRef, useCallback, useEffect } from 'react';

export interface PerformanceMetrics {
  renderCount: number;
  lastRenderTime: number;
  averageRenderTime: number;
  memoryUsage?: number;
}

export const usePerformanceMonitor = (componentName: string) => {
  const renderCount = useRef(0);
  const renderTimes = useRef<number[]>([]);
  const lastRenderTime = useRef<number>(0);
  const startTime = useRef<number>(0);

  const startRender = useCallback(() => {
    startTime.current = performance.now();
  }, []);

  const endRender = useCallback(() => {
    const endTime = performance.now();
    const renderTime = endTime - startTime.current;
    
    renderCount.current += 1;
    lastRenderTime.current = renderTime;
    renderTimes.current.push(renderTime);
    
    // Keep only last 10 render times for average calculation
    if (renderTimes.current.length > 10) {
      renderTimes.current.shift();
    }
    
    // Log performance in development
    if (__DEV__) {
      console.log(`[Performance] ${componentName}:`, {
        renderCount: renderCount.current,
        renderTime: `${renderTime.toFixed(2)}ms`,
        averageRenderTime: `${getAverageRenderTime().toFixed(2)}ms`,
      });
    }
  }, [componentName]);

  const getAverageRenderTime = useCallback(() => {
    if (renderTimes.current.length === 0) return 0;
    return renderTimes.current.reduce((sum, time) => sum + time, 0) / renderTimes.current.length;
  }, []);

  const getMetrics = useCallback((): PerformanceMetrics => {
    return {
      renderCount: renderCount.current,
      lastRenderTime: lastRenderTime.current,
      averageRenderTime: getAverageRenderTime(),
    };
  }, [getAverageRenderTime]);

  const resetMetrics = useCallback(() => {
    renderCount.current = 0;
    renderTimes.current = [];
    lastRenderTime.current = 0;
  }, []);

  // Monitor memory usage in development
  useEffect(() => {
    if (__DEV__ && 'memory' in performance) {
      const memoryInfo = (performance as any).memory;
      console.log(`[Memory] ${componentName}:`, {
        usedJSHeapSize: `${(memoryInfo.usedJSHeapSize / 1024 / 1024).toFixed(2)}MB`,
        totalJSHeapSize: `${(memoryInfo.totalJSHeapSize / 1024 / 1024).toFixed(2)}MB`,
        jsHeapSizeLimit: `${(memoryInfo.jsHeapSizeLimit / 1024 / 1024).toFixed(2)}MB`,
      });
    }
  }, [componentName]);

  return {
    startRender,
    endRender,
    getMetrics,
    resetMetrics,
  };
};