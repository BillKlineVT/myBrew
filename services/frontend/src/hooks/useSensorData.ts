import { useState, useCallback, useEffect } from 'react'
import { useWebSocket } from './useWebSocket'
import { api } from '../api/client'
import type { SensorReading } from '../api/client'

const MAX_HISTORY = 300  // ~5 min at 1 s/sample

export function useSensorData() {
  const [latest, setLatest] = useState<SensorReading>({})
  const [history, setHistory] = useState<SensorReading[]>([])

  // Seed with latest reading on mount so data shows immediately
  useEffect(() => {
    api.getLatestSensor().then((r) => {
      if (r) setLatest(r)
    }).catch(() => {})
  }, [])

  const handleMessage = useCallback((data: unknown) => {
    const reading = data as SensorReading
    setLatest(reading)
    setHistory((prev) => {
      const next = [...prev, { ...reading, timestamp: reading.timestamp ?? new Date().toISOString() }]
      return next.length > MAX_HISTORY ? next.slice(next.length - MAX_HISTORY) : next
    })
  }, [])

  const { isConnected } = useWebSocket(handleMessage)

  return { latest, history, isConnected }
}
