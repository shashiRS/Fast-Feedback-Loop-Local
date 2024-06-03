// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_EGOVEHICLE_GUISCHEMA_H_
#define FLATBUFFERS_GENERATED_EGOVEHICLE_GUISCHEMA_H_

#include "flatbuffers/flatbuffers.h"

namespace GuiSchema {

struct ShapePoint;
struct ShapePointBuilder;
struct ShapePointT;

struct EgoVehicle;
struct EgoVehicleBuilder;
struct EgoVehicleT;

struct EgoVehicleList;
struct EgoVehicleListBuilder;
struct EgoVehicleListT;

struct ShapePointT : public flatbuffers::NativeTable {
  typedef ShapePoint TableType;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

struct ShapePoint FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ShapePointT NativeTableType;
  typedef ShapePointBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           verifier.EndTable();
  }
  ShapePointT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(ShapePointT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<ShapePoint> Pack(flatbuffers::FlatBufferBuilder &_fbb, const ShapePointT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct ShapePointBuilder {
  typedef ShapePoint Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(ShapePoint::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(ShapePoint::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(ShapePoint::VT_Z, z, 0.0f);
  }
  explicit ShapePointBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ShapePoint> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ShapePoint>(end);
    return o;
  }
};

inline flatbuffers::Offset<ShapePoint> CreateShapePoint(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  ShapePointBuilder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

flatbuffers::Offset<ShapePoint> CreateShapePoint(flatbuffers::FlatBufferBuilder &_fbb, const ShapePointT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct EgoVehicleT : public flatbuffers::NativeTable {
  typedef EgoVehicle TableType;
  float width = 0.0f;
  float length = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  float yaw = 0.0f;
  float velocity = 0.0f;
  float yawRate = 0.0f;
  float height = 0.0f;
  std::vector<std::unique_ptr<GuiSchema::ShapePointT>> shapePoints{};
  EgoVehicleT() = default;
  EgoVehicleT(const EgoVehicleT &o);
  EgoVehicleT(EgoVehicleT&&) FLATBUFFERS_NOEXCEPT = default;
  EgoVehicleT &operator=(EgoVehicleT o) FLATBUFFERS_NOEXCEPT;
};

struct EgoVehicle FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EgoVehicleT NativeTableType;
  typedef EgoVehicleBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_WIDTH = 4,
    VT_LENGTH = 6,
    VT_X = 8,
    VT_Y = 10,
    VT_YAW = 12,
    VT_VELOCITY = 14,
    VT_YAWRATE = 16,
    VT_HEIGHT = 18,
    VT_SHAPEPOINTS = 20
  };
  float width() const {
    return GetField<float>(VT_WIDTH, 0.0f);
  }
  float length() const {
    return GetField<float>(VT_LENGTH, 0.0f);
  }
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float yaw() const {
    return GetField<float>(VT_YAW, 0.0f);
  }
  float velocity() const {
    return GetField<float>(VT_VELOCITY, 0.0f);
  }
  float yawRate() const {
    return GetField<float>(VT_YAWRATE, 0.0f);
  }
  float height() const {
    return GetField<float>(VT_HEIGHT, 0.0f);
  }
  const flatbuffers::Vector<flatbuffers::Offset<GuiSchema::ShapePoint>> *shapePoints() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<GuiSchema::ShapePoint>> *>(VT_SHAPEPOINTS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_WIDTH, 4) &&
           VerifyField<float>(verifier, VT_LENGTH, 4) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_YAW, 4) &&
           VerifyField<float>(verifier, VT_VELOCITY, 4) &&
           VerifyField<float>(verifier, VT_YAWRATE, 4) &&
           VerifyField<float>(verifier, VT_HEIGHT, 4) &&
           VerifyOffset(verifier, VT_SHAPEPOINTS) &&
           verifier.VerifyVector(shapePoints()) &&
           verifier.VerifyVectorOfTables(shapePoints()) &&
           verifier.EndTable();
  }
  EgoVehicleT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(EgoVehicleT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<EgoVehicle> Pack(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct EgoVehicleBuilder {
  typedef EgoVehicle Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_width(float width) {
    fbb_.AddElement<float>(EgoVehicle::VT_WIDTH, width, 0.0f);
  }
  void add_length(float length) {
    fbb_.AddElement<float>(EgoVehicle::VT_LENGTH, length, 0.0f);
  }
  void add_x(float x) {
    fbb_.AddElement<float>(EgoVehicle::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(EgoVehicle::VT_Y, y, 0.0f);
  }
  void add_yaw(float yaw) {
    fbb_.AddElement<float>(EgoVehicle::VT_YAW, yaw, 0.0f);
  }
  void add_velocity(float velocity) {
    fbb_.AddElement<float>(EgoVehicle::VT_VELOCITY, velocity, 0.0f);
  }
  void add_yawRate(float yawRate) {
    fbb_.AddElement<float>(EgoVehicle::VT_YAWRATE, yawRate, 0.0f);
  }
  void add_height(float height) {
    fbb_.AddElement<float>(EgoVehicle::VT_HEIGHT, height, 0.0f);
  }
  void add_shapePoints(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::ShapePoint>>> shapePoints) {
    fbb_.AddOffset(EgoVehicle::VT_SHAPEPOINTS, shapePoints);
  }
  explicit EgoVehicleBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<EgoVehicle> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<EgoVehicle>(end);
    return o;
  }
};

inline flatbuffers::Offset<EgoVehicle> CreateEgoVehicle(
    flatbuffers::FlatBufferBuilder &_fbb,
    float width = 0.0f,
    float length = 0.0f,
    float x = 0.0f,
    float y = 0.0f,
    float yaw = 0.0f,
    float velocity = 0.0f,
    float yawRate = 0.0f,
    float height = 0.0f,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::ShapePoint>>> shapePoints = 0) {
  EgoVehicleBuilder builder_(_fbb);
  builder_.add_shapePoints(shapePoints);
  builder_.add_height(height);
  builder_.add_yawRate(yawRate);
  builder_.add_velocity(velocity);
  builder_.add_yaw(yaw);
  builder_.add_y(y);
  builder_.add_x(x);
  builder_.add_length(length);
  builder_.add_width(width);
  return builder_.Finish();
}

inline flatbuffers::Offset<EgoVehicle> CreateEgoVehicleDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    float width = 0.0f,
    float length = 0.0f,
    float x = 0.0f,
    float y = 0.0f,
    float yaw = 0.0f,
    float velocity = 0.0f,
    float yawRate = 0.0f,
    float height = 0.0f,
    const std::vector<flatbuffers::Offset<GuiSchema::ShapePoint>> *shapePoints = nullptr) {
  auto shapePoints__ = shapePoints ? _fbb.CreateVector<flatbuffers::Offset<GuiSchema::ShapePoint>>(*shapePoints) : 0;
  return GuiSchema::CreateEgoVehicle(
      _fbb,
      width,
      length,
      x,
      y,
      yaw,
      velocity,
      yawRate,
      height,
      shapePoints__);
}

flatbuffers::Offset<EgoVehicle> CreateEgoVehicle(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct EgoVehicleListT : public flatbuffers::NativeTable {
  typedef EgoVehicleList TableType;
  std::vector<std::unique_ptr<GuiSchema::EgoVehicleT>> egoVehicles{};
  EgoVehicleListT() = default;
  EgoVehicleListT(const EgoVehicleListT &o);
  EgoVehicleListT(EgoVehicleListT&&) FLATBUFFERS_NOEXCEPT = default;
  EgoVehicleListT &operator=(EgoVehicleListT o) FLATBUFFERS_NOEXCEPT;
};

struct EgoVehicleList FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EgoVehicleListT NativeTableType;
  typedef EgoVehicleListBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_EGOVEHICLES = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> *egoVehicles() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> *>(VT_EGOVEHICLES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_EGOVEHICLES) &&
           verifier.VerifyVector(egoVehicles()) &&
           verifier.VerifyVectorOfTables(egoVehicles()) &&
           verifier.EndTable();
  }
  EgoVehicleListT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(EgoVehicleListT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<EgoVehicleList> Pack(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleListT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct EgoVehicleListBuilder {
  typedef EgoVehicleList Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_egoVehicles(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::EgoVehicle>>> egoVehicles) {
    fbb_.AddOffset(EgoVehicleList::VT_EGOVEHICLES, egoVehicles);
  }
  explicit EgoVehicleListBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<EgoVehicleList> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<EgoVehicleList>(end);
    return o;
  }
};

inline flatbuffers::Offset<EgoVehicleList> CreateEgoVehicleList(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<GuiSchema::EgoVehicle>>> egoVehicles = 0) {
  EgoVehicleListBuilder builder_(_fbb);
  builder_.add_egoVehicles(egoVehicles);
  return builder_.Finish();
}

inline flatbuffers::Offset<EgoVehicleList> CreateEgoVehicleListDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> *egoVehicles = nullptr) {
  auto egoVehicles__ = egoVehicles ? _fbb.CreateVector<flatbuffers::Offset<GuiSchema::EgoVehicle>>(*egoVehicles) : 0;
  return GuiSchema::CreateEgoVehicleList(
      _fbb,
      egoVehicles__);
}

flatbuffers::Offset<EgoVehicleList> CreateEgoVehicleList(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleListT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline ShapePointT *ShapePoint::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<ShapePointT>(new ShapePointT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void ShapePoint::UnPackTo(ShapePointT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = x(); _o->x = _e; }
  { auto _e = y(); _o->y = _e; }
  { auto _e = z(); _o->z = _e; }
}

inline flatbuffers::Offset<ShapePoint> ShapePoint::Pack(flatbuffers::FlatBufferBuilder &_fbb, const ShapePointT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateShapePoint(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<ShapePoint> CreateShapePoint(flatbuffers::FlatBufferBuilder &_fbb, const ShapePointT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const ShapePointT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _x = _o->x;
  auto _y = _o->y;
  auto _z = _o->z;
  return GuiSchema::CreateShapePoint(
      _fbb,
      _x,
      _y,
      _z);
}

inline EgoVehicleT::EgoVehicleT(const EgoVehicleT &o)
      : width(o.width),
        length(o.length),
        x(o.x),
        y(o.y),
        yaw(o.yaw),
        velocity(o.velocity),
        yawRate(o.yawRate),
        height(o.height) {
  shapePoints.reserve(o.shapePoints.size());
  for (const auto &v : o.shapePoints) { shapePoints.emplace_back((v) ? new GuiSchema::ShapePointT(*v) : nullptr); }
}

inline EgoVehicleT &EgoVehicleT::operator=(EgoVehicleT o) FLATBUFFERS_NOEXCEPT {
  std::swap(width, o.width);
  std::swap(length, o.length);
  std::swap(x, o.x);
  std::swap(y, o.y);
  std::swap(yaw, o.yaw);
  std::swap(velocity, o.velocity);
  std::swap(yawRate, o.yawRate);
  std::swap(height, o.height);
  std::swap(shapePoints, o.shapePoints);
  return *this;
}

inline EgoVehicleT *EgoVehicle::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<EgoVehicleT>(new EgoVehicleT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void EgoVehicle::UnPackTo(EgoVehicleT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = width(); _o->width = _e; }
  { auto _e = length(); _o->length = _e; }
  { auto _e = x(); _o->x = _e; }
  { auto _e = y(); _o->y = _e; }
  { auto _e = yaw(); _o->yaw = _e; }
  { auto _e = velocity(); _o->velocity = _e; }
  { auto _e = yawRate(); _o->yawRate = _e; }
  { auto _e = height(); _o->height = _e; }
  { auto _e = shapePoints(); if (_e) { _o->shapePoints.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { if(_o->shapePoints[_i]) { _e->Get(_i)->UnPackTo(_o->shapePoints[_i].get(), _resolver); } else { _o->shapePoints[_i] = std::unique_ptr<GuiSchema::ShapePointT>(_e->Get(_i)->UnPack(_resolver)); }; } } }
}

inline flatbuffers::Offset<EgoVehicle> EgoVehicle::Pack(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateEgoVehicle(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<EgoVehicle> CreateEgoVehicle(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const EgoVehicleT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _width = _o->width;
  auto _length = _o->length;
  auto _x = _o->x;
  auto _y = _o->y;
  auto _yaw = _o->yaw;
  auto _velocity = _o->velocity;
  auto _yawRate = _o->yawRate;
  auto _height = _o->height;
  auto _shapePoints = _o->shapePoints.size() ? _fbb.CreateVector<flatbuffers::Offset<GuiSchema::ShapePoint>> (_o->shapePoints.size(), [](size_t i, _VectorArgs *__va) { return CreateShapePoint(*__va->__fbb, __va->__o->shapePoints[i].get(), __va->__rehasher); }, &_va ) : 0;
  return GuiSchema::CreateEgoVehicle(
      _fbb,
      _width,
      _length,
      _x,
      _y,
      _yaw,
      _velocity,
      _yawRate,
      _height,
      _shapePoints);
}

inline EgoVehicleListT::EgoVehicleListT(const EgoVehicleListT &o) {
  egoVehicles.reserve(o.egoVehicles.size());
  for (const auto &v : o.egoVehicles) { egoVehicles.emplace_back((v) ? new GuiSchema::EgoVehicleT(*v) : nullptr); }
}

inline EgoVehicleListT &EgoVehicleListT::operator=(EgoVehicleListT o) FLATBUFFERS_NOEXCEPT {
  std::swap(egoVehicles, o.egoVehicles);
  return *this;
}

inline EgoVehicleListT *EgoVehicleList::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<EgoVehicleListT>(new EgoVehicleListT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void EgoVehicleList::UnPackTo(EgoVehicleListT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = egoVehicles(); if (_e) { _o->egoVehicles.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { if(_o->egoVehicles[_i]) { _e->Get(_i)->UnPackTo(_o->egoVehicles[_i].get(), _resolver); } else { _o->egoVehicles[_i] = std::unique_ptr<GuiSchema::EgoVehicleT>(_e->Get(_i)->UnPack(_resolver)); }; } } }
}

inline flatbuffers::Offset<EgoVehicleList> EgoVehicleList::Pack(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleListT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateEgoVehicleList(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<EgoVehicleList> CreateEgoVehicleList(flatbuffers::FlatBufferBuilder &_fbb, const EgoVehicleListT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const EgoVehicleListT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _egoVehicles = _o->egoVehicles.size() ? _fbb.CreateVector<flatbuffers::Offset<GuiSchema::EgoVehicle>> (_o->egoVehicles.size(), [](size_t i, _VectorArgs *__va) { return CreateEgoVehicle(*__va->__fbb, __va->__o->egoVehicles[i].get(), __va->__rehasher); }, &_va ) : 0;
  return GuiSchema::CreateEgoVehicleList(
      _fbb,
      _egoVehicles);
}

inline const GuiSchema::EgoVehicleList *GetEgoVehicleList(const void *buf) {
  return flatbuffers::GetRoot<GuiSchema::EgoVehicleList>(buf);
}

inline const GuiSchema::EgoVehicleList *GetSizePrefixedEgoVehicleList(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<GuiSchema::EgoVehicleList>(buf);
}

inline const char *EgoVehicleListIdentifier() {
  return "EV01";
}

inline bool EgoVehicleListBufferHasIdentifier(const void *buf) {
  return flatbuffers::BufferHasIdentifier(
      buf, EgoVehicleListIdentifier());
}

inline bool SizePrefixedEgoVehicleListBufferHasIdentifier(const void *buf) {
  return flatbuffers::BufferHasIdentifier(
      buf, EgoVehicleListIdentifier(), true);
}

inline bool VerifyEgoVehicleListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<GuiSchema::EgoVehicleList>(EgoVehicleListIdentifier());
}

inline bool VerifySizePrefixedEgoVehicleListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<GuiSchema::EgoVehicleList>(EgoVehicleListIdentifier());
}

inline void FinishEgoVehicleListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<GuiSchema::EgoVehicleList> root) {
  fbb.Finish(root, EgoVehicleListIdentifier());
}

inline void FinishSizePrefixedEgoVehicleListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<GuiSchema::EgoVehicleList> root) {
  fbb.FinishSizePrefixed(root, EgoVehicleListIdentifier());
}

inline std::unique_ptr<GuiSchema::EgoVehicleListT> UnPackEgoVehicleList(
    const void *buf,
    const flatbuffers::resolver_function_t *res = nullptr) {
  return std::unique_ptr<GuiSchema::EgoVehicleListT>(GetEgoVehicleList(buf)->UnPack(res));
}

inline std::unique_ptr<GuiSchema::EgoVehicleListT> UnPackSizePrefixedEgoVehicleList(
    const void *buf,
    const flatbuffers::resolver_function_t *res = nullptr) {
  return std::unique_ptr<GuiSchema::EgoVehicleListT>(GetSizePrefixedEgoVehicleList(buf)->UnPack(res));
}

}  // namespace GuiSchema

#endif  // FLATBUFFERS_GENERATED_EGOVEHICLE_GUISCHEMA_H_