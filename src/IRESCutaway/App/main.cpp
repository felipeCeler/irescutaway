#include <QApplication>
#include <iostream>

#include <GUI/Qt/MainWindow/MainWindow.hpp>

int main ( int argc , char *argv[] )
{

	QApplication app ( argc , argv );
	if ( !QGLFormat::hasOpenGL ( ) )
	{
		std::cerr << "This system has no OpenGL support" << std::endl;
		return 1;
	}

	MainWindow * form = new MainWindow;

	form->show ( );

	form->open( "/media/d/Workspace/IRESCutaway/build/build/bin/zmap.ires2.ires",true );

	form->ui->doubleSpinMin->setValue( 7000.0 );
	form->ui->doubleSpinMax->setValue( 7005.0 );

	form->on_pushButton_changePropertyRange_clicked();

	return app.exec ( );
}

