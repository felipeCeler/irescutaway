#ifndef _MYMAINWINDOW_
#define _MYMAINWINDOW_

#include <QtGui/QtGui>

#include <QMessageBox>

#include "ui_IRESReader.h"

#include <GUI/Qt/GLWidget/GLWidget.hpp>

class QWorkspace;

class MainWindow: public QMainWindow , private Ui::MainWindow
{
		Q_OBJECT

	public:
		MainWindow ( QMainWindow *parent = 0 );
		void open ( QString fileName = QString ( ) , bool who = true );
		void keyPressEvent ( QKeyEvent *e );

	public slots:

		void on_action_Open_IRES_triggered ( );
		void on_action_Load_Shaders_triggered ( );
		void on_pushButton_do_something_clicked ( );

		void updateDoubleSpinMax( int property_index );
		void updateDoubleSpinMin( int property_index );



	private:
		bool showfullScreen_;
		GLWidget* glWidget;
};

#endif
