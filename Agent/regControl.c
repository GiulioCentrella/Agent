

#include "global.h"
#include "version.h"

/*
    Define window title for main with version, revision, etc. (see version.rc.h for title structure)
*/
LPTSTR lpszProgramName = REGSHOT_TITLE;  // tfx  add program titile


LPTSTR lpszIniFileName      = TEXT("regshot.ini"); // tfx
LPTSTR lpszLanguageFileName = TEXT("language.ini");

LPTSTR lpszExtDir;
LPTSTR lpszOutputPath;
LPTSTR lpszLastSaveDir;
LPTSTR lpszLastOpenDir;
LPTSTR lpszWindowsDirName;
LPTSTR lpszTempPath;
LPTSTR lpszStartDir;
LPTSTR lpszLanguageIni;  // For language.ini
LPTSTR lpszCurrentTranslator;
LPTSTR lpszRegshotIni;

MSG        msg;          // Windows MSG struct
HWND       hWnd;         // The handle of REGSHOT
HMENU      hMenu;        // Handle of popup menu
LPREGSHOT  lpMenuShot;   // Pointer to current Shot for popup menus and alike
RECT       rect;         // Window RECT
BROWSEINFO BrowseInfo1;  // BrowseINFO struct

BOOL swith= FALSE;

#ifdef USEHEAPALLOC_DANGER
HANDLE hHeap;  // 1.8.2
#endif


void *th_regControl()
{


#ifdef USEHEAPALLOC_DANGER
    hHeap = GetProcessHeap();
#endif

    size_t  nLen;
    HWND hDlg;
    //enlarge some buffer in 201201
               lpszLanguage       = NULL;
               lpszExtDir         = MYALLOC0(EXTDIRLEN * sizeof(TCHAR));      // EXTDIRLEN is actually MAX_PATH * 4
               lpszLanguageIni    = MYALLOC0((MAX_PATH + 1 + _tcslen(lpszLanguageFileName)) * sizeof(TCHAR));   // for language.ini
               lpszRegshotIni     = MYALLOC0((MAX_PATH + 1 + _tcslen(lpszIniFileName)) * sizeof(TCHAR));   // for regshot.ini
               //lpszMessage        = MYALLOC0(REGSHOT_MESSAGE_LENGTH * sizeof(TCHAR));  // For status bar text message store
               lpszWindowsDirName = MYALLOC0(MAX_PATH * sizeof(TCHAR));
               lpszTempPath       = MYALLOC0(MAX_PATH * sizeof(TCHAR));
               lpszStartDir       = MYALLOC0(MAX_PATH * sizeof(TCHAR));
               lpszOutputPath     = MYALLOC0(MAX_PATH * sizeof(TCHAR));  // store last save/open hive file dir, +1 for possible change in CompareShots()
               lpgrszLangSection  = NULL;

               ZeroMemory(&Shot1, sizeof(Shot1));
               ZeroMemory(&Shot2, sizeof(Shot2));
               ZeroMemory(&CompareResult, sizeof(CompareResult));

               GetWindowsDirectory(lpszWindowsDirName, MAX_PATH);  // length incl. NULL character
               lpszWindowsDirName[MAX_PATH] = (TCHAR)'\0';

               GetTempPath(MAX_PATH, lpszTempPath);  // length incl. NULL character
               lpszTempPath[MAX_PATH] = (TCHAR)'\0';

               //_asm int 3;
               GetCurrentDirectory(MAX_PATH, lpszStartDir);  // length incl. NULL character // fixed in 1.8.2 former version used getcommandline()
               lpszStartDir[MAX_PATH] = (TCHAR)'\0';

               _tcscpy(lpszLanguageIni, lpszStartDir);
               nLen = _tcslen(lpszLanguageIni);
               if (0 < nLen) {
                   nLen--;
                   if (lpszLanguageIni[nLen] != (TCHAR)'\\') {
                       _tcscat(lpszLanguageIni, TEXT("\\"));
                   }
               }
               _tcscat(lpszLanguageIni, lpszLanguageFileName);

               _tcscpy(lpszRegshotIni, lpszStartDir);
               nLen = _tcslen(lpszRegshotIni);
               if (0 < nLen) {
                   nLen--;
                   if (lpszRegshotIni[nLen] != (TCHAR)'\\') {
                       _tcscat(lpszRegshotIni, TEXT("\\"));
                   }
               }
               _tcscat(lpszRegshotIni, lpszIniFileName);

               lpszLastSaveDir = lpszOutputPath;
               lpszLastOpenDir = lpszOutputPath;

               LoadSettingsFromIni(hDlg); // tfx


               int exists(const char *filename){
            	  if(access (filename, F_OK) != -1){
            		  //printf("FirstShot exists..Snap SecondShot..\n");
            		  return 1;
            	  }
            	  return 0;
               }


              if(exists(REGSHOT_PATH)){
            	  //load shot1
            	  LoadShot(&Shot1);
            	  //SHOT2
            	  Shot(&Shot2);

            	  printf("SecondShot create...\n");

            	  //salvo shot2 in shot1 path
            	  SaveShot(&Shot2);

            	  //compare
            	  CompareShots();
            	  OutputComparisonResult();

            	  //Libero la memoria
            	  FreeShot(&Shot1);
            	  FreeShot(&Shot2);
            	  FreeCompareResult();

            	  printf("CompareShot terminate success!");

              }else{
            	  printf("FirstShot create.");
            	  Shot(&Shot1);
            	  SaveShot(&Shot1);
            	  FreeShot(&Shot1);
              }


              printf("\n---Snap REG COMPLETE---\n\n");


}

#ifdef REPLACEMENT_STRNLEN
size_t strnlen(const char *lpszText, size_t cchMax)
{
    size_t cchActual;

    for (cchActual = 0; (cchActual < cchMax) && ('\0' != lpszText[cchActual]); cchActual++) {
    }

    return cchActual;
}
#endif



