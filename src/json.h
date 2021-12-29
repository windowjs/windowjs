#ifndef WINDOWJS_JSON_H
#define WINDOWJS_JSON_H

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "fail.h"

class Json {
 public:
  using ListType = std::vector<Json>;
  using DictionaryType = std::unordered_map<std::string, Json>;

  enum JsonType {
    NUL,
    BOOL,
    INT,
    DOUBLE,
    STRING,
    LIST,
    DICTIONARY,
  };

  ~Json();

  Json() : type_(NUL) {}

  static Json Null() { return Json(); }

  explicit Json(bool value) : type_(BOOL), b_(value) {}

  explicit Json(int value) : type_(INT), i_(value) {}
  explicit Json(long value) : type_(INT), i_(value) {}

  explicit Json(double value) : type_(DOUBLE), d_(value) {}

  explicit Json(const char* value)
      : type_(STRING), s_(new std::string(value)) {}
  explicit Json(const std::string& value)
      : type_(STRING), s_(new std::string(value)) {}
  explicit Json(std::string&& value)
      : type_(STRING), s_(new std::string(value)) {}

  explicit Json(ListType&& value) : type_(LIST), l_(new ListType(value)) {}

  static Json EmptyList() { return Json(ListType()); }

  explicit Json(DictionaryType&& value)
      : type_(DICTIONARY), o_(new DictionaryType(value)) {}

  static Json EmptyDictionary() { return Json(DictionaryType()); }

  Json(const Json& json) : type_(NUL) { *this = json; }
  Json(Json&& json) : type_(json.type_), o_(json.o_) { json.type_ = NUL; }

  Json& operator=(const Json& json);
  Json& operator=(Json&& json);

  bool operator==(const Json& json) const;
  bool operator!=(const Json& json) const { return !(*this == json); }

  // Type check.

  JsonType Type() const { return type_; }

  bool IsNull() const { return type_ == NUL; }
  bool IsBool() const { return type_ == BOOL; }
  bool IsInt() const { return type_ == INT; }
  bool IsDouble() const { return type_ == DOUBLE; }
  bool IsString() const { return type_ == STRING; }
  bool IsList() const { return type_ == LIST; }
  bool IsDictionary() const { return type_ == DICTIONARY; }

  // Null.

  void Nullify();

  // Booleans.

  bool Bool() const {
    ASSERT(type_ == BOOL);
    return b_;
  }

  Json& operator=(bool value) {
    Nullify();
    type_ = BOOL;
    b_ = value;
    return *this;
  }

  // Integers.

  long Long() const {
    ASSERT(type_ == INT);
    return i_;
  }

  Json& operator=(int value) {
    Nullify();
    type_ = INT;
    i_ = value;
    return *this;
  }

  Json& operator=(long value) {
    Nullify();
    type_ = INT;
    i_ = value;
    return *this;
  }

  // Doubles.

  double Double() const {
    ASSERT(type_ == DOUBLE);
    return d_;
  }

  Json& operator=(double value) {
    Nullify();
    type_ = DOUBLE;
    d_ = value;
    return *this;
  }

  // Strings.

  const std::string& String() const {
    ASSERT(type_ == STRING);
    return *s_;
  }

  std::string MoveString() {
    ASSERT(type_ == STRING);
    std::string result = std::move(*s_);
    Nullify();
    return result;
  }

  Json& operator=(const char* value) {
    Nullify();
    type_ = STRING;
    s_ = new std::string(value);
    return *this;
  }

  Json& operator=(const std::string& value) {
    Nullify();
    type_ = STRING;
    s_ = new std::string(value);
    return *this;
  }

  Json& operator=(std::string&& value) {
    Nullify();
    type_ = STRING;
    s_ = new std::string(value);
    return *this;
  }

  // Lists.

  const ListType& List() const {
    ASSERT(type_ == LIST);
    return *l_;
  }

  ListType& MutableList() {
    ASSERT(type_ == LIST);
    return *l_;
  }

  int Size() const {
    ASSERT(type_ == LIST || type_ == DICTIONARY);
    return type_ == LIST ? l_->size() : o_->size();
  }

  void Resize(int size) {
    ASSERT(type_ == LIST);
    l_->resize(size);
  }

  void Reserve(int size) {
    ASSERT(type_ == LIST);
    l_->reserve(size);
  }

  Json& Append(bool value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(int value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(static_cast<long>(value));
    return *this;
  }

  Json& Append(long value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(double value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(const char* value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(const std::string& value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(const Json& value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  Json& Append(Json&& value) {
    ASSERT(type_ == LIST);
    l_->emplace_back(value);
    return *this;
  }

  const Json& operator[](int index) const {
    ASSERT(type_ == LIST);
    return (*l_)[index];
  }

  Json& operator[](int index) {
    ASSERT(type_ == LIST);
    return (*l_)[index];
  }

  // Dictionaries.

  const DictionaryType& Dictionary() const {
    ASSERT(type_ == DICTIONARY);
    return *o_;
  }

  DictionaryType& MutableDictionary() {
    ASSERT(type_ == DICTIONARY);
    return *o_;
  }

  bool Contains(const std::string& key) const {
    ASSERT(type_ == DICTIONARY);
    return o_->find(key) != o_->end();
  }

  void Remove(const std::string& key) const {
    ASSERT(type_ == DICTIONARY);
    o_->erase(key);
  }

  const Json& operator[](const std::string& key) const {
    ASSERT(type_ == DICTIONARY);
    return (*o_)[key];
  }

  Json& operator[](const std::string& key) {
    ASSERT(type_ == DICTIONARY);
    return (*o_)[key];
  }

  // Serialization.

  std::string ToString() const;
  std::string ToPrettyString(const std::string& indent = "") const;

  static std::string EscapeString(std::string_view value);

  static std::unique_ptr<Json> Parse(std::string_view input,
                                     std::string* error = nullptr);

  friend std::ostream& operator<<(std::ostream& o, const Json& json) {
    o << json.ToPrettyString();
    return o;
  }

 private:
  JsonType type_;
  union {
    bool b_;
    long i_;
    double d_;
    std::string* s_;
    ListType* l_;
    DictionaryType* o_;
  };
};

#endif  // WINDOWJS_JSON_H
