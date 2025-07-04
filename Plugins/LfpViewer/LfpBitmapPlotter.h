/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2024 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __LFPBITMAPPLOTTER_H__
#define __LFPBITMAPPLOTTER_H__

#include <VisualizerWindowHeaders.h>

#include <array>
#include <vector>

#include "LfpDisplayClasses.h"
#include "LfpDisplayNode.h"

namespace LfpViewer
{

/**
    Interface class for different plotting methods.
 */
class LfpBitmapPlotter
{
public:
    /** Constructor */
    LfpBitmapPlotter (LfpDisplay* lfpDisplay) : display (lfpDisplay) {}

    /** Destructor */
    virtual ~LfpBitmapPlotter() {}

    /** Plots one subsample of data from a single channel to the bitmap provided */
    virtual void plot (Image::BitmapData& bitmapData, LfpBitmapPlotterInfo& plotterInfo) = 0;

protected:
    LfpDisplay* display;
};

}; // namespace LfpViewer
#endif
