import { useRef, useState } from 'react'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { Recipe } from '../api/client'

export default function RecipesTab() {
  const qc = useQueryClient()
  const fileRef = useRef<HTMLInputElement>(null)
  const [selectedId, setSelectedId] = useState<number | null>(null)

  const { data: recipes = [], isLoading } = useQuery({
    queryKey: ['recipes'],
    queryFn: api.getRecipes,
  })

  const { data: recipe } = useQuery({
    queryKey: ['recipe', selectedId],
    queryFn: () => api.getRecipe(selectedId!),
    enabled: selectedId != null,
  })

  const { data: sessions } = useQuery({
    queryKey: ['sessions'],
    queryFn: api.getSessions,
  })

  const activeSession = sessions?.find((s) => s.ended_at == null) ?? null

  const importMut = useMutation({
    mutationFn: api.importBeerXML,
    onSuccess: (imported) => {
      qc.invalidateQueries({ queryKey: ['recipes'] })
      if (imported.id != null) setSelectedId(imported.id)
    },
  })

  const deleteMut = useMutation({
    mutationFn: api.deleteRecipe,
    onSuccess: (_data, id) => {
      qc.invalidateQueries({ queryKey: ['recipes'] })
      if (selectedId === id) setSelectedId(null)
    },
  })

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0]
    if (file) importMut.mutate(file)
    e.target.value = ''
  }

  return (
    <div style={styles.container}>
      {/* Left panel: recipe list */}
      <div style={styles.leftPanel}>
        <div style={styles.panelHeader}>
          <span style={styles.panelTitle}>Recipes</span>
          <button
            style={styles.importBtn}
            onClick={() => fileRef.current?.click()}
            disabled={importMut.isPending}
          >
            {importMut.isPending ? 'Importing…' : '📂 Import BeerXML'}
          </button>
          <input
            ref={fileRef}
            type="file"
            accept=".xml"
            style={{ display: 'none' }}
            onChange={handleFileChange}
          />
        </div>

        {importMut.isError && (
          <div style={styles.errorMsg}>Import failed: {String(importMut.error)}</div>
        )}

        {isLoading && <div style={styles.emptyMsg}>Loading…</div>}
        {!isLoading && recipes.length === 0 && (
          <div style={styles.emptyMsg}>No recipes yet. Import a BeerXML file to get started.</div>
        )}

        <div style={styles.recipeList}>
          {recipes.map((r) => (
            <div
              key={r.id}
              style={{
                ...styles.recipeRow,
                ...(selectedId === r.id ? styles.recipeRowActive : {}),
              }}
              onClick={() => setSelectedId(r.id ?? null)}
            >
              <span style={styles.recipeName}>{r.name ?? 'Unnamed'}</span>
              <span style={styles.recipeType}>{r.type ?? ''}</span>
              <button
                style={styles.deleteBtn}
                onClick={(e) => {
                  e.stopPropagation()
                  if (r.id != null) deleteMut.mutate(r.id)
                }}
                title="Delete recipe"
              >
                ✕
              </button>
            </div>
          ))}
        </div>
      </div>

      {/* Right panel: recipe detail */}
      <div style={styles.rightPanel}>
        {selectedId == null || recipe == null ? (
          <div style={styles.emptyDetail}>
            Select a recipe or import a BeerXML file
          </div>
        ) : (
          <RecipeDetail recipe={recipe} activeSessionId={activeSession?.id ?? null} />
        )}
      </div>
    </div>
  )
}

function RecipeDetail({ recipe, activeSessionId }: { recipe: Recipe; activeSessionId: number | null }) {
  const qc = useQueryClient()
  const [applyMsg, setApplyMsg] = useState<string | null>(null)
  const sortedSteps = [...(recipe.mash_steps ?? [])].sort(
    (a, b) => (a.step_number ?? 0) - (b.step_number ?? 0)
  )

  const canApply =
    activeSessionId != null &&
    recipe.id != null &&
    recipe.strike_temp != null &&
    (recipe.mash_steps ?? []).length > 0

  async function handleApplyToSession() {
    if (!canApply) return
    try {
      await api.updateSession(activeSessionId!, { recipe_id: recipe.id })
      const hlt = await api.getPID('HLT')
      await api.setPID('HLT', { ...hlt, set_temp: recipe.strike_temp!, enabled: true })
      const mlt = await api.getPID('MLT')
      await api.setPID('MLT', { ...mlt, set_temp: sortedSteps[0].temperature!, enabled: true })
      qc.invalidateQueries({ queryKey: ['sessions'] })
      qc.invalidateQueries({ queryKey: ['pid', 'HLT'] })
      qc.invalidateQueries({ queryKey: ['pid', 'MLT'] })
      setApplyMsg('Recipe applied to session ✓')
      setTimeout(() => setApplyMsg(null), 3000)
    } catch (e) {
      setApplyMsg(`Error: ${e}`)
      setTimeout(() => setApplyMsg(null), 4000)
    }
  }

  return (
    <div style={styles.detail}>
      {/* Header stats */}
      <div style={styles.detailHeader}>
        <div style={styles.detailHeaderRow}>
          <div>
            <div style={styles.detailName}>{recipe.name ?? 'Unnamed Recipe'}</div>
            {recipe.brewer && <div style={styles.detailBrewer}>by {recipe.brewer}</div>}
          </div>
          <div style={styles.applyArea}>
            <button
              style={{ ...styles.applyBtn, ...(canApply ? {} : styles.applyBtnDisabled) }}
              onClick={handleApplyToSession}
              disabled={!canApply}
              title={canApply ? 'Apply this recipe to the current brew session' : 'No active session or missing strike temp / mash steps'}
            >
              ▶ Use for Current Session
            </button>
            {applyMsg && (
              <span style={applyMsg.startsWith('Error') ? styles.applyError : styles.applySuccess}>
                {applyMsg}
              </span>
            )}
          </div>
        </div>
      </div>

      <div style={styles.statGrid}>
        <StatCell label="Type" value={recipe.type ?? '—'} />
        <StatCell label="Batch Size" value={recipe.batch_size != null ? `${recipe.batch_size.toFixed(1)} L` : '—'} />
        <StatCell label="Boil Time" value={recipe.boil_time != null ? `${recipe.boil_time} min` : '—'} />
        <StatCell label="Strike Temp" value={recipe.strike_temp != null ? `${recipe.strike_temp.toFixed(1)} °F` : '—'} />
        <StatCell label="Pitch Temp" value={recipe.pitch_temp != null ? `${recipe.pitch_temp.toFixed(1)} °F` : '—'} />
        <StatCell label="Efficiency" value={recipe.efficiency != null ? `${recipe.efficiency.toFixed(0)}%` : '—'} />
      </div>

      {/* Mash Steps */}
      {sortedSteps.length > 0 && (
        <div style={styles.tableSection}>
          <div style={styles.tableTitle}>Mash Steps</div>
          <table style={styles.table}>
            <thead>
              <tr>
                <th style={styles.th}>#</th>
                <th style={styles.th}>Name</th>
                <th style={styles.th}>Temp (°F)</th>
                <th style={styles.th}>Duration (min)</th>
              </tr>
            </thead>
            <tbody>
              {sortedSteps.map((s, i) => (
                <tr key={s.id ?? i} style={i % 2 === 0 ? styles.trEven : styles.trOdd}>
                  <td style={styles.td}>{(s.step_number ?? i) + 1}</td>
                  <td style={styles.td}>{s.name ?? '—'}</td>
                  <td style={styles.td}>{s.temperature != null ? s.temperature.toFixed(1) : '—'}</td>
                  <td style={styles.td}>{s.duration != null ? s.duration.toFixed(0) : '—'}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}

      {/* Hop Additions */}
      {(recipe.hop_additions ?? []).length > 0 && (
        <div style={styles.tableSection}>
          <div style={styles.tableTitle}>Hop Additions</div>
          <table style={styles.table}>
            <thead>
              <tr>
                <th style={styles.th}>Name</th>
                <th style={styles.th}>Alpha %</th>
                <th style={styles.th}>Amount (oz)</th>
                <th style={styles.th}>Use</th>
                <th style={styles.th}>Time (min)</th>
              </tr>
            </thead>
            <tbody>
              {recipe.hop_additions.map((h, i) => (
                <tr key={h.id ?? i} style={i % 2 === 0 ? styles.trEven : styles.trOdd}>
                  <td style={styles.td}>{h.name ?? '—'}</td>
                  <td style={styles.td}>{h.alpha != null ? h.alpha.toFixed(1) : '—'}</td>
                  <td style={styles.td}>{h.amount != null ? h.amount.toFixed(2) : '—'}</td>
                  <td style={styles.td}>{h.use ?? '—'}</td>
                  <td style={styles.td}>{h.time != null ? h.time.toFixed(0) : '—'}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  )
}

function StatCell({ label, value }: { label: string; value: string }) {
  return (
    <div style={styles.statCell}>
      <div style={styles.statLabel}>{label}</div>
      <div style={styles.statValue}>{value}</div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  container: {
    display: 'flex',
    gap: 16,
    height: '100%',
    minHeight: 0,
  },
  leftPanel: {
    width: 260,
    flexShrink: 0,
    background: '#1e2430',
    border: '1px solid #2d3748',
    borderRadius: 10,
    padding: 12,
    display: 'flex',
    flexDirection: 'column',
    gap: 8,
  },
  panelHeader: {
    display: 'flex',
    alignItems: 'center',
    gap: 8,
    flexWrap: 'wrap',
  },
  panelTitle: {
    fontSize: 13,
    fontWeight: 700,
    color: '#a0aec0',
    textTransform: 'uppercase',
    letterSpacing: '0.08em',
    flex: 1,
  },
  importBtn: {
    fontSize: 12,
    fontWeight: 600,
    background: '#2d3748',
    color: '#e2e8f0',
    border: '1px solid #4a5568',
    borderRadius: 6,
    padding: '5px 10px',
    cursor: 'pointer',
    whiteSpace: 'nowrap',
  },
  errorMsg: {
    fontSize: 12,
    color: '#fc8181',
    background: '#3a1a1a',
    border: '1px solid #742a2a',
    borderRadius: 4,
    padding: '6px 8px',
  },
  emptyMsg: {
    fontSize: 13,
    color: '#718096',
    padding: '12px 0',
  },
  recipeList: {
    display: 'flex',
    flexDirection: 'column',
    gap: 2,
    overflowY: 'auto',
    flex: 1,
  },
  recipeRow: {
    display: 'flex',
    alignItems: 'center',
    gap: 6,
    padding: '8px 10px',
    borderRadius: 6,
    cursor: 'pointer',
    background: 'transparent',
    border: '1px solid transparent',
  },
  recipeRowActive: {
    background: '#2d3748',
    border: '1px solid #4a5568',
  },
  recipeName: {
    flex: 1,
    fontSize: 13,
    color: '#e2e8f0',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  recipeType: {
    fontSize: 11,
    color: '#718096',
    whiteSpace: 'nowrap',
  },
  deleteBtn: {
    fontSize: 11,
    background: 'transparent',
    color: '#718096',
    border: 'none',
    cursor: 'pointer',
    padding: '2px 4px',
    borderRadius: 3,
    lineHeight: 1,
  },
  rightPanel: {
    flex: 1,
    background: '#1e2430',
    border: '1px solid #2d3748',
    borderRadius: 10,
    overflow: 'auto',
  },
  emptyDetail: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    height: '100%',
    minHeight: 200,
    fontSize: 14,
    color: '#718096',
  },
  detail: {
    padding: 20,
    display: 'flex',
    flexDirection: 'column',
    gap: 20,
  },
  detailHeader: {
    display: 'flex',
    flexDirection: 'column',
    gap: 4,
  },
  detailHeaderRow: {
    display: 'flex',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    gap: 12,
    flexWrap: 'wrap',
  },
  applyArea: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-end',
    gap: 6,
    flexShrink: 0,
  },
  applyBtn: {
    fontSize: 13,
    fontWeight: 700,
    background: '#1a3a2a',
    color: '#68d391',
    border: '1px solid #276749',
    borderRadius: 6,
    padding: '7px 14px',
    cursor: 'pointer',
    whiteSpace: 'nowrap' as const,
  },
  applyBtnDisabled: {
    background: '#1e2430',
    color: '#4a5568',
    border: '1px solid #2d3748',
    cursor: 'not-allowed',
  },
  applySuccess: {
    fontSize: 12,
    color: '#68d391',
  },
  applyError: {
    fontSize: 12,
    color: '#fc8181',
  },
  detailName: {
    fontSize: 22,
    fontWeight: 700,
    color: '#e2e8f0',
  },
  detailBrewer: {
    fontSize: 14,
    color: '#718096',
  },
  statGrid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(130px, 1fr))',
    gap: 10,
  },
  statCell: {
    background: '#161b27',
    border: '1px solid #2d3748',
    borderRadius: 6,
    padding: '8px 12px',
  },
  statLabel: {
    fontSize: 11,
    color: '#718096',
    textTransform: 'uppercase',
    letterSpacing: '0.06em',
    marginBottom: 4,
  },
  statValue: {
    fontSize: 16,
    fontWeight: 600,
    color: '#e2e8f0',
    fontFamily: 'monospace',
  },
  tableSection: {
    display: 'flex',
    flexDirection: 'column',
    gap: 8,
  },
  tableTitle: {
    fontSize: 13,
    fontWeight: 700,
    color: '#a0aec0',
    textTransform: 'uppercase',
    letterSpacing: '0.08em',
  },
  table: {
    width: '100%',
    borderCollapse: 'collapse',
    fontSize: 13,
  },
  th: {
    textAlign: 'left',
    padding: '6px 10px',
    fontSize: 11,
    color: '#718096',
    textTransform: 'uppercase',
    letterSpacing: '0.06em',
    borderBottom: '1px solid #2d3748',
  },
  td: {
    padding: '7px 10px',
    color: '#e2e8f0',
    fontFamily: 'monospace',
    fontSize: 13,
  },
  trEven: {
    background: '#161b27',
  },
  trOdd: {
    background: 'transparent',
  },
}
