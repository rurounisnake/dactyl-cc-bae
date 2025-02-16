#include "key_data.h"

#include <glm/glm.hpp>
#include "key.h"
#include "scad.h"
#include "transform.h"

namespace scad {

constexpr double kDefaultKeySpacing = 19;
constexpr double kPipeKeySpacing = 23.8;
constexpr double kReturnKeySpacing = 30.95;

KeyData::KeyData(TransformList key_origin) {

    key_backspace.Configure([&](Key& k) {
      k.name = "key_backspace";
      k.SetParent(key_origin);
      k.SetPosition(60, -9.18, 42.83);
      k.t().rz = -21;
      k.t().rx = 12;
      k.t().ry = -4.5;
    });

    // Second thumb key.
    key_delete.Configure([&](Key& k) {
      k.name = "key_delete";
      k.SetParent(key_backspace);
      k.SetPosition(kPipeKeySpacing, 10, 0);
    });

    // Bottom side key.
    key_end.Configure([&](Key& k) {
      k.name = "key_end";
      k.SetParent(key_backspace);
      k.SetPosition(kReturnKeySpacing, -9, 0);
    });

    // Middle side key.
    key_home.Configure([&](Key& k) {
      k.name = "key_home";
        k.SetParent(key_backspace);
      k.SetPosition(0, 10 + kDefaultKeySpacing, 0);
    });

    // Top side key;
    key_alt.Configure([&](Key& k) {
      k.name = "key_alt";
      k.SetParent(key_backspace);
      k.SetPosition(kDefaultKeySpacing + kDefaultKeySpacing, 10 + kDefaultKeySpacing, 0);
    });

    // Top left key.
    key_ctrl.Configure([&](Key& k) {
      k.name = "key_ctrl";
      k.SetParent(key_backspace);
      k.SetPosition(kDefaultKeySpacing, 10 + kDefaultKeySpacing, 0);
    });

    // tip of the cap. Adjust the keys position so that the origin is at the switch top.
    double switch_top_z_offset = 10;
    for (Key* key : all_keys()) {
      key->AddTransform();
      key->disable_switch_z_offset = true;
      key->t().z -= 10;
    }
  }
}  // namespace scad

