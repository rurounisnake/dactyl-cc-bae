#pragma once

#include "key.h"
#include "transform.h"
#include "iostream"

namespace scad {

// Key positioning data and description of layout and grouping of keys.
struct KeyData {
  KeyData(TransformList origin);

  Key key_backspace;
  Key key_delete;
  Key key_end;
  Key key_home;
  Key key_ctrl;
  Key key_alt;

  std::vector<Key*> thumb_keys() {
    return {&key_delete, &key_backspace, &key_ctrl, &key_alt, &key_home, &key_end};
  }

  std::vector<Key*> all_keys() {
    std::vector<Key*> keys;
    for (Key* key : thumb_keys()) {
      keys.push_back(key);
    }
    return keys;
  }
};

}  // namespace scad
