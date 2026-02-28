import { useState } from 'react'
import { useSensorData } from './hooks/useSensorData'
import MainDashboard from './components/MainDashboard'
import HLTTab from './components/HLTTab'
import MLTTab from './components/MLTTab'
import BoilTab from './components/BoilTab'
import ChillTab from './components/ChillTab'
import SessionTab from './components/SessionTab'
import RecipesTab from './components/RecipesTab'

const TABS = ['Overview', 'HLT', 'MLT', 'Boil', 'Chill', 'Session', 'Recipes'] as const
type Tab = typeof TABS[number]

export default function App() {
  const [activeTab, setActiveTab] = useState<Tab>('Overview')
  const { latest, history, isConnected } = useSensorData()

  return (
    <div style={styles.shell}>
      <header style={styles.header}>
        <span style={styles.logo}>🍺 myBrew</span>
        <nav style={styles.nav}>
          {TABS.map((tab) => (
            <button
              key={tab}
              style={{ ...styles.tab, ...(activeTab === tab ? styles.tabActive : {}) }}
              onClick={() => setActiveTab(tab)}
            >
              {tab}
            </button>
          ))}
        </nav>
      </header>

      <main style={styles.main}>
        {activeTab === 'Overview' && <MainDashboard latest={latest} isConnected={isConnected} />}
        {activeTab === 'HLT'      && <HLTTab  latest={latest} history={history} />}
        {activeTab === 'MLT'      && <MLTTab  latest={latest} history={history} />}
        {activeTab === 'Boil'     && <BoilTab latest={latest} history={history} />}
        {activeTab === 'Chill'    && <ChillTab latest={latest} history={history} />}
        {activeTab === 'Session'  && <SessionTab />}
        {activeTab === 'Recipes'  && <RecipesTab />}
      </main>
    </div>
  )
}

const styles: Record<string, React.CSSProperties> = {
  shell: {
    display: 'flex',
    flexDirection: 'column',
    height: '100vh',
    background: '#0f1117',
  },
  header: {
    display: 'flex',
    alignItems: 'center',
    gap: 24,
    padding: '0 24px',
    height: 52,
    background: '#161b27',
    borderBottom: '1px solid #2d3748',
    flexShrink: 0,
  },
  logo: {
    fontSize: 18,
    fontWeight: 700,
    color: '#f6c90e',
    letterSpacing: '-0.5px',
  },
  nav: {
    display: 'flex',
    gap: 4,
  },
  tab: {
    padding: '6px 16px',
    borderRadius: 6,
    border: 'none',
    background: 'transparent',
    color: '#718096',
    fontSize: 14,
    fontWeight: 500,
    transition: 'background 0.15s, color 0.15s',
  },
  tabActive: {
    background: '#2d3748',
    color: '#e2e8f0',
  },
  main: {
    flex: 1,
    overflow: 'auto',
    padding: 24,
  },
}
