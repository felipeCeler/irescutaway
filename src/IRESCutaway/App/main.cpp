#include <GUI/Qt/MainWindow/MainWindow.hpp>


#include <QApplication>
#include <iostream>


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
	form->glWidget->loadPly("/media/d/Workspace/Image-Cutaway/Files/sphere.ply");
	//form->open( "/media/d/Workspace/IRESCutaway/Files/models/opaDynamic_v2_NEW01_FaceProp.ires",true );/


	form->ui->doubleSpinMin->setValue(280000.0);
	form->ui->doubleSpinMax->setValue(290000.0);

	form->ui->horizontalSlider_PrimariesSaturation->setValue(70);
	form->ui->horizontalSlider_PrimariesLuminance->setValue(50);

	form->ui->horizontalSlider_SecondariesSaturation->setValue(30);
	form->ui->horizontalSlider_SecondariesLuminance->setValue(50);

	form->ui->horizontalSlider_ShellStaturation->setValue(30);
	form->ui->horizontalSlider_ShellLuminance->setValue(50);

	form->ui->horizontalSliderLineWith->setValue(2);


//
//	form->changeProperty();

	return app.exec ( );
}

