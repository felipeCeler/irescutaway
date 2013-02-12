#include <QtGui>
#include <QtGui/QInputDialog>
#include <iostream>

#include <GUI/Qt/MainWindow/MainWindow.hpp>

#include <IRESClasses/CornerPointGrid_test.cpp>

MainWindow::MainWindow ( QMainWindow *parent ) :
	QMainWindow ( parent )
{

	QGLFormat glFormat;
	glFormat.setSwapInterval ( 1 ); // vsync
	glFormat.setVersion(3,3);
	glFormat.setProfile(QGLFormat::CompatibilityProfile);
	glFormat.setSampleBuffers(true);
	glFormat.setSamples(4);

	setupUi ( this );
	this->glWidget = new GLWidget ( this );
	this->setCentralWidget ( glWidget );

	QIcon icon;
	icon.addFile ( ":/Icons/caju.png" , QSize ( ) , QIcon::Normal , QIcon::Off );
	setWindowIcon ( icon );
	showfullScreen_ = 0;

}

void MainWindow::open(QString pFilename,bool who ) {

	glWidget->openIRES( pFilename.toLocal8Bit().constData() );

	if ( glWidget->isIresWasOpenedSucessufully( ))
	{
		for ( int i = 0 ; i < glWidget->ires_cornerPoint_test_.static_porperties.size( ); ++i )
			this->comboBox_choose_an_property_->addItem(  QString::fromStdString( glWidget->ires_cornerPoint_test_.static_porperties[i].name ) );
	}
}

void MainWindow::on_pushButton_do_something_clicked ()
{
	QMessageBox msgBox;

	if ( glWidget->isIresWasOpenedSucessufully( ))
	{
	msgBox.setText( QString::fromStdString( glWidget->ires_cornerPoint_test_.static_porperties[this->comboBox_choose_an_property_->currentIndex()].name));

	}else
	{
		msgBox.setText("Open a file first my friend !");
	}
	msgBox.exec();

}

void MainWindow::on_action_Open_IRES_triggered()
{
	// Asks for a file name to open.
	QString filename = QFileDialog::getOpenFileName (
			this,
			tr("Choose a Reservoir *.ires file"),
			".",
			tr("ires reservoir model (*.ires)"));

	// Tries to load file if filename not empty.
	if ( filename != "" )
	{

		open ( filename , true );

	}
}

void MainWindow::on_action_Load_Shaders_triggered()
{
	glWidget->LoadShaders();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if(e->key()==Qt::Key_Return && e->modifiers()==Qt::AltModifier)
	{
		showfullScreen_ = !showfullScreen_;
		if ( showfullScreen_ )
		{
			showFullScreen ( );
		}else
		{
			showNormal();
		}
		e->accept();
	}
	else e->ignore();
}
