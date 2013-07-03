#ifndef _MYMAINWINDOW_
#define _MYMAINWINDOW_

#include <QtGui/QtGui>
#include <QMessageBox>

#include "ui_IRESCutaway.h"

#include <GUI/Qt/GLWidget/GLWidget.hpp>
#include <GUI/Qt/AboutWidget/AboutWidgetIRESReader.hpp>

class MainWindow: public QMainWindow
{
		Q_OBJECT

	public:
		MainWindow ( QMainWindow *parent = 0 );
		void open ( QString fileName = QString ( ) , bool who = true );
		void keyPressEvent ( QKeyEvent *e );

	public slots:

		void on_action_Open_IRES_triggered ( );
		void on_action_Load_Shaders_triggered ( );
		void on_pushButton_changePropertyRange_clicked ( );
		void on_pushButton_Reset_IJK_clicked( );
		void on_pushButton_changeIJK_clicked( );


		void on_action_Bounding_Box_Approach_triggered ( );
		void on_action_Burns_Approach_triggered ( );

		void updateDoubleSpinMax( int property_index );
		void updateDoubleSpinMin( int property_index );

		void on_spinBox_Cluster_Number_valueChanged  ( int i);





	private:
		Ui::MainWindow * ui;

		bool showfullScreen_;

		GLWidget * glWidget;

		AboutWidgetIRESReader * aboutIRESReader;
};

#endif
