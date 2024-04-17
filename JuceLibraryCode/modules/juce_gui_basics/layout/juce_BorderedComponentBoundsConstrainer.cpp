/*
  ==============================================================================

   This file is part of the JUCE 8 technical preview.
   Copyright (c) Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

void BorderedComponentBoundsConstrainer::checkBounds (Rectangle<int>& bounds,
                                                      const Rectangle<int>& previousBounds,
                                                      const Rectangle<int>& limits,
                                                      bool isStretchingTop,
                                                      bool isStretchingLeft,
                                                      bool isStretchingBottom,
                                                      bool isStretchingRight)
{
    if (auto* decorated = getWrappedConstrainer())
    {
        const auto border = getAdditionalBorder();
        const auto requestedBounds = bounds;

        border.subtractFrom (bounds);
        decorated->checkBounds (bounds,
                                border.subtractedFrom (previousBounds),
                                limits,
                                isStretchingTop,
                                isStretchingLeft,
                                isStretchingBottom,
                                isStretchingRight);
        border.addTo (bounds);
        bounds = bounds.withPosition (requestedBounds.getPosition());

        if (isStretchingTop && ! isStretchingBottom)
            bounds = bounds.withBottomY (previousBounds.getBottom());

        if (! isStretchingTop && isStretchingBottom)
            bounds = bounds.withY (previousBounds.getY());

        if (isStretchingLeft && ! isStretchingRight)
            bounds = bounds.withRightX (previousBounds.getRight());

        if (! isStretchingLeft && isStretchingRight)
            bounds = bounds.withX (previousBounds.getX());
    }
    else
    {
        ComponentBoundsConstrainer::checkBounds (bounds,
                                                 previousBounds,
                                                 limits,
                                                 isStretchingTop,
                                                 isStretchingLeft,
                                                 isStretchingBottom,
                                                 isStretchingRight);
    }
}

} // namespace juce
