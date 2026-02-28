import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'
import type { SensorReading } from '../api/client'

interface Props {
  latest: SensorReading
  history: SensorReading[]
}

export default function MLTTab({ latest, history }: Props) {
  const chartData = history.slice(-120).map((r, i) => ({ t: i, temp: r.mash_temp }))

  return (
    <div style={styles.container}>
      <h2 style={styles.h2}>Mash/Lauter Tun (MLT)</h2>
      <div style={styles.row}>
        <div style={styles.panel}>
          <div style={styles.stat}>
            <span style={styles.statLabel}>Mash Temp</span>
            <span style={styles.statVal}>{latest.mash_temp?.toFixed(1) ?? '—'} °F</span>
          </div>
          <div style={{ marginTop: 8, fontSize: 13, color: '#718096' }}>
            Mash / Vorlauf / Sparge controlled via HLT temperature and pump (GPIO 18).
          </div>
        </div>
      </div>
      <div style={styles.chartWrap}>
        <ResponsiveContainer width="100%" height={250}>
          <LineChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" stroke="#2d3748" />
            <XAxis dataKey="t" hide />
            <YAxis domain={['auto', 'auto']} stroke="#63b3ed" />
            <Tooltip contentStyle={{ background: '#1a202c', border: '1px solid #4a5568' }} />
            <Legend />
            <Line type="monotone" dataKey="temp" stroke="#63b3ed" dot={false} name="Mash Temp °F" />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  container: { padding: 8 },
  h2: { margin: '0 0 16px', fontSize: 20, fontWeight: 600 },
  row: { display: 'flex', gap: 16, flexWrap: 'wrap', marginBottom: 16 },
  panel: { background: '#1e2430', border: '1px solid #2d3748', borderRadius: 8, padding: 16, minWidth: 220 },
  stat: { display: 'flex', flexDirection: 'column', marginBottom: 12 },
  statLabel: { fontSize: 11, color: '#718096', textTransform: 'uppercase' },
  statVal: { fontSize: 32, fontWeight: 700, fontFamily: 'monospace', color: '#e2e8f0' },
  chartWrap: { background: '#1e2430', border: '1px solid #2d3748', borderRadius: 8, padding: 16 },
}
