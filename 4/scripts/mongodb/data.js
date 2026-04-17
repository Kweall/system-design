db = db.getSiblingDB('pillow_db');

db.properties.deleteMany({});
db.users.deleteMany({});

db.users.insertMany([
  { user_id: 1, username: 'alice', email: 'alice@example.com', created_at: new Date() },
  { user_id: 2, username: 'bob', email: 'bob@example.com', created_at: new Date() },
  { user_id: 3, username: 'charlie', email: 'charlie@example.com', created_at: new Date() },
  { user_id: 4, username: 'diana', email: 'diana@example.com', created_at: new Date() },
  { user_id: 5, username: 'eve', email: 'eve@example.com', created_at: new Date() }
]);

db.properties.insertMany([
  {
    property_id: 101,
    title: 'Modern Apartment',
    description: 'Spacious 2-bedroom apartment in city center',
    price: 250000,
    city: 'Moscow',
    rooms: 2,
    owner: { user_id: 1, username: 'alice' },
    viewings: [
      {
        viewing_id: 1001,
        user_id: 2,
        scheduled_time: new Date('2026-05-10T14:00:00Z'),
        status: 'scheduled',
        notes: 'Interested in the apartment',
        created_at: new Date()
      }
    ],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 102,
    title: 'Cozy Studio',
    description: 'Perfect for students near university',
    price: 150000,
    city: 'Moscow',
    rooms: 1,
    owner: { user_id: 1, username: 'alice' },
    viewings: [
      {
        viewing_id: 1002,
        user_id: 3,
        scheduled_time: new Date('2026-05-12T11:00:00Z'),
        status: 'confirmed',
        notes: 'Will bring family',
        created_at: new Date()
      }
    ],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 103,
    title: 'Luxury Penthouse',
    description: 'Panoramic city view, 3 bedrooms',
    price: 500000,
    city: 'Moscow',
    rooms: 3,
    owner: { user_id: 2, username: 'bob' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 104,
    title: 'Family House',
    description: 'Big garden, 4 bedrooms, quiet area',
    price: 350000,
    city: 'Saint Petersburg',
    rooms: 4,
    owner: { user_id: 2, username: 'bob' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 105,
    title: 'Beach Apartment',
    description: 'Near the sea, great view',
    price: 200000,
    city: 'Sochi',
    rooms: 2,
    owner: { user_id: 3, username: 'charlie' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 106,
    title: 'Mountain Chalet',
    description: 'Cozy house in mountains, fireplace',
    price: 280000,
    city: 'Krasnaya Polyana',
    rooms: 3,
    owner: { user_id: 3, username: 'charlie' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 107,
    title: 'City Loft',
    description: 'Industrial style apartment',
    price: 220000,
    city: 'Moscow',
    rooms: 2,
    owner: { user_id: 4, username: 'diana' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 108,
    title: 'Country Cottage',
    description: 'Peaceful countryside living',
    price: 180000,
    city: 'Tver',
    rooms: 3,
    owner: { user_id: 4, username: 'diana' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 109,
    title: 'Penthouse Suite',
    description: 'Luxury living with terrace',
    price: 450000,
    city: 'Saint Petersburg',
    rooms: 3,
    owner: { user_id: 5, username: 'eve' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 110,
    title: 'Economy Studio',
    description: 'Budget friendly option',
    price: 90000,
    city: 'Moscow',
    rooms: 1,
    owner: { user_id: 5, username: 'eve' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 111,
    title: 'Modern Villa',
    description: 'High-end finishes, pool, gym',
    price: 600000,
    city: 'Moscow',
    rooms: 5,
    owner: { user_id: 1, username: 'alice' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 112,
    title: 'Townhouse',
    description: 'Modern townhouse in suburbs',
    price: 270000,
    city: 'Moscow',
    rooms: 3,
    owner: { user_id: 2, username: 'bob' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 113,
    title: 'Apartment with View',
    description: 'City view, newly renovated',
    price: 210000,
    city: 'Moscow',
    rooms: 2,
    owner: { user_id: 3, username: 'charlie' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 114,
    title: 'Small Studio',
    description: 'Compact living space',
    price: 80000,
    city: 'Saint Petersburg',
    rooms: 1,
    owner: { user_id: 4, username: 'diana' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 115,
    title: 'Riverfront Apartment',
    description: 'Beautiful river view',
    price: 320000,
    city: 'Moscow',
    rooms: 3,
    owner: { user_id: 5, username: 'eve' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 116,
    title: 'Historical Loft',
    description: 'Old building, high ceilings',
    price: 195000,
    city: 'Saint Petersburg',
    rooms: 2,
    owner: { user_id: 1, username: 'alice' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 117,
    title: 'Park View Apartment',
    description: 'Overlooking the central park',
    price: 275000,
    city: 'Moscow',
    rooms: 2,
    owner: { user_id: 2, username: 'bob' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 118,
    title: 'Business Studio',
    description: 'Perfect for remote work',
    price: 125000,
    city: 'Moscow',
    rooms: 1,
    owner: { user_id: 3, username: 'charlie' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 119,
    title: 'Luxury Townhouse',
    description: 'Premium location, garage included',
    price: 550000,
    city: 'Moscow',
    rooms: 4,
    owner: { user_id: 4, username: 'diana' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    property_id: 120,
    title: 'Eco House',
    description: 'Sustainable materials, solar panels',
    price: 380000,
    city: 'Moscow',
    rooms: 3,
    owner: { user_id: 5, username: 'eve' },
    viewings: [],
    created_at: new Date(),
    updated_at: new Date()
  }
]);

print('Users:', db.users.countDocuments());
print('Properties:', db.properties.countDocuments());