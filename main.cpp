
#include <QApplication>
#include <QQmlApplicationEngine>

#include "frames.hpp"

int main( int argc, char ** argv )
{
	Stock::Frames::registerQmlType();

	QGuiApplication app( argc, argv );

	QQmlApplicationEngine engine;

	engine.load( QUrl( "qrc:/main.qml" ) );

	if( engine.rootObjects().isEmpty() )
		return -1;

	return QApplication::exec();
}
