import { useState } from 'react'
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { SensorReading, PIDSettings } from '../api/client'

interface Props {
  latest: SensorReading
  history: SensorReading[]
}

export default function BoilTab({ latest, history }: Props) {
  const qc = useQueryClient()
  const { data: gpio } = useQuery({ queryKey: ['gpio'], queryFn: api.getGPIO, refetchInterval: 2000 })

  const gpioMut = useMutation({
    mutationFn: api.setGPIO,
    onSuccess: () => qc.invalidateQueries({ queryKey: ['gpio'] }),
  })

  const chartData = history.slice(-120).map((r, i) => ({ t: i, temp: r.boil_temp, level: r.boil_level }))

  return (
    <div style={styles.container}>
      <h2 style={styles.h2}>Boil Kettle (BK)</h2>
      <div style={styles.row}>
        <div style={styles.panel}>
          <div style={styles.stat}>
            <span style={styles.statLabel}>Boil Temp</span>
            <span style={styles.statVal}>{latest.boil_temp?.toFixed(1) ?? '—'} °F</span>
          </div>
          <div style={styles.stat}>
            <span style={styles.statLabel}>Level</span>
            <span style={styles.statVal}>{latest.boil_level?.toFixed(1) ?? '—'} gal</span>
          </div>
        </div>
        <div style={styles.panel}>
          <h3 style={styles.h3}>Pump Control</h3>
          <div style={styles.gpioRow}>
            <span style={styles.gpioLabel}>Pump (GPIO 18)</span>
            <button
              style={{ ...styles.btn, background: gpio?.gpio_18 ? '#e53e3e' : '#38a169' }}
              onClick={() => gpioMut.mutate({ ...gpio, gpio_18: !gpio?.gpio_18 })}
            >
              {gpio?.gpio_18 ? 'OFF' : 'ON'}
            </button>
          </div>
          <div style={styles.gpioRow}>
            <span style={styles.gpioLabel}>Stir Motor (GPIO 22)</span>
            <button
              style={{ ...styles.btn, background: gpio?.gpio_22 ? '#e53e3e' : '#38a169' }}
              onClick={() => gpioMut.mutate({ ...gpio, gpio_22: !gpio?.gpio_22 })}
            >
              {gpio?.gpio_22 ? 'OFF' : 'ON'}
            </button>
          </div>
        </div>
      </div>
      <div style={styles.chartWrap}>
        <ResponsiveContainer width="100%" height={250}>
          <LineChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" stroke="#2d3748" />
            <XAxis dataKey="t" hide />
            <YAxis yAxisId="temp" domain={['auto', 'auto']} stroke="#f6ad55" />
            <YAxis yAxisId="lvl" orientation="right" domain={[0, 14]} stroke="#4299e1" />
            <Tooltip contentStyle={{ background: '#1a202c', border: '1px solid #4a5568' }} />
            <Legend />
            <Line yAxisId="temp" type="monotone" dataKey="temp"  stroke="#f6ad55" dot={false} name="Boil Temp °F" />
            <Line yAxisId="lvl"  type="monotone" dataKey="level" stroke="#4299e1" dot={false} name="Level (gal)" />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  container: { padding: 8 },
  h2: { margin: '0 0 16px', fontSize: 20, fontWeight: 600 },
  h3: { margin: '0 0 10px', fontSize: 14, fontWeight: 600 },
  row: { display: 'flex', gap: 16, flexWrap: 'wrap', marginBottom: 16 },
  panel: { background: '#1e2430', border: '1px solid #2d3748', borderRadius: 8, padding: 16, minWidth: 220 },
  stat: { display: 'flex', flexDirection: 'column', marginBottom: 12 },
  statLabel: { fontSize: 11, color: '#718096', textTransform: 'uppercase' },
  statVal: { fontSize: 32, fontWeight: 700, fontFamily: 'monospace', color: '#e2e8f0' },
  gpioRow: { display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: 10 },
  gpioLabel: { fontSize: 13, color: '#a0aec0' },
  btn: { color: '#fff', border: 'none', borderRadius: 6, padding: '6px 16px', cursor: 'pointer', fontSize: 13 },
  chartWrap: { background: '#1e2430', border: '1px solid #2d3748', borderRadius: 8, padding: 16 },
}
