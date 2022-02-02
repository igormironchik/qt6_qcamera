
#ifndef FRAMES_HPP_INCLUDED
#define FRAMES_HPP_INCLUDED

// Qt include.
#include <QVideoSink>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QPointer>


namespace Stock {

//
// Frames
//

//! Frames listener.
class Frames
	:	public QVideoSink
{
	Q_OBJECT

	Q_PROPERTY( QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged )
	Q_PROPERTY( qreal angle READ angle NOTIFY angleChanged )
	Q_PROPERTY( qreal xScale READ xScale NOTIFY xScaleChanged )
	Q_PROPERTY( qreal yScale READ yScale NOTIFY yScaleChanged )
	Q_PROPERTY( QString formatString READ formatString NOTIFY formatStringChanged )
	Q_PROPERTY( QString fpsString READ fpsString NOTIFY fpsStringChanged )

signals:
	//! Video sink changed.
	void videoSinkChanged();

	void angleChanged();
	void xScaleChanged();
	void yScaleChanged();
	void formatStringChanged();
	void fpsStringChanged();

public:
	static void registerQmlType();

	explicit Frames( QObject * parent = nullptr );
	~Frames() override;

	//! \return Sink of video output.
	QVideoSink * videoSink() const;
	//! Set sink of video output.
	void setVideoSink( QVideoSink * newVideoSink );

	qreal angle() const;
	qreal xScale() const;
	qreal yScale() const;
	QString formatString() const;
	QString fpsString() const;

private slots:
	//! Init camera.
	void initCam();
	//! Stop camera.
	void stopCam();
	//! Video frame changed.
	void newFrame( const QVideoFrame & frame );
	//! 1s timer.
	void timer();

private:
	Q_DISABLE_COPY( Frames )

	//! Camera.
	QCamera * m_cam;
	//! Capture.
	QMediaCaptureSession m_capture;
	//! Video sink of video output.
	QPointer< QVideoSink > m_videoSink;
	//! Format string.
	QString m_formatString;
	//! FPS string.
	QString m_fpsString;
	//! FPS.
	int m_fps;
	//! Timer.
	QTimer * m_1s;
}; // class Frames

} /* namespace Stock */

#endif // FRAMES_HPP_INCLUDED
