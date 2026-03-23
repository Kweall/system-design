# Yandex Userver Development Environment

Docker-окружение для разработки с использованием Yandex Userver.

## Описание

Этот проект предоставляет готовое Docker-окружение для разработки на C++ с фреймворком Yandex Userver. Внутри контейнера уже установлены все необходимые зависимости для сборки и запуска сервисов на Userver.

## Структура

```
.
├── Dockerfile              # Образ с Userver и инструментами разработки
├── docker-compose.yaml     # Конфигурация для запуска контейнера
└── pillow-api/             # REST API сервис
```

## Требования

- Docker
- Docker Compose

## Запуск окружения

### 1. Сборка и запуск контейнера

```bash
docker-compose up -d
```

### 2. Вход в контейнер

```bash
docker exec -it userver-dev bash
```

### 3. Проверка работоспособности

После входа в контейнер проверьте, что Userver установлен:

```bash
find /usr -name "*userver*" 2>/dev/null | head -5

g++ --version
cmake --version
```

## Работа с проектами

### Создание нового проекта

Все проекты размещаются в директории `userver-projects/` на хосте. Эта директория монтируется в `/workspace` внутри контейнера.

```bash
# На хосте
cd userver-projects
mkdir new-project
cd new-project

# В контейнере
cd /workspace/new-project
```

### Сборка и запуск проекта

Пример для проекта `pillow-api`:

```bash
# Внутри контейнера
cd /workspace/pillow-api

# Сборка
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Запуск
./pillow-api --config ../config/config.yaml
```

### Тестирование

```bash
# В другом терминале на хосте
curl http://localhost:8080/ping

# Или внутри контейнера
curl http://localhost:8080/ping
```

## Проброшенные порты

| Порт | Назначение |
|------|------------|
| 8080 | Основной HTTP сервер |
| 8081 | Дополнительные сервисы |

## Монтируемые тома

| Путь в контейнере | Назначение |
|-------------------|------------|
| `/workspace` | Директория с проектами (монтируется `./projects`) |
| `/root/.bash_history` | История команд bash |
| `/root/.cache` | Кэш сборки |
