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

//	form->open( "D:\\Workspace\\RESCutaway\\build\\build\\bin\\zmap.ires2.ires",true );
//	form->open( "/home/ricardomarroquim/devel/irescutaway/build/build/bin/zmap.ires2.ires",true );
	form->open( "/media/d/Workspace/IRESCutaway/Files/models/zmapDynamic_v2_NEW01_FaceProp.ires",true );
	//form->open( "/media/d/Workspace/IRESCutaway/Files/models/opaDynamic_v2_NEW01_FaceProp.ires",true );/


//	form->ui->doubleSpinMin->setValue( 270000.0 );
//	form->ui->doubleSpinMax->setValue( 271000.0 );
//
//	form->changeProperty();

	return app.exec ( );
}

