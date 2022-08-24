/*
 *
 *  Copyright (c) 2013
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "addaccount.h"
#include "ui_addaccount.h"

#include "gmailauthorization.h"

#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>

addaccount::addaccount( QDialog * parent,
			settings& e,
			gmailauthorization::getAuth& k,
			addaccount::Actions s,
			addaccount::GMailInfo& n ) :
	QDialog( parent ),
	m_ui( new Ui::addaccount ),
	m_getAuthorization( k ),
	m_actions( std::move( s ) ),
	m_gmailAccountInfo( n ),
	m_setting( e )
{
	m_ui->setupUi( this ) ;

	//this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	m_edit = false ;

	connect( m_ui->pushButtonAdd,&QPushButton::clicked,this,&addaccount::add ) ;
	connect( m_ui->pushButtonCancel,&QPushButton::clicked,this,&addaccount::cancel ) ;

	m_ui->pushButtonAdd->setMinimumHeight( 31 ) ;
	m_ui->pushButtonCancel->setMinimumHeight( 31 ) ;

	class accs : public gmailauthorization::actions
	{
	public:
		accs( addaccount * acc ) : m_parent( acc )
		{
		}
		void cancel() override
		{
			m_parent->cancel() ;
		}
		void getToken( const QString& e,const QByteArray& s ) override
		{
			if( e.isEmpty() ){

				m_parent->HideUI() ;

				std::cout << "ERROR: Failed To Generate Token\n" ;
				std::cout << s.constData() << std::endl ;
			}else{
				m_parent->getLabels( e ) ;
			}
		}
	private:
		addaccount * m_parent ;
	};

	gmailauthorization::instance( this,
				      m_setting,
				      m_getAuthorization,
				      { util::type_identity< accs >(),this } ) ;
}

addaccount::addaccount( QDialog * parent,
			settings& m,
			const accounts::entry& e,
			gmailauthorization::getAuth& k,
			addaccount::Actions s,
			addaccount::GMailInfo& n ) :
	QDialog( parent ),
	m_ui( new Ui::addaccount ),
	m_getAuthorization( k ),
	m_actions( std::move( s ) ),
	m_gmailAccountInfo( n ),
	m_setting( m )
{
	m_ui->setupUi( this );
	//this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	m_edit = true ;

	m_ui->lineEditName->setText( e.accName ) ;
	m_ui->lineEditLabel->setText( util::namesFromJson( e.accLabels ) ) ;

	m_ui->lineEditName->setToolTip( QString() ) ;

	connect( m_ui->pushButtonAdd,&QPushButton::clicked,this,&addaccount::add ) ;
	connect( m_ui->pushButtonCancel,&QPushButton::clicked,this,&addaccount::cancel ) ;

	if( m_edit ){

		m_ui->pushButtonAdd->setText( tr( "Edit" ) ) ;
		this->setWindowTitle( tr( "Edit Account" ) ) ;
	}

	this->show() ;
}

void addaccount::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void addaccount::getLabels( const QString& e )
{
	m_key = e ;

	class meaw : public addaccount::gmailAccountInfo
	{
	public:
		meaw( addaccount * acc ) : m_parent( acc )
		{
		}
		void operator()( addaccount::labels s ) override
		{
			m_parent->m_labels = std::move( s ) ;
			m_parent->show() ;
		}
	private:
		addaccount * m_parent ;
	};

	m_gmailAccountInfo( e,{ util::type_identity< meaw >(),this } ) ;
	this->show() ;
}

addaccount::~addaccount()
{
	delete m_ui ;
}

void addaccount::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->cancel() ;
}

void addaccount::add()
{
	if( m_edit ){

		m_actions.edit( { m_ui->lineEditName->text(),m_ui->lineEditLabel->text(),{} } ) ;

		this->HideUI() ;
	}else{
		auto accName  = this->m_ui->lineEditName->text() ;

		if( accName.isEmpty() ){

			QMessageBox msg( this ) ;

			msg.setText( tr( "ERROR: One Or More Required Field Is Empty" ) ) ;
			msg.exec() ;
		}else{
			auto lbs = util::labelsToJson( this->m_ui->lineEditLabel->text(),m_labels.entries ) ;

			m_actions.results( { accName,std::move( lbs ),m_key } ) ;

			this->HideUI() ;
		}
	}
}

void addaccount::cancel()
{
	m_actions.cancel() ;
	this->HideUI() ;
}

addaccount::actions::~actions()
{
}

addaccount::gmailAccountInfo::~gmailAccountInfo()
{
}

addaccount::gMailInfo::~gMailInfo()
{
}
