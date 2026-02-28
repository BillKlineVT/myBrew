import type { SensorReading } from '../api/client'

interface Props {
  latest: SensorReading
}

function TempCard({ label, value }: { label: string; value?: number }) {
  return (
    <div style={styles.card}>
      <div style={styles.cardLabel}>{label}</div>
      <div style={styles.cardValue}>
        {value != null ? `${value.toFixed(1)} °F` : '—'}
      </div>
    </div>
  )
}

function LevelCard({ label, value }: { label: string; value?: number }) {
  const pct = value != null ? Math.min(100, (value / 14) * 100) : 0
  return (
    <div style={styles.card}>
      <div style={styles.cardLabel}>{label}</div>
      <div style={styles.cardValue}>{value != null ? `${value.toFixed(1)} gal` : '—'}</div>
      <div style={styles.levelBar}>
        <div style={{ ...styles.levelFill, width: `${pct}%` }} />
      </div>
    </div>
  )
}

export default function MainDashboard({ latest }: Props) {
  return (
    <div>
      <h2 style={styles.heading}>Brew Overview</h2>
      <div style={styles.grid}>
        <TempCard label="HLT Temp"   value={latest.hlt_temp} />
        <TempCard label="Mash Temp"  value={latest.mash_temp} />
        <TempCard label="Boil Temp"  value={latest.boil_temp} />
        <TempCard label="Chill Temp" value={latest.chill_temp} />
        <LevelCard label="HLT Level"  value={latest.hlt_level} />
        <LevelCard label="Boil Level" value={latest.boil_level} />
        <div style={styles.card}>
          <div style={styles.cardLabel}>HLT PWM</div>
          <div style={styles.cardValue}>
            {latest.hlt_pwm != null ? `${latest.hlt_pwm.toFixed(1)}%` : '—'}
          </div>
        </div>
      </div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  heading: { margin: '0 0 16px', fontSize: 20, fontWeight: 600 },
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(160px, 1fr))',
    gap: 12,
  },
  card: {
    background: '#1e2430',
    borderRadius: 8,
    padding: '14px 16px',
    border: '1px solid #2d3748',
  },
  cardLabel: { fontSize: 12, color: '#718096', marginBottom: 6, textTransform: 'uppercase' },
  cardValue: { fontSize: 28, fontWeight: 700, color: '#e2e8f0', fontFamily: 'monospace' },
  levelBar: { height: 6, background: '#2d3748', borderRadius: 3, marginTop: 8 },
  levelFill: { height: '100%', background: '#4299e1', borderRadius: 3, transition: 'width 0.5s' },
}
