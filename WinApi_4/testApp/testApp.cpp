#include <stdio.h>
#include <windows.h>

void main() 
{

HANDLE hFile;
OVERLAPPED over;
DWORD dByte, dwError;
int iByteToRead = 1;
BOOL bResult;
int iWord = 0, tek = 0, pred = 0, iBreak = 0, iBegin = 1, iEnd = 1;
wchar_t errMessage[256];

over.Offset = 0;
over.OffsetHigh = 0;

char cBuffer[1];
over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
if (over.hEvent == NULL)
{
    printf("\nError create event!!!");
    exit(0);
}

hFile = CreateFile( //L"C:\\Users\\progr\\Desktop\\randomWords.txt"
                    L"C:\\Users\\progr\\source\\repos\\C++\\WinAPI_Cpp\\WinApi_4\\randomWords.txt", // slovechko
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                    NULL
                  );

if (hFile == INVALID_HANDLE_VALUE)
{
    printf("\nError opening file!!!");
    exit(0);
}
while (1)
{
    bResult = ReadFile(hFile, cBuffer, iByteToRead, &dByte, &over);

    if (!bResult)
    {
        switch (dwError = GetLastError())
        {
            case ERROR_HANDLE_EOF:
            {
                printf("%ld", dwError);
                iBreak = 1;
                break;
            }

            case ERROR_IO_PENDING:
            {
                over.Offset++; 

                pred = tek;
                if (cBuffer[0] == ' ' || cBuffer[0] == '\x0D' || cBuffer[0] == '\n')tek = 0;
                else tek = 1;
                if (tek == 1 && pred == 0)iWord++;
                bResult = GetOverlappedResult(hFile, &over, &dByte, FALSE);

                if (!bResult)
                {
                    switch (dwError = GetLastError())
                    {
                        case ERROR_HANDLE_EOF:
                        {
                            iBreak = 1;
                            break;
                        }
                        default:
                        {
                            printf("%ld", dwError);
                            iBreak = 1;
                            break;
                        }
                    }// end of switch (dwError = GetLastError())
                }
                break;
            } // end of case
            default:
            {
                printf("%ld", dwError);
                iBreak = 1;
                break;
            }

        } // end of switch (dwError = GetLastError())
    } // end of if
    if (iBreak == 1)break;
    over.Offset++;
    pred = tek;
    if (cBuffer[0] == ' ' || cBuffer[0] == '\x0D' || cBuffer[0] == '\n')tek = 0;
    else tek = 1;
    if (tek == 1 && pred == 0)iWord++; //???????????? ????????????????????? ?????? ??????????????? ?????? ?????????????????? ??????????????????
}
printf("\nIn text %d words!!!", iWord);
getchar();
}