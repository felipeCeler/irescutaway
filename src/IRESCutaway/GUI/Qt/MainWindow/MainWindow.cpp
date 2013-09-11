#include <QtGui>
#include <QtGui/QInputDialog>
#include <iostream>

#include <GUI/Qt/MainWindow/MainWindow.hpp>


MainWindow::MainWindow ( QMainWindow *parent ) :
	QMainWindow ( parent )
{

	QGLFormat glFormat;
	glFormat.setSwapInterval ( 0 ); // vsync
//	glFormat.setVersion(3,3);
//	glFormat.setProfile(QGLFormat::CompatibilityProfile);
//	glFormat.setSampleBuffers(true);
//	glFormat.setSamples(4);

	ui = new Ui::MainWindow;
	ui->setupUi( this );

	aboutIRESReader = new AboutWidgetIRESReader( this );

	glWidget = new GLWidget ( glFormat, ui->viewer_ );
	ui->viewer_verticalLayout_->addWidget(glWidget);
	ui->tabWidget_->setCurrentIndex(0);

	ui->properties_tableWidget_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	QIcon icon;
	icon.addFile ( ":/Icons/caju.png" , QSize ( ) , QIcon::Normal , QIcon::Off );
	setWindowIcon ( icon );

	showfullScreen_ = 0;

	// Dock Widget Layer Initialization

	ui->dockWidgetLayers->setHidden( true );
	connect(ui->comboBox_choose_an_property_, SIGNAL(activated(int)), this, SLOT(updateDoubleSpinMin(int)));
	connect(ui->comboBox_choose_an_property_, SIGNAL(activated(int)), this, SLOT(updateDoubleSpinMax(int)));
	// Just the name of the function: so changeProperty , not glWidget->changeProperty
	connect(ui->comboBox_choose_an_property_, SIGNAL(activated(int)), glWidget, SLOT(changeProperty(int)));
	// Sliders
	connect(ui->horizontalslider_min_I, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMinI (int)));
	connect(ui->horizontalslider_max_I, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMaxI (int)));
	connect(ui->horizontalslider_min_J, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMinJ (int)));
	connect(ui->horizontalslider_max_J, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMaxJ (int)));
	connect(ui->horizontalslider_min_K, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMinK (int)));
	connect(ui->horizontalslider_max_K, SIGNAL(valueChanged(int)), glWidget, SLOT(changeMaxK (int)));


	// About Widget
	connect(ui->action_About, SIGNAL(triggered()), aboutIRESReader, SLOT(show()));

	// Controls over the rendering
	QActionGroup* CutawayTypeGroup = new QActionGroup ( this );
	CutawayTypeGroup->setExclusive(1);
	CutawayTypeGroup->addAction ( ui->action_No_Cutaway );
	CutawayTypeGroup->addAction ( ui->action_Raw_Cutaway );
	CutawayTypeGroup->addAction ( ui->action_IRES_Cutaway );

	// Change the Cutaway Type
	connect(ui->action_No_Cutaway,           SIGNAL(toggled(bool)), glWidget, SLOT(setNoCutawayVisibility(bool)));
	connect(ui->action_Raw_Cutaway,          SIGNAL(toggled(bool)), glWidget, SLOT(setRawCutawayVisibility(bool)));
	connect(ui->action_IRES_Cutaway,       SIGNAL(toggled(bool)), glWidget, SLOT(setIRESCutawayVisibility(bool)));
	// Change visibility  of cells
	connect(ui->action_Show_Primary_Cells,   SIGNAL(toggled(bool)), glWidget, SLOT(setPrimaryVisibility(bool)));
	connect(ui->action_Show_Secondary_Cells, SIGNAL(toggled(bool)), glWidget, SLOT(setSecondaryVisibility(bool)));

}

void MainWindow::open( QString pFilename,bool who ) {

	glWidget->openIRESCharles( pFilename.toLocal8Bit().constData() );


	// Filling Dock Widget Layers

	if ( glWidget->isIRESOpen( ))
	{
		QString title ( reinterpret_cast<char*>(glWidget->reservoir_model_.header_v2_.title) );



		ui->simulator_name_->setText (  title );

		updateDoubleSpinMax( 0 );
		updateDoubleSpinMin( 0 );
		ui->properties_tableWidget_->setRowCount(2*glWidget->reservoir_model_.static_porperties.size( ));
		ui->comboBox_choose_an_property_->clear();

		for ( int i = 0 ; i < glWidget->reservoir_model_.static_porperties.size( ); ++i )
		{

			float min =  *std::min_element ( glWidget->reservoir_model_.static_porperties[i].values_.begin( ),glWidget->reservoir_model_.static_porperties[i].values_.end( ) );
			float max =  *std::max_element ( glWidget->reservoir_model_.static_porperties[i].values_.begin( ),glWidget->reservoir_model_.static_porperties[i].values_.end( ) );


			ui->comboBox_choose_an_property_->addItem(  QString::fromStdString( glWidget->reservoir_model_.static_porperties[i].name ) );
			ui->properties_tableWidget_->setSpan(i * 2, 0, 2, 1);
			ui->properties_tableWidget_->setItem(i * 2, 0, new QTableWidgetItem(  QString::fromStdString( glWidget->reservoir_model_.static_porperties[i].name ) ) );

			ui->properties_tableWidget_->setItem(i    , 1, new QTableWidgetItem(  QString::number( min )) );
			ui->properties_tableWidget_->setItem(i * 2, 1, new QTableWidgetItem(  QString::number( max )) );

			ui->properties_tableWidget_->setItem(i , 0, new QTableWidgetItem(  QString::fromStdString( glWidget->reservoir_model_.static_porperties[i].name ) ) );
			ui->properties_tableWidget_->setItem(i , 1, new QTableWidgetItem(  QString( QString::number( min )+" - "+QString::number( max )) ));
			// ui->properties_tableWidget_->setItem(i * 2, 1, new QTableWidgetItem(  QString::number( max )) );

		}

		ui->label_I_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction)+" ]");
		ui->label_J_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction)+" ]");
		ui->label_K_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction)+" ]");


		ui->horizontalslider_max_I->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction+1);
		ui->horizontalslider_max_J->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction+1);
		ui->horizontalslider_max_K->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction+1);

		ui->horizontalslider_min_I->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction+1);
		ui->horizontalslider_min_J->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction+1);
		ui->horizontalslider_min_K->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction+1);

		ui->horizontalslider_max_I->setValue ( glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction+1 );
		ui->horizontalslider_max_J->setValue ( glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction+1 );
		ui->horizontalslider_max_K->setValue ( glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction+1 );

		ui->horizontalslider_min_I->setValue ( 0 );
		ui->horizontalslider_min_J->setValue ( 0 );
		ui->horizontalslider_min_K->setValue ( 0 );
	}

}

void MainWindow::updateDoubleSpinMin( int property_index )
{

	float min =  *std::min_element ( glWidget->reservoir_model_.static_porperties[property_index].values_.begin( ),glWidget->reservoir_model_.static_porperties[property_index].values_.end( ) );
	float max =  *std::max_element ( glWidget->reservoir_model_.static_porperties[property_index].values_.begin( ),glWidget->reservoir_model_.static_porperties[property_index].values_.end( ) );

	ui->doubleSpinMin->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMin->setMaximum ( static_cast<double> (max));

	ui->doubleSpinMax->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMax->setMaximum ( static_cast<double> (max) );

	qDebug() << "Min: "<< min;

	ui->doubleSpinMin->setValue(static_cast<double> (min));


}

void MainWindow::updateDoubleSpinMax( int property_index )
{


	float min =  *std::min_element ( glWidget->reservoir_model_.static_porperties[property_index].values_.begin( ),glWidget->reservoir_model_.static_porperties[property_index].values_.end( ) );
	float max =  *std::max_element ( glWidget->reservoir_model_.static_porperties[property_index].values_.begin( ),glWidget->reservoir_model_.static_porperties[property_index].values_.end( ) );

	ui->doubleSpinMin->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMin->setMaximum ( static_cast<double> (max));

	ui->doubleSpinMax->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMax->setMaximum ( static_cast<double> (max) );

	qDebug() << "Max: "<< max;

	ui->doubleSpinMax->setValue(static_cast<double> (max));

}

void MainWindow::on_pushButton_changePropertyRange_clicked()
{
	QMessageBox msgBox;

	if ( glWidget->isIRESOpen( ))
	{
		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBox_choose_an_property_->currentIndex() );
	}else
	{
		//msgBox.setText("First of all. Open a file my friend !!  ");
	}
	//msgBox.exec();

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

void MainWindow::on_pushButton_Reset_IJK_clicked( )
{
	ui->label_I_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction)+" ]");
	ui->label_J_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction)+" ]");
	ui->label_K_range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction)+" ]");


	ui->horizontalslider_max_I->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction+1);
	ui->horizontalslider_max_J->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction+1);
	ui->horizontalslider_max_K->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction+1);

	ui->horizontalslider_min_I->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_I_Direction+1);
	ui->horizontalslider_min_J->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_J_Direction+1);
	ui->horizontalslider_min_K->setMaximum(glWidget->reservoir_model_.header_.number_of_Blocks_in_K_Direction+1);

	ui->horizontalslider_max_I->setValue ( 0 );
	ui->horizontalslider_max_J->setValue ( 0 );
	ui->horizontalslider_max_K->setValue ( 0 );

	ui->horizontalslider_min_I->setValue ( 0 );
	ui->horizontalslider_min_J->setValue ( 0 );
	ui->horizontalslider_min_K->setValue ( 0 );

}

void MainWindow::on_action_Load_Shaders_triggered()
{
	glWidget->loadShaders();
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

void MainWindow::on_spinBox_Cluster_Number_valueChanged  ( int i)
{
	glWidget->updateCluster(i);
}

void MainWindow::on_action_Trackball_triggered  ( )
{
}

void MainWindow::on_action_Fly_Camera_triggered ( )
{
}

void MainWindow::on_action_Freeze_View_triggered()
{
	glWidget->freezeView();
}

void MainWindow::on_doubleSpinMin_valueChanged  ( double i)
{
	if ( glWidget->isIRESOpen( ) && glWidget->dynamic( ))
	{
		ui->doubleSpinMax->setValue( i + 3000);

		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBox_choose_an_property_->currentIndex() );
		std::cout << "Mudou" << std::endl;
	}else
	{
		//msgBox.setText("First of all. Open a file my friend !!  ");
	}


}
void MainWindow::on_doubleSpinMax_valueChanged  ( double i)
{

	if ( glWidget->isIRESOpen( ) && glWidget->dynamic( ) )
	{
		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBox_choose_an_property_->currentIndex() );
		std::cout << "doubleSpinMax ::::::::::::: " << ui->doubleSpinMin->value() << std::endl;
	}else
	{
		//msgBox.setText("First of all. Open a file my friend !!  ");
	}

}
