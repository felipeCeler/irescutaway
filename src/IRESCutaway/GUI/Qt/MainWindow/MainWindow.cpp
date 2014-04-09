#include <QtGui>
#include <QtGui/QInputDialog>
#include <iostream>

#include <GUI/Qt/MainWindow/MainWindow.hpp>


MainWindow::MainWindow ( QMainWindow *parent ) :
	QMainWindow ( parent )
{

	QGLFormat glFormat;
	glFormat.setSwapInterval ( 0 ); // vsync

	ui = new Ui::MainWindow;
	ui->setupUi( this );

	aboutIRESReader = new AboutWidgetIRESReader( this );

	glWidget = new GLWidget ( glFormat, this );

	setCentralWidget( glWidget );

	QIcon icon;
	icon.addFile ( ":/Icons/celerSystemIcon.svg" , QSize ( ) , QIcon::Normal , QIcon::Off );
	setWindowIcon ( icon );

	showfullScreen_ = 0;

	fps = new QLabel;

	fps->setText("fps 60s");

	ui->statusBar->addPermanentWidget(fps,0);

	WidgetSignalSlotConnection( );

	setFocusPolicy ( Qt::StrongFocus );

}

void MainWindow::WidgetSignalSlotConnection( )
{
        // Dock Widget Layer Initialization

        ui->dockWidgetStatic->setHidden( true );
        ui->dockWidgetDynamic->setHidden( true );
        ui->dockWidgetIJKViewer->setHidden( true );
        ui->dockWidgetBeauty->setHidden( true );
        ui->dockWidgetBrenchMarking->setHidden(true);

        connect(ui->comboBoxProperty, SIGNAL(activated(int)), this, SLOT(updateDoubleSpinMin(int)));
        connect(ui->comboBoxProperty, SIGNAL(activated(int)), this, SLOT(updateDoubleSpinMax(int)));

        connect(ui->comboBoxProperty_Dynamic, SIGNAL(activated(int)), this,     SLOT(updateDynamicDoubleSpinMin(int)));
        connect(ui->comboBoxProperty_Dynamic, SIGNAL(activated(int)), this,     SLOT(updateDynamicDoubleSpinMax(int)));
        connect(ui->comboBoxProperty_Dynamic, SIGNAL(activated(int)), glWidget, SLOT(changeDynamicProperty (int)));

        connect(ui->sliderTimeLine_Dynamic, SIGNAL(valueChanged(int)), glWidget, SLOT(changeTimeStep(int)));

        // Just the name of the function: so changeProperty , not glWidget->changeProperty
        connect(ui->comboBoxProperty, SIGNAL(activated(int)), glWidget, SLOT(changeProperty(int)));
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
        CutawayTypeGroup->addAction ( ui->action_Paper_Demo );
        CutawayTypeGroup->addAction ( ui->action_IRES_Cutaway );
        CutawayTypeGroup->addAction ( ui->action_IRES_Cutaway_Static );
        CutawayTypeGroup->addAction ( ui->action_IRES_Cutaway_Dynamic );

        // Rendering Type
        connect(ui->action_Paper_Demo,           SIGNAL(toggled(bool)), glWidget, SLOT(setPaperDemoVisibility(bool)));
        connect(ui->action_IRES_Cutaway   ,      SIGNAL(toggled(bool)), glWidget, SLOT(setIRESCutawayVisibility(bool)));
        connect(ui->action_IRES_Cutaway_Static,  SIGNAL(toggled(bool)), glWidget, SLOT(setIRESCutawayStaticVisibility(bool)));
        connect(ui->action_IRES_Cutaway_Dynamic, SIGNAL(toggled(bool)), glWidget, SLOT(setIRESCutawayDynamicVisibility(bool)));
        // Cells Visibility
        connect(ui->action_Show_Primary_Cells,   SIGNAL(toggled(bool)), glWidget, SLOT(setPrimaryVisibility(bool)));
        connect(ui->action_Show_Secondary_Cells, SIGNAL(toggled(bool)), glWidget, SLOT(setSecondaryVisibility(bool)));

        // Feature Visibility

        connect(ui->action_Fault,   SIGNAL(toggled(bool)), glWidget, SLOT(showFault(bool)));
        connect(ui->action_BorderLine, SIGNAL(toggled(bool)), glWidget, SLOT(showBorderLines(bool)));

        connect(ui->horizontalSliderMeanFilterSize, SIGNAL(valueChanged(int)), glWidget, SLOT(meanFilterSize(int)));
        connect(ui->horizontalSliderLineWith      , SIGNAL(valueChanged(int)), glWidget, SLOT(borderLinesSize(int)));

        // Benchmark

        connect(glWidget, SIGNAL(fpsChanged(const QString&)), fps, SLOT(setText(const QString&)));
        connect(glWidget, SIGNAL(fpsChanged(const QString&)), ui->label_fps_slot, SLOT(setText(const QString&)));
}


void MainWindow::open( QString pFilename,bool who ) {

	glWidget->openIRES_v2( pFilename.toLocal8Bit().constData() );


	loadStatic ( );
        loadDynamic ( );

}

void MainWindow::loadStatic( )
{

        if ( glWidget->isIRESOpen( ))
        {
                QString title ( reinterpret_cast<char*>(glWidget->reservoir_model_.header_v2_.title) );

                ui->simulatorName->setText (  title );
                ui->label_ModelNameSlot->setText ( title );
                ui->label_NumberOfCellsSlot->setText( QString::number( glWidget->reservoir_model_.number_of_blocks_ ) );

                updateDoubleSpinMax( 0 );
                updateDoubleSpinMin( 0 );
                ui->comboBoxProperty->clear();

                for ( std::size_t i = 0 ; i < 2; ++i )
                {
                        ui->comboBoxProperty->addItem(  QString::fromStdString( glWidget->reservoir_model_.static_name[i] ) );
                }

                ui->label_I_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numI)+" ]");
                ui->label_J_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numJ)+" ]");
                ui->label_K_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numK)+" ]");


                ui->horizontalslider_max_I->setMaximum(glWidget->reservoir_model_.header_v2_.numI+1);
                ui->horizontalslider_max_J->setMaximum(glWidget->reservoir_model_.header_v2_.numJ+1);
                ui->horizontalslider_max_K->setMaximum(glWidget->reservoir_model_.header_v2_.numK+1);

                ui->horizontalslider_min_I->setMaximum(glWidget->reservoir_model_.header_v2_.numI+1);
                ui->horizontalslider_min_J->setMaximum(glWidget->reservoir_model_.header_v2_.numJ+1);
                ui->horizontalslider_min_K->setMaximum(glWidget->reservoir_model_.header_v2_.numK+1);

                ui->horizontalslider_max_I->setValue ( glWidget->reservoir_model_.header_v2_.numI+1 );
                ui->horizontalslider_max_J->setValue ( glWidget->reservoir_model_.header_v2_.numJ+1 );
                ui->horizontalslider_max_K->setValue ( glWidget->reservoir_model_.header_v2_.numK+1 );

                ui->horizontalslider_min_I->setValue ( 0 );
                ui->horizontalslider_min_J->setValue ( 0 );
                ui->horizontalslider_min_K->setValue ( 0 );

                changeProperty();
        }

}
void MainWindow::loadDynamic( )
{
	if ( glWidget->isIRESOpen( ) )
	{

		QString title ( reinterpret_cast<char*>(glWidget->reservoir_model_.header_v2_.title) );

		ui->labelSimulatorDynamic->setText (  title );

		updateDynamicDoubleSpinMin( 0 );
		updateDynamicDoubleSpinMax( 0 );

		ui->sliderTimeLine_Dynamic->setMinimum(0);
		ui->comboBoxProperty_Dynamic->clear();

		for ( std::size_t i = 0; i < glWidget->reservoir_model_.dynamic_properties.size(); i++ )
		{
			ui->comboBoxProperty_Dynamic->addItem(  QString::fromStdString( glWidget->reservoir_model_.dynamic_properties[i].name ) );
		}

		ui->sliderTimeLine_Dynamic->setMaximum(glWidget->reservoir_model_.dynamic_properties[0].min_.size()-1);
		ui->labelTimeStepsShow_Dynamic->setText( QString::number (glWidget->reservoir_model_.dynamic_properties[0].min_.size()) );

	}
}

void MainWindow::updateDynamicDoubleSpinMin( int property_index  )
{

	float min =  glWidget->reservoir_model_.dynamic_properties[property_index].min_[0];
	float max =  glWidget->reservoir_model_.dynamic_properties[property_index].max_[0];

	ui->doubleSpinMin_Dynamic->setMinimum ( static_cast<float> (min) );
	ui->doubleSpinMin_Dynamic->setMaximum ( static_cast<float> (max));

	ui->doubleSpinMax_Dynamic->setMinimum ( static_cast<float> (min) );
	ui->doubleSpinMax_Dynamic->setMaximum ( static_cast<float> (max) );

	qDebug() << "Min: "<< min;

	//ui->doubleSpinMin_Dynamic->setValue(static_cast<double> (min));

}

void MainWindow::updateDynamicDoubleSpinMax( int property_index  )
{

	float min =  glWidget->reservoir_model_.dynamic_properties[property_index].min_[0];
	float max =  glWidget->reservoir_model_.dynamic_properties[property_index].max_[0];

	ui->doubleSpinMin_Dynamic->setMinimum ( static_cast<float> (min) );
	ui->doubleSpinMin_Dynamic->setMaximum ( static_cast<float> (max) );

	ui->doubleSpinMax_Dynamic->setMinimum ( static_cast<float> (min) );
	ui->doubleSpinMax_Dynamic->setMaximum ( static_cast<float> (max) );

	qDebug() << "Max: "<< max;

	//ui->doubleSpinMax_Dynamic->setValue(static_cast<float> (max));

}

void MainWindow::updateDoubleSpinMin( int property_index )
{

	float min =  glWidget->reservoir_model_.static_min[property_index];
	float max =  glWidget->reservoir_model_.static_max[property_index];

	ui->doubleSpinMin->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMin->setMaximum ( static_cast<double> (max));

	ui->doubleSpinMax->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMax->setMaximum ( static_cast<double> (max) );

	qDebug() << "Min: "<< min;

	ui->doubleSpinMin->setValue(static_cast<double> (min));


}

void MainWindow::updateDoubleSpinMax( int property_index )
{


	float min =  glWidget->reservoir_model_.static_min[property_index];
	float max =  glWidget->reservoir_model_.static_max[property_index];

	ui->doubleSpinMin->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMin->setMaximum ( static_cast<double> (max));

	ui->doubleSpinMax->setMinimum ( static_cast<double> (min) );
	ui->doubleSpinMax->setMaximum ( static_cast<double> (max) );

	qDebug() << "Max: "<< max;

	ui->doubleSpinMax->setValue(static_cast<double> (max));

}

void MainWindow::changeProperty()
{
	QMessageBox msgBox;

	if ( glWidget->isIRESOpen( ))
	{
		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBoxProperty->currentIndex() );
	}
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

void MainWindow::on_action_Open_PLY_triggered()
{
        // Asks for a file name to open.
        QString filename = QFileDialog::getOpenFileName (
                        this,
                        tr("Choose a Reservoir *.ply file"),
                        ".",
                        tr("ires reservoir model (*.ply)"));

        // Tries to load file if filename not empty.
        if ( filename != "" )
        {

                glWidget->loadPly(filename);

        }
}

void MainWindow::on_pushButton_Reset_IJK_clicked( )
{
	ui->label_I_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numI)+" ]");
	ui->label_J_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numJ)+" ]");
	ui->label_K_Range->setText("[ 0 - "+QString::number(glWidget->reservoir_model_.header_v2_.numK)+" ]");


	ui->horizontalslider_max_I->setMaximum(glWidget->reservoir_model_.header_v2_.numI+1);
	ui->horizontalslider_max_J->setMaximum(glWidget->reservoir_model_.header_v2_.numJ+1);
	ui->horizontalslider_max_K->setMaximum(glWidget->reservoir_model_.header_v2_.numK+1);

	ui->horizontalslider_min_I->setMaximum(glWidget->reservoir_model_.header_v2_.numI+1);
	ui->horizontalslider_min_J->setMaximum(glWidget->reservoir_model_.header_v2_.numJ+1);
	ui->horizontalslider_min_K->setMaximum(glWidget->reservoir_model_.header_v2_.numK+1);

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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_F)
	{
		showfullScreen_ = !showfullScreen_;
		if ( showfullScreen_ )
		{
                        this->menuBar()->setHidden(true);
                        this->statusBar()->setHidden(true);
		        showFullScreen();
		}else
		{
                        this->menuBar()->setHidden(false);
                        this->statusBar()->setHidden(false);
			showNormal();
		}
		event->accept();
	}
	else event->ignore();
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
	if ( glWidget->isIRESOpen( ))
	{

		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBoxProperty->currentIndex() );

	}else
	{
		//msgBox.setText("First of all. Open a file my friend !!  ");
	}
}

void MainWindow::on_doubleSpinMax_valueChanged  ( double i)
{

	if ( glWidget->isIRESOpen( )  )
	{
		glWidget->changePropertyRange( ui->doubleSpinMin->value(), ui->doubleSpinMax->value(), ui->comboBoxProperty->currentIndex() );
		std::cout << "doubleSpinMax ::::::::::::: " << ui->doubleSpinMin->value() << std::endl;
	}else
	{
		//msgBox.setText("First of all. Open a file my friend !!  ");
	}
}

void MainWindow::on_horizontalSliderMeanFilterSize_valueChanged  ( int i)
{
        ui->labelKernelSize->setText(QString::number(i));
}
