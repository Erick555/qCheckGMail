/*
 * copyright: 2013
 * name : mhogo mchungu
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

#include "lxqt_wallet_interface.h"
#include "lxqt_internal_wallet.h"
#include "../backend/lxqtwallet.h"

/*
 * This header file is generated at configure time by a routine that checks if kwallet and gnome keyrings are to be supported
 */
#include "storage_manager.h"

#if HAS_KWALLET_SUPPORT
#include "lxqt_kwallet.h"
#endif

/*
 * No support for this backend for now
 */
#define HAS_GNOME_KEYRING_SUPPORT 0

lxqt::Wallet::Wallet::Wallet()
{

}

lxqt::Wallet::Wallet::~Wallet()
{

}

lxqt::Wallet::Wallet * lxqt::Wallet::getWalletBackend( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internalBackEnd ){
		return new lxqt::Wallet::internalWallet() ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return new lxqt::Wallet::kwallet();
		#else
			return NULL ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return new lxqt::Wallet::gnomeKeyring() ;
		#else
			return NULL ;
		#endif
	}

	return NULL ;
}

bool lxqt::Wallet::backEndIsSupported( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internalBackEnd ){
		return true ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return true ;
		#else
			return false ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return true ;
		#else
			return false ;
		#endif
	}

	return false ;
}

bool lxqt::Wallet::deleteWallet( lxqt::Wallet::walletBackEnd bk,const QString& walletName,const QString& applicationName )
{
	QString appName ;
	if( applicationName.isEmpty() ){
		appName = walletName ;
	}else{
		appName = applicationName ;
	}

	if( bk == lxqt::Wallet::internalBackEnd ){
		return lxqt_wallet_delete_wallet( walletName.toAscii().constData(),appName.toAscii().constData() ) == lxqt_wallet_no_error ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return KWallet::Wallet::deleteWallet( walletName ) == 0 ;
		#else
			return false ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return false ;
		#else
			return false ;
		#endif
	}

	return false ;
}

bool lxqt::Wallet::walletExists( lxqt::Wallet::walletBackEnd bk,const QString& walletName,const QString& applicationName )
{
	QString appName ;
	if( applicationName.isEmpty() ){
		appName = walletName ;
	}else{
		appName = applicationName ;
	}

	if( bk == lxqt::Wallet::internalBackEnd ){
		return lxqt_wallet_exists( walletName.toAscii().constData(),appName.toAscii().constData() ) ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return !KWallet::Wallet::folderDoesNotExist( walletName,appName ) ;
		#else
			return false ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return false ;
		#else
			return false ;
		#endif
	}

	return false ;
}

