import type { ReactNode } from 'react'

interface VesselCardProps {
  title: string
  imageSrc: string
  temp?: number
  setTemp?: number
  level?: number
  levelMax?: number
  pwm?: number
  controls?: ReactNode
}

export default function VesselCard({
  title,
  imageSrc,
  temp,
  setTemp,
  level,
  levelMax = 14,
  pwm,
  controls,
}: VesselCardProps) {
  const levelPct = level != null ? Math.min(100, (level / levelMax) * 100) : 0
  const isOffSetpoint = temp != null && setTemp != null && Math.abs(temp - setTemp) > 5
  const tempColor = isOffSetpoint ? '#ed8936' : '#e2e8f0'

  return (
    <div style={styles.card}>
      <div style={styles.title}>{title}</div>

      {/* Vessel image with liquid level overlay */}
      <div style={styles.imageWrap}>
        <img src={imageSrc} alt={title} style={styles.image} />
        <div
          style={{
            ...styles.levelOverlay,
            height: `${levelPct}%`,
          }}
        />
      </div>

      {/* Level bar */}
      <div style={styles.levelRow}>
        <div style={styles.levelBar}>
          <div style={{ ...styles.levelFill, width: `${levelPct}%` }} />
        </div>
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
    position: 'relative',
    width: '100%',
    height: 160,
    borderRadius: 6,
    overflow: 'hidden',
    background: '#111',
  },
  image: {
    width: '100%',
    height: '100%',
    objectFit: 'cover',
    display: 'block',
  },
  levelOverlay: {
    position: 'absolute',
    bottom: 0,
    left: 0,
    right: 0,
    background: 'rgba(66, 153, 225, 0.35)',
    transition: 'height 0.8s ease',
    pointerEvents: 'none',
  },
  levelRow: {
    display: 'flex',
    alignItems: 'center',
    gap: 8,
  },
  levelBar: {
    flex: 1,
    height: 5,
    background: '#2d3748',
    borderRadius: 3,
  },
  levelFill: {
    height: '100%',
    background: '#4299e1',
    borderRadius: 3,
    transition: 'width 0.8s ease',
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
