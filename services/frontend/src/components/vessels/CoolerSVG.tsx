interface Props {
  levelPct: number  // 0–100
}

export default function CoolerSVG({ levelPct }: Props) {
  const pct = Math.max(0, Math.min(100, levelPct))
  // Inner body fill area: y=52 to y=188 (height=136)
  const innerTop = 52
  const innerHeight = 136
  const fillHeight = (pct / 100) * innerHeight
  const fillY = innerTop + innerHeight - fillHeight

  return (
    <svg
      viewBox="0 0 200 220"
      width="100%"
      height="100%"
      xmlns="http://www.w3.org/2000/svg"
    >
      <defs>
        <clipPath id="cooler-body-clip">
          {/* Inner cooler body */}
          <rect x="28" y="52" width="144" height="136" rx="4" />
        </clipPath>
      </defs>

      {/* Body shadow */}
      <rect x="26" y="50" width="148" height="144" rx="8" fill="#1a202c" />

      {/* Outer body */}
      <rect x="24" y="48" width="152" height="148" rx="8" fill="#2d3748" stroke="#4a5568" strokeWidth="1.5" />

      {/* Inner body */}
      <rect x="28" y="52" width="144" height="136" rx="4" fill="#1a202c" />

      {/* Liquid level fill — clipped to inner body */}
      <rect
        x="28"
        y={fillY}
        width="144"
        height={fillHeight}
        fill="rgba(66,153,225,0.5)"
        clipPath="url(#cooler-body-clip)"
        style={{ transition: 'y 0.8s ease, height 0.8s ease' }}
      />

      {/* Liquid surface shimmer */}
      {pct > 2 && (
        <rect
          x="28"
          y={fillY}
          width="144"
          height="3"
          fill="rgba(144,205,244,0.6)"
          clipPath="url(#cooler-body-clip)"
          style={{ transition: 'y 0.8s ease' }}
        />
      )}

      {/* Lid */}
      <rect x="20" y="38" width="160" height="14" rx="4" fill="#3a4a5e" stroke="#4a5568" strokeWidth="1.5" />
      {/* Lid highlight */}
      <rect x="24" y="40" width="152" height="4" rx="2" fill="rgba(255,255,255,0.08)" />

      {/* Lid latch */}
      <rect x="88" y="33" width="24" height="8" rx="3" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      <rect x="96" y="35" width="8" height="4" rx="1" fill="#2d3748" />

      {/* Left handle */}
      <path d="M16 80 Q6 80 6 92 Q6 104 16 104" fill="none" stroke="#4a5568" strokeWidth="6" strokeLinecap="round" />
      <path d="M16 80 Q6 80 6 92 Q6 104 16 104" fill="none" stroke="#2d3748" strokeWidth="3" strokeLinecap="round" />

      {/* Right handle */}
      <path d="M184 80 Q194 80 194 92 Q194 104 184 104" fill="none" stroke="#4a5568" strokeWidth="6" strokeLinecap="round" />
      <path d="M184 80 Q194 80 194 92 Q194 104 184 104" fill="none" stroke="#2d3748" strokeWidth="3" strokeLinecap="round" />

      {/* Drainage spigot */}
      <rect x="82" y="196" width="36" height="10" rx="3" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      <rect x="94" y="206" width="12" height="8" rx="2" fill="#2d3748" stroke="#4a5568" strokeWidth="1" />
      {/* Spigot handle */}
      <rect x="108" y="198" width="14" height="6" rx="2" fill="#718096" />

      {/* Insulation texture lines */}
      <line x1="32" y1="68" x2="168" y2="68" stroke="#374151" strokeWidth="0.5" strokeDasharray="3,8" />
      <line x1="32" y1="82" x2="168" y2="82" stroke="#374151" strokeWidth="0.5" strokeDasharray="3,8" />
    </svg>
  )
}
