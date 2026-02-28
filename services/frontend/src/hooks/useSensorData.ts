import { useState, useCallback } from 'react'
import { useWebSocket } from './useWebSocket'
import type { SensorReading } from '../api/client'

const MAX_HISTORY = 300  // ~5 min at 1 s/sample

export function useSensorData() {
  const [latest, setLatest] = useState<SensorReading>({})
  const [history, setHistory] = useState<SensorReading[]>([])

  const handleMessage = useCallback((data: unknown) => {
    const reading = data as SensorReading
    setLatest(reading)
    setHistory((prev) => {
      const next = [...prev, { ...reading, timestamp: reading.timestamp ?? new Date().toISOString() }]
      return next.length > MAX_HISTORY ? next.slice(next.length - MAX_HISTORY) : next
    })
  }, [])

  useWebSocket(handleMessage)

  return { latest, history }
}
