#include "colors.inc"

#include "stones.inc"

camera
{
  look_at <0, 1, 2>
  location <0, 2, -3>
}

sphere
{
  <0, 1, 2>, 2
  texture
  {
    pigment
    {
      color Yellow
    }
  }
}

light_source
{
  <2, 4, -3>
  color White
}

