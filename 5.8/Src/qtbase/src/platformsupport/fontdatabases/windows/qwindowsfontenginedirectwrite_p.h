/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWINDOWSFONTENGINEDIRECTWRITE_H
#define QWINDOWSFONTENGINEDIRECTWRITE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

#ifndef QT_NO_DIRECTWRITE

#include <QtGui/private/qfontengine_p.h>
#include <QtCore/QSharedPointer>

struct IDWriteFont;
struct IDWriteFontFace;
struct IDWriteFontFile;
struct IDWriteFactory;
struct IDWriteBitmapRenderTarget;
struct IDWriteGdiInterop;
struct IDWriteGlyphRunAnalysis;

QT_BEGIN_NAMESPACE

class QWindowsFontEngineData;

class QWindowsFontEngineDirectWrite : public QFontEngine
{
public:
    explicit QWindowsFontEngineDirectWrite(IDWriteFontFace *directWriteFontFace,
                                    qreal pixelSize,
                                    const QSharedPointer<QWindowsFontEngineData> &d);
    ~QWindowsFontEngineDirectWrite();

    void initFontInfo(const QFontDef &request, int dpi);

    QFixed lineThickness() const Q_DECL_OVERRIDE;
    QFixed underlinePosition() const Q_DECL_OVERRIDE;
    bool getSfntTableData(uint tag, uchar *buffer, uint *length) const Q_DECL_OVERRIDE;
    QFixed emSquareSize() const Q_DECL_OVERRIDE;

    glyph_t glyphIndex(uint ucs4) const Q_DECL_OVERRIDE;
    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                      ShaperFlags flags) const Q_DECL_OVERRIDE;
    void recalcAdvances(QGlyphLayout *glyphs, ShaperFlags) const Q_DECL_OVERRIDE;

    void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                         QPainterPath *path, QTextItem::RenderFlags flags) Q_DECL_OVERRIDE;

    glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) Q_DECL_OVERRIDE;
    glyph_metrics_t boundingBox(glyph_t g) Q_DECL_OVERRIDE;
    glyph_metrics_t alphaMapBoundingBox(glyph_t glyph, QFixed,
                                        const QTransform &matrix, GlyphFormat) Q_DECL_OVERRIDE;

    QFixed ascent() const Q_DECL_OVERRIDE;
    QFixed capHeight() const Q_DECL_OVERRIDE;
    QFixed descent() const Q_DECL_OVERRIDE;
    QFixed leading() const Q_DECL_OVERRIDE;
    QFixed xHeight() const Q_DECL_OVERRIDE;
    qreal maxCharWidth() const Q_DECL_OVERRIDE;
    FaceId faceId() const Q_DECL_OVERRIDE;

    bool supportsSubPixelPositions() const Q_DECL_OVERRIDE;

    QImage alphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition) Q_DECL_OVERRIDE;
    QImage alphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition, const QTransform &t) Q_DECL_OVERRIDE;
    QImage alphaRGBMapForGlyph(glyph_t t, QFixed subPixelPosition, const QTransform &xform) Q_DECL_OVERRIDE;
    QImage bitmapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t) Q_DECL_OVERRIDE;

    QFontEngine *cloneWithSize(qreal pixelSize) const Q_DECL_OVERRIDE;
    Qt::HANDLE handle() const Q_DECL_OVERRIDE;

    const QSharedPointer<QWindowsFontEngineData> &fontEngineData() const { return m_fontEngineData; }

    static QString fontNameSubstitute(const QString &familyName);

    IDWriteFontFace *directWriteFontFace() const { return m_directWriteFontFace; }

    void setUniqueFamilyName(const QString &newName) { m_uniqueFamilyName = newName; }

private:
    QImage imageForGlyph(glyph_t t, QFixed subPixelPosition, int margin, const QTransform &xform);
    void collectMetrics();
    void renderGlyphRun(QImage *destination, float r, float g, float b, float a, IDWriteGlyphRunAnalysis *glyphAnalysis, const QRect &boundingRect);
    static QString filenameFromFontFile(IDWriteFontFile *fontFile);

    const QSharedPointer<QWindowsFontEngineData> m_fontEngineData;

    IDWriteFontFace *m_directWriteFontFace;
    IDWriteBitmapRenderTarget *m_directWriteBitmapRenderTarget;

    QFixed m_lineThickness;
    QFixed m_underlinePosition;
    int m_unitsPerEm;
    QFixed m_ascent;
    QFixed m_capHeight;
    QFixed m_descent;
    QFixed m_xHeight;
    QFixed m_lineGap;
    FaceId m_faceId;
    QString m_uniqueFamilyName;
};

QT_END_NAMESPACE

#endif // QT_NO_DIRECTWRITE

#endif // QWINDOWSFONTENGINEDIRECTWRITE_H
