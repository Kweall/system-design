-- Таблица пользователей
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Индекс для поиска по username (используется при логине)
CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
-- Индекс для поиска по email
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);

-- Таблица объектов недвижимости
CREATE TABLE IF NOT EXISTS properties (
    id SERIAL PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    description TEXT,
    price DECIMAL(12,2) NOT NULL CHECK (price >= 0),
    city VARCHAR(100) NOT NULL,
    rooms INTEGER NOT NULL CHECK (rooms >= 1 AND rooms <= 10),
    owner_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Индекс для поиска по городу (используется в фильтрации)
CREATE INDEX IF NOT EXISTS idx_properties_city ON properties(city);
-- Индекс для поиска по цене
CREATE INDEX IF NOT EXISTS idx_properties_price ON properties(price);
-- Индекс для поиска по владельцу
CREATE INDEX IF NOT EXISTS idx_properties_owner_id ON properties(owner_id);
-- Составной индекс для фильтрации по городу и цене
CREATE INDEX IF NOT EXISTS idx_properties_city_price ON properties(city, price);

-- Таблица просмотров
CREATE TABLE IF NOT EXISTS viewings (
    id SERIAL PRIMARY KEY,
    property_id INTEGER NOT NULL REFERENCES properties(id) ON DELETE CASCADE,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    scheduled_time TIMESTAMP NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'scheduled' CHECK (status IN ('scheduled', 'confirmed', 'cancelled', 'completed')),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Индекс для поиска просмотров пользователя
CREATE INDEX IF NOT EXISTS idx_viewings_user_id ON viewings(user_id);
-- Индекс для поиска просмотров объекта
CREATE INDEX IF NOT EXISTS idx_viewings_property_id ON viewings(property_id);
-- Индекс для поиска по времени
CREATE INDEX IF NOT EXISTS idx_viewings_scheduled_time ON viewings(scheduled_time);
-- Составной индекс для статуса и времени
CREATE INDEX IF NOT EXISTS idx_viewings_status_time ON viewings(status, scheduled_time);

-- Таблица для хранения токенов (опционально, можно заменить на Redis)
CREATE TABLE IF NOT EXISTS tokens (
    id SERIAL PRIMARY KEY,
    token VARCHAR(255) UNIQUE NOT NULL,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_tokens_token ON tokens(token);
CREATE INDEX IF NOT EXISTS idx_tokens_user_id ON tokens(user_id);
CREATE INDEX IF NOT EXISTS idx_tokens_expires_at ON tokens(expires_at);