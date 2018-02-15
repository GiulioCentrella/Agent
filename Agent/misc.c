
#include "global.h"


// ----------------------------------------------------------------------
// Show error message
// ----------------------------------------------------------------------
VOID ErrMsg(LPTSTR lpszErrMsg)
{
    MessageBox(hWnd, lpszErrMsg, asLangTexts[iszTextError].lpszText, MB_ICONHAND);
}


#ifdef DEBUGLOG
// debug log files
LPTSTR lpszDebugTryToGetValueLog = TEXT("debug_trytogetvalue.log");
LPTSTR lpszDebugValueNameDataLog = TEXT("debug_valuenamedata.log");
LPTSTR lpszDebugKeyLog           = TEXT("debug_key.log");

// ----------------------------------------------------------------------
// Write message to debug log file
// ----------------------------------------------------------------------
VOID DebugLog(LPTSTR lpszFileName, LPTSTR lpszDbgMsg, BOOL fAddCRLF)
{
    size_t nLen;
    DWORD nPos;

    hFile = CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        ErrMsg(asLangTexts[iszTextErrorCreateFile].lpszText);
        return;
    }

    nPos = SetFilePointer(hFile, 0, NULL, FILE_END);
    if (0xFFFFFFFF == nPos) {
        ErrMsg(asLangTexts[iszTextErrorMoveFP].lpszText);
    } else {
        nLen = _tcslen(lpszDbgMsg) * sizeof(TCHAR);
        WriteFile(hFile, lpszDbgMsg, (DWORD)nLen, &NBW, NULL);
        if (NBW != nLen) {
            //ErrMsg(asLangTexts[iszTextErrorWriteFile].lpszText);
        }
        if (fAddCRLF) {
            WriteFile(hFile, lpszCRLF, (DWORD)(_tcslen(lpszCRLF) * sizeof(TCHAR)), &NBW, NULL);
        }
    }

    CloseHandle(hFile);
}
#endif


// ----------------------------------------------------------------------
// Replace invalid chars for a file name
// ----------------------------------------------------------------------
LPTSTR lpszInvalid = TEXT("\\/:*?\"<>|");  // 1.8.2

BOOL ReplaceInvalidFileNameChars(LPTSTR lpszFileName)
{
    size_t nInvalidLen;
    size_t nFileNameLen;
    size_t i, j;
    BOOL fFileNameIsLegal;
    BOOL fCharIsValid;

    nInvalidLen = _tcslen(lpszInvalid);
    nFileNameLen = _tcslen(lpszFileName);

    fFileNameIsLegal = FALSE;
    for (i = 0; i < nFileNameLen; i++) {
        fCharIsValid = TRUE;  // valid chars expected

        if ((TCHAR)'\t' == lpszFileName[i]) {  // replace tab with space
            lpszFileName[i] = (TCHAR)' ';
        } else {  // replace invalid char with underscore
            for (j = 0; j < nInvalidLen; j++) {
                if (lpszFileName[i] == lpszInvalid[j]) {
                    lpszFileName[i] = (TCHAR)'_';
                    fCharIsValid = FALSE;
                    break;
                }
            }
        }

        if ((fCharIsValid) && ((TCHAR)' ' != lpszFileName[i])) {  // At least one valid non-space char needed
            fFileNameIsLegal = TRUE;
        }
    }
    return fFileNameIsLegal;
}


// ----------------------------------------------------------------------
// Find lpszSearch's position in lpgrszSection
//
// Functionality is similar to GetPrivateProfileString(), but return value is a
// pointer inside a memory block with all strings (normally a section buffer),
// which avoids handling multiple pointers individually.
// The section buffer must not contain unnecessary whitespaces, comments,
// empty lines, etc. Windows' GetPrivateProfileSection() already cares about this.
// ----------------------------------------------------------------------
LPTSTR FindKeyInIniSection(LPTSTR lpgrszSection, LPTSTR lpszSearch, size_t cchSectionLen, size_t cchSearchLen)
{
    size_t i;
    size_t nszSectionLen;

    if ((NULL == lpgrszSection) || (NULL == lpszSearch) || (1 > cchSearchLen)) {
        return NULL;
    }

    for (i = 0; i < cchSectionLen; i++) {
        if (0 == lpgrszSection[i]) {  // reached the end of the section buffer
            break;
        }

        nszSectionLen = _tcslen(&lpgrszSection[i]);
        if (nszSectionLen <= cchSearchLen) {
            i += nszSectionLen;  // skip this string as it is too short (or would result in an empty return string)
            continue;
        }



        i += nszSectionLen;  // skip this string as it does not match
    }

    return NULL;
}
