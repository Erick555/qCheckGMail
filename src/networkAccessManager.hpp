/*
 * copyright: 2016
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef NetworkAccessManager_H
#define NetworkAccessManager_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QEventLoop>

#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <iostream>

class NetworkAccessManager : public QObject
{
        Q_OBJECT
public:
	using NetworkReply = std::unique_ptr< QNetworkReply,void( * )( QNetworkReply * ) > ;
	using function_t   = std::function< void( QNetworkReply& ) > ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	NetworkAccessManager()
	{
		connect( &m_manager,SIGNAL( finished( QNetworkReply * ) ),
			 this,SLOT( networkReply( QNetworkReply * ) ),Qt::QueuedConnection ) ;
	}
#else
	NetworkAccessManager()
	{
                connect( &m_manager,&QNetworkAccessManager::finished,
                         this,&NetworkAccessManager::networkReply,Qt::QueuedConnection ) ;
	}
#endif
	QNetworkAccessManager& QtNAM()
	{
		return m_manager ;
	}
	void get( QNetworkReply ** e,const QNetworkRequest& r,function_t f )
	{
		auto s = m_manager.get( r ) ;

		if( e ){

			*e = s ;
		}

		m_entries.emplace_back( s,true,std::move( f ) ) ;
	}
	NetworkReply get( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop s ;

		auto function = [ & ]( QNetworkReply& e ){ q = std::addressof( e ) ; s.quit() ;	} ;

		m_entries.emplace_back( std::make_tuple( m_manager.get( r ),false,std::move( function ) ) ) ;

		s.exec() ;

		return { q,[]( QNetworkReply * e ){ e->deleteLater() ; } } ;
	}
	template< typename T >
	void post( const QNetworkRequest& r,const T& e,function_t f )
	{
		m_entries.emplace_back( std::make_tuple( m_manager.post( r,e ),true,std::move( f ) ) ) ;
	}
	template< typename T >
	NetworkReply post( const QNetworkRequest& r,const T& e )
	{
		QNetworkReply * q ;

		QEventLoop s ;

		auto function = [ & ]( QNetworkReply& e ){ q = std::addressof( e ) ; s.quit() ;	} ;

		m_entries.emplace_back( std::make_tuple( m_manager.post( r,e ),false,std::move( function ) ) ) ;

		s.exec() ;

		return { q,[]( QNetworkReply * e ){ e->deleteLater() ; } } ;
	}
	void head( const QNetworkRequest& r,function_t f )
	{
		m_entries.emplace_back( std::make_tuple( m_manager.head( r ),true,std::move( f ) ) ) ;
	}
	NetworkReply head( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop s ;

		auto function = [ & ]( QNetworkReply& e ){ q = std::addressof( e ) ; s.quit() ;	} ;

		m_entries.emplace_back( std::make_tuple( m_manager.head( r ),false,std::move( function ) ) ) ;

		s.exec() ;

		return { q,[]( QNetworkReply * e ){ e->deleteLater() ; } } ;
	}
	void cancel( QNetworkReply * e )
	{
		auto s = this->find_network_reply( e ) ;

		if( s != -1 ){

			m_entries.erase( m_entries.begin() + s ) ;

			e->close() ;
			e->abort() ;

			if( std::get< bool >( m_entries[ s ] ) ){

				e->deleteLater() ;
			}
		}
	}
	int find_network_reply( QNetworkReply * e )
	{
		for( decltype( m_entries.size() ) s = 0 ; s < m_entries.size() ; s++ ){

			if( std::get< QNetworkReply * >( m_entries[ s ] ) == e ){

				return static_cast< int >( s ) ;
			}
		}

		return -1 ;
	}
private slots:
	void networkReply( QNetworkReply * e )
	{
		auto s = this->find_network_reply( e ) ;

		if( s != -1 ){

			std::get< function_t >( m_entries[ s ] )( *e ) ;

			m_entries.erase( m_entries.begin() + s ) ;

			if( std::get< bool >( m_entries[ s ] ) ){

				e->deleteLater() ;
			}
		}
	}
private:
	std::vector< std::tuple< QNetworkReply *,bool,function_t > > m_entries ;
	QNetworkAccessManager m_manager ;
};

#endif
