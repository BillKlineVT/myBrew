import type { ReactNode } from 'react'

interface VesselCardProps {
  title: string
  vesselSvg: ReactNode
  temp?: number
  setTemp?: number
  level?: number
  pwm?: number
  controls?: ReactNode
}

export default function VesselCard({
  title,
  vesselSvg,
  temp,
  setTemp,
  level,
  pwm,
  controls,
}: VesselCardProps) {
  const isOffSetpoint = temp != null && setTemp != null && Math.abs(temp - setTemp) > 5
  const tempColor = isOffSetpoint ? '#ed8936' : '#e2e8f0'

  return (
    <div style={styles.card}>
      <div style={styles.title}>{title}</div>

      {/* SVG vessel illustration with embedded level fill */}
      <div style={styles.imageWrap}>
        {vesselSvg}
      </div>

      {/* Level label */}
      <div style={styles.levelRow}>
        <span style={styles.levelLabel}>
          {level != null ? `${level.toFixed(1)} gal` : '—'}
        </span>
      </div>

      {/* Stats */}
      <div style={styles.stats}>
        <div style={styles.tempBig}>
          <span style={{ ...styles.tempVal, color: tempColor }}>
            {temp != null ? `${temp.toFixed(1)} °F` : '—'}
          </span>
          {setTemp != null && (
            <span style={styles.setTemp}>→ {setTemp.toFixed(1)} set</span>
          )}
        </div>
        {pwm != null && (
          <div style={styles.statRow}>
            <span style={styles.statLabel}>PWM</span>
            <span style={styles.statVal}>{pwm.toFixed(0)}%</span>
          </div>
        )}
      </div>

      {/* Controls slot */}
      {controls && <div style={styles.controls}>{controls}</div>}
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  card: {
    background: '#1e2430',
    border: '1px solid #2d3748',
    borderRadius: 10,
    padding: 14,
    display: 'flex',
    flexDirection: 'column',
    gap: 10,
    flex: 1,
    minWidth: 200,
  },
  title: {
    fontSize: 13,
    fontWeight: 700,
    color: '#a0aec0',
    textTransform: 'uppercase',
    letterSpacing: '0.08em',
  },
  imageWrap: {
    width: '100%',
    height: 160,
  },
  levelRow: {
    display: 'flex',
    alignItems: 'center',
  },
  levelLabel: {
    fontSize: 12,
    color: '#718096',
    whiteSpace: 'nowrap',
  },
  stats: {
    display: 'flex',
    flexDirection: 'column',
    gap: 4,
  },
  tempBig: {
    display: 'flex',
    flexDirection: 'column',
  },
  tempVal: {
    fontSize: 30,
    fontWeight: 700,
    fontFamily: 'monospace',
    lineHeight: 1.1,
  },
  setTemp: {
    fontSize: 12,
    color: '#718096',
    marginTop: 2,
  },
  statRow: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  statLabel: {
    fontSize: 11,
    color: '#718096',
    textTransform: 'uppercase',
  },
  statVal: {
    fontSize: 14,
    color: '#a0aec0',
    fontFamily: 'monospace',
  },
  controls: {
    display: 'flex',
    gap: 8,
    flexWrap: 'wrap',
    marginTop: 4,
  },
}
