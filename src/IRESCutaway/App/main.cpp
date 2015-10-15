#include <GUI/Qt/MainWindow/MainWindow.hpp>


#include <QApplication>
#include <QDir>
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


        //! Binary absolute location
	QDir shadersDir = QDir ( qApp->applicationDirPath ( ) );

	//! Debug Verion: to load the update shaders
        qDebug () << "Directory " << shadersDir.path ();
        shadersDir.cdUp ();
        shadersDir.cdUp ();
        shadersDir.cdUp ();
        qDebug () << "Directory " << shadersDir.path ();

	#if defined(_WIN32) || defined(_WIN64) // Windows Directory Style
	/* Do windows stuff */
	        QString shaderDirectory (shadersDir.path ()+"\\Files\\models\\");
	        QString plyDirectory (shadersDir.path ()+"\\Files\\ply\\");
	#elif defined(__linux__)               // Linux Directory Style
	/* Do linux stuff */
	        QString shaderDirectory (shadersDir.path ()+"/Files/models/");
	        QString plyDirectory (shadersDir.path ()+"/Files/ply/");
	#else
	/* Error, both can't be defined or undefined same time */
	        std::cout <<  "Operate System not supported !"
                halt();
	#endif


//	form->open( "D:\\Workspace\\RESCutaway\\build\\build\\bin\\zmap.ires2.ires",true );
//	form->open( "/home/ricardomarroquim/devel/irescutaway/build/build/bin/zmap.ires2.ires",true );
	form->open( (shaderDirectory + "petrellThrustDynamic_v2_NEW01_FaceProp.ires"),true );
	form->glWidget->loadPly((plyDirectory + "sphere.ply"));
	//form->open( "/media/d/Workspace/IRESCutaway/Files/models/opaDynamic_v2_NEW01_FaceProp.ires",true );/

//
//	form->ui->doubleSpinMin->setValue(280000.0);
//	form->ui->doubleSpinMax->setValue(290000.0);

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

