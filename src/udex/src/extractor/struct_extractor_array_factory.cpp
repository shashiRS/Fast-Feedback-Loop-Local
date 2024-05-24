#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>

namespace next {
namespace udex {
namespace extractor {

bool StructExtractorArrayFactory::UpdateUrlInVector(
    std::vector<std::shared_ptr<StructExtractorBasic>> &raw_object_vector) {

  size_t num_objects = raw_object_vector.size();

  if (num_objects == 0) {
    return true;
  }

  int count = 0;
  for (const auto &obj : raw_object_vector) {
    std::string non_array_url = obj->url_;
    UpdateUrlAsArray(obj, count);
    count++;
  }

  //  compute shift offset

  // get offset of object 0
  if (num_objects >= 1) {
    UpdateOffset(raw_object_vector[0]);
  } else {
    return true;
  }

  // more objects -> compute shift and object offsets of object 1 for shift
  if (num_objects >= 2) {
    UpdateOffset(raw_object_vector[1]);
  } else {
    return true;
  }

  size_t offset = raw_object_vector[1]->offset_ - raw_object_vector[0]->offset_;

  for (size_t i_obj = 2; i_obj < raw_object_vector.size(); i_obj++) {
    if (!copyOffsetsAndType(raw_object_vector[0], raw_object_vector[i_obj])) {
      return false;
    }
    shiftOffset(raw_object_vector[i_obj], i_obj * offset);
  }

  return true;
}

void StructExtractorArrayFactory::shiftOffset(std::shared_ptr<StructExtractorBasic> obj, size_t shift) {
  shiftOffset(obj.get(), shift);
}

void StructExtractorArrayFactory::shiftOffset(StructExtractorBasic *obj, size_t shift) {
  obj->offset_ += shift;
  for (const auto &child : obj->children_) {
    shiftOffset(child, shift);
  }
}

bool StructExtractorArrayFactory::copyOffsetsAndType(std::shared_ptr<StructExtractorBasic> obj_from,
                                                     std::shared_ptr<StructExtractorBasic> obj_to) {
  // check if same structure
  if (obj_from->children_.size() != obj_to->children_.size()) {
    return false;
  }

  obj_to->offset_ = obj_from->offset_;
  obj_to->CopyStructInfo(obj_from->type_, obj_from->array_length_);
  for (size_t i_obj = 0; i_obj < obj_from->children_.size(); i_obj++) {
    // check if same child
    if (obj_from->original_url != obj_to->original_url) {
      return false;
    }
    copyOffsetsAndType(obj_from->children_[i_obj], obj_to->children_[i_obj]);
  }
  return true;
}
bool StructExtractorArrayFactory::copyOffsetsAndType(StructExtractorBasic *obj_from, StructExtractorBasic *obj_to) {
  // check if same structure
  if (((obj_from == nullptr) || (obj_to == nullptr)) || (obj_from->children_.size() != obj_to->children_.size())) {
    return false;
  }
  obj_to->offset_ = obj_from->offset_;
  obj_to->CopyStructInfo(obj_from->type_, obj_from->array_length_);
  for (size_t i_obj = 0; i_obj < obj_from->children_.size(); i_obj++) {
    // check if same child
    if (obj_from->array_url_ != obj_to->array_url_) {
      return false;
    }
    copyOffsetsAndType(obj_from->children_[i_obj], obj_to->children_[i_obj]);
  }
  return true;
}

void StructExtractorArrayFactory::UpdateOffset(std::shared_ptr<StructExtractorBasic> obj) { UpdateOffset(obj.get()); }

void StructExtractorArrayFactory::UpdateOffset(StructExtractorBasic *obj) {
  if (obj == nullptr) {
    return;
  }
  next::udex::SignalInfo signal_info = obj->signal_info_provider_->getSignalInfo(obj->url_);
  obj->offset_ = signal_info.offset;
  obj->CopyStructInfo(signal_info.signal_type, signal_info.array_size);
  for (const auto &child : obj->children_) {
    UpdateOffset(child);
  }
}

void StructExtractorArrayFactory::UpdateUrlAsArray(std::shared_ptr<StructExtractorBasic> obj, int index) {
  if ((obj == nullptr) || (obj->array_url_.size() == 0) || (obj->parent_url_.size() == 0)) {
    return;
  }
  obj->array_url_ = obj->array_url_ + "[" + std::to_string(index) + "]";
  obj->url_ = obj->parent_url_ + obj->array_url_;
  for (const auto &child : obj->children_) {
    UpdateParentUrl(child);
  }
}

void StructExtractorArrayFactory::UpdateParentUrl(StructExtractorBasic *obj) {
  if (obj == nullptr) {
    return;
  }
  obj->parent_url_ = obj->parent_->url_;
  obj->url_ = obj->parent_url_ + obj->array_url_;
  for (const auto &child : obj->children_) {
    UpdateParentUrl(child);
  }
}

} // namespace extractor
} // namespace udex
} // namespace next
