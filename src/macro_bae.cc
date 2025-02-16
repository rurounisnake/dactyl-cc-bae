#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "key.h"
#include "key_data.h"
#include "scad.h"
#include "transform.h"

using namespace scad;

// Add the caps into the stl for testing.
constexpr bool kAddCaps = false;

enum class Direction { UP, DOWN, LEFT, RIGHT };

void AddShapes(std::vector<Shape>* shapes, std::vector<Shape> to_add) {
  for (Shape s : to_add) {
    shapes->push_back(s);
  }
}

int main() {
  printf("generating..\n");
  TransformList key_origin;
  key_origin.Translate(-20, -40, 3);

  // This is where all of the logic to position the keys is done. Everything below is cosmetic
  // trying to build the case.
  KeyData d(key_origin);

  d.key_backspace.extra_width_bottom = 12;
  d.key_backspace.extra_width_left = 6;
  d.key_backspace.extra_width_right = 2;
  d.key_delete.extra_width_top = 3;
  d.key_delete.extra_width_right = 20.15;
  d.key_delete.extra_width_left = 5;
  d.key_ctrl.extra_width_top = 3;
  d.key_alt.extra_width_top = 3;
  d.key_alt.extra_width_right = 6;
  d.key_alt.extra_width_left = 3;
  d.key_end.extra_width_top = 3;
  d.key_end.extra_width_bottom = 3;
  d.key_end.extra_width_right = 13;
  d.key_end.extra_width_left = 12;
  d.key_home.extra_width_left = 6;
  d.key_home.extra_width_right = 1;
  d.key_home.extra_width_top = 3;
  d.key_home.extra_width_bottom = 12;

  std::vector<Shape> shapes;
  std::vector<Key*> keys = {&d.key_backspace, &d.key_ctrl, &d.key_alt, &d.key_home, &d.key_delete, &d.key_end};
  
  for (Key* key : keys) {
    shapes.push_back(key->GetSwitch());
    if (kAddCaps) {
      shapes.push_back(key->GetCap().Color("red"));
    }
  }

  //
  // Make the wall
  //
  {
    struct WallPoint {
      WallPoint(TransformList transforms,
                Direction out_direction,
                float extra_distance = 0,
                float extra_width = 0)
          : transforms(transforms),
            out_direction(out_direction),
            extra_distance(extra_distance),
            extra_width(extra_width) {
      }
      TransformList transforms;
      Direction out_direction;
      float extra_distance;
      float extra_width;
    };

    Direction up = Direction::UP;
    Direction down = Direction::DOWN;
    Direction left = Direction::LEFT;
    Direction right = Direction::RIGHT;

    std::vector<WallPoint> wall_points = {
        // Start top left and go anticlockwise

        {d.key_home.GetTopRight(), up, 0, .5},
        {d.key_home.GetTopLeft(), up, 0, .5},
        {d.key_home.GetTopLeft(), left, 0, .5},
        {d.key_home.GetBottomLeft(), left, 0, .5},

        {d.key_backspace.GetTopLeft(), left, 0, .5},
        {d.key_backspace.GetBottomLeft(), left, 0, .5},
        {d.key_backspace.GetBottomLeft(), down, 0, .5},
        {d.key_backspace.GetBottomRight(), down, 0, .5},

        {d.key_end.GetBottomLeft(), down, 0, .5},
        {d.key_end.GetBottomRight(), down, 0, .5},
        {d.key_end.GetBottomRight(), right, 0, .5},
        {d.key_end.GetTopRight(), right, 0, .5},

        {d.key_delete.GetBottomRight(), right, 0, .5},
        {d.key_delete.GetBottomRight(), right, 0, .5},

        {d.key_alt.GetBottomRight(), right, 0, .5},
        {d.key_alt.GetTopRight(), right, 0, .5},
        {d.key_alt.GetTopRight(), up, 0, .5},

        {d.key_ctrl.GetTopRight(), up, 0, .5},
        {d.key_ctrl.GetTopLeft(), up, 0, .5},
    };

    std::vector<std::vector<Shape>> wall_slices;
    for (WallPoint point : wall_points) {
      Shape s1 = point.transforms.Apply(GetPostConnector());

      TransformList t = point.transforms;
      glm::vec3 out_dir;
      float distance = 4.8 + point.extra_distance;
      switch (point.out_direction) {
        case Direction::UP:
          t.AppendFront(TransformList().Translate(0, distance, 0).RotateX(-20));
          break;
        case Direction::DOWN:
          t.AppendFront(TransformList().Translate(0, -1 * distance, 0).RotateX(20));
          break;
        case Direction::LEFT:
          t.AppendFront(TransformList().Translate(-1 * distance, 0, 0).RotateY(-20));
          break;
        case Direction::RIGHT:
          t.AppendFront(TransformList().Translate(distance, 0, 0).RotateY(20));
          break;
      }
 
      // Make sure the section extruded to the bottom is thick enough. With certain angles the
      // projection is very small if you just use the post connector from the transform. Compute
      // an explicit shape.
      const glm::vec3 post_offset(0, 0, -4);
      const glm::vec3 p = point.transforms.Apply(post_offset);
      const glm::vec3 p2 = t.Apply(post_offset);

      glm::vec3 out_v = p2 - p;
      out_v.z = 0;
      const glm::vec3 in_v = -1.f * glm::normalize(out_v);

      float width = 3.3 + point.extra_width;
      Shape s2 = Hull(Cube(.1).Translate(p2), Cube(.1).Translate(p2 + (width * in_v)));

      std::vector<Shape> slice;
      slice.push_back(Hull(s1, s2));
      slice.push_back(Hull(s2, s2.Projection().LinearExtrude(.1).TranslateZ(.05)));

      wall_slices.push_back(slice);
    }

    for (size_t i = 0; i < wall_slices.size(); ++i) {
      auto& slice = wall_slices[i];
      auto& next_slice = wall_slices[(i + 1) % wall_slices.size()];
      for (size_t j = 0; j < slice.size(); ++j) {
        shapes.push_back(Hull(slice[j], next_slice[j]));
        // Uncomment for testing. Much faster and easier to visualize.
        // shapes.push_back(slice[j]);
      }
    }
  }

  // Add all the screw inserts.
  std::vector<Shape> screw_holes;
  std::vector<Shape> screw_holes_bottom;
  {
    double screw_height = 8;
    double screw_radius = 3.5 / 2.0;
    double screw_insert_radius = 4.3 / 2.0;
    Shape screw_hole = Cylinder(screw_height + 12, screw_insert_radius, 40);
    Shape screw_hole_bottom = Cylinder(screw_height + 12, screw_radius, 40);
    Shape screw_insert =
        Cylinder(screw_height, screw_insert_radius + 3, 30).TranslateZ(screw_height / 2);

    glm::vec3 screw_left_top = d.key_home.GetTopLeft().Apply(kOrigin);
    screw_left_top.z = 0;
    screw_left_top.x += 3;
    screw_left_top.y += -3;

    glm::vec3 screw_right_top = d.key_alt.GetTopRight().Apply(kOrigin);
    screw_right_top.z = 0;
    screw_right_top.x += -3.5;
    screw_right_top.y += -0.5;

    glm::vec3 screw_right_bottom = d.key_backspace.GetBottomRight().Apply(kOrigin);
    screw_right_bottom.z = 0;
    screw_right_bottom.x += -19;
    screw_right_bottom.y += 13;

    glm::vec3 screw_left_bottom = d.key_end.GetBottomRight().Apply(kOrigin);
    screw_left_bottom.z = 0;
    screw_left_bottom.x += -1.5;
    screw_left_bottom.y += 7;


   shapes.push_back(Union(
        screw_insert.Translate(screw_left_top),
        screw_insert.Translate(screw_right_top),
        screw_insert.Translate(screw_right_bottom),
        screw_insert.Translate(screw_left_bottom)
    ));
    screw_holes = {
        screw_hole.Translate(screw_left_top),
        screw_hole.Translate(screw_right_top),
        screw_hole.Translate(screw_right_bottom),
        screw_hole.Translate(screw_left_bottom)
    };
    screw_holes_bottom = {
        screw_hole_bottom.Translate(screw_left_top),
        screw_hole_bottom.Translate(screw_right_top),
        screw_hole_bottom.Translate(screw_right_bottom),
        screw_hole_bottom.Translate(screw_left_bottom)
    };
  }

  std::vector<Shape> negative_shapes;
  AddShapes(&negative_shapes, screw_holes);

  Shape result = UnionAll(shapes);
  // Subtracting is expensive to preview and is best to disable while testing.
  result = result.Subtract(UnionAll(negative_shapes));
  result.MirrorX().WriteToFile("macro_bae.scad");
  
  // Bottom plate
  {
    std::vector<Shape> bottom_plate_shapes = {result};
    for (Key* key : d.all_keys()) {
      bottom_plate_shapes.push_back(Hull(key->GetSwitch()));
    }
  
    Shape bottom_plate = UnionAll(bottom_plate_shapes)
                             .Projection()
                             .LinearExtrude(1.5)
                             .Subtract(UnionAll(screw_holes_bottom));
    bottom_plate.MirrorX().WriteToFile("bottom_macro_bae.scad");
  }

  return 0;
}

