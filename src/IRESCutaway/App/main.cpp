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

	//form->open( "D:\\Workspace\\RESCutaway\\build\\build\\bin\\zmap.ires2.ires",true );
	form->open( "/media/d/Workspace/IRESCutaway/build/build/bin/zmap_ires2_NEW01_FaceProp.ires",true );
        //form->open( "/home/ricardomarroquim/devel/irescutaway/build/build/bin/zmap.ires2.ires",true );

	form->ui->doubleSpinMin->setValue( 7000.0 );
	form->ui->doubleSpinMax->setValue( 7005.0 );

	form->changeProperty();

	return app.exec ( );
}

