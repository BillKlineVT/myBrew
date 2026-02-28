import { useState, useEffect, useRef } from 'react'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { ChecklistEntry } from '../api/client'

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

function formatDateRange(started: string, ended?: string): string {
  const s = new Date(started).toLocaleString([], { month: 'short', day: 'numeric', hour: '2-digit', minute: '2-digit' })
  if (!ended) return s
  const e = new Date(ended).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
  return `${s} – ${e}`
}

function formatCountdown(ms: number): string {
  if (ms < 0) return '00:00'
  const totalSec = Math.floor(ms / 1000)
  const m = Math.floor(totalSec / 60)
  const s = totalSec % 60
  return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`
}

export default function SessionTab() {
  const [now, setNow] = useState(Date.now())
  const [nameInput, setNameInput] = useState<string | null>(null)
  const firedHopsRef = useRef<Set<number>>(new Set())
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
  const pastSessions = sessions?.filter((s) => s.ended_at != null) ?? []
  const sessionId = activeSession?.id

  // Sync name input when active session changes
  useEffect(() => {
    if (activeSession != null) {
      setNameInput(activeSession.name ?? '')
    }
  }, [activeSession?.id])

  const { data: checklist } = useQuery({
    queryKey: ['checklist', sessionId],
    queryFn: () => api.getChecklist(sessionId!),
    enabled: sessionId != null,
    refetchInterval: 3_000,
  })

  const recipeId = activeSession?.recipe_id
  const { data: recipe } = useQuery({
    queryKey: ['recipe', recipeId],
    queryFn: () => api.getRecipe(recipeId!),
    enabled: recipeId != null,
  })

  const updateNameMut = useMutation({
    mutationFn: (name: string) => api.updateSession(sessionId!, { name }),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['sessions'] }),
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

  // Hop countdown logic
  const boilEntry = checklist?.find((e) => e.step_name === 'Boil')
  const boilStartMs = boilEntry?.completed_at
    ? new Date(boilEntry.completed_at).getTime()
    : null

  const boilHops = recipe?.hop_additions
    .filter((h) => h.use === 'Boil' && h.time != null)
    .sort((a, b) => (b.time ?? 0) - (a.time ?? 0)) ?? []

  // Browser notification effect
  useEffect(() => {
    if (!boilStartMs || !recipe || boilHops.length === 0) return
    if (Notification.permission === 'default') {
      Notification.requestPermission()
    }
    boilHops.forEach((hop) => {
      if (hop.id == null || hop.time == null) return
      const fireAt = boilStartMs + ((recipe.boil_time ?? 60) - hop.time) * 60_000
      const remaining = fireAt - now
      if (remaining <= 0 && remaining > -5_000 && !firedHopsRef.current.has(hop.id)) {
        firedHopsRef.current.add(hop.id)
        if (Notification.permission === 'granted') {
          new Notification('🍺 Hop Addition!', {
            body: `Add ${hop.name} (${hop.amount != null ? (hop.amount * 1000).toFixed(0) + 'g' : '?'}) now!`,
          })
        }
      }
    })
  }, [now, boilStartMs, boilHops, recipe])

  return (
    <div style={styles.page}>
      {/* Active session card */}
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
            {/* Inline name editor */}
            <div style={styles.infoRow}>
              <span style={styles.label}>Name:</span>
              <input
                style={styles.nameInput}
                value={nameInput ?? ''}
                placeholder="Unnamed Brew"
                onChange={(e) => setNameInput(e.target.value)}
                onBlur={() => {
                  if (nameInput !== null && nameInput !== (activeSession.name ?? '')) {
                    updateNameMut.mutate(nameInput)
                  }
                }}
                onKeyDown={(e) => {
                  if (e.key === 'Enter') {
                    ;(e.target as HTMLInputElement).blur()
                  }
                }}
              />
            </div>
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
            {activeSession.recipe_id && recipe && (
              <div style={styles.infoRow}>
                <span style={styles.label}>Recipe:</span>
                <span style={{ ...styles.value, color: '#63b3ed' }}>{recipe.name ?? '—'}</span>
              </div>
            )}
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
              {checklist.map((entry, idx) => (
                <ChecklistRow
                  key={entry.id ?? idx}
                  entry={entry}
                  idx={idx}
                  firstIncompleteIdx={firstIncompleteIdx}
                  onComplete={() => completeMut.mutate(entry.step_name)}
                  isPending={completeMut.isPending}
                />
              ))}
            </div>
          )}
        </div>
      )}

      {/* Hop countdown card */}
      {activeSession != null && recipe != null && boilHops.length > 0 && (
        <div style={styles.card}>
          <div style={styles.cardHeader}>
            <span style={styles.cardTitle}>Hop Additions</span>
          </div>
          {boilStartMs == null ? (
            <p style={styles.emptyState}>Waiting for "Boil" step to be marked complete…</p>
          ) : (
            <div style={styles.stepList}>
              {boilHops.map((hop, i) => {
                if (hop.time == null) return null
                const fireAt = boilStartMs + ((recipe.boil_time ?? 60) - hop.time) * 60_000
                const remaining = fireAt - now
                const isDone = remaining < 0
                const isNow = remaining >= 0 && remaining < 60_000

                return (
                  <div
                    key={hop.id ?? i}
                    style={{
                      ...styles.stepRow,
                      ...(isNow ? styles.hopRowNow : {}),
                      ...(isDone ? styles.hopRowDone : {}),
                    }}
                  >
                    <span style={styles.stepIcon}>
                      {isDone ? '✅' : isNow ? '🔴' : '○'}
                    </span>
                    <span style={{ ...styles.stepName, ...(isDone ? styles.stepNameCompleted : {}) }}>
                      {hop.name ?? '?'}{' '}
                      {hop.amount != null ? `${(hop.amount * 1000).toFixed(0)}g` : ''}
                      {' — '}at {hop.time}min
                    </span>
                    <span style={isDone ? styles.hopDoneLabel : isNow ? styles.hopNowLabel : styles.hopTimer}>
                      {isDone ? 'Done' : isNow ? 'ADD NOW' : formatCountdown(remaining)}
                    </span>
                  </div>
                )
              })}
            </div>
          )}
        </div>
      )}

      {/* Past sessions card */}
      {pastSessions.length > 0 && (
        <div style={styles.card}>
          <div style={styles.cardHeader}>
            <span style={styles.cardTitle}>Past Sessions</span>
          </div>
          <div style={styles.stepList}>
            {pastSessions.map((s) => (
              <div key={s.id} style={styles.pastRow}>
                <span style={styles.pastName}>{s.name ?? 'Unnamed Brew'}</span>
                <span style={styles.pastDate}>
                  {s.started_at ? formatDateRange(s.started_at, s.ended_at) : '—'}
                </span>
              </div>
            ))}
          </div>
        </div>
      )}
    </div>
  )
}

function ChecklistRow({
  entry,
  idx,
  firstIncompleteIdx,
  onComplete,
  isPending,
}: {
  entry: ChecklistEntry
  idx: number
  firstIncompleteIdx: number
  onComplete: () => void
  isPending: boolean
}) {
  const isCompleted = entry.completed_at != null
  const isCurrent = idx === firstIncompleteIdx

  return (
    <div
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
          onClick={onComplete}
          disabled={isPending}
        >
          ✓ Mark Complete
        </button>
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
    flexShrink: 0,
  },
  value: {
    fontSize: 14,
    color: '#e2e8f0',
  },
  nameInput: {
    flex: 1,
    fontSize: 15,
    fontWeight: 600,
    background: 'transparent',
    border: '1px solid transparent',
    borderBottom: '1px solid #4a5568',
    color: '#e2e8f0',
    padding: '2px 4px',
    outline: 'none',
    borderRadius: 0,
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
  hopRowNow: {
    background: '#3a1a1a',
    border: '1px solid #742a2a',
  },
  hopRowDone: {
    opacity: 0.6,
  },
  hopTimer: {
    fontFamily: 'monospace',
    fontSize: 14,
    fontWeight: 700,
    color: '#f6c90e',
    marginLeft: 'auto',
    flexShrink: 0,
  },
  hopNowLabel: {
    fontFamily: 'monospace',
    fontSize: 14,
    fontWeight: 700,
    color: '#fc8181',
    marginLeft: 'auto',
    flexShrink: 0,
  },
  hopDoneLabel: {
    fontSize: 12,
    color: '#68d391',
    marginLeft: 'auto',
    flexShrink: 0,
  },
  pastRow: {
    display: 'flex',
    alignItems: 'center',
    gap: 12,
    padding: '8px 10px',
    borderRadius: 6,
    borderBottom: '1px solid #2d3748',
  },
  pastName: {
    flex: 1,
    fontSize: 14,
    fontWeight: 600,
    color: '#e2e8f0',
  },
  pastDate: {
    fontSize: 12,
    color: '#718096',
  },
}
