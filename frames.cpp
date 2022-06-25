
/*!
	\author Igor Mironchik (igor dot mironchik at gmail dot com).

	MIT License

	Copyright (c) 2022 Igor Mironchik

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "frames.hpp"

// Qt include.
#include <QQmlEngine>
#include <QCameraDevice>
#include <QThreadPool>
#include <QTimer>

// libyuv include.
#include <libyuv.h>


namespace Qt6QCamera {

//
// libyuvFormat
//

libyuv::FourCC libyuvFormat( QVideoFrameFormat::PixelFormat f )
{
	// Enlarge this switch to handle more pixel formats if you need it.
	switch( f )
	{
		case QVideoFrameFormat::Format_YUYV :
			return libyuv::FOURCC_YUYV;

		case QVideoFrameFormat::Format_UYVY :
			return libyuv::FOURCC_UYVY;

		case QVideoFrameFormat::Format_YUV420P :
			return libyuv::FOURCC_I420;

		case QVideoFrameFormat::Format_YUV422P :
			return libyuv::FOURCC_I422;

		case QVideoFrameFormat::Format_NV12 :
			return libyuv::FOURCC_NV12;

		case QVideoFrameFormat::Format_NV21 :
			return libyuv::FOURCC_NV21;

		default :
			return libyuv::FOURCC_ANY;
	}
}

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
	QVideoFrame f = frame;
	f.map( QVideoFrame::ReadOnly );

	if( f.isValid() )
	{
		const auto fmt = QVideoFrameFormat::imageFormatFromPixelFormat( f.pixelFormat() );

		QImage image;

		if( fmt != QImage::Format_Invalid )
			image = QImage( f.bits( 0 ), f.width(), f.height(), f.bytesPerLine( 0 ), fmt );
		else if( f.pixelFormat() == QVideoFrameFormat::Format_Jpeg )
			image.loadFromData( f.bits( 0 ), f.mappedBytes( 0 ) );
		else
		{
			const auto format = libyuvFormat( f.pixelFormat() );

			if( format != libyuv::FOURCC_ANY )
			{
				std::vector< uint8_t > data( f.width() * f.height() * 4, 0 );

				libyuv::ConvertToARGB( static_cast< uint8_t* > ( f.bits( 0 ) ),
					f.bytesPerLine( 0 ) * f.height(),
					&data[ 0 ],
					f.width() * 4,
					0, 0,
					f.width(),
					f.height(),
					f.width(),
					f.height(),
					libyuv::kRotate0,
					format );

				image = QImage( static_cast< uchar* > ( &data[ 0 ] ), f.width(), f.height(),
					f.width() * 4, QImage::Format_ARGB32 ).copy();
			}
			else
				qWarning() << "Unsupported video frame format:" << format;
		}

		f.unmap();

		// Here image is valid QImage with correct data.
		// You can do what you need with QImage. Enjoy.
	}

	++m_fps;

	if( m_videoSink )
		m_videoSink->setVideoFrame( frame );
}

void
Frames::registerQmlType()
{
	qmlRegisterType< Qt6QCamera::Frames > ( "Frames", 0, 1, "Frames" );
}

void
Frames::initCam()
{
	m_cam = new QCamera( this );

	const auto settings = m_cam->cameraDevice().videoFormats();

	// Here you can select format of video frame.
	int i = 0;
	const auto s = settings.at( i );

	m_cam->setFocusMode( QCamera::FocusModeAuto );
	m_cam->setCameraFormat( s );

	m_capture.setCamera( m_cam );
	m_capture.setVideoSink( this );

	m_formatString = QString( "%1x%2 at %3 fps, %4" ).arg( QString::number( s.resolution().width() ),
		QString::number( s.resolution().height() ), QString::number( (int) s.maxFrameRate() ),
		QVideoFrameFormat::pixelFormatToString( s.pixelFormat() ) );

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

} /* namespace Qt6QCamera */
