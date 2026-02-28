-- myBrew PostgreSQL Schema
-- Applied at container startup via Alembic/init script

CREATE TABLE IF NOT EXISTS sensor_readings (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    hlt_temp FLOAT,
    hlt_pwm FLOAT,
    mash_temp FLOAT,
    boil_temp FLOAT,
    chill_temp FLOAT,
    hlt_level FLOAT,
    boil_level FLOAT
);

CREATE TABLE IF NOT EXISTS gpio_state (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    gpio_18 BOOLEAN DEFAULT FALSE,  -- pump relay
    gpio_22 BOOLEAN DEFAULT FALSE,  -- stir motor relay
    gpio_24 BOOLEAN DEFAULT FALSE,  -- HLT fill valve relay
    gpio_27 BOOLEAN DEFAULT FALSE   -- reserved
);

CREATE TABLE IF NOT EXISTS pid_settings (
    vessel VARCHAR PRIMARY KEY,     -- 'HLT' or 'BK'
    set_temp FLOAT,
    p_val FLOAT,
    i_val FLOAT,
    d_val FLOAT,
    enabled BOOLEAN DEFAULT FALSE,
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

-- Seed default PID rows
INSERT INTO pid_settings (vessel, set_temp, p_val, i_val, d_val, enabled)
VALUES
    ('HLT', 0.0, 2.0, 0.0, 1.0, FALSE),
    ('BK',  0.0, 2.0, 0.0, 1.0, FALSE)
ON CONFLICT (vessel) DO NOTHING;

CREATE TABLE IF NOT EXISTS brewery_settings (
    key VARCHAR PRIMARY KEY,
    value VARCHAR
);

-- Seed default brewery settings
INSERT INTO brewery_settings (key, value) VALUES
    ('grain_absorption',    '0.125'),
    ('deadspace_hlt',       '0.5'),
    ('deadspace_mash',      '0.5'),
    ('boil_off_rate',       '1.0'),
    ('shrinkage',           '0.04'),
    ('trub_loss',           '0.5'),
    ('hlt_temp_cal',        '0.0'),
    ('mash_temp_cal',       '0.0'),
    ('boil_temp_cal',       '0.0'),
    ('chill_temp_cal',      '0.0')
ON CONFLICT (key) DO NOTHING;

CREATE TABLE IF NOT EXISTS brew_sessions (
    id SERIAL PRIMARY KEY,
    started_at TIMESTAMPTZ DEFAULT NOW(),
    pre_boil_gravity VARCHAR,
    mash_ph FLOAT,
    ground_water_temp FLOAT
);

CREATE TABLE IF NOT EXISTS checklist_entries (
    id SERIAL PRIMARY KEY,
    session_id INTEGER REFERENCES brew_sessions(id) ON DELETE CASCADE,
    step_name VARCHAR NOT NULL,
    completed_at TIMESTAMPTZ
);

CREATE TABLE IF NOT EXISTS recipes (
    id SERIAL PRIMARY KEY,
    name VARCHAR,
    type VARCHAR,
    brewer VARCHAR,
    batch_size FLOAT,
    boil_size FLOAT,
    boil_time FLOAT,
    efficiency FLOAT,
    grain_weight FLOAT,
    strike_temp FLOAT,
    pitch_temp FLOAT,
    xml_content TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS hop_additions (
    id SERIAL PRIMARY KEY,
    recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE,
    name VARCHAR,
    alpha FLOAT,
    amount FLOAT,
    use VARCHAR,
    time FLOAT
);

CREATE TABLE IF NOT EXISTS mash_steps (
    id SERIAL PRIMARY KEY,
    recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE,
    name VARCHAR,
    temperature FLOAT,
    duration FLOAT,
    step_number INTEGER
);

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_sensor_readings_timestamp ON sensor_readings(timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_gpio_state_timestamp ON gpio_state(timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_checklist_session ON checklist_entries(session_id);
CREATE INDEX IF NOT EXISTS idx_hop_recipe ON hop_additions(recipe_id);
CREATE INDEX IF NOT EXISTS idx_mash_recipe ON mash_steps(recipe_id);
