db = db.getSiblingDB('pillow_db');

db.properties.insertOne({
  property_id: 106,
  title: 'New Apartment',
  price: 180000,
  city: 'Moscow',
  rooms: 2,
  owner: { user_id: 1, username: 'alice' },
  viewings: [],
  created_at: new Date(),
  updated_at: new Date()
});

db.properties.find({ city: 'Moscow' }).pretty();

db.properties.find({ price: { $gt: 300000 } }).pretty();

db.properties.find({ price: { $gte: 200000, $lte: 400000 } }).pretty();

db.properties.find({ 'owner.username': 'alice' }).pretty();

db.properties.updateOne(
  { property_id: 106 },
  { $set: { price: 170000, updated_at: new Date() } }
);

db.properties.updateOne(
  { property_id: 101 },
  {
    $push: {
      viewings: {
        viewing_id: 1003,
        user_id: 1,
        scheduled_time: new Date('2026-05-15T16:00:00Z'),
        status: 'scheduled',
        notes: 'Second viewing',
        created_at: new Date()
      }
    },
    $set: { updated_at: new Date() }
  }
);

db.properties.updateOne(
  { property_id: 101, 'viewings.viewing_id': 1001 },
  { $set: { 'viewings.$.status': 'confirmed', 'viewings.$.updated_at': new Date() } }
);

db.properties.deleteOne({ property_id: 106 });

db.properties.aggregate([
  { $group: { _id: '$city', avgPrice: { $avg: '$price' }, count: { $sum: 1 } } },
  { $sort: { avgPrice: -1 } }
]).forEach(printjson);

db.properties.aggregate([
  { $match: { 'viewings.0': { $exists: true } } },
  { $project: { title: 1, city: 1, price: 1, viewingsCount: { $size: '$viewings' } } }
]).forEach(printjson);

db.properties.aggregate([
  { $unwind: { path: '$viewings', preserveNullAndEmptyArrays: false } },
  { $group: { _id: '$viewings.user_id', totalViewings: { $sum: 1 } } },
  { $sort: { totalViewings: -1 } }
]).forEach(printjson);