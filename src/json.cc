#include "json.h"

Json::~Json() {
  Nullify();
}

Json& Json::operator=(const Json& json) {
  if (this == &json) {
    return *this;
  }

  Nullify();
  type_ = json.type_;

  switch (type_) {
    case NUL: break;
    case BOOL: b_ = json.b_; break;
    case INT: i_ = json.i_; break;
    case DOUBLE: d_ = json.d_; break;
    case STRING: s_ = new std::string(*json.s_); break;
    case LIST: l_ = new ListType(*json.l_); break;
    case DICTIONARY: o_ = new DictionaryType(*json.o_); break;
  }

  return *this;
}

Json& Json::operator=(Json&& json) {
  if (this == &json) {
    return *this;
  }

  Nullify();
  type_ = json.type_;
  o_ = json.o_;
  json.type_ = NUL;

  return *this;
}

bool Json::operator==(const Json& json) const {
  if (type_ != json.type_) {
    return false;
  }

  switch (type_) {
    case NUL: return true;
    case BOOL: return b_ == json.b_;
    case INT: return i_ == json.i_;
    case DOUBLE: return d_ == json.d_;
    case STRING: return *s_ == *json.s_;
    case LIST: return *l_ == *json.l_;
    case DICTIONARY: return *o_ == *json.o_;
  }
  ASSERT(false);
  return false;
}

void Json::Nullify() {
  if (type_ == STRING) {
    delete s_;
  } else if (type_ == LIST) {
    delete l_;
  } else if (type_ == DICTIONARY) {
    delete o_;
  }
  type_ = NUL;
}

std::string Json::ToString() const {
  switch (type_) {
    case NUL: return "null";
    case BOOL: return b_ ? "true" : "false";
    case INT: return std::to_string(i_);
    case DOUBLE: return std::to_string(d_);
    case STRING: return EscapeString(*s_);
    case LIST: {
      std::string result;
      result.append(1, '[');
      bool first = true;
      for (const auto& json : *l_) {
        if (first) {
          first = false;
        } else {
          result.append(1, ',');
        }
        result.append(json.ToString());
      }
      result.append(1, ']');
      return result;
    }
    case DICTIONARY: {
      std::string result;
      result.append(1, '{');
      const DictionaryType& o = *o_;
      bool first = true;
      for (const auto& p : o) {
        if (first) {
          first = false;
        } else {
          result.append(1, ',');
        }
        result.append(EscapeString(p.first));
        result.append(1, ':');
        result.append(p.second.ToString());
      }
      result.append(1, '}');
      return result;
    }
  }
  ASSERT(false);
  return {};
}

std::string Json::ToPrettyString(const std::string& indent) const {
  switch (type_) {
    case NUL: return "null";
    case BOOL: return b_ ? "true" : "false";
    case INT: return std::to_string(i_);
    case DOUBLE: return std::to_string(d_);
    case STRING: return EscapeString(*s_);
    case LIST: {
      std::string result;
      result.append(1, '[');
      bool first = true;
      for (const auto& json : *l_) {
        if (first) {
          first = false;
        } else {
          result.append(", ");
        }
        result.append(json.ToPrettyString(indent));
      }
      result.append(1, ']');
      return result;
    }
    case DICTIONARY: {
      std::string result;
      const DictionaryType& o = *o_;
      if (o.empty())
        return "{}";
      result.append("{\n");
      bool first = true;
      for (const auto& p : o) {
        if (first) {
          first = false;
        } else {
          result.append(",\n");
        }
        result.append(indent).append("  ").append(EscapeString(p.first));
        result.append(": ");
        result.append(p.second.ToPrettyString(indent + "  "));
      }
      result.append("\n").append(indent).append(1, '}');
      return result;
    }
  }
  ASSERT(false);
  return {};
}

// static
std::string Json::EscapeString(std::string_view value) {
  std::string result;
  result.reserve(value.size() + 2);
  result.append(1, '"');
  for (char c : value) {
    if (c == '"') {
      result.append("\\\"");
    } else if (c == '\\') {
      result.append("\\\\");
    } else if (c == '/') {
      result.append("\\/");
    } else if (c == '\b') {
      result.append("\\b");
    } else if (c == '\f') {
      result.append("\\f");
    } else if (c == '\n') {
      result.append("\\n");
    } else if (c == '\r') {
      result.append("\\r");
    } else if (c == '\t') {
      result.append("\\t");
    } else {
      result.append(1, c);
    }
  }
  result.append(1, '"');
  return result;
}

namespace {

void SkipWhites(std::string_view& s) {
  while (!s.empty() && std::isspace(s[0])) {
    s.remove_prefix(1);
  }
}

bool Eat(std::string_view& input, char c) {
  SkipWhites(input);
  if (input.empty()) {
    return false;
  }
  if (input[0] == c) {
    input.remove_prefix(1);
    return true;
  }
  return false;
}

std::string_view EatNextWord(std::string_view& input) {
  size_t k = 0;
  while (k < input.size() && std::islower(input[k])) {
    ++k;
  }
  std::string_view word = input.substr(0, k);
  input.remove_prefix(k);
  return word;
}

bool ParseSomething(std::string_view& input, Json* json, std::string* error);

bool ParseList(std::string_view& input, Json* json, std::string* error) {
  bool first = true;
  while (!Eat(input, ']')) {
    if (first) {
      first = false;
    } else if (!Eat(input, ',')) {
      if (error) {
        *error = "Expected , or ]";
      }
      return false;
    }
    Json item;
    if (!ParseSomething(input, &item, error)) {
      return false;
    }
    json->Append(std::move(item));
  }
  return true;
}

bool ParseDictionary(std::string_view& input, Json* json, std::string* error) {
  bool first = true;
  while (!Eat(input, '}')) {
    if (first) {
      first = false;
    } else if (!Eat(input, ',')) {
      if (error) {
        *error = "Expected , or }";
      }
      return false;
    }
    Json key;
    if (!ParseSomething(input, &key, error))
      return false;
    if (!key.IsString()) {
      if (error) {
        *error = "Expected string";
      }
      return false;
    }
    if (json->Contains(key.String())) {
      if (error) {
        *error = "Duplicated key " + key.String();
      }
      return false;
    }
    if (!Eat(input, ':')) {
      if (error) {
        *error = "Expected :";
      }
      return false;
    }
    Json item;
    if (!ParseSomething(input, &item, error)) {
      return false;
    }
    (*json)[key.String()] = std::move(item);
  }
  return true;
}

bool ParseSomething(std::string_view& input, Json* json, std::string* error) {
  if (Eat(input, '[')) {
    *json = Json::EmptyList();
    return ParseList(input, json, error);
  } else if (Eat(input, '{')) {
    *json = Json::EmptyDictionary();
    return ParseDictionary(input, json, error);
  } else if (Eat(input, '"')) {
    std::string result;
    while (!input.empty() && input[0] != '"') {
      if (input[0] == '\\') {
        if (input.size() < 2) {
          if (error) {
            *error = "Incomplete string escape";
          }
          return false;
        }
        if (input[1] == '"') {
          result.append(1, '"');
        } else if (input[1] == '\\') {
          result.append(1, '\\');
        } else if (input[1] == '/') {
          result.append(1, '/');
        } else if (input[1] == 'b') {
          result.append(1, '\b');
        } else if (input[1] == 'f') {
          result.append(1, '\f');
        } else if (input[1] == 'n') {
          result.append(1, '\n');
        } else if (input[1] == 'r') {
          result.append(1, '\r');
        } else if (input[1] == 't') {
          result.append(1, '\t');
        } else {
          result.append(1, input[1]);
        }
        input.remove_prefix(2);
      } else {
        result.append(1, input[0]);
        input.remove_prefix(1);
      }
    }
    if (input.empty()) {
      if (error) {
        *error = "Incomplete string";
      }
      return false;
    }
    input.remove_prefix(1);
    *json = result;
    return true;
  } else if (input.empty()) {
    if (error) {
      *error = "Premature end of input";
    }
    return false;
  } else {
    const char* begin = input.data();
    char* end = nullptr;
    double d = strtod(begin, &end);
    if (end > begin) {
      input.remove_prefix(end - begin);
      long l = d;
      if (d == l) {
        *json = l;
      } else {
        *json = d;
      }
      return true;
    }
    std::string_view word = EatNextWord(input);
    if (word == "true") {
      *json = true;
    } else if (word == "false") {
      *json = false;
    } else if (word == "null") {
      *json = Json();
    } else {
      if (error) {
        *error = "Unrecognized token: \"";
        *error += word;
        *error += "\", next input: ";
        *error += input.substr(0, 20);
      }
      return false;
    }
    return true;
  }
}

}  // namespace

// static
std::unique_ptr<Json> Json::Parse(std::string_view input, std::string* error) {
  SkipWhites(input);
  if (input.empty()) {
    if (error) {
      *error = "Empty input";
    }
    return nullptr;
  }
  std::unique_ptr<Json> json(new Json);
  if (!ParseSomething(input, json.get(), error)) {
    return nullptr;
  }
  SkipWhites(input);
  if (!input.empty()) {
    if (error) {
      *error = "Trailing characters";
    }
    return nullptr;
  }
  return json;
}
