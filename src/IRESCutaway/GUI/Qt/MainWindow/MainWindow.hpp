#ifndef _MYMAINWINDOW_
#define _MYMAINWINDOW_

#include <GUI/Qt/GLWidget/GLWidget.hpp>
#include <GUI/Qt/AboutWidget/AboutWidgetIRESReader.hpp>

//#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <iostream>

#include "ui_IRESCutaway.h"

class MainWindow: public QMainWindow
{
		Q_OBJECT

	public:
		MainWindow ( QMainWindow *parent = 0 );
		void open ( QString fileName = QString ( ) , bool who = true );
		Ui::MainWindow * ui;

		void keyPressEvent ( QKeyEvent * event );

		GLWidget *              glWidget;

	public slots:

		// Camera Slots
		void on_action_Trackball_triggered   ( );
		void on_action_Fly_Camera_triggered  ( );
		void on_action_Freeze_View_triggered ( );
		// Load Shaders
		void on_action_Load_Shaders_triggered ( );
		// --
		void on_action_Open_IRES_triggered ( );
		void on_action_Open_PLY_triggered ( );
		void on_pushButton_Reset_IJK_clicked( );


		void loadStatic( );
		void updateDoubleSpinMax( int property_index );
		void updateDoubleSpinMin( int property_index );

		void on_doubleSpinMin_valueChanged  ( double i);
		void on_doubleSpinMax_valueChanged  ( double i);
		//
		void changeProperty ( );


		void loadDynamic ( );
		void updateDynamicDoubleSpinMax( int property_index );
		void updateDynamicDoubleSpinMin( int property_index );

		void on_horizontalSliderMeanFilterSize_valueChanged  ( int i);

//		void on_horizontalSlider_PrimariesLuminance_valueChanged  ( int i );
//		void on_horizontalSlider_PrimariesSaturation_valueChanged  ( int i );
//
//		void on_horizontalSlider_SecondariesLuminance_valueChanged  ( int i );
//		void on_horizontalSlider_SecondariesStaturation_valueChanged  ( int i);
//
//                void on_horizontalSlider_SellLuminance_valueChanged  ( int i );
//                void on_horizontalSlider_SellStaturation_valueChanged  ( int i );

		//
		void WidgetSignalSlotConnection();
		void setFullScreen ();


	private:


		bool                    showfullScreen_;

		AboutWidgetIRESReader * aboutIRESReader;

		QLabel *                fps;
};

#endif
