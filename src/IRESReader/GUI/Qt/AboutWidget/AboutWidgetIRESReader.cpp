/*
 * AboutWidgetIRESReader.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: felipe
 */

#include "AboutWidgetIRESReader.hpp"

AboutWidgetIRESReader::AboutWidgetIRESReader ( QWidget *parent ) : QDialog ( parent )
{
	// TODO Auto-generated constructor stub
	ui = new Ui::AboutDialog;
	ui->setupUi ( this );
	setWindowFlags(Qt::Dialog);

}

AboutWidgetIRESReader::~AboutWidgetIRESReader ( )
{
	// TODO Auto-generated destructor stub
	delete ui;
}

