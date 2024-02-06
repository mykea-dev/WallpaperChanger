#pragma comment(lib, "urlmon.lib")

#include <iostream>
#include <windows.h>
#include "wininet.h"
#include "shlobj.h"
#include <iostream>
#include <urlmon.h>
#include <direct.h>
#include <locale>

using namespace std;

#pragma (push)
#pragma warning (disable: 4996)

string appdata = string(getenv("APPDATA")) + "\\WallpaperChanger";

#pragma (pop)

LPWSTR ctowc(const char* from)
{
    if (from == nullptr) return nullptr;
    
    const int nChars = MultiByteToWideChar(CP_ACP, 0, from, -1, nullptr, 0);
    const auto to = new wchar_t[nChars];
    MultiByteToWideChar(CP_ACP, 0, from, -1, const_cast<LPWSTR>(to), nChars);
    return to;
}

void SetWallpaper(const LPCWSTR file, const DWORD dwStyle = WPSTYLE_STRETCH)
{
    HRESULT status = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    IActiveDesktop* desktop;

    status = CoCreateInstance(CLSID_ActiveDesktop, nullptr, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (void**) &desktop);
    WALLPAPEROPT wOption;
    ZeroMemory(&wOption, sizeof(WALLPAPEROPT));
    wOption.dwSize=sizeof(WALLPAPEROPT);
    wOption.dwStyle = dwStyle;
    
    status = desktop->SetWallpaper(file, 0);
    wcout << "SetWallpaper status: " << !status << "\n";
    
    status = desktop->SetWallpaperOptions(&wOption,0);
    wcout << "SetWallpaperOptions status: " << !status << "\n";
    
    status = desktop->ApplyChanges(AD_APPLY_ALL);
    wcout << "ApplyChanges status: " << !status << "\n";
    
    desktop->Release();
    CoUninitialize();
    wstring str(file);
    wcout << str;
    //SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID) str.c_str(), SPIF_UPDATEINIFILE);
}

int DownloadWallpaperImage(string URL)
{
    cout << "Downloading...\n";
    string FilePath = appdata + "\\image.png";
    
    const auto tempUrl = wstring(URL.begin(), URL.end());
    const auto tempPath = wstring(FilePath.begin(), FilePath.end());

    const auto wideStringUrl = tempUrl.c_str();
    const auto wideStringPath = tempPath.c_str();

    HRESULT reason = URLDownloadToFile(nullptr, wideStringUrl, wideStringPath, 0, nullptr);
    if (S_OK == reason) {
        cout << "Download success\n";
        return 1;
    }
    const char* error = (reason == INET_E_DOWNLOAD_FAILURE) ? "inet failure out of memory" : "out of memory";
    cout << "Download fails: " << error << "\n";
    return 0;
}

char* ToLowerCase(char* str)
{
    for (size_t i = 0; i < strlen(str); ++i) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
    return str;
}

int DownloadAndSet(const string& URL, const DWORD dwStyle = WPSTYLE_STRETCH)
{
    if (!DownloadWallpaperImage(URL)) return 0;
        
    const string imagePath = appdata + "\\image.png";
    SetWallpaper(ctowc(imagePath.c_str()), dwStyle);
    cout << "Success.";
    return 1;
}

DWORD ParseStyle(char* str)
{
    str = ToLowerCase(str);
    if (strcmp(ToLowerCase(str), "center") == 0) return WPSTYLE_CENTER;
    if (strcmp(ToLowerCase(str), "max") == 0) return WPSTYLE_MAX;
    if (strcmp(ToLowerCase(str), "span") == 0) return WPSTYLE_SPAN;
    if (strcmp(ToLowerCase(str), "tile") == 0) return WPSTYLE_TILE;
    if (strcmp(ToLowerCase(str), "croptofit") == 0) return WPSTYLE_CROPTOFIT;
    if (strcmp(ToLowerCase(str), "keepaspect") == 0) return WPSTYLE_KEEPASPECT;
    return WPSTYLE_STRETCH;
}

int main(const int argc, char* argv[])
{
    _mkdir(appdata.c_str());

    //https://vk.cc/cukFrj //ON RELEASE:
    
    const string SPIDER("https://w7.pngwing.com/pngs/895/199/png-transparent-spider-man-heroes-download-with-transparent-background-free-thumbnail.png");
    const string WINDOWS("https://static1.makeuseofimages.com/wordpress/wp-content/uploads/2022/05/00-lead-windows-default-wallpaper.jpg?q=50&fit=contain&w=1140&h=&dpr=1.5");
    const string WIN11_SUNSET("https://r4.wallpaperflare.com/wallpaper/203/636/834/minimalism-landscape-digital-windows-11-hd-wallpaper-9e8e3bc0a3fca7e692545948225de200.jpg");
    const string WIN11_VIOLET("https://r4.wallpaperflare.com/wallpaper/218/952/879/windows-11-microsoft-minimalism-digital-art-hd-wallpaper-e01035795f367ec338df8736eb964418.jpg");
    
    if (argc <= 2)
    {
        wcout << "Usage: " << argv[0] <<" \n 1: --url <url to image> \n 1: --path <path to image>\n 1: --default <SPIDER,WINDOWS,W11_V,W11_S>\n 2: --style <STRETCH,CENTER,MAX,SPAN,TILE,CROPTOFIT,KEEPASPECT>\n";
        if (!DownloadWallpaperImage(WIN11_VIOLET)) return 0;
        
        const string imagePath = appdata + "\\image.png";
        SetWallpaper(ctowc(imagePath.c_str()));
        remove(imagePath.c_str());
        cout << "Success.";
        return -1;
    }
    
    if (strcmp(argv[1], "--path") == 0)
    {
        DWORD style = WPSTYLE_STRETCH;
        if (argc >= 4) style = ParseStyle(argv[4]);
        
        SetWallpaper(ctowc(argv[2]), style);
        cout << "Success.";
        return 0;
    }

    if (strcmp(argv[1], "--url") == 0)
    {
        DWORD style = WPSTYLE_STRETCH;
        if (argc >= 4) style = ParseStyle(argv[4]);
        
        return DownloadAndSet(argv[2], style);
    }
    
    if (strcmp(argv[1], "--default") == 0)
    {
        DWORD style = WPSTYLE_STRETCH;
        if (argc >= 4) style = ParseStyle(argv[4]);
        
        if (strcmp(ToLowerCase(argv[2]), "spider") == 0) return DownloadAndSet(SPIDER, style);
        if (strcmp(ToLowerCase(argv[2]), "windows") == 0) return DownloadAndSet(WINDOWS, style);
        if (strcmp(ToLowerCase(argv[2]), "w11_s") == 0) return DownloadAndSet(WIN11_SUNSET, style);
        if (strcmp(ToLowerCase(argv[2]), "w11_v") == 0) return DownloadAndSet(WIN11_VIOLET, style);
    }
    
    cout << "Not valid arguments.";
    cin.get();
    return 0;
}
