import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query'
import { api } from '../api/client'
import type { SensorReading, GPIOState } from '../api/client'
import VesselCard from './VesselCard'
import StatusBar from './StatusBar'
import CoolerSVG from './vessels/CoolerSVG'
import KettleSVG from './vessels/KettleSVG'

interface Props {
  latest: SensorReading
  isConnected: boolean
}

function ToggleButton({
  label,
  active,
  onClick,
}: {
  label: string
  active: boolean
  onClick: () => void
}) {
  return (
    <button
      onClick={onClick}
      style={{
        display: 'flex',
        alignItems: 'center',
        gap: 5,
        background: active ? '#2c5f2e' : '#2d3748',
        border: `1px solid ${active ? '#38a169' : '#4a5568'}`,
        borderRadius: 6,
        color: active ? '#68d391' : '#a0aec0',
        fontSize: 12,
        fontWeight: 600,
        padding: '5px 10px',
        cursor: 'pointer',
        transition: 'all 0.15s',
      }}
    >
      <span style={{ fontSize: 8, color: active ? '#68d391' : '#718096' }}>●</span>
      {label}
    </button>
  )
}

export default function MainDashboard({ latest, isConnected }: Props) {
  const qc = useQueryClient()

  const { data: gpio } = useQuery({
    queryKey: ['gpio'],
    queryFn: api.getGPIO,
    refetchInterval: 2000,
  })

  const { data: hltPID } = useQuery({
    queryKey: ['pid', 'HLT'],
    queryFn: () => api.getPID('HLT'),
    refetchInterval: 5000,
  })

  const { data: bkPID } = useQuery({
    queryKey: ['pid', 'BK'],
    queryFn: () => api.getPID('BK'),
    refetchInterval: 5000,
  })

  const gpioMut = useMutation({
    mutationFn: api.setGPIO,
    onSuccess: () => qc.invalidateQueries({ queryKey: ['gpio'] }),
  })

  const pidMut = useMutation({
    mutationFn: ({ vessel, settings }: { vessel: string; settings: Parameters<typeof api.setPID>[1] }) =>
      api.setPID(vessel, settings),
    onSuccess: (_data, vars) => qc.invalidateQueries({ queryKey: ['pid', vars.vessel] }),
  })

  const toggleGPIO = (pin: keyof GPIOState) => {
    if (!gpio) return
    gpioMut.mutate({ ...gpio, [pin]: !gpio[pin] })
  }

  const toggleHLTPID = () => {
    if (!hltPID) return
    pidMut.mutate({ vessel: 'HLT', settings: { ...hltPID, enabled: !hltPID.enabled } })
  }

  return (
    <div style={styles.container}>
      <StatusBar isConnected={isConnected} latest={latest} hltPID={hltPID} />

      <div style={styles.vessels}>
        {/* HLT */}
        <VesselCard
          title="HLT"
          vesselSvg={<CoolerSVG levelPct={(latest.hlt_level ?? 0) / 14 * 100} />}
          temp={latest.hlt_temp}
          setTemp={hltPID?.set_temp}
          level={latest.hlt_level}
          pwm={latest.hlt_pwm}
          controls={
            <>
              <ToggleButton
                label="PID"
                active={hltPID?.enabled ?? false}
                onClick={toggleHLTPID}
              />
              <ToggleButton
                label="Fill"
                active={gpio?.gpio_24 ?? false}
                onClick={() => toggleGPIO('gpio_24')}
              />
            </>
          }
        />

        {/* MLT */}
        <VesselCard
          title="MLT"
          vesselSvg={<CoolerSVG levelPct={(latest.hlt_level ?? 0) / 14 * 100} />}
          temp={latest.mash_temp}
        />

        {/* Boil Kettle */}
        <VesselCard
          title="Boil Kettle"
          vesselSvg={<KettleSVG levelPct={(latest.boil_level ?? 0) / 14 * 100} />}
          temp={latest.boil_temp}
          setTemp={bkPID?.enabled ? bkPID?.set_temp : undefined}
          level={latest.boil_level}
          controls={
            <>
              <ToggleButton
                label="Pump"
                active={gpio?.gpio_18 ?? false}
                onClick={() => toggleGPIO('gpio_18')}
              />
              <ToggleButton
                label="Stir"
                active={gpio?.gpio_22 ?? false}
                onClick={() => toggleGPIO('gpio_22')}
              />
            </>
          }
        />
      </div>

      {/* Chill strip */}
      <div style={styles.chillStrip}>
        <span style={styles.chillIcon}>🌡</span>
        <span style={styles.chillLabel}>Chill:</span>
        <span style={styles.chillVal}>
          {latest.chill_temp != null ? `${latest.chill_temp.toFixed(1)} °F` : '—'}
        </span>
        <span style={styles.chillTarget}>Target: 65–70 °F (ale yeast)</span>
      </div>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  container: {
    display: 'flex',
    flexDirection: 'column',
    gap: 0,
  },
  vessels: {
    display: 'flex',
    gap: 14,
    flexWrap: 'wrap',
    marginBottom: 14,
  },
  chillStrip: {
    display: 'flex',
    alignItems: 'center',
    gap: 8,
    background: '#1e2430',
    border: '1px solid #2d3748',
    borderRadius: 8,
    padding: '10px 16px',
  },
  chillIcon: {
    fontSize: 16,
  },
  chillLabel: {
    fontSize: 13,
    color: '#718096',
    textTransform: 'uppercase',
    fontWeight: 600,
  },
  chillVal: {
    fontSize: 18,
    fontWeight: 700,
    fontFamily: 'monospace',
    color: '#63b3ed',
  },
  chillTarget: {
    fontSize: 13,
    color: '#718096',
    marginLeft: 8,
  },
}
