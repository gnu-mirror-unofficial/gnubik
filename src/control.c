/*
    Control functions for the Cube
    Copyright (C) 1998,  2003  John Darrington
                  2004  John Darrington,  Dale Mellor
		  2011  John Darrington

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <config.h>

#include "control.h"
#include "select.h"

#include "glarea.h"
#include "ui.h"

#include <math.h>

/* return the turn direction,  based upon the turn axis vector */
static int
turnDir (GLfloat * vector)
{
  /* This is a horrendous kludge.  It works only because we know that
     vector is arithemtically very simple */

  return (vector[0] + vector[1] + vector[2] > 0);
}


/* Convert a vector to a axis number.  Obviously this assumes the vector
is orthogonal to the frame of reference ( another nasty kludge ). */
static int
vector2axis (GLfloat * vector)
{
  return vector[0] != 0 ? 0 : vector[1] != 0 ? 1 : 2;
}

#define X {1, 0, 0, 0}
#define _X {-1, 0, 0, 0}
#define Y {0, 1, 0, 0}
#define _Y {0, -1, 0, 0}
#define Z {0, 0, 1, 0}
#define _Z {0, 0, -1, 0}


/* Determine the axis about which to rotate the slice,  from the objects
selected by the cursor position */
static void
getTurnAxis (const struct facet_selection *items, GLfloat * vector)
{
  Matrix t;

  /* Each edge (quadrant) on a block represents a different axis */
  const GLfloat axes[6][4][4] = {
    {_Y, X, Y, _X},
    {Y, X, _Y, _X},
    {Z, X, _Z, _X},
    {_Z, X, Z, _X},
    {_Y, _Z, Y, Z},
    {Y, _Z, _Y, Z},
  };

  /* Fetch the selected block's transformation from its original
     orientation */
  if (get_block_transform (the_cube, items->block, t) != 0)
    {
      fprintf (stderr, "Attempt to fetch non-existant block transform\n");
      exit (1);
    }

  /* Select the untransformed vector for the selected edge
     and transform it,  so that we go the right way */
  transform (t, axes[items->face][items->quadrant], vector);
}

float cursorAngle;

/* 
   This func is called whenever a new set of polygons have been selected.
 */
void
selection_func (gpointer data)
{
  struct move_data *pending_movement = data;
  const struct facet_selection *selection = 0;

  if (is_animating ())
    return;

  if (0 != (selection = select_get (the_cublet_selection)))
    {
      GLfloat turn_axis[4];
      vector v;

      getTurnAxis (selection, turn_axis);
      pending_movement->axis = vector2axis (turn_axis);


      pending_movement->dir = turnDir (turn_axis);


      /* !!!!!! We are accessing private cube data. */
      pending_movement->slice
	=
	the_cube->blocks[selection->block].transformation[12 +
							  pending_movement->axis];
      
      /* Default to a turn of 90 degrees */
      pending_movement->turns = 1;


      /* Here we take the orientation of the selected quadrant and multiply it
         by the projection matrix.  The result gives us the angle (on the screen)
	 at which the mouse cursor needs to be drawn. */
      {
	Matrix proj;

	glGetFloatv (GL_PROJECTION_MATRIX, proj);

	get_quadrant_vector (the_cube, selection->block,
			     selection->face, selection->quadrant, v);

	transform_in_place (proj, v);

	cursorAngle = atan2 (v[0], v[1]) * 180.0 / M_PI;
      }
    }
  else
    {
      pending_movement->dir = -1;
      pending_movement->axis = -1;
    }

  postRedisplay (the_display_context);
}