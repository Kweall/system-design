-- 1. Поиск объектов по городу и цене (основной запрос)
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, title, description, price, city, rooms, owner_id, created_at
FROM properties
WHERE city = 'Moscow' AND price BETWEEN 200000 AND 300000
ORDER BY price;

-- 2. Получение объектов пользователя с JOIN на users
EXPLAIN (ANALYZE, BUFFERS)
SELECT p.id, p.title, p.price, p.city, p.rooms, u.username as owner_name
FROM properties p
JOIN users u ON p.owner_id = u.id
WHERE p.owner_id = 1
ORDER BY p.created_at DESC;

-- 3. Получение просмотров пользователя с информацией об объекте
EXPLAIN (ANALYZE, BUFFERS)
SELECT v.id, v.scheduled_time, v.status, v.notes,
       p.title as property_title, p.city, p.price
FROM viewings v
JOIN properties p ON v.property_id = p.id
WHERE v.user_id = 2 AND v.status = 'scheduled'
ORDER BY v.scheduled_time;

-- 4. Агрегация: количество просмотров по статусам
EXPLAIN (ANALYZE, BUFFERS)
SELECT status, COUNT(*) as count
FROM viewings
GROUP BY status;

-- 5. Агрегация: средняя цена объектов по городу
EXPLAIN (ANALYZE, BUFFERS)
SELECT city, COUNT(*) as properties_count, AVG(price) as avg_price
FROM properties
GROUP BY city
ORDER BY avg_price DESC;

-- 6. Поиск пользователя по username (логин)
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, username, email, created_at
FROM users
WHERE username = 'alice';

-- 7. Создание нового объекта (с проверкой существования пользователя)
BEGIN;
INSERT INTO properties (title, description, price, city, rooms, owner_id)
VALUES ('New Apartment', 'Description', 200000, 'Moscow', 2, 1)
RETURNING id;
COMMIT;

-- 8. Обновление объекта (с проверкой прав)
UPDATE properties
SET price = 230000, updated_at = CURRENT_TIMESTAMP
WHERE id = 1 AND owner_id = 1
RETURNING id;

-- 9. Удаление объекта (с проверкой прав)
DELETE FROM properties
WHERE id = 1 AND owner_id = 1
RETURNING id;

-- 10. Создание просмотра
INSERT INTO viewings (property_id, user_id, scheduled_time, notes)
VALUES (1, 2, '2026-05-01 14:00:00', 'Would like to see')
RETURNING id;