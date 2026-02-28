import { useState, useEffect } from 'react'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { SensorReading, PIDSettings } from '../api/client'

interface Props {
  isConnected: boolean
  latest: SensorReading
  hltPID?: PIDSettings
}

function formatElapsed(ms: number): string {
  if (ms < 0) return '--:--:--'
  const totalSec = Math.floor(ms / 1000)
  const h = Math.floor(totalSec / 3600)
  const m = Math.floor((totalSec % 3600) / 60)
  const s = totalSec % 60
  return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`
}

export default function StatusBar({ isConnected, latest, hltPID }: Props) {
  const [now, setNow] = useState(Date.now())
  const qc = useQueryClient()

  // Tick every second
  useEffect(() => {
    const id = setInterval(() => setNow(Date.now()), 1000)
    return () => clearInterval(id)
  }, [])

  const { data: sessions } = useQuery({
    queryKey: ['sessions'],
    queryFn: api.getSessions,
    refetchInterval: 30_000,
  })

  // Active session = most recent session with no ended_at
  const activeSession = sessions?.find((s) => s.ended_at == null) ?? null
  const sessionId = activeSession?.id

  const { data: checklist } = useQuery({
    queryKey: ['checklist', sessionId],
    queryFn: () => api.getChecklist(sessionId!),
    enabled: sessionId != null,
    refetchInterval: 5_000,
  })

  const startMut = useMutation({
    mutationFn: () => api.createSession({}),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['sessions'] }),
  })

  const stopMut = useMutation({
    mutationFn: () => api.endSession(sessionId!),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['sessions'] }),
  })

  // If session ended, count to ended_at; otherwise count to now
  const endTime = activeSession?.ended_at
    ? new Date(activeSession.ended_at).getTime()
    : now
  const elapsedMs = activeSession?.started_at
    ? endTime - new Date(activeSession.started_at).getTime()
    : -1

  const currentStep = checklist?.find((e) => !e.completed_at)?.step_name ?? null

  const isAlarm =
    hltPID?.enabled &&
    latest.hlt_temp != null &&
    hltPID?.set_temp != null &&
    Math.abs(latest.hlt_temp - hltPID.set_temp) > 10

  return (
    <div style={styles.bar}>
      <div style={styles.section}>
        <span style={styles.icon}>⏱</span>
        <span style={styles.mono}>{formatElapsed(elapsedMs)}</span>
      </div>

      <div style={styles.section}>
        <span style={styles.icon}>📋</span>
        <span style={styles.stepText}>
          {currentStep ? `Current step: ${currentStep}` : activeSession ? 'Brew in progress' : 'No active session'}
        </span>
      </div>

      <div style={styles.section}>
        <span style={isAlarm ? styles.alarmBadge : styles.okBadge}>
          {isAlarm ? '🔴 ALARM' : '✅ No Alarm'}
        </span>
        <span style={isConnected ? styles.wsPillGreen : styles.wsPillRed}>
          {isConnected ? '🟢 Live' : '🔴 Offline'}
        </span>

        {activeSession == null ? (
          <button
            style={styles.startBtn}
            onClick={() => startMut.mutate()}
            disabled={startMut.isPending}
          >
            ▶ Start Brew
          </button>
        ) : (
          <button
            style={styles.stopBtn}
            onClick={() => stopMut.mutate()}
            disabled={stopMut.isPending || activeSession.ended_at != null}
          >
            ⏹ End Brew
          </button>
        )}
      </div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  bar: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    background: '#161b27',
    border: '1px solid #2d3748',
    borderRadius: 8,
    padding: '8px 16px',
    marginBottom: 16,
    flexWrap: 'wrap',
    gap: 12,
  },
  section: {
    display: 'flex',
    alignItems: 'center',
    gap: 6,
  },
  icon: {
    fontSize: 14,
  },
  mono: {
    fontFamily: 'monospace',
    fontSize: 16,
    fontWeight: 700,
    color: '#e2e8f0',
    letterSpacing: '0.05em',
  },
  stepText: {
    fontSize: 14,
    color: '#a0aec0',
  },
  alarmBadge: {
    fontSize: 13,
    fontWeight: 600,
    color: '#fc8181',
  },
  okBadge: {
    fontSize: 13,
    color: '#68d391',
  },
  wsPillGreen: {
    fontSize: 12,
    background: '#1a3a2a',
    color: '#68d391',
    border: '1px solid #276749',
    borderRadius: 12,
    padding: '2px 10px',
  },
  wsPillRed: {
    fontSize: 12,
    background: '#3a1a1a',
    color: '#fc8181',
    border: '1px solid #742a2a',
    borderRadius: 12,
    padding: '2px 10px',
  },
  startBtn: {
    fontSize: 12,
    fontWeight: 700,
    background: '#1a3a2a',
    color: '#68d391',
    border: '1px solid #276749',
    borderRadius: 6,
    padding: '4px 12px',
    cursor: 'pointer',
  },
  stopBtn: {
    fontSize: 12,
    fontWeight: 700,
    background: '#3a1a1a',
    color: '#fc8181',
    border: '1px solid #742a2a',
    borderRadius: 6,
    padding: '4px 12px',
    cursor: 'pointer',
  },
}
