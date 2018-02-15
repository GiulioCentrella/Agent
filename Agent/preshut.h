/*
 * preshut.h
 *
 *  Created on: 25 gen 2018
 *      Author: admin
 */

#ifndef PRESHUT_H_
#define PRESHUT_H_
#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#ifndef WINADVAPI
#define WINADVAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SERVICE_CONFIG_PRESHUTDOWN_INFO  7

#ifndef _SERVICE_PRESHUTDOWN_INFO_DEFINED_
#define _SERVICE_PRESHUTDOWN_INFO_DEFINED_
typedef struct _SERVICE_PRESHUTDOWN_INFO {
     DWORD dwPreshutdownTimeout;
} SERVICE_PRESHUTDOWN_INFO, *LPSERVICE_PRESHUTDOWN_INFO;
 #endif
#ifdef __cplusplus
}
#endif

#endif /* PRESHUT_H_ */
