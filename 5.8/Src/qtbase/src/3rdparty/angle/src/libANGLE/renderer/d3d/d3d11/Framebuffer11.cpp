//
// Copyright 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Framebuffer11.cpp: Implements the Framebuffer11 class.

#include "libANGLE/renderer/d3d/d3d11/Framebuffer11.h"

#include "common/debug.h"
#include "libANGLE/renderer/d3d/d3d11/Buffer11.h"
#include "libANGLE/renderer/d3d/d3d11/Clear11.h"
#include "libANGLE/renderer/d3d/d3d11/TextureStorage11.h"
#include "libANGLE/renderer/d3d/d3d11/Renderer11.h"
#include "libANGLE/renderer/d3d/d3d11/renderer11_utils.h"
#include "libANGLE/renderer/d3d/d3d11/RenderTarget11.h"
#include "libANGLE/renderer/d3d/d3d11/formatutils11.h"
#include "libANGLE/renderer/d3d/TextureD3D.h"
#include "libANGLE/Framebuffer.h"
#include "libANGLE/FramebufferAttachment.h"
#include "libANGLE/Texture.h"

namespace rx
{

Framebuffer11::Framebuffer11(const gl::Framebuffer::Data &data, Renderer11 *renderer)
    : FramebufferD3D(data, renderer), mRenderer(renderer)
{
    ASSERT(mRenderer != nullptr);
}

Framebuffer11::~Framebuffer11()
{
}

static gl::Error InvalidateAttachmentSwizzles(const gl::FramebufferAttachment *attachment)
{
    if (attachment && attachment->type() == GL_TEXTURE)
    {
        gl::Texture *texture = attachment->getTexture();

        TextureD3D *textureD3D = GetImplAs<TextureD3D>(texture);

        TextureStorage *texStorage = nullptr;
        gl::Error error = textureD3D->getNativeTexture(&texStorage);
        if (error.isError())
        {
            return error;
        }

        if (texStorage)
        {
            TextureStorage11 *texStorage11 = GetAs<TextureStorage11>(texStorage);
            ASSERT(texStorage11);

            texStorage11->invalidateSwizzleCacheLevel(attachment->mipLevel());
        }
    }

    return gl::Error(GL_NO_ERROR);
}

gl::Error Framebuffer11::invalidateSwizzles() const
{
    for (const auto &colorAttachment : mData.getColorAttachments())
    {
        if (colorAttachment.isAttached())
        {
            gl::Error error = InvalidateAttachmentSwizzles(&colorAttachment);
            if (error.isError())
            {
                return error;
            }
        }
    }

    gl::Error error = InvalidateAttachmentSwizzles(mData.getDepthAttachment());
    if (error.isError())
    {
        return error;
    }

    error = InvalidateAttachmentSwizzles(mData.getStencilAttachment());
    if (error.isError())
    {
        return error;
    }

    return gl::Error(GL_NO_ERROR);
}

gl::Error Framebuffer11::clear(const gl::Data &data, const ClearParameters &clearParams)
{
    Clear11 *clearer = mRenderer->getClearer();
    gl::Error error(GL_NO_ERROR);

    const gl::FramebufferAttachment *colorAttachment = mData.getFirstColorAttachment();
    if (clearParams.scissorEnabled == true && colorAttachment != nullptr &&
        UsePresentPathFast(mRenderer, colorAttachment))
    {
        // If the current framebuffer is using the default colorbuffer, and present path fast is
        // active, and the scissor rect is enabled, then we should invert the scissor rect
        // vertically
        ClearParameters presentPathFastClearParams = clearParams;
        gl::Extents framebufferSize                = colorAttachment->getSize();
        presentPathFastClearParams.scissor.y       = framebufferSize.height -
                                               presentPathFastClearParams.scissor.y -
                                               presentPathFastClearParams.scissor.height;
        error = clearer->clearFramebuffer(presentPathFastClearParams, mData);
    }
    else
    {
        error = clearer->clearFramebuffer(clearParams, mData);
    }

    if (error.isError())
    {
        return error;
    }

    error = invalidateSwizzles();
    if (error.isError())
    {
        return error;
    }

    return gl::Error(GL_NO_ERROR);
}

gl::Error Framebuffer11::invalidate(size_t count, const GLenum *attachments)
{
    return invalidateBase(count, attachments, false);
}

gl::Error Framebuffer11::discard(size_t count, const GLenum *attachments)
{
    return invalidateBase(count, attachments, true);
}

gl::Error Framebuffer11::invalidateBase(size_t count, const GLenum *attachments, bool useEXTBehavior) const
{
#if defined(ANGLE_ENABLE_D3D11_1)
    ID3D11DeviceContext1 *deviceContext1 = mRenderer->getDeviceContext1IfSupported();

    if (!deviceContext1)
    {
        // DiscardView() is only supported on ID3D11DeviceContext1
        return gl::Error(GL_NO_ERROR);
    }

    bool foundDepth = false;
    bool foundStencil = false;

    for (size_t i = 0; i < count; ++i)
    {
        switch (attachments[i])
        {
          // Handle depth and stencil attachments. Defer discarding until later.
          case GL_DEPTH_STENCIL_ATTACHMENT:
            foundDepth = true;
            foundStencil = true;
            break;
          case GL_DEPTH_EXT:
          case GL_DEPTH_ATTACHMENT:
            foundDepth = true;
            break;
          case GL_STENCIL_EXT:
          case GL_STENCIL_ATTACHMENT:
            foundStencil = true;
            break;
          default:
            {
                // Handle color attachments
                ASSERT((attachments[i] >= GL_COLOR_ATTACHMENT0 && attachments[i] <= GL_COLOR_ATTACHMENT15) ||
                       (attachments[i] == GL_COLOR));

                RenderTarget11 *renderTarget = nullptr;
                ID3D11View *colorView = nullptr;
                gl::Error error(GL_NO_ERROR);
                size_t colorAttachmentID = 0;

                if (attachments[i] == GL_COLOR)
                {
                    colorAttachmentID = 0;
                }
                else
                {
                    colorAttachmentID = attachments[i] - GL_COLOR_ATTACHMENT0;
                }

                if (mData.getColorAttachment(static_cast<unsigned int>(colorAttachmentID)))
                {
                    error = mData.getColorAttachment(static_cast<unsigned int>(colorAttachmentID))
                                ->getRenderTarget(&renderTarget);
                    if (error.isError())
                    {
                        return error;
                    }

                    colorView = renderTarget->getRenderTargetView();

                    if (colorView != nullptr)
                    {
                        deviceContext1->DiscardView(colorView);
                    }
                }

                break;
            }
        }
    }

    bool discardDepth = false;
    bool discardStencil = false;

    // The D3D11 renderer uses the same view for depth and stencil buffers, so we must be careful.
    if (useEXTBehavior)
    {
        // In the extension, if the app discards only one of the depth and stencil attachments, but
        // those are backed by the same packed_depth_stencil buffer, then both images become undefined.
        discardDepth = foundDepth;

        // Don't bother discarding the stencil buffer if the depth buffer will already do it
        discardStencil = foundStencil && (!discardDepth || mData.getDepthAttachment() == nullptr);
    }
    else
    {
        // In ES 3.0.4, if a specified attachment has base internal format DEPTH_STENCIL but the
        // attachments list does not include DEPTH_STENCIL_ATTACHMENT or both DEPTH_ATTACHMENT and
        // STENCIL_ATTACHMENT, then only the specified portion of every pixel in the subregion of pixels
        // of the DEPTH_STENCIL buffer may be invalidated, and the other portion must be preserved.
        discardDepth = (foundDepth && foundStencil) || (foundDepth && (mData.getStencilAttachment() == nullptr));
        discardStencil = (foundStencil && (mData.getDepthAttachment() == nullptr));
    }

    if (discardDepth && mData.getDepthAttachment())
    {
        RenderTarget11 *renderTarget = nullptr;
        ID3D11View *depthView = nullptr;
        gl::Error error(GL_NO_ERROR);

        error = mData.getDepthAttachment()->getRenderTarget(&renderTarget);
        if (error.isError())
        {
            return error;
        }

        depthView = renderTarget->getDepthStencilView();

        if (depthView != nullptr)
        {
            deviceContext1->DiscardView(depthView);
        }
    }

    if (discardStencil && mData.getStencilAttachment())
    {
        RenderTarget11 *renderTarget = nullptr;
        ID3D11View *stencilView = nullptr;
        gl::Error error(GL_NO_ERROR);

        error = mData.getStencilAttachment()->getRenderTarget(&renderTarget);
        if (error.isError())
        {
            return error;
        }

        stencilView = renderTarget->getDepthStencilView();

        if (stencilView != nullptr)
        {
            deviceContext1->DiscardView(stencilView);
        }
    }
#endif // ANGLE_ENABLE_D3D11_1

    return gl::Error(GL_NO_ERROR);
}

gl::Error Framebuffer11::invalidateSub(size_t, const GLenum *, const gl::Rectangle &)
{
    // A no-op implementation conforms to the spec, so don't call UNIMPLEMENTED()
    return gl::Error(GL_NO_ERROR);
}

gl::Error Framebuffer11::readPixelsImpl(const gl::Rectangle &area,
                                        GLenum format,
                                        GLenum type,
                                        size_t outputPitch,
                                        const gl::PixelPackState &pack,
                                        uint8_t *pixels) const
{
    const gl::FramebufferAttachment *readAttachment = mData.getReadAttachment();
    ASSERT(readAttachment);

    gl::Buffer *packBuffer = pack.pixelBuffer.get();
    if (packBuffer != nullptr)
    {
        if (pack.rowLength != 0 || pack.skipRows != 0 || pack.skipPixels != 0)
        {
            UNIMPLEMENTED();
            return gl::Error(GL_INVALID_OPERATION,
                             "Unimplemented pixel store parameters in readPixelsImpl");
        }

        Buffer11 *packBufferStorage = GetImplAs<Buffer11>(packBuffer);
        PackPixelsParams packParams(area, format, type, static_cast<GLuint>(outputPitch), pack,
                                    reinterpret_cast<ptrdiff_t>(pixels));

        return packBufferStorage->packPixels(*readAttachment, packParams);
    }

    return mRenderer->readFromAttachment(*readAttachment, area, format, type,
                                         static_cast<GLuint>(outputPitch), pack, pixels);
}

gl::Error Framebuffer11::blit(const gl::Rectangle &sourceArea, const gl::Rectangle &destArea, const gl::Rectangle *scissor,
                              bool blitRenderTarget, bool blitDepth, bool blitStencil, GLenum filter,
                              const gl::Framebuffer *sourceFramebuffer)
{
    if (blitRenderTarget)
    {
        const gl::FramebufferAttachment *readBuffer = sourceFramebuffer->getReadColorbuffer();
        ASSERT(readBuffer);

        RenderTargetD3D *readRenderTarget = nullptr;
        gl::Error error = readBuffer->getRenderTarget(&readRenderTarget);
        if (error.isError())
        {
            return error;
        }
        ASSERT(readRenderTarget);

        const auto &colorAttachments = mData.getColorAttachments();
        const auto &drawBufferStates = mData.getDrawBufferStates();

        for (size_t colorAttachment = 0; colorAttachment < colorAttachments.size(); colorAttachment++)
        {
            const gl::FramebufferAttachment &drawBuffer = colorAttachments[colorAttachment];

            if (drawBuffer.isAttached() &&
                drawBufferStates[colorAttachment] != GL_NONE)
            {
                RenderTargetD3D *drawRenderTarget = nullptr;
                error = drawBuffer.getRenderTarget(&drawRenderTarget);
                if (error.isError())
                {
                    return error;
                }
                ASSERT(drawRenderTarget);

                const bool invertColorSource   = UsePresentPathFast(mRenderer, readBuffer);
                gl::Rectangle actualSourceArea = sourceArea;
                if (invertColorSource)
                {
                    RenderTarget11 *readRenderTarget11 = GetAs<RenderTarget11>(readRenderTarget);
                    actualSourceArea.y                 = readRenderTarget11->getHeight() - sourceArea.y;
                    actualSourceArea.height            = -sourceArea.height;
                }

                const bool invertColorDest   = UsePresentPathFast(mRenderer, &drawBuffer);
                gl::Rectangle actualDestArea = destArea;
                if (invertColorDest)
                {
                    RenderTarget11 *drawRenderTarget11 = GetAs<RenderTarget11>(drawRenderTarget);
                    actualDestArea.y                   = drawRenderTarget11->getHeight() - destArea.y;
                    actualDestArea.height              = -destArea.height;
                }

                error = mRenderer->blitRenderbufferRect(actualSourceArea, actualDestArea,
                                                        readRenderTarget, drawRenderTarget, filter,
                                                        scissor, blitRenderTarget, false, false);
                if (error.isError())
                {
                    return error;
                }
            }
        }
    }

    if (blitDepth || blitStencil)
    {
        const gl::FramebufferAttachment *readBuffer = sourceFramebuffer->getDepthOrStencilbuffer();
        ASSERT(readBuffer);

        RenderTargetD3D *readRenderTarget = nullptr;
        gl::Error error = readBuffer->getRenderTarget(&readRenderTarget);
        if (error.isError())
        {
            return error;
        }
        ASSERT(readRenderTarget);

        const gl::FramebufferAttachment *drawBuffer = mData.getDepthOrStencilAttachment();
        ASSERT(drawBuffer);

        RenderTargetD3D *drawRenderTarget = nullptr;
        error = drawBuffer->getRenderTarget(&drawRenderTarget);
        if (error.isError())
        {
            return error;
        }
        ASSERT(drawRenderTarget);

        error = mRenderer->blitRenderbufferRect(sourceArea, destArea, readRenderTarget, drawRenderTarget, filter, scissor,
                                                false, blitDepth, blitStencil);
        if (error.isError())
        {
            return error;
        }
    }

    gl::Error error = invalidateSwizzles();
    if (error.isError())
    {
        return error;
    }

    return gl::Error(GL_NO_ERROR);
}

GLenum Framebuffer11::getRenderTargetImplementationFormat(RenderTargetD3D *renderTarget) const
{
    RenderTarget11 *renderTarget11 = GetAs<RenderTarget11>(renderTarget);
    const d3d11::DXGIFormat &dxgiFormatInfo = d3d11::GetDXGIFormatInfo(renderTarget11->getDXGIFormat());
    return dxgiFormatInfo.internalFormat;
}

}
