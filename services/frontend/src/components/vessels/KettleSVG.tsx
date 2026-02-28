interface Props {
  levelPct: number  // 0–100
}

export default function KettleSVG({ levelPct }: Props) {
  const pct = Math.max(0, Math.min(100, levelPct))
  // Kettle inner fill area: y=42 to y=178 (height=136)
  const innerTop = 42
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
        {/* Trapezoidal clip for kettle body */}
        <clipPath id="kettle-body-clip">
          <polygon points="30,178 20,42 180,42 170,178" />
        </clipPath>
      </defs>

      {/* Kettle body shadow */}
      <polygon points="32,180 22,44 178,44 168,180" fill="#1a202c" />

      {/* Kettle body (trapezoidal — slightly wider at top) */}
      <polygon
        points="30,178 20,42 180,42 170,178"
        fill="#2d3748"
        stroke="#4a5568"
        strokeWidth="1.5"
        strokeLinejoin="round"
      />

      {/* Inner surface */}
      <polygon points="34,175 25,46 175,46 166,175" fill="#1a202c" />

      {/* Liquid level fill — clipped to kettle body */}
      <rect
        x="15"
        y={fillY}
        width="170"
        height={fillHeight + 10}
        fill="rgba(66,153,225,0.5)"
        clipPath="url(#kettle-body-clip)"
        style={{ transition: 'y 0.8s ease, height 0.8s ease' }}
      />

      {/* Liquid surface shimmer */}
      {pct > 2 && (
        <rect
          x="15"
          y={fillY}
          width="170"
          height="3"
          fill="rgba(144,205,244,0.6)"
          clipPath="url(#kettle-body-clip)"
          style={{ transition: 'y 0.8s ease' }}
        />
      )}

      {/* Lid rim */}
      <ellipse cx="100" cy="42" rx="83" ry="8" fill="#3a4a5e" stroke="#4a5568" strokeWidth="1.5" />
      <ellipse cx="100" cy="40" rx="83" ry="6" fill="#4a5568" stroke="#718096" strokeWidth="1" />

      {/* Lid */}
      <ellipse cx="100" cy="35" rx="70" ry="8" fill="#3a4a5e" stroke="#4a5568" strokeWidth="1.5" />
      <ellipse cx="100" cy="33" rx="68" ry="6" fill="#2d3748" />
      {/* Lid highlight */}
      <ellipse cx="100" cy="30" rx="40" ry="3" fill="rgba(255,255,255,0.07)" />

      {/* Lid knob */}
      <rect x="91" y="22" width="18" height="12" rx="5" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      <rect x="95" y="19" width="10" height="6" rx="3" fill="#3a4a5e" />

      {/* Left handle bracket */}
      <rect x="6" y="80" width="16" height="5" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      {/* Left handle loop */}
      <path d="M8 82 Q-4 82 -4 96 Q-4 110 8 110" fill="none" stroke="#718096" strokeWidth="5" strokeLinecap="round" />
      <path d="M8 82 Q-4 82 -4 96 Q-4 110 8 110" fill="none" stroke="#4a5568" strokeWidth="2.5" strokeLinecap="round" />
      <rect x="6" y="107" width="16" height="5" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />

      {/* Right handle bracket */}
      <rect x="178" y="80" width="16" height="5" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      {/* Right handle loop */}
      <path d="M192 82 Q204 82 204 96 Q204 110 192 110" fill="none" stroke="#718096" strokeWidth="5" strokeLinecap="round" />
      <path d="M192 82 Q204 82 204 96 Q204 110 192 110" fill="none" stroke="#4a5568" strokeWidth="2.5" strokeLinecap="round" />
      <rect x="178" y="107" width="16" height="5" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />

      {/* Bottom ball valve */}
      <rect x="88" y="178" width="24" height="8" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />
      <circle cx="100" cy="190" r="8" fill="#3a4a5e" stroke="#718096" strokeWidth="1.5" />
      <circle cx="100" cy="190" r="4" fill="#2d3748" />
      {/* Valve handle */}
      <rect x="96" y="196" width="8" height="10" rx="2" fill="#4a5568" stroke="#718096" strokeWidth="1" />

      {/* Weld seam */}
      <line x1="30" y1="178" x2="20" y2="42" stroke="#374151" strokeWidth="2" strokeDasharray="4,6" opacity="0.4" />
      <line x1="170" y1="178" x2="180" y2="42" stroke="#374151" strokeWidth="2" strokeDasharray="4,6" opacity="0.4" />
    </svg>
  )
}
