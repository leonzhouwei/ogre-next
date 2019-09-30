/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-present Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef _OgreIrradianceField_H_
#define _OgreIrradianceField_H_

#include "OgreHlmsPbsPrerequisites.h"

#include "OgreId.h"
#include "OgreVector3.h"

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    struct IrradianceFieldSettings
    {
        /** Number of rays per pixel in terms of mDepthProbeResolution.

            e.g. if mNumRaysPerPixel = 4, mDepthProbeResolution = 16 and mIrradianceResolution = 8,
            then the depth texture has 4 rays per pixel, but the irradiance texture will have
            16 rays per pixel
        */
        uint16 mNumRaysPerPixel;
        /// Square resolution of a single probe, depth variance, e.g. 8u means each probe is 8x8.
        uint8 mDepthProbeResolution;
        /// Square resolution of a single probe, irradiance e.g. 4u means each probe is 4x4.
        /// Must be mIrradianceResolution <= mDepthProbeResolution
        /// mDepthProbeResolution must be multiple of mIrradianceResolution
        uint8 mIrradianceResolution;

        /// Number of probes in all three XYZ axes.
        /// Must be power of two
        uint32 mNumProbes[3];

    protected:
        /// mSubsamples.size() == mNumRaysPerPixel
        /// Contains the offsets for generating the rays
        vector<Vector2>::type mSubsamples;

    public:
        IrradianceFieldSettings();

        void testValidity( void )
        {
            OGRE_ASSERT_LOW( mIrradianceResolution <= mDepthProbeResolution );
            OGRE_ASSERT_LOW( ( mDepthProbeResolution % mIrradianceResolution ) == 0u );
            for( size_t i = 0u; i < 3u; ++i )
            {
                OGRE_ASSERT_LOW( ( mNumProbes[i] & ( mNumProbes[i] - 1u ) ) == 0u &&
                                 "Num probes must be a power of 2" );
            }
        }

        uint32 getTotalNumProbes( void ) const;
        uint32 getDepthProbeFullResolution( void ) const;
        uint32 getIrradProbeFullResolution( void ) const;

        const vector<Vector2>::type &getSubsamples( void ) const { return mSubsamples; }
    };

    class _OgreHlmsPbsExport IrradianceField : public IdObject
    {
        IrradianceFieldSettings mSettings;
        /// Number of probes processed so far.
        /// We process the entire field across multiple frames.
        uint32 mNumProbesProcessed;

        Vector3 mFieldOrigin;
        Vector3 mFieldSize;

        TextureGpu *mIrradianceTex;
        TextureGpu *mDepthVarianceTex;

        HlmsComputeJob *mGenerationJob;

        TexBufferPacked *mDirectionsBuffer;

        TextureGpuManager *mTextureManager;

        void fillDirections( float *RESTRICT_ALIAS outBuffer );

    public:
        IrradianceField();
        ~IrradianceField();

        void createTextures( void );
        void destroyTextures( void );

        /**
         * @brief initialize
         * @param settings
         * @param fieldOrigin
         * @param fieldSize
         */
        void initialize( const IrradianceFieldSettings &settings, const Vector3 &fieldOrigin,
                         const Vector3 &fieldSize );

        void update( uint32 probesPerFrame = 200u );
    };
}  // namespace Ogre

#include "OgreHeaderSuffix.h"

#endif