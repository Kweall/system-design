-- Тестовые пользователи (пароль: 'password123' в хэшированном виде)
-- В реальном проекте используйте bcrypt, здесь для демонстрации plain text
INSERT INTO users (username, password_hash, email) VALUES
('alice', 'hash_alice123', 'alice@example.com'),
('bob', 'hash_bob123', 'bob@example.com'),
('charlie', 'hash_charlie123', 'charlie@example.com'),
('diana', 'hash_diana123', 'diana@example.com'),
('eve', 'hash_eve123', 'eve@example.com'),
('frank', 'hash_frank123', 'frank@example.com'),
('grace', 'hash_grace123', 'grace@example.com'),
('henry', 'hash_henry123', 'henry@example.com'),
('ivy', 'hash_ivy123', 'ivy@example.com'),
('jack', 'hash_jack123', 'jack@example.com');

-- Тестовые объекты недвижимости (все rooms от 1 до 10)
INSERT INTO properties (title, description, price, city, rooms, owner_id) VALUES
('Modern Apartment', 'Spacious 2-bedroom apartment in city center', 250000.00, 'Moscow', 2, 1),
('Cozy Studio', 'Perfect for students near university', 150000.00, 'Moscow', 1, 1),
('Luxury Penthouse', 'Panoramic view, 3 bedrooms', 500000.00, 'Moscow', 3, 2),
('Family House', 'Big garden, 4 bedrooms', 350000.00, 'Saint Petersburg', 4, 2),
('Office Space', 'Commercial property in business center', 300000.00, 'Moscow', 2, 3),
('Beach Apartment', 'Near the sea, great view', 200000.00, 'Sochi', 2, 3),
('Mountain Chalet', 'Cozy house in mountains', 280000.00, 'Krasnaya Polyana', 3, 4),
('City Loft', 'Industrial style apartment', 220000.00, 'Moscow', 2, 4),
('Country Cottage', 'Peaceful countryside living', 180000.00, 'Tver', 3, 5),
('Penthouse Suite', 'Luxury living with terrace', 450000.00, 'Saint Petersburg', 3, 5),
('Economy Studio', 'Budget friendly option', 90000.00, 'Moscow', 1, 6),
('Modern Villa', 'High-end finishes, pool', 600000.00, 'Moscow', 5, 6),
('Townhouse', 'Modern townhouse in suburbs', 270000.00, 'Moscow', 3, 7),
('Apartment with View', 'City view, renovated', 210000.00, 'Moscow', 2, 7),
('Small Studio', 'Compact living space', 80000.00, 'Saint Petersburg', 1, 8);

-- Тестовые просмотры (теперь property_id существуют)
INSERT INTO viewings (property_id, user_id, scheduled_time, status, notes) VALUES
(1, 2, '2026-04-10 14:00:00', 'scheduled', 'Interested in the apartment'),
(1, 3, '2026-04-11 11:00:00', 'scheduled', 'Would like to see'),
(2, 4, '2026-04-12 16:00:00', 'confirmed', 'Will bring family'),
(3, 5, '2026-04-13 10:00:00', 'scheduled', NULL),
(4, 6, '2026-04-14 15:00:00', 'cancelled', 'Already found another'),
(5, 7, '2026-04-15 12:00:00', 'scheduled', 'For office use'),
(6, 8, '2026-04-16 14:00:00', 'completed', 'Loved the apartment'),
(7, 9, '2026-04-17 11:00:00', 'scheduled', 'Vacation home'),
(8, 10, '2026-04-18 13:00:00', 'scheduled', NULL),
(9, 1, '2026-04-19 10:00:00', 'confirmed', 'Interested in buying'),
(10, 2, '2026-04-20 15:00:00', 'scheduled', NULL),
(11, 3, '2026-04-21 12:00:00', 'scheduled', 'Student housing');
