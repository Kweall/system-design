db = db.getSiblingDB('pillow_db');

print('Тест валидации');

print('\n1. Insert valid document...');
try {
    db.properties.insertOne({
        property_id: 999,
        title: 'Valid Apartment',
        price: 100000,
        city: 'Moscow',
        rooms: 2,
        owner: { user_id: 1, username: 'alice' },
        viewings: [],
        created_at: new Date(),
        updated_at: new Date()
    });
    print('✅ Valid document inserted');
} catch(e) {
    print('❌ Error:', e);
}

// Невалидный документ (отрицательная цена)
print('\n2. Insert document with negative price...');
try {
    db.properties.insertOne({
        property_id: 998,
        title: 'Invalid Apartment',
        price: -100,
        city: 'Moscow',
        rooms: 2,
        owner: { user_id: 1, username: 'alice' },
        viewings: [],
        created_at: new Date(),
        updated_at: new Date()
    });
    print('❌ Should have failed, but inserted');
} catch(e) {
    print('✅ Correctly rejected:', e.message);
}

// Невалидный документ (rooms > 10)
print('\n3. Insert document with rooms = 15...');
try {
    db.properties.insertOne({
        property_id: 997,
        title: 'Invalid Apartment',
        price: 100000,
        city: 'Moscow',
        rooms: 15,
        owner: { user_id: 1, username: 'alice' },
        viewings: [],
        created_at: new Date(),
        updated_at: new Date()
    });
    print('❌ Should have failed, but inserted');
} catch(e) {
    print('✅ Correctly rejected:', e.message);
}

// Невалидный документ (отсутствует обязательное поле owner)
print('\n4. Insert document without owner...');
try {
    db.properties.insertOne({
        property_id: 996,
        title: 'Invalid Apartment',
        price: 100000,
        city: 'Moscow',
        rooms: 2,
        viewings: [],
        created_at: new Date(),
        updated_at: new Date()
    });
    print('❌ Should have failed, but inserted');
} catch(e) {
    print('✅ Correctly rejected:', e.message);
}

// Очистка тестовых данных
db.properties.deleteMany({ property_id: { $in: [999, 998, 997, 996] } });
print('\n✅ Validation test completed');