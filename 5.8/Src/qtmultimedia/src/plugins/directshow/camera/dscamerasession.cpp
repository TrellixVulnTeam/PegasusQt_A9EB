/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
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

#include <QtCore/qdebug.h>
#include <QFile>
#include <QtConcurrent/QtConcurrentRun>
#include <QtMultimedia/qabstractvideobuffer.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#include <QtMultimedia/qcameraimagecapture.h>
#include <private/qmemoryvideobuffer_p.h>

#include "dscamerasession.h"
#include "dsvideorenderer.h"
#include "directshowcameraglobal.h"

QT_BEGIN_NAMESPACE


namespace {
// DirectShow helper implementation
void _CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
    *pmtTarget = *pmtSource;
    if (pmtTarget->cbFormat != 0) {
        pmtTarget->pbFormat = reinterpret_cast<BYTE *>(CoTaskMemAlloc(pmtTarget->cbFormat));
        if (pmtTarget->pbFormat)
            memcpy(pmtTarget->pbFormat, pmtSource->pbFormat, pmtTarget->cbFormat);
    }
    if (pmtTarget->pUnk != NULL) {
        // pUnk should not be used.
        pmtTarget->pUnk->AddRef();
    }
}

void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL) {
        // pUnk should not be used.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}
} // end namespace

static HRESULT getPin(IBaseFilter *filter, PIN_DIRECTION pinDir, IPin **pin);


class SampleGrabberCallbackPrivate : public ISampleGrabberCB
{
public:
    explicit SampleGrabberCallbackPrivate(DSCameraSession *session)
        : m_ref(1)
        , m_session(session)
    { }

    virtual ~SampleGrabberCallbackPrivate() { }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_ref);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0)
            delete this;
        return ref;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        if (NULL == ppvObject)
            return E_POINTER;
        if (riid == IID_IUnknown /*__uuidof(IUnknown) */ ) {
            *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if (riid == IID_ISampleGrabberCB /*__uuidof(ISampleGrabberCB)*/ ) {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        Q_UNUSED(Time)
        Q_UNUSED(pSample)
        return E_NOTIMPL;
    }

    STDMETHODIMP BufferCB(double time, BYTE *pBuffer, long bufferLen)
    {
        // We display frames as they arrive, the presentation time is
        // irrelevant
        Q_UNUSED(time);

        if (m_session) {
           m_session->onFrameAvailable(reinterpret_cast<const char *>(pBuffer),
                                       bufferLen);
        }

        return S_OK;
    }

private:
    ULONG m_ref;
    DSCameraSession *m_session;
};

QVideoFrame::PixelFormat pixelFormatFromMediaSubtype(GUID uid)
{
    if (uid == MEDIASUBTYPE_ARGB32)
        return QVideoFrame::Format_ARGB32;
    else if (uid == MEDIASUBTYPE_RGB32)
        return QVideoFrame::Format_RGB32;
    else if (uid == MEDIASUBTYPE_RGB24)
        return QVideoFrame::Format_RGB24;
    else if (uid == MEDIASUBTYPE_RGB565)
        return QVideoFrame::Format_RGB565;
    else if (uid == MEDIASUBTYPE_RGB555)
        return QVideoFrame::Format_RGB555;
    else if (uid == MEDIASUBTYPE_AYUV)
        return QVideoFrame::Format_AYUV444;
    else if (uid == MEDIASUBTYPE_I420 || uid == MEDIASUBTYPE_IYUV)
        return QVideoFrame::Format_YUV420P;
    else if (uid == MEDIASUBTYPE_YV12)
        return QVideoFrame::Format_YV12;
    else if (uid == MEDIASUBTYPE_UYVY)
        return QVideoFrame::Format_UYVY;
    else if (uid == MEDIASUBTYPE_YUYV || uid == MEDIASUBTYPE_YUY2)
        return QVideoFrame::Format_YUYV;
    else if (uid == MEDIASUBTYPE_NV12)
        return QVideoFrame::Format_NV12;
    else if (uid == MEDIASUBTYPE_MJPG)
        return QVideoFrame::Format_Jpeg;
    else if (uid == MEDIASUBTYPE_IMC1)
        return QVideoFrame::Format_IMC1;
    else if (uid == MEDIASUBTYPE_IMC2)
        return QVideoFrame::Format_IMC2;
    else if (uid == MEDIASUBTYPE_IMC3)
        return QVideoFrame::Format_IMC3;
    else if (uid == MEDIASUBTYPE_IMC4)
        return QVideoFrame::Format_IMC4;
    else
        return QVideoFrame::Format_Invalid;
}


DSCameraSession::DSCameraSession(QObject *parent)
    : QObject(parent)
    , m_graphBuilder(Q_NULLPTR)
    , m_filterGraph(Q_NULLPTR)
    , m_sourceDeviceName(QLatin1String("default"))
    , m_sourceFilter(Q_NULLPTR)
    , m_needsHorizontalMirroring(false)
    , m_previewFilter(Q_NULLPTR)
    , m_previewSampleGrabber(Q_NULLPTR)
    , m_nullRendererFilter(Q_NULLPTR)
    , m_previewStarted(false)
    , m_surface(Q_NULLPTR)
    , m_previewPixelFormat(QVideoFrame::Format_Invalid)
    , m_readyForCapture(false)
    , m_imageIdCounter(0)
    , m_currentImageId(-1)
    , m_status(QCamera::UnloadedStatus)
{
    ZeroMemory(&m_sourceFormat, sizeof(m_sourceFormat));

    connect(this, SIGNAL(statusChanged(QCamera::Status)),
            this, SLOT(updateReadyForCapture()));
}

DSCameraSession::~DSCameraSession()
{
    unload();
}

void DSCameraSession::setSurface(QAbstractVideoSurface* surface)
{
    m_surface = surface;
}

void DSCameraSession::setDevice(const QString &device)
{
    m_sourceDeviceName = device;
}

QCameraViewfinderSettings DSCameraSession::viewfinderSettings() const
{
    return m_status == QCamera::ActiveStatus ? m_actualViewfinderSettings : m_viewfinderSettings;
}

void DSCameraSession::setViewfinderSettings(const QCameraViewfinderSettings &settings)
{
    m_viewfinderSettings = settings;
}

qreal DSCameraSession::scaledImageProcessingParameterValue(
        const ImageProcessingParameterInfo &sourceValueInfo)
{
    if (sourceValueInfo.currentValue == sourceValueInfo.defaultValue) {
        return 0.0f;
    } else if (sourceValueInfo.currentValue < sourceValueInfo.defaultValue) {
        return ((sourceValueInfo.currentValue - sourceValueInfo.minimumValue)
                / qreal(sourceValueInfo.defaultValue - sourceValueInfo.minimumValue))
                + (-1.0f);
    } else {
        return ((sourceValueInfo.currentValue - sourceValueInfo.defaultValue)
                / qreal(sourceValueInfo.maximumValue - sourceValueInfo.defaultValue));
    }
}

qint32 DSCameraSession::sourceImageProcessingParameterValue(
        qreal scaledValue, const ImageProcessingParameterInfo &valueRange)
{
    if (qFuzzyIsNull(scaledValue)) {
        return valueRange.defaultValue;
    } else if (scaledValue < 0.0f) {
        return ((scaledValue - (-1.0f)) * (valueRange.defaultValue - valueRange.minimumValue))
                + valueRange.minimumValue;
    } else {
        return (scaledValue * (valueRange.maximumValue - valueRange.defaultValue))
                + valueRange.defaultValue;
    }
}

static QCameraImageProcessingControl::ProcessingParameter searchRelatedResultingParameter(
        QCameraImageProcessingControl::ProcessingParameter sourceParameter)
{
    if (sourceParameter == QCameraImageProcessingControl::WhiteBalancePreset)
        return QCameraImageProcessingControl::ColorTemperature;
    return sourceParameter;
}

bool DSCameraSession::isImageProcessingParameterSupported(
        QCameraImageProcessingControl::ProcessingParameter parameter) const
{
    const QCameraImageProcessingControl::ProcessingParameter resultingParameter =
            searchRelatedResultingParameter(parameter);

    return m_imageProcessingParametersInfos.contains(resultingParameter);
}

bool DSCameraSession::isImageProcessingParameterValueSupported(
        QCameraImageProcessingControl::ProcessingParameter parameter,
        const QVariant &value) const
{
    const QCameraImageProcessingControl::ProcessingParameter resultingParameter =
            searchRelatedResultingParameter(parameter);

    QMap<QCameraImageProcessingControl::ProcessingParameter,
            ImageProcessingParameterInfo>::const_iterator sourceValueInfo =
            m_imageProcessingParametersInfos.constFind(resultingParameter);

    if (sourceValueInfo == m_imageProcessingParametersInfos.constEnd())
        return false;

    switch (parameter) {

    case QCameraImageProcessingControl::WhiteBalancePreset: {
        const QCameraImageProcessing::WhiteBalanceMode checkedValue =
                value.value<QCameraImageProcessing::WhiteBalanceMode>();
        // Supports only the Manual and the Auto values
        if (checkedValue != QCameraImageProcessing::WhiteBalanceManual
                && checkedValue != QCameraImageProcessing::WhiteBalanceAuto) {
            return false;
        }
    }
        break;

    case QCameraImageProcessingControl::ColorTemperature: {
        const qint32 checkedValue = value.toInt();
        if (checkedValue < (*sourceValueInfo).minimumValue
                || checkedValue > (*sourceValueInfo).maximumValue) {
            return false;
        }
    }
        break;

    case QCameraImageProcessingControl::ContrastAdjustment: // falling back
    case QCameraImageProcessingControl::SaturationAdjustment: // falling back
    case QCameraImageProcessingControl::BrightnessAdjustment: // falling back
    case QCameraImageProcessingControl::SharpeningAdjustment: {
        const qint32 sourceValue = sourceImageProcessingParameterValue(
                    value.toReal(), (*sourceValueInfo));
        if (sourceValue < (*sourceValueInfo).minimumValue
                || sourceValue > (*sourceValueInfo).maximumValue)
            return false;
    }
        break;

    default:
        return false;
    }

    return true;
}

QVariant DSCameraSession::imageProcessingParameter(
        QCameraImageProcessingControl::ProcessingParameter parameter) const
{
    if (!m_graphBuilder) {
        qWarning() << "failed to access to the graph builder";
        return QVariant();
    }

    const QCameraImageProcessingControl::ProcessingParameter resultingParameter =
            searchRelatedResultingParameter(parameter);

    QMap<QCameraImageProcessingControl::ProcessingParameter,
            ImageProcessingParameterInfo>::const_iterator sourceValueInfo =
            m_imageProcessingParametersInfos.constFind(resultingParameter);

    if (sourceValueInfo == m_imageProcessingParametersInfos.constEnd())
        return QVariant();

    switch (parameter) {

    case QCameraImageProcessingControl::WhiteBalancePreset:
        return QVariant::fromValue<QCameraImageProcessing::WhiteBalanceMode>(
                    (*sourceValueInfo).capsFlags == VideoProcAmp_Flags_Auto
                    ? QCameraImageProcessing::WhiteBalanceAuto
                    : QCameraImageProcessing::WhiteBalanceManual);

    case QCameraImageProcessingControl::ColorTemperature:
        return QVariant::fromValue<qint32>((*sourceValueInfo).currentValue);

    case QCameraImageProcessingControl::ContrastAdjustment: // falling back
    case QCameraImageProcessingControl::SaturationAdjustment: // falling back
    case QCameraImageProcessingControl::BrightnessAdjustment: // falling back
    case QCameraImageProcessingControl::SharpeningAdjustment:
        return scaledImageProcessingParameterValue((*sourceValueInfo));

    default:
        return QVariant();
    }
}

void DSCameraSession::setImageProcessingParameter(
        QCameraImageProcessingControl::ProcessingParameter parameter,
        const QVariant &value)
{
    if (!m_graphBuilder) {
        qWarning() << "failed to access to the graph builder";
        return;
    }

    const QCameraImageProcessingControl::ProcessingParameter resultingParameter =
            searchRelatedResultingParameter(parameter);

    QMap<QCameraImageProcessingControl::ProcessingParameter,
            ImageProcessingParameterInfo>::iterator sourceValueInfo =
            m_imageProcessingParametersInfos.find(resultingParameter);

    if (sourceValueInfo == m_imageProcessingParametersInfos.constEnd())
        return;

    LONG sourceValue = 0;
    LONG capsFlags = VideoProcAmp_Flags_Manual;

    switch (parameter) {

    case QCameraImageProcessingControl::WhiteBalancePreset: {
        const QCameraImageProcessing::WhiteBalanceMode checkedValue =
                value.value<QCameraImageProcessing::WhiteBalanceMode>();
        // Supports only the Manual and the Auto values
        if (checkedValue == QCameraImageProcessing::WhiteBalanceManual)
            capsFlags = VideoProcAmp_Flags_Manual;
        else if (checkedValue == QCameraImageProcessing::WhiteBalanceAuto)
            capsFlags = VideoProcAmp_Flags_Auto;
        else
            return;

        sourceValue = ((*sourceValueInfo).hasBeenExplicitlySet)
                ? (*sourceValueInfo).currentValue
                : (*sourceValueInfo).defaultValue;
    }
        break;

    case QCameraImageProcessingControl::ColorTemperature:
        sourceValue = value.isValid() ?
                    value.value<qint32>() : (*sourceValueInfo).defaultValue;
        capsFlags = (*sourceValueInfo).capsFlags;
        break;

    case QCameraImageProcessingControl::ContrastAdjustment: // falling back
    case QCameraImageProcessingControl::SaturationAdjustment: // falling back
    case QCameraImageProcessingControl::BrightnessAdjustment: // falling back
    case QCameraImageProcessingControl::SharpeningAdjustment:
        if (value.isValid()) {
            sourceValue = sourceImageProcessingParameterValue(
                        value.toReal(), (*sourceValueInfo));
        } else {
            sourceValue = (*sourceValueInfo).defaultValue;
        }
        break;

    default:
        return;
    }

    IAMVideoProcAmp *pVideoProcAmp = NULL;
    HRESULT hr = m_graphBuilder->FindInterface(
                NULL,
                NULL,
                m_sourceFilter,
                IID_IAMVideoProcAmp,
                reinterpret_cast<void**>(&pVideoProcAmp)
                );

    if (FAILED(hr) || !pVideoProcAmp) {
        qWarning() << "failed to find the video proc amp";
        return;
    }

    hr = pVideoProcAmp->Set(
                (*sourceValueInfo).videoProcAmpProperty,
                sourceValue,
                capsFlags);

    pVideoProcAmp->Release();

    if (FAILED(hr)) {
        qWarning() << "failed to set the parameter value";
    } else {
        (*sourceValueInfo).capsFlags = capsFlags;
        (*sourceValueInfo).hasBeenExplicitlySet = true;
        (*sourceValueInfo).currentValue = sourceValue;
    }
}

bool DSCameraSession::load()
{
    unload();

    setStatus(QCamera::LoadingStatus);

    bool succeeded = createFilterGraph();
    if (succeeded)
        setStatus(QCamera::LoadedStatus);
    else
        setStatus(QCamera::UnavailableStatus);

    return succeeded;
}

bool DSCameraSession::unload()
{
    if (!m_graphBuilder)
        return false;

    if (!stopPreview())
        return false;

    setStatus(QCamera::UnloadingStatus);

    m_needsHorizontalMirroring = false;
    m_supportedViewfinderSettings.clear();
    for (AM_MEDIA_TYPE f : qAsConst(m_supportedFormats))
        _FreeMediaType(f);
    m_supportedFormats.clear();
    SAFE_RELEASE(m_sourceFilter);
    SAFE_RELEASE(m_previewSampleGrabber);
    SAFE_RELEASE(m_previewFilter);
    SAFE_RELEASE(m_nullRendererFilter);
    SAFE_RELEASE(m_filterGraph);
    SAFE_RELEASE(m_graphBuilder);

    setStatus(QCamera::UnloadedStatus);

    return true;
}

bool DSCameraSession::startPreview()
{
    if (m_previewStarted)
        return true;

    if (!m_graphBuilder)
        return false;

    setStatus(QCamera::StartingStatus);

    HRESULT hr = S_OK;
    IMediaControl* pControl = 0;

    if (!configurePreviewFormat()) {
        qWarning() << "Failed to configure preview format";
        goto failed;
    }

    if (!connectGraph())
        goto failed;

    if (m_surface)
        m_surface->start(m_previewSurfaceFormat);

    hr = m_filterGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        qWarning() << "failed to get stream control";
        goto failed;
    }
    hr = pControl->Run();
    pControl->Release();

    if (FAILED(hr)) {
        qWarning() << "failed to start";
        goto failed;
    }

    setStatus(QCamera::ActiveStatus);
    m_previewStarted = true;
    return true;

failed:
    // go back to a clean state
    if (m_surface && m_surface->isActive())
        m_surface->stop();
    disconnectGraph();
    setStatus(QCamera::LoadedStatus);
    return false;
}

bool DSCameraSession::stopPreview()
{
    if (!m_previewStarted)
        return true;

    setStatus(QCamera::StoppingStatus);

    IMediaControl* pControl = 0;
    HRESULT hr = m_filterGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        qWarning() << "failed to get stream control";
        goto failed;
    }

    hr = pControl->Stop();
    pControl->Release();
    if (FAILED(hr)) {
        qWarning() << "failed to stop";
        goto failed;
    }

    disconnectGraph();

    _FreeMediaType(m_sourceFormat);
    ZeroMemory(&m_sourceFormat, sizeof(m_sourceFormat));

    m_previewStarted = false;
    setStatus(QCamera::LoadedStatus);
    return true;

failed:
    setStatus(QCamera::ActiveStatus);
    return false;
}

void DSCameraSession::setStatus(QCamera::Status status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(m_status);
}

bool DSCameraSession::isReadyForCapture()
{
    return m_readyForCapture;
}

void DSCameraSession::updateReadyForCapture()
{
    bool isReady = (m_status == QCamera::ActiveStatus && m_imageCaptureFileName.isEmpty());
    if (isReady != m_readyForCapture) {
        m_readyForCapture = isReady;
        emit readyForCaptureChanged(isReady);
    }
}

int DSCameraSession::captureImage(const QString &fileName)
{
    ++m_imageIdCounter;

    if (!m_readyForCapture) {
        emit captureError(m_imageIdCounter, QCameraImageCapture::NotReadyError,
                          tr("Camera not ready for capture"));
        return m_imageIdCounter;
    }

    m_imageCaptureFileName = m_fileNameGenerator.generateFileName(fileName,
                                                         QMediaStorageLocation::Pictures,
                                                         QLatin1String("IMG_"),
                                                         QLatin1String("jpg"));

    updateReadyForCapture();

    m_captureMutex.lock();
    m_currentImageId = m_imageIdCounter;
    m_captureMutex.unlock();

    return m_imageIdCounter;
}

void DSCameraSession::onFrameAvailable(const char *frameData, long len)
{
    // !!! Not called on the main thread

    // Deep copy, the data might be modified or freed after the callback returns
    QByteArray data(frameData, len);

    m_presentMutex.lock();

    // (We should be getting only RGB32 data)
    int stride = m_previewSize.width() * 4;

    // In case the source produces frames faster than we can display them,
    // only keep the most recent one
    m_currentFrame = QVideoFrame(new QMemoryVideoBuffer(data, stride),
                                 m_previewSize,
                                 m_previewPixelFormat);

    m_presentMutex.unlock();

    // Image capture
    QMutexLocker locker(&m_captureMutex);
    if (m_currentImageId != -1 && !m_capturedFrame.isValid()) {
        m_capturedFrame = m_currentFrame;
        QMetaObject::invokeMethod(this, "imageExposed",  Qt::QueuedConnection, Q_ARG(int, m_currentImageId));
    }

    QMetaObject::invokeMethod(this, "presentFrame", Qt::QueuedConnection);
}

void DSCameraSession::presentFrame()
{
    m_presentMutex.lock();

    if (m_currentFrame.isValid() && m_surface) {
        m_surface->present(m_currentFrame);
        m_currentFrame = QVideoFrame();
    }

    m_presentMutex.unlock();

    QImage captureImage;
    int captureId;

    m_captureMutex.lock();

    if (m_capturedFrame.isValid()) {
        Q_ASSERT(m_previewPixelFormat == QVideoFrame::Format_RGB32);

        m_capturedFrame.map(QAbstractVideoBuffer::ReadOnly);

        captureImage = QImage(m_capturedFrame.bits(),
                                m_previewSize.width(), m_previewSize.height(),
                                QImage::Format_RGB32);

        captureImage = captureImage.mirrored(m_needsHorizontalMirroring); // also causes a deep copy of the data

        m_capturedFrame.unmap();

        captureId = m_currentImageId;

        QtConcurrent::run(this, &DSCameraSession::saveCapturedImage,
                          m_currentImageId, captureImage, m_imageCaptureFileName);

        m_imageCaptureFileName.clear();
        m_currentImageId = -1;

        m_capturedFrame = QVideoFrame();
    }

    m_captureMutex.unlock();

    if (!captureImage.isNull())
        emit imageCaptured(captureId, captureImage);

    updateReadyForCapture();
}

void DSCameraSession::saveCapturedImage(int id, const QImage &image, const QString &path)
{
    if (image.save(path, "JPG")) {
        emit imageSaved(id, path);
    } else {
        emit captureError(id, QCameraImageCapture::ResourceError,
                          tr("Could not save image to file."));
    }
}

bool DSCameraSession::createFilterGraph()
{
    // Previously containered in <qedit.h>.
    static const IID iID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
    static const CLSID cLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
    static const CLSID cLSID_NullRenderer = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

    HRESULT hr;
    IMoniker* pMoniker = NULL;
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;

    // Create the filter graph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,
            IID_IGraphBuilder, (void**)&m_filterGraph);
    if (FAILED(hr)) {
        qWarning() << "failed to create filter graph";
        goto failed;
    }

    // Create the capture graph builder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2, (void**)&m_graphBuilder);
    if (FAILED(hr)) {
        qWarning() << "failed to create graph builder";
        goto failed;
    }

    // Attach the filter graph to the capture graph
    hr = m_graphBuilder->SetFiltergraph(m_filterGraph);
    if (FAILED(hr)) {
        qWarning() << "failed to connect capture graph and filter graph";
        goto failed;
    }

    // Find the Capture device
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          reinterpret_cast<void**>(&pDevEnum));
    if (SUCCEEDED(hr)) {
        // Create an enumerator for the video capture category
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        pDevEnum->Release();
        if (S_OK == hr) {
            pEnum->Reset();
            IMalloc *mallocInterface = 0;
            CoGetMalloc(1, (LPMALLOC*)&mallocInterface);
            //go through and find all video capture devices
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {

                BSTR strName = 0;
                hr = pMoniker->GetDisplayName(NULL, NULL, &strName);
                if (SUCCEEDED(hr)) {
                    QString output = QString::fromWCharArray(strName);
                    mallocInterface->Free(strName);
                    if (m_sourceDeviceName.contains(output)) {
                        hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_sourceFilter);
                        if (SUCCEEDED(hr)) {
                            pMoniker->Release();
                            break;
                        }
                    }
                }
                pMoniker->Release();
            }
            mallocInterface->Release();
            if (NULL == m_sourceFilter)
            {
                if (m_sourceDeviceName.contains(QLatin1String("default")))
                {
                    pEnum->Reset();
                    // still have to loop to discard bind to storage failure case
                    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
                        IPropertyBag *pPropBag = 0;

                        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
                        if (FAILED(hr)) {
                            pMoniker->Release();
                            continue; // Don't panic yet
                        }

                        // No need to get the description, just grab it

                        hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_sourceFilter);
                        pPropBag->Release();
                        pMoniker->Release();
                        if (SUCCEEDED(hr)) {
                            break; // done, stop looping through
                        }
                        else
                        {
                            qWarning() << "Object bind failed";
                        }
                    }
                }
            }
            pEnum->Release();
        }
    }

    if (!m_sourceFilter) {
        qWarning() << "No capture device found";
        goto failed;
    }

    // Sample grabber filter
    hr = CoCreateInstance(cLSID_SampleGrabber, NULL,CLSCTX_INPROC,
                          IID_IBaseFilter, (void**)&m_previewFilter);
    if (FAILED(hr)) {
        qWarning() << "failed to create sample grabber";
        goto failed;
    }

    hr = m_previewFilter->QueryInterface(iID_ISampleGrabber, (void**)&m_previewSampleGrabber);
    if (FAILED(hr)) {
        qWarning() << "failed to get sample grabber";
        goto failed;
    }

    {
        SampleGrabberCallbackPrivate *callback = new SampleGrabberCallbackPrivate(this);
        m_previewSampleGrabber->SetCallback(callback, 1);
        m_previewSampleGrabber->SetOneShot(FALSE);
        m_previewSampleGrabber->SetBufferSamples(FALSE);
        callback->Release();
    }

    // Null renderer. Input connected to the sample grabber's output. Simply
    // discard the samples it receives.
    hr = CoCreateInstance(cLSID_NullRenderer, NULL, CLSCTX_INPROC,
                          IID_IBaseFilter, (void**)&m_nullRendererFilter);
    if (FAILED(hr)) {
        qWarning() << "failed to create null renderer";
        goto failed;
    }

    updateSourceCapabilities();

    return true;

failed:
    m_needsHorizontalMirroring = false;
    SAFE_RELEASE(m_sourceFilter);
    SAFE_RELEASE(m_previewSampleGrabber);
    SAFE_RELEASE(m_previewFilter);
    SAFE_RELEASE(m_nullRendererFilter);
    SAFE_RELEASE(m_filterGraph);
    SAFE_RELEASE(m_graphBuilder);

    return false;
}

bool DSCameraSession::configurePreviewFormat()
{
    // Resolve viewfinder settings
    int settingsIndex = 0;
    QCameraViewfinderSettings resolvedViewfinderSettings;
    for (const QCameraViewfinderSettings &s : qAsConst(m_supportedViewfinderSettings)) {
        if ((m_viewfinderSettings.resolution().isEmpty() || m_viewfinderSettings.resolution() == s.resolution())
                && (qFuzzyIsNull(m_viewfinderSettings.minimumFrameRate()) || qFuzzyCompare((float)m_viewfinderSettings.minimumFrameRate(), (float)s.minimumFrameRate()))
                && (qFuzzyIsNull(m_viewfinderSettings.maximumFrameRate()) || qFuzzyCompare((float)m_viewfinderSettings.maximumFrameRate(), (float)s.maximumFrameRate()))
                && (m_viewfinderSettings.pixelFormat() == QVideoFrame::Format_Invalid || m_viewfinderSettings.pixelFormat() == s.pixelFormat())
                && (m_viewfinderSettings.pixelAspectRatio().isEmpty() || m_viewfinderSettings.pixelAspectRatio() == s.pixelAspectRatio())) {
            resolvedViewfinderSettings = s;
            break;
        }
        ++settingsIndex;
    }

    if (resolvedViewfinderSettings.isNull()) {
        qWarning("Invalid viewfinder settings");
        return false;
    }

    m_actualViewfinderSettings = resolvedViewfinderSettings;

    _CopyMediaType(&m_sourceFormat, &m_supportedFormats[settingsIndex]);
    // Set frame rate.
    // We don't care about the minimumFrameRate, DirectShow only allows to set an
    // average frame rate, so set that to the maximumFrameRate.
    VIDEOINFOHEADER *videoInfo = reinterpret_cast<VIDEOINFOHEADER*>(m_sourceFormat.pbFormat);
    videoInfo->AvgTimePerFrame = 10000000 / resolvedViewfinderSettings.maximumFrameRate();

    // We only support RGB32, if the capture source doesn't support
    // that format, the graph builder will automatically insert a
    // converter.

    if (m_surface && !m_surface->supportedPixelFormats(QAbstractVideoBuffer::NoHandle)
            .contains(QVideoFrame::Format_RGB32)) {
        qWarning() << "Video surface needs to support RGB32 pixel format";
        return false;
    }

    m_previewPixelFormat = QVideoFrame::Format_RGB32;
    m_previewSize = resolvedViewfinderSettings.resolution();
    m_previewSurfaceFormat = QVideoSurfaceFormat(m_previewSize,
                                                 m_previewPixelFormat,
                                                 QAbstractVideoBuffer::NoHandle);
    m_previewSurfaceFormat.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);

    HRESULT hr;
    IAMStreamConfig* pConfig = 0;
    hr = m_graphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
                                       &MEDIATYPE_Video,
                                       m_sourceFilter,
                                       IID_IAMStreamConfig, (void**)&pConfig);
    if (FAILED(hr)) {
        qWarning() << "Failed to get config for capture device";
        return false;
    }

    hr = pConfig->SetFormat(&m_sourceFormat);

    pConfig->Release();

    if (FAILED(hr)) {
        qWarning() << "Unable to set video format on capture device";
        return false;
    }

    // Set sample grabber format (always RGB32)
    AM_MEDIA_TYPE grabberFormat;
    ZeroMemory(&grabberFormat, sizeof(grabberFormat));
    grabberFormat.majortype = MEDIATYPE_Video;
    grabberFormat.subtype = MEDIASUBTYPE_RGB32;
    grabberFormat.formattype = FORMAT_VideoInfo;
    hr = m_previewSampleGrabber->SetMediaType(&grabberFormat);
    if (FAILED(hr)) {
        qWarning() << "Failed to set video format on grabber";
        return false;
    }

    return true;
}

void DSCameraSession::updateImageProcessingParametersInfos()
{
    if (!m_graphBuilder) {
        qWarning() << "failed to access to the graph builder";
        return;
    }

    IAMVideoProcAmp *pVideoProcAmp = NULL;
    const HRESULT hr = m_graphBuilder->FindInterface(
                NULL,
                NULL,
                m_sourceFilter,
                IID_IAMVideoProcAmp,
                reinterpret_cast<void**>(&pVideoProcAmp)
                );

    if (FAILED(hr) || !pVideoProcAmp) {
        qWarning() << "failed to find the video proc amp";
        return;
    }

    for (int property = VideoProcAmp_Brightness; property <= VideoProcAmp_Gain; ++property) {

        QCameraImageProcessingControl::ProcessingParameter processingParameter; // not initialized

        switch (property) {
        case VideoProcAmp_Brightness:
            processingParameter = QCameraImageProcessingControl::BrightnessAdjustment;
            break;
        case VideoProcAmp_Contrast:
            processingParameter = QCameraImageProcessingControl::ContrastAdjustment;
            break;
        case VideoProcAmp_Saturation:
            processingParameter = QCameraImageProcessingControl::SaturationAdjustment;
            break;
        case VideoProcAmp_Sharpness:
            processingParameter = QCameraImageProcessingControl::SharpeningAdjustment;
            break;
        case VideoProcAmp_WhiteBalance:
            processingParameter = QCameraImageProcessingControl::ColorTemperature;
            break;
        default: // unsupported or not implemented yet parameter
            continue;
        }

        ImageProcessingParameterInfo sourceValueInfo;
        LONG steppingDelta = 0;

        HRESULT hr = pVideoProcAmp->GetRange(
                    property,
                    &sourceValueInfo.minimumValue,
                    &sourceValueInfo.maximumValue,
                    &steppingDelta,
                    &sourceValueInfo.defaultValue,
                    &sourceValueInfo.capsFlags);

        if (FAILED(hr))
            continue;

        hr = pVideoProcAmp->Get(
                    property,
                    &sourceValueInfo.currentValue,
                    &sourceValueInfo.capsFlags);

        if (FAILED(hr))
            continue;

        sourceValueInfo.videoProcAmpProperty = static_cast<VideoProcAmpProperty>(property);

        m_imageProcessingParametersInfos.insert(processingParameter, sourceValueInfo);
    }

    pVideoProcAmp->Release();
}

bool DSCameraSession::connectGraph()
{
    HRESULT hr = m_filterGraph->AddFilter(m_sourceFilter, L"Capture Filter");
    if (FAILED(hr)) {
        qWarning() << "failed to add capture filter to graph";
        return false;
    }

    hr = m_filterGraph->AddFilter(m_previewFilter, L"Sample Grabber");
    if (FAILED(hr)) {
        qWarning() << "failed to add sample grabber to graph";
        return false;
    }

    hr = m_filterGraph->AddFilter(m_nullRendererFilter, L"Null Renderer");
    if (FAILED(hr)) {
        qWarning() << "failed to add null renderer to graph";
        return false;
    }

    hr = m_graphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      m_sourceFilter,
                                      m_previewFilter,
                                      m_nullRendererFilter);
    if (FAILED(hr)) {
        qWarning() << "Graph failed to connect filters" << hr;
        return false;
    }

    return true;
}

void DSCameraSession::disconnectGraph()
{
    IPin *pPin = 0;
    HRESULT hr = getPin(m_sourceFilter, PINDIR_OUTPUT, &pPin);
    if (SUCCEEDED(hr)) {
        m_filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = NULL;
    }

    hr = getPin(m_previewFilter, PINDIR_INPUT, &pPin);
    if (SUCCEEDED(hr)) {
        m_filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = NULL;
    }

    hr = getPin(m_previewFilter, PINDIR_OUTPUT, &pPin);
    if (SUCCEEDED(hr)) {
        m_filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = NULL;
    }

    hr = getPin(m_nullRendererFilter, PINDIR_INPUT, &pPin);
    if (SUCCEEDED(hr)) {
        m_filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = NULL;
    }

    // To avoid increasing the memory usage every time the graph is re-connected it's
    // important that all filters are released; also the ones added by the "Intelligent Connect".
    IEnumFilters *enumFilters = NULL;
    hr = m_filterGraph->EnumFilters(&enumFilters);
    if (SUCCEEDED(hr))  {
        IBaseFilter *filter = NULL;
        while (enumFilters->Next(1, &filter, NULL) == S_OK) {
                m_filterGraph->RemoveFilter(filter);
                enumFilters->Reset();
                filter->Release();
        }
        enumFilters->Release();
    }
}

static bool qt_frameRateRangeGreaterThan(const QCamera::FrameRateRange &r1, const QCamera::FrameRateRange &r2)
{
    return r1.maximumFrameRate > r2.maximumFrameRate;
}

void DSCameraSession::updateSourceCapabilities()
{
    HRESULT hr;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig* pConfig = 0;

    m_supportedViewfinderSettings.clear();
    m_needsHorizontalMirroring = false;
    for (AM_MEDIA_TYPE f : qAsConst(m_supportedFormats))
        _FreeMediaType(f);
    m_supportedFormats.clear();
    m_imageProcessingParametersInfos.clear();

    IAMVideoControl *pVideoControl = 0;
    hr = m_graphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                       m_sourceFilter,
                                       IID_IAMVideoControl, (void**)&pVideoControl);
    if (FAILED(hr)) {
        qWarning() << "Failed to get the video control";
    } else {
        IPin *pPin = 0;
        hr = getPin(m_sourceFilter, PINDIR_OUTPUT, &pPin);
        if (FAILED(hr)) {
            qWarning() << "Failed to get the pin for the video control";
        } else {
            long supportedModes;
            hr = pVideoControl->GetCaps(pPin, &supportedModes);
            if (FAILED(hr)) {
                qWarning() << "Failed to get the supported modes of the video control";
            } else if (supportedModes & VideoControlFlag_FlipHorizontal) {
                long mode;
                hr = pVideoControl->GetMode(pPin, &mode);
                if (FAILED(hr))
                    qWarning() << "Failed to get the mode of the video control";
                else if (supportedModes & VideoControlFlag_FlipHorizontal)
                    m_needsHorizontalMirroring = (mode & VideoControlFlag_FlipHorizontal);
            }
            pPin->Release();
        }
        pVideoControl->Release();
    }

    hr = m_graphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                       m_sourceFilter,
                                       IID_IAMStreamConfig, (void**)&pConfig);
    if (FAILED(hr)) {
        qWarning() << "failed to get config on capture device";
        return;
    }

    int iCount;
    int iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr)) {
        qWarning() << "failed to get capabilities";
        return;
    }

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (hr == S_OK) {
            QVideoFrame::PixelFormat pixelFormat = pixelFormatFromMediaSubtype(pmt->subtype);

            if (pmt->majortype == MEDIATYPE_Video
                    && pmt->formattype == FORMAT_VideoInfo
                    && pixelFormat != QVideoFrame::Format_Invalid) {

                pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
                QSize resolution(pvi->bmiHeader.biWidth, pvi->bmiHeader.biHeight);

                QList<QCamera::FrameRateRange> frameRateRanges;

                if (pVideoControl) {
                    IPin *pPin = 0;
                    hr = getPin(m_sourceFilter, PINDIR_OUTPUT, &pPin);
                    if (FAILED(hr)) {
                        qWarning() << "Failed to get the pin for the video control";
                    } else {
                        long listSize = 0;
                        LONGLONG *frameRates = 0;
                        SIZE size = { resolution.width(), resolution.height() };
                        if (SUCCEEDED(pVideoControl->GetFrameRateList(pPin, iIndex, size,
                                                                      &listSize, &frameRates))) {
                            for (long i = 0; i < listSize; ++i) {
                                qreal fr = qreal(10000000) / frameRates[i];
                                frameRateRanges.append(QCamera::FrameRateRange(fr, fr));
                            }

                            // Make sure higher frame rates come first
                            std::sort(frameRateRanges.begin(), frameRateRanges.end(), qt_frameRateRangeGreaterThan);
                        }
                        pPin->Release();
                    }
                }

                if (frameRateRanges.isEmpty()) {
                    frameRateRanges.append(QCamera::FrameRateRange(qreal(10000000) / scc.MaxFrameInterval,
                                                                   qreal(10000000) / scc.MinFrameInterval));
                }

                for (const QCamera::FrameRateRange &frameRateRange : qAsConst(frameRateRanges)) {
                    QCameraViewfinderSettings settings;
                    settings.setResolution(resolution);
                    settings.setMinimumFrameRate(frameRateRange.minimumFrameRate);
                    settings.setMaximumFrameRate(frameRateRange.maximumFrameRate);
                    settings.setPixelFormat(pixelFormat);
                    settings.setPixelAspectRatio(1, 1);
                    m_supportedViewfinderSettings.append(settings);

                    AM_MEDIA_TYPE format;
                    _CopyMediaType(&format, pmt);
                    m_supportedFormats.append(format);
                }


            }
            _FreeMediaType(*pmt);
        }
    }

    pConfig->Release();

    updateImageProcessingParametersInfos();
}

HRESULT getPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) {
        return hr;
    }

    pEnum->Reset();
    while (pEnum->Next(1, &pPin, NULL) == S_OK) {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir) {
            pEnum->Release();
            *ppPin = pPin;
            return S_OK;
        }
        pPin->Release();
    }
    pEnum->Release();
    return E_FAIL;
}

QT_END_NAMESPACE
