/*
 * SnapIpUrl.c
 *
 *  Created on: 23 nov 2017
 *      Author: admin
 */

#include "global.h"

void *th_snapIP_URL() {

	char psBuffer[10000];
	FILE *chkdsk;
	FILE *chkdsk2;


		if ((chkdsk = _popen("ipconfig /displaydns > Ipconfig.txt ", "r"))
				== NULL)
			exit(1);

		if ((chkdsk2 = _popen(
				"netstat -n  | findstr ESTABLISHED > netstat.txt ", "r"))
				== NULL)
			exit(1);

		while (!feof(chkdsk) && !feof(chkdsk2)) {
			if (fgets(psBuffer, 10000, chkdsk) != NULL
					&& fgets(psBuffer, 10000, chkdsk2) != NULL)
				printf(psBuffer);
		}

		printf("\nProcess returned %d\n", _pclose(chkdsk));

		printf("---Snap IP COMPLETE---\n\n");

}

