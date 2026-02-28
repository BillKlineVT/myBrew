import { useState, useEffect } from 'react'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'

function formatElapsed(ms: number): string {
  if (ms < 0) return '--:--:--'
  const totalSec = Math.floor(ms / 1000)
  const h = Math.floor(totalSec / 3600)
  const m = Math.floor((totalSec % 3600) / 60)
  const s = totalSec % 60
  return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`
}

function formatTime(iso: string): string {
  return new Date(iso).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' })
}

export default function SessionTab() {
  const [now, setNow] = useState(Date.now())
  const qc = useQueryClient()

  useEffect(() => {
    const id = setInterval(() => setNow(Date.now()), 1000)
    return () => clearInterval(id)
  }, [])

  const { data: sessions } = useQuery({
    queryKey: ['sessions'],
    queryFn: api.getSessions,
    refetchInterval: 10_000,
  })

  const activeSession = sessions?.find((s) => s.ended_at == null) ?? null
  const sessionId = activeSession?.id

  const { data: checklist } = useQuery({
    queryKey: ['checklist', sessionId],
    queryFn: () => api.getChecklist(sessionId!),
    enabled: sessionId != null,
    refetchInterval: 3_000,
  })

  const completeMut = useMutation({
    mutationFn: (step: string) => api.completeStep(sessionId!, step),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['checklist', sessionId] }),
  })

  const endMut = useMutation({
    mutationFn: () => api.endSession(sessionId!),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['sessions'] }),
  })

  const endTime = activeSession?.ended_at
    ? new Date(activeSession.ended_at).getTime()
    : now
  const elapsedMs = activeSession?.started_at
    ? endTime - new Date(activeSession.started_at).getTime()
    : -1

  const completedCount = checklist?.filter((e) => e.completed_at != null).length ?? 0
  const totalCount = checklist?.length ?? 0
  const firstIncompleteIdx = checklist?.findIndex((e) => e.completed_at == null) ?? -1

  return (
    <div style={styles.page}>
      {/* Session info card */}
      <div style={styles.card}>
        <div style={styles.cardHeader}>
          <span style={styles.cardTitle}>Brew Session</span>
        </div>

        {activeSession == null ? (
          <p style={styles.emptyState}>
            No active brew session. Start one from the Overview tab.
          </p>
        ) : (
          <div style={styles.sessionInfo}>
            <div style={styles.infoRow}>
              <span style={styles.label}>Started:</span>
              <span style={styles.value}>
                {activeSession.started_at
                  ? new Date(activeSession.started_at).toLocaleString()
                  : '—'}
              </span>
            </div>
            <div style={styles.infoRow}>
              <span style={styles.label}>Elapsed:</span>
              <span style={styles.elapsed}>{formatElapsed(elapsedMs)}</span>
            </div>
            <button
              style={styles.endBtn}
              onClick={() => endMut.mutate()}
              disabled={endMut.isPending || activeSession.ended_at != null}
            >
              ⏹ End Brew
            </button>
          </div>
        )}
      </div>

      {/* Checklist card */}
      {activeSession != null && (
        <div style={styles.card}>
          <div style={styles.cardHeader}>
            <span style={styles.cardTitle}>Brew Checklist</span>
            <span style={styles.progress}>
              {completedCount} / {totalCount} steps completed
            </span>
          </div>

          {!checklist || checklist.length === 0 ? (
            <p style={styles.emptyState}>No checklist steps found.</p>
          ) : (
            <div style={styles.stepList}>
              {checklist.map((entry, idx) => {
                const isCompleted = entry.completed_at != null
                const isCurrent = idx === firstIncompleteIdx

                return (
                  <div
                    key={entry.id ?? idx}
                    style={{
                      ...styles.stepRow,
                      ...(isCurrent ? styles.stepRowCurrent : {}),
                      ...(isCompleted ? styles.stepRowCompleted : {}),
                    }}
                  >
                    <span style={styles.stepIcon}>
                      {isCompleted ? '✅' : isCurrent ? '→' : '○'}
                    </span>
                    <span
                      style={{
                        ...styles.stepName,
                        ...(isCompleted ? styles.stepNameCompleted : {}),
                        ...(!isCompleted && !isCurrent ? styles.stepNamePending : {}),
                      }}
                    >
                      {entry.step_name}
                    </span>
                    {isCompleted && entry.completed_at && (
                      <span style={styles.completedAt}>{formatTime(entry.completed_at)}</span>
                    )}
                    {isCurrent && (
                      <button
                        style={styles.completeBtn}
                        onClick={() => completeMut.mutate(entry.step_name)}
                        disabled={completeMut.isPending}
                      >
                        ✓ Mark Complete
                      </button>
                    )}
                  </div>
                )
              })}
            </div>
          )}
        </div>
      )}
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  page: {
    display: 'flex',
    flexDirection: 'column',
    gap: 20,
    maxWidth: 700,
  },
  card: {
    background: '#161b27',
    border: '1px solid #2d3748',
    borderRadius: 10,
    padding: '16px 20px',
  },
  cardHeader: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    marginBottom: 14,
  },
  cardTitle: {
    fontSize: 16,
    fontWeight: 700,
    color: '#e2e8f0',
  },
  progress: {
    fontSize: 13,
    color: '#a0aec0',
  },
  emptyState: {
    fontSize: 14,
    color: '#718096',
    margin: 0,
  },
  sessionInfo: {
    display: 'flex',
    flexDirection: 'column',
    gap: 10,
  },
  infoRow: {
    display: 'flex',
    alignItems: 'center',
    gap: 10,
  },
  label: {
    fontSize: 13,
    color: '#718096',
    width: 70,
  },
  value: {
    fontSize: 14,
    color: '#e2e8f0',
  },
  elapsed: {
    fontFamily: 'monospace',
    fontSize: 20,
    fontWeight: 700,
    color: '#f6c90e',
    letterSpacing: '0.05em',
  },
  endBtn: {
    alignSelf: 'flex-start',
    marginTop: 4,
    fontSize: 13,
    fontWeight: 700,
    background: '#3a1a1a',
    color: '#fc8181',
    border: '1px solid #742a2a',
    borderRadius: 6,
    padding: '6px 14px',
    cursor: 'pointer',
  },
  stepList: {
    display: 'flex',
    flexDirection: 'column',
    gap: 2,
  },
  stepRow: {
    display: 'flex',
    alignItems: 'center',
    gap: 10,
    padding: '8px 10px',
    borderRadius: 6,
  },
  stepRowCurrent: {
    background: '#1a2a3a',
    border: '1px solid #2b6cb0',
  },
  stepRowCompleted: {
    opacity: 0.6,
  },
  stepIcon: {
    fontSize: 14,
    width: 20,
    textAlign: 'center',
    flexShrink: 0,
  },
  stepName: {
    flex: 1,
    fontSize: 14,
    color: '#e2e8f0',
  },
  stepNameCompleted: {
    textDecoration: 'line-through',
    color: '#718096',
  },
  stepNamePending: {
    color: '#718096',
  },
  completedAt: {
    fontSize: 12,
    color: '#718096',
    marginLeft: 'auto',
  },
  completeBtn: {
    fontSize: 12,
    fontWeight: 700,
    background: '#1a3a2a',
    color: '#68d391',
    border: '1px solid #276749',
    borderRadius: 6,
    padding: '4px 12px',
    cursor: 'pointer',
    flexShrink: 0,
  },
}
