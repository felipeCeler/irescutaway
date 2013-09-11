/*
 * AboutWidgetIRESReader.hpp
 *
 *  Created on: Apr 15, 2013
 *      Author: felipe
 */

#ifndef ABOUTWIDGETIRESREADER_HPP_
#define ABOUTWIDGETIRESREADER_HPP_

#include <QtGui/QDialog>

#include "ui_AboutWidgetIRESReader.h"

class AboutWidgetIRESReader: public QDialog
{
		Q_OBJECT
	public:
		AboutWidgetIRESReader ( QWidget *parent = 0 );
		virtual ~AboutWidgetIRESReader ( );
	private:

		Ui::AboutDialog * ui;

};

#endif /* ABOUTWIDGETIRESREADER_HPP_ */
