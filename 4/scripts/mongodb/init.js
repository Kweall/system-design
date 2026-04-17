db = db.getSiblingDB('pillow_db');

db.createCollection('properties', {
  validator: {
    $jsonSchema: {
      bsonType: 'object',
      required: ['property_id', 'title', 'price', 'city', 'rooms', 'owner', 'created_at'],
      properties: {
        property_id: {
          bsonType: 'int',
          description: 'must be an integer and is required'
        },
        title: {
          bsonType: 'string',
          description: 'must be a string and is required'
        },
        description: {
          bsonType: 'string'
        },
        price: {
          bsonType: ['double', 'int'],
          minimum: 0,
          description: 'must be a number >= 0'
        },
        city: {
          bsonType: 'string',
          description: 'must be a string and is required'
        },
        rooms: {
          bsonType: 'int',
          minimum: 1,
          maximum: 10,
          description: 'must be an integer between 1 and 10'
        },
        owner: {
          bsonType: 'object',
          required: ['user_id', 'username'],
          properties: {
            user_id: { bsonType: 'int' },
            username: { bsonType: 'string' }
          }
        },
        viewings: {
          bsonType: 'array',
          items: {
            bsonType: 'object',
            required: ['viewing_id', 'user_id', 'scheduled_time', 'status'],
            properties: {
              viewing_id: { bsonType: 'int' },
              user_id: { bsonType: 'int' },
              scheduled_time: { bsonType: 'date' },
              status: {
                bsonType: 'string',
                enum: ['scheduled', 'confirmed', 'cancelled', 'completed']
              },
              notes: { bsonType: 'string' },
              created_at: { bsonType: 'date' }
            }
          }
        },
        created_at: { bsonType: 'date' },
        updated_at: { bsonType: 'date' }
      }
    }
  }
});

db.properties.createIndex({ property_id: 1 }, { unique: true });
db.properties.createIndex({ city: 1 });
db.properties.createIndex({ price: 1 });
db.properties.createIndex({ city: 1, price: 1 });
db.properties.createIndex({ 'owner.user_id': 1 });

db.createCollection('users', {
  validator: {
    $jsonSchema: {
      bsonType: 'object',
      required: ['user_id', 'username', 'email', 'created_at'],
      properties: {
        user_id: { bsonType: 'int' },
        username: { bsonType: 'string' },
        email: { bsonType: 'string' },
        created_at: { bsonType: 'date' }
      }
    }
  }
});

db.users.createIndex({ user_id: 1 }, { unique: true });
db.users.createIndex({ username: 1 });

print('MongoDB initialization completed successfully');