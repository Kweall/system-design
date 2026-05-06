<<<<<<< HEAD
# System Design

Репозиторий с лабораторными работами по курсу «Архитектура программных систем».

## Структура репозитория

| Директория | Тема | Описание |
|------------|------|----------|
| [01_structurizr](1/) | Документирование | Structurizr DSL, C4-модель, ADR |
| [02_rest](2/) | REST API | Разработка REST API сервиса |
=======
# Домашнее задание 03: PostgreSQL (24 вариант)

REST API сервис для управления недвижимостью с подключением к PostgreSQL.

## Выполненные требования

### Проектирование схемы базы данных

Спроектирована реляционная схема БД для системы управления недвижимостью.

**Таблицы:**

| Таблица | Описание | PK | FK |
|---------|----------|-----|-----|
| users | Пользователи системы | id | - |
| properties | Объекты недвижимости | id | owner_id → users(id) |
| viewings | Просмотры объектов | id | property_id → properties(id), user_id → users(id) |

**Типы данных и ограничения:**

```sql
-- Таблица пользователей
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Таблица объектов недвижимости
CREATE TABLE properties (
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

-- Таблица просмотров
CREATE TABLE viewings (
    id SERIAL PRIMARY KEY,
    property_id INTEGER NOT NULL REFERENCES properties(id) ON DELETE CASCADE,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    scheduled_time TIMESTAMP NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'scheduled' 
        CHECK (status IN ('scheduled', 'confirmed', 'cancelled', 'completed')),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### Создание базы данных

- PostgreSQL 14 запущен в Docker контейнере
- Созданы все таблицы через SQL скрипты
- Добавлены тестовые данные (10 пользователей, 15 объектов, 12 просмотров)

**Запуск PostgreSQL:**
```bash
docker run -d \
  --name pillow-postgres \
  -e POSTGRES_USER=postgres \
  -e POSTGRES_PASSWORD=postgres \
  -e POSTGRES_DB=pillow_db \
  -p 5432:5432 \
  postgres:15-alpine
```

**Применение схемы:**
```bash
docker exec -i pillow-postgres psql -U postgres -d pillow_db < sql/01_schema.sql
docker exec -i pillow-postgres psql -U postgres -d pillow_db < sql/02_data.sql
```

### Создание индексов

Созданы индексы для оптимизации часто выполняемых запросов:

| Индекс | Таблица | Колонки | Назначение |
|--------|---------|---------|------------|
| idx_users_username | users | username | Быстрый поиск при логине |
| idx_users_email | users | email | Быстрый поиск по email |
| idx_properties_city | properties | city | Фильтрация по городу |
| idx_properties_price | properties | price | Фильтрация по цене |
| idx_properties_owner_id | properties | owner_id | Получение объектов пользователя |
| idx_properties_city_price | properties | city, price | Составной фильтр |
| idx_viewings_user_id | viewings | user_id | Получение просмотров пользователя |
| idx_viewings_property_id | viewings | property_id | Получение просмотров объекта |
| idx_viewings_scheduled_time | viewings | scheduled_time | Сортировка по времени |
| idx_viewings_status_time | viewings | status, scheduled_time | Фильтр по статусу и времени |

**Создание индексов:**
```sql
CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
CREATE INDEX IF NOT EXISTS idx_properties_city ON properties(city);
CREATE INDEX IF NOT EXISTS idx_properties_price ON properties(price);
CREATE INDEX IF NOT EXISTS idx_properties_city_price ON properties(city, price);
CREATE INDEX IF NOT EXISTS idx_viewings_user_id ON viewings(user_id);
CREATE INDEX IF NOT EXISTS idx_viewings_status_time ON viewings(status, scheduled_time);
```

### Оптимизация запросов

**Запрос 1: Поиск объектов по городу и цене**

До оптимизации (без индексов):
```
Seq Scan on properties (cost=0.00..1.00 rows=10 width=...)
```

После создания индексов idx_properties_city_price:
```
Index Scan using idx_properties_city_price on properties 
(cost=0.00..8.27 rows=1 width=...)
Index Cond: ((city = 'Moscow'::text) AND (price >= 200000) AND (price <= 300000))
```

**Запрос 2: Получение просмотров пользователя**

До оптимизации:
```
Seq Scan on viewings (cost=0.00..1.00 rows=10 width=...)
Filter: (user_id = 1)
```

После создания индекса idx_viewings_user_id:
```
Index Scan using idx_viewings_user_id on viewings 
(cost=0.00..8.27 rows=1 width=...)
Index Cond: (user_id = 1)
```

**Запрос 3: Получение объектов пользователя с JOIN**

```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT p.id, p.title, p.price, p.city, p.rooms, u.username as owner_name
FROM properties p
JOIN users u ON p.owner_id = u.id
WHERE p.owner_id = 1
ORDER BY p.created_at DESC;
```

План выполнения использует индекс idx_properties_owner_id для быстрого поиска.

### Подключение API к базе данных

API переписан для работы с PostgreSQL вместо in-memory хранилища.

**Основные изменения:**
- Каждый хендлер получает `pg_cluster_` из компонента Postgres
- SQL запросы выполняются через `pg_cluster_->Execute()`
- Аутентификация через JWT-like токены с проверкой в БД
- Валидация данных перед вставкой

**Пример хендлера для создания объекта:**
```cpp
auto result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "INSERT INTO properties (title, description, price, city, rooms, owner_id) "
    "VALUES ($1, $2, $3, $4, $5, $6) RETURNING id, created_at",
    title, description, price, city, rooms, user_id);
```

## Технологии

- C++
- Yandex Userver
- PostgreSQL 14
- Docker

## Запуск проекта

### Через Docker Compose

```bash
docker-compose up --build
```

### Локальный запуск

```bash
# Запуск PostgreSQL
service postgresql start
sudo -u postgres psql -d pillow_db -f sql/01_schema.sql
sudo -u postgres psql -d pillow_db -f sql/02_data.sql

# Сборка API
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Запуск
./pillow-api --config ../config/config.yaml
```


## Структура проекта

```
pillow-api/
├── src/
│   ├── handlers/           # HTTP хендлеры
│   └── main.cpp            # Точка входа
├── config/
│   └── config.yaml         # Конфигурация
├── sql/
│   ├── 01_schema.sql       # Создание таблиц и индексов
│   ├── 02_data.sql         # Тестовые данные
│   └── 03_queries.sql      # Примеры запросов с EXPLAIN
├── tests/                  # Интеграционные тесты
├── openapi.yaml            # OpenAPI спецификация
├── Dockerfile
├── docker-compose.yaml
└── README.md
```
>>>>>>> 00f80b6a8be36ba1ffea8b9c9dbf60334c27323c
