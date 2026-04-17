# Домашнее задание 04: MongoDB (24 вариант)

REST API сервис для управления недвижимостью с подключением к MongoDB.

## Выполненные требования

## MongoDB

### Запуск

MongoDB автоматически поднимается в `docker-compose.yaml`. Доступна на порту `27017`.

### Инициализация

При первом запуске выполняются скрипты из `scripts/mongodb/init.js`:
- Создание коллекций `users` и `properties`
- Создание индексов
- Валидация схемы через `$jsonSchema`

### Тестовые данные

```bash
docker exec -i pillow-mongodb mongosh -u admin -p admin123 --authenticationDatabase admin pillow_db < scripts/mongodb/data.js
```

### Запросы и агрегации

```bash
docker exec -i pillow-mongodb mongosh -u admin -p admin123 --authenticationDatabase admin pillow_db < scripts/mongodb/queries.js
```

### API endpoint

- `GET /api/mongo/properties/{id}` — получение объекта недвижимости из MongoDB (со всеми вложенными просмотрами)

Пример:
```bash
curl http://localhost:8082/api/mongo/properties/101
```

### Валидация схемы

```bash
docker exec -i pillow-mongodb mongosh -u admin -p admin123 --authenticationDatabase admin pillow_db < validation.js
```

### Документная модель

Подробное описание с обоснованием выбора embedded/references — в файле `schema_design.md`.
