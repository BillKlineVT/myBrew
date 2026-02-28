/**
 * Typed API client for myBrew backend (via api-gateway).
 */

const BASE = '/api'

async function request<T>(
  method: string,
  path: string,
  body?: unknown,
): Promise<T> {
  const res = await fetch(`${BASE}${path}`, {
    method,
    headers: body ? { 'Content-Type': 'application/json' } : undefined,
    body: body ? JSON.stringify(body) : undefined,
  })
  if (!res.ok) {
    const text = await res.text()
    throw new Error(`${method} ${path} → ${res.status}: ${text}`)
  }
  if (res.status === 204) return undefined as T
  return res.json() as Promise<T>
}

const get   = <T>(path: string) => request<T>('GET', path)
const post  = <T>(path: string, body: unknown) => request<T>('POST', path, body)
const put   = <T>(path: string, body: unknown) => request<T>('PUT', path, body)
const patch = <T>(path: string, body: unknown) => request<T>('PATCH', path, body)
const del   = <T>(path: string) => request<T>('DELETE', path)

// ── Types ─────────────────────────────────────────────────────────────────────

export interface SensorReading {
  id?: number
  timestamp?: string
  hlt_temp?: number
  hlt_pwm?: number
  mash_temp?: number
  boil_temp?: number
  chill_temp?: number
  hlt_level?: number
  boil_level?: number
}

export interface GPIOState {
  id?: number
  timestamp?: string
  gpio_18: boolean
  gpio_22: boolean
  gpio_24: boolean
  gpio_27: boolean
}

export interface PIDSettings {
  vessel: string
  set_temp: number
  p_val: number
  i_val: number
  d_val: number
  enabled: boolean
}

export interface BrewerySetting {
  key: string
  value: string
}

export interface BrewSession {
  id?: number
  started_at?: string
  ended_at?: string
  pre_boil_gravity?: string
  mash_ph?: number
  ground_water_temp?: number
}

export interface ChecklistEntry {
  id?: number
  session_id: number
  step_name: string
  completed_at?: string
}

export interface HopAddition {
  id?: number
  recipe_id?: number
  name?: string
  alpha?: number
  amount?: number
  use?: string
  time?: number
}

export interface MashStep {
  id?: number
  recipe_id?: number
  name?: string
  temperature?: number
  duration?: number
  step_number?: number
}

export interface Recipe {
  id?: number
  name?: string
  type?: string
  brewer?: string
  batch_size?: number
  boil_size?: number
  boil_time?: number
  efficiency?: number
  grain_weight?: number
  strike_temp?: number
  pitch_temp?: number
  hop_additions: HopAddition[]
  mash_steps: MashStep[]
}

// ── API calls ─────────────────────────────────────────────────────────────────

export const api = {
  // Sensors
  getSensors: (limit = 100) => get<SensorReading[]>(`/sensors?limit=${limit}`),
  getLatestSensor: () => get<SensorReading>('/sensors/latest'),

  // GPIO
  getGPIO: () => get<GPIOState>('/gpio'),
  setGPIO: (state: Partial<GPIOState>) => put<GPIOState>('/gpio', state),

  // PID
  getPID: (vessel: string) => get<PIDSettings>(`/pid/${vessel}`),
  setPID: (vessel: string, settings: PIDSettings) => put<PIDSettings>(`/pid/${vessel}`, settings),

  // Brewery settings
  getSettings: () => get<BrewerySetting[]>('/settings'),
  setSetting: (key: string, value: string) =>
    put<BrewerySetting>(`/settings/${key}`, { key, value }),

  // Brew sessions
  getSessions: () => get<BrewSession[]>('/sessions'),
  createSession: (session: BrewSession) => post<BrewSession>('/sessions', session),
  endSession: (id: number) => patch<BrewSession>(`/sessions/${id}/end`, {}),
  getChecklist: (sessionId: number) =>
    get<ChecklistEntry[]>(`/sessions/${sessionId}/checklist`),
  completeStep: (sessionId: number, step: string) =>
    post<ChecklistEntry>(`/sessions/${sessionId}/checklist/${encodeURIComponent(step)}`, {}),

  // Recipes
  getRecipes: () => get<Recipe[]>('/recipes'),
  getRecipe: (id: number) => get<Recipe>(`/recipes/${id}`),
  createRecipe: (recipe: Recipe) => post<Recipe>('/recipes', recipe),
  deleteRecipe: (id: number) => del<void>(`/recipes/${id}`),
  importBeerXML: async (file: File): Promise<Recipe> => {
    const form = new FormData()
    form.append('file', file)
    const res = await fetch(`${BASE}/recipes/import`, { method: 'POST', body: form })
    if (!res.ok) throw new Error(`Import failed: ${res.status}`)
    return res.json()
  },
}
