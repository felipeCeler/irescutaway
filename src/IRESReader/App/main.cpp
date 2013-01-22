#include <QApplication>
#include <iostream>

#include <GUI/Qt/MainWindow/MainWindow.hpp>

// Learning ifstream
#include<fstream>

int main ( int argc , char *argv[] )
{

	QApplication app ( argc , argv );
	if ( !QGLFormat::hasOpenGL ( ) )
	{
		std::cerr << "This system has no OpenGL support" << std::endl;
		return 1;
	}

	//MainWindow * form = new MainWindow;

	//form->show();

	std::ifstream inFile;

	inFile.open ( "/media/d/Workspace/iofile/src/ioFile/App/test.ply" , std::ios::in );

	char a[80];

	if ( inFile.fail ( ) )
	{
		std::cout << " error! myabe the file doesn't exist " << std::endl;
		inFile.close ( );
	}
	else if ( !inFile )
	{
		std::cout << "While opening a file an error is encountered" << std::endl;
	}
	else
	{
		std::cout << "File is successfully opened" << std::endl;

		while ( !inFile.eof ( ) )
		{
			inFile >> a;
			std::cout << a << std::endl;
		}
	}

	return app.exec ( );
}




