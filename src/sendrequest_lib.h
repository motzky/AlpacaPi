//*****************************************************************************
//#include	"sendrequest_lib.h"

#ifndef _SENDREQUEST_LIB_H
#define _SENDREQUEST_LIB_H

#ifndef _JSON_PARSE_H_
	#include	"json_parse.h"
#endif // _JSON_PARSE_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define		kReadBuffLen		2000
#define		kLargeBufferSize	12000


void	PrintIPaddressToString(const long ipAddress, char *ipString);
bool	GetJsonResponse(	struct sockaddr_in	*deviceAddress,
							const int			port,
							const char			*sendData,
							const char			*dataString,
							SJP_Parser_t		*jsonParser);
bool	SendPutCommand(		struct sockaddr_in	*deviceAddress,
							const int			port,
							const char			*putCommand,
							const char			*dataString,
							SJP_Parser_t		*jsonParser);


int		OpenSocketAndSendRequest(	struct sockaddr_in	*deviceAddress,
									const int			port,
									const char			*get_put_string,	//*	must be either GET or PUT
									const char			*sendData,
									const char			*dataString);

#ifdef __cplusplus
}
#endif


#endif // _SENDREQUEST_LIB_H
