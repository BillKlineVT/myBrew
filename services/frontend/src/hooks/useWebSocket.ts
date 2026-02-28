import { useEffect, useRef, useCallback, useState } from 'react'

type MessageHandler = (data: unknown) => void

export function useWebSocket(onMessage: MessageHandler) {
  const ws = useRef<WebSocket | null>(null)
  const reconnectTimer = useRef<ReturnType<typeof setTimeout> | null>(null)
  const handlerRef = useRef(onMessage)
  handlerRef.current = onMessage
  const [isConnected, setIsConnected] = useState(false)

  const connect = useCallback(() => {
    const protocol = window.location.protocol === 'https:' ? 'wss' : 'ws'
    const url = `${protocol}://${window.location.host}/ws`
    const socket = new WebSocket(url)

    socket.onopen = () => {
      console.log('[WS] connected')
      setIsConnected(true)
      if (reconnectTimer.current) {
        clearTimeout(reconnectTimer.current)
        reconnectTimer.current = null
      }
    }

    socket.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data as string)
        handlerRef.current(data)
      } catch {
        // ignore parse errors
      }
    }

    socket.onclose = () => {
      console.log('[WS] disconnected — reconnecting in 3s')
      setIsConnected(false)
      reconnectTimer.current = setTimeout(connect, 3000)
    }

    socket.onerror = () => {
      setIsConnected(false)
      socket.close()
    }

    ws.current = socket
  }, [])

  useEffect(() => {
    connect()
    return () => {
      if (reconnectTimer.current) clearTimeout(reconnectTimer.current)
      ws.current?.close()
    }
  }, [connect])

  return { isConnected }
}
