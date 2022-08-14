// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Colour functions
//


/**
 * ACES tone mapping curve fit to go from HDR to LDR
 * https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve
 *
 * @arg colour: The linear colour to tone map.
 *
 * @returns: The tone mapped colour.
 */
inline float3 ACESToneMap(const float3 &colour)
{
    return saturate(
        (colour * (2.51f * colour + 0.03f)) / (colour * (2.43f * colour + 0.59f) + 0.14f)
    );
}
