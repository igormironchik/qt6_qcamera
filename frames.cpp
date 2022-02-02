
#include "frames.hpp"

// Qt include.
#include <QQmlEngine>
#include <QCameraDevice>
#include <QThreadPool>
#include <QTimer>


namespace Stock {

//
// Frames
//

Frames::Frames( QObject * parent )
	:	QVideoSink( parent )
	,	m_cam( nullptr )
	,	m_fps( 0 )
	,	m_1s( new QTimer( this ) )
{
	connect( m_1s, &QTimer::timeout, this, &Frames::timer );

	connect( this, &QVideoSink::videoFrameChanged,
			this, &Frames::newFrame );

	m_1s->setInterval( 1000 );
	m_1s->start();

	initCam();
}

Frames::~Frames()
{
	stopCam();
}

QVideoSink *
Frames::videoSink() const
{
	return m_videoSink.get();
}

void
Frames::setVideoSink( QVideoSink * newVideoSink )
{
	if( m_videoSink == newVideoSink )
		return;

	m_videoSink = newVideoSink;

	emit videoSinkChanged();
}

void
Frames::newFrame( const QVideoFrame & frame )
{
	++m_fps;

	if( m_videoSink )
		m_videoSink->setVideoFrame( frame );
}

void
Frames::registerQmlType()
{
	qmlRegisterType< Stock::Frames > ( "Frames", 0, 1, "Frames" );
}

QString
pixelFormatToString( QVideoFrameFormat::PixelFormat f )
{
	switch( f )
	{
		case QVideoFrameFormat::Format_ARGB8888 :
			return QStringLiteral( "ARGB8888" );

		case QVideoFrameFormat::Format_ARGB8888_Premultiplied :
			return QStringLiteral( "ARGB8888P" );

		case QVideoFrameFormat::Format_XRGB8888 :
			return QStringLiteral( "XRGB8888" );

		case QVideoFrameFormat::Format_BGRA8888 :
			return QStringLiteral( "BGRA8888" );

		case QVideoFrameFormat::Format_BGRA8888_Premultiplied :
			return QStringLiteral( "BGRA8888P" );

		case QVideoFrameFormat::Format_BGRX8888 :
			return QStringLiteral( "BGRX8888" );

		case QVideoFrameFormat::Format_ABGR8888 :
			return QStringLiteral( "ARGB8888" );

		case QVideoFrameFormat::Format_XBGR8888 :
			return QStringLiteral( "XBGR8888" );

		case QVideoFrameFormat::Format_RGBA8888 :
			return QStringLiteral( "RGBA8888" );

		case QVideoFrameFormat::Format_RGBX8888 :
			return QStringLiteral( "RGBX8888" );

		case QVideoFrameFormat::Format_AYUV :
			return QStringLiteral( "AYUV" );

		case QVideoFrameFormat::Format_AYUV_Premultiplied :
			return QStringLiteral( "AYUVP" );

		case QVideoFrameFormat::Format_YUV420P :
			return QStringLiteral( "YUV420P" );

		case QVideoFrameFormat::Format_YUV422P :
			return QStringLiteral( "YUV422P" );

		case QVideoFrameFormat::Format_YV12 :
			return QStringLiteral( "YV12" );

		case QVideoFrameFormat::Format_UYVY :
			return QStringLiteral( "UYVY" );

		case QVideoFrameFormat::Format_YUYV :
			return QStringLiteral( "YUYV" );

		case QVideoFrameFormat::Format_NV12 :
			return QStringLiteral( "NV12" );

		case QVideoFrameFormat::Format_NV21 :
			return QStringLiteral( "NV21" );

		case QVideoFrameFormat::Format_IMC1 :
			return QStringLiteral( "IMC1" );

		case QVideoFrameFormat::Format_IMC2 :
			return QStringLiteral( "IMC2" );

		case QVideoFrameFormat::Format_IMC3 :
			return QStringLiteral( "IMC3" );

		case QVideoFrameFormat::Format_IMC4 :
			return QStringLiteral( "IMC4" );

		case QVideoFrameFormat::Format_Y8 :
			return QStringLiteral( "Y8" );

		case QVideoFrameFormat::Format_Y16 :
			return QStringLiteral( "Y16" );

		case QVideoFrameFormat::Format_P010 :
			return QStringLiteral( "P010" );

		case QVideoFrameFormat::Format_P016 :
			return QStringLiteral( "P016" );

		case QVideoFrameFormat::Format_Jpeg :
			return QStringLiteral( "JPEG" );

		case QVideoFrameFormat::Format_SamplerExternalOES :
			return QStringLiteral( "SEOES" );

		case QVideoFrameFormat::Format_SamplerRect :
			return QStringLiteral( "SR" );

		case QVideoFrameFormat::Format_Invalid :
			return QStringLiteral( "Invalid" );

		default :
			return QStringLiteral( "Unknown" );
	}
}

void
Frames::initCam()
{
	m_cam = new QCamera( this );

	const auto settings = m_cam->cameraDevice().videoFormats();

	int i = 0;

	const auto s = settings.at( i );

	m_cam->setFocusMode( QCamera::FocusModeAuto );
	m_cam->setCameraFormat( s );

	m_capture.setCamera( m_cam );
	m_capture.setVideoSink( this );

	m_formatString = QString( "%1x%2 at %3 fps, %4" ).arg( QString::number( s.resolution().width() ),
		QString::number( s.resolution().height() ), QString::number( (int) s.maxFrameRate() ),
		pixelFormatToString( s.pixelFormat() ) );

	emit formatStringChanged();

	m_cam->start();
}

qreal
Frames::angle() const
{
	return 0.0;
}

qreal
Frames::xScale() const
{
	return 1.0;
}

qreal
Frames::yScale() const
{
	return 1.0;
}

QString
Frames::formatString() const
{
	return m_formatString;
}

QString
Frames::fpsString() const
{
	return m_fpsString;
}

void
Frames::stopCam()
{
	m_cam->stop();

	disconnect( m_cam, 0, 0, 0 );
	m_cam->setParent( nullptr );

	delete m_cam;

	m_cam = nullptr;
}

void
Frames::timer()
{
	m_fpsString = QString( "%1 fps" ).arg( QString::number( m_fps ) );
	m_fps = 0;
	emit fpsStringChanged();
}

} /* namespace Stock */
