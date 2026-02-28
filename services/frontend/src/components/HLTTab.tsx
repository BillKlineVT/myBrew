import { useState, useEffect } from 'react'
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts'
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { SensorReading, PIDSettings } from '../api/client'

interface Props {
  latest: SensorReading
  history: SensorReading[]
}

export default function HLTTab({ latest, history }: Props) {
  const qc = useQueryClient()
  const { data: pid } = useQuery({
    queryKey: ['pid', 'HLT'],
    queryFn: () => api.getPID('HLT'),
    refetchInterval: 5000,
  })

  const [form, setForm] = useState<PIDSettings>({
    vessel: 'HLT', set_temp: 0, p_val: 2, i_val: 0, d_val: 1, enabled: false,
  })

  useEffect(() => {
    if (pid) setForm(pid)
  }, [pid])

  const mutation = useMutation({
    mutationFn: (s: PIDSettings) => api.setPID('HLT', s),
    onSuccess: () => qc.invalidateQueries({ queryKey: ['pid', 'HLT'] }),
  })

  const chartData = history.slice(-120).map((r, i) => ({
    t: i,
    temp: r.hlt_temp,
    pwm: r.hlt_pwm,
    setpoint: form.set_temp,
  }))

  return (
    <div style={styles.container}>
      <h2 style={styles.h2}>Hot Liquor Tank (HLT)</h2>
      <div style={styles.row}>
        {/* Current readings */}
        <div style={styles.panel}>
          <div style={styles.stat}>
            <span style={styles.statLabel}>Current Temp</span>
            <span style={styles.statVal}>{latest.hlt_temp?.toFixed(1) ?? '—'} °F</span>
          </div>
          <div style={styles.stat}>
            <span style={styles.statLabel}>PWM Output</span>
            <span style={styles.statVal}>{latest.hlt_pwm?.toFixed(1) ?? '—'}%</span>
          </div>
          <div style={styles.stat}>
            <span style={styles.statLabel}>Level</span>
            <span style={styles.statVal}>{latest.hlt_level?.toFixed(1) ?? '—'} gal</span>
          </div>
        </div>

        {/* PID controls */}
        <div style={styles.panel}>
          <h3 style={styles.h3}>PID Settings</h3>
          <label style={styles.label}>
            Setpoint (°F)
            <input style={styles.input} type="number" step="0.5"
              value={form.set_temp}
              onChange={e => setForm(f => ({ ...f, set_temp: +e.target.value }))} />
          </label>
          <label style={styles.label}>
            P
            <input style={styles.input} type="number" step="0.1"
              value={form.p_val}
              onChange={e => setForm(f => ({ ...f, p_val: +e.target.value }))} />
          </label>
          <label style={styles.label}>
            I
            <input style={styles.input} type="number" step="0.01"
              value={form.i_val}
              onChange={e => setForm(f => ({ ...f, i_val: +e.target.value }))} />
          </label>
          <label style={styles.label}>
            D
            <input style={styles.input} type="number" step="0.1"
              value={form.d_val}
              onChange={e => setForm(f => ({ ...f, d_val: +e.target.value }))} />
          </label>
          <label style={{ ...styles.label, flexDirection: 'row', gap: 8, alignItems: 'center' }}>
            <input type="checkbox" checked={form.enabled}
              onChange={e => setForm(f => ({ ...f, enabled: e.target.checked }))} />
            PID Enabled
          </label>
          <button style={styles.btn} onClick={() => mutation.mutate(form)}>
            Apply
          </button>
        </div>
      </div>

      {/* Chart */}
      <div style={styles.chartWrap}>
        <ResponsiveContainer width="100%" height={250}>
          <LineChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" stroke="#2d3748" />
            <XAxis dataKey="t" hide />
            <YAxis yAxisId="temp" domain={['auto', 'auto']} stroke="#fc8181" />
            <YAxis yAxisId="pwm" orientation="right" domain={[0, 100]} stroke="#68d391" />
            <Tooltip contentStyle={{ background: '#1a202c', border: '1px solid #4a5568' }} />
            <Legend />
            <Line yAxisId="temp" type="monotone" dataKey="temp" stroke="#fc8181" dot={false} name="Temp °F" />
            <Line yAxisId="temp" type="monotone" dataKey="setpoint" stroke="#faf089" dot={false} strokeDasharray="4 2" name="Setpoint" />
            <Line yAxisId="pwm"  type="monotone" dataKey="pwm"  stroke="#68d391" dot={false} name="PWM %" />
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
  label: { display: 'flex', flexDirection: 'column', gap: 4, marginBottom: 8, fontSize: 13, color: '#a0aec0' },
  input: { background: '#2d3748', border: '1px solid #4a5568', borderRadius: 4, color: '#e2e8f0', padding: '4px 8px', fontSize: 14 },
  btn: { marginTop: 8, background: '#3182ce', color: '#fff', border: 'none', borderRadius: 6, padding: '8px 20px', cursor: 'pointer', fontSize: 14 },
  chartWrap: { background: '#1e2430', border: '1px solid #2d3748', borderRadius: 8, padding: 16 },
}
