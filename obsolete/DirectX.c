
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <windows.h>
#include <ddraw.h>
/* stdio for debug output */
#include <stdio.h>
#include "resource.h"

#include "compat.h"
#include "thrust.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "options.h"

#define PSTARTX ((WIDTH-PUSEX)>>1)
#define PSTARTY ((HEIGHT-PUSEY-24)>>1)

HINSTANCE hInstance;
ATOM atomWndClass = 0;
HANDLE hWnd = 0;
LPDIRECTDRAW lpDD;
LPDIRECTDRAWPALETTE lpDDP;
LPDIRECTDRAWSURFACE lpDDS;
word WIDTH;
word HEIGHT;

extern LRESULT __stdcall WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern void keypump(void);
extern int in_focus;
void storescr(void);
void paintscr(void);

static HRESULT lockscr(DDSURFACEDESC *ddsd);
static void hline(int x1, int y, int x2, ui8 color);
static void vline(int x, int y1, int y2, ui8 color);
static void macrosleep(unsigned long us);

static ui8 scr[320*200];
static ui8 scrstate=0;

void
storescr(void)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  word y;
  ui8 *tmp;

  in_focus = 0;

  if(!lpDDS)
    return;
  if(scrstate)
    return;

  hr = IDirectDrawSurface_IsLost(lpDDS);
  if(hr == DDERR_SURFACELOST)
    hr = IDirectDrawSurface_Restore(lpDDS);
  if(hr != DD_OK)
    return;

  ddsd.dwSize = sizeof(ddsd);
  hr = IDirectDrawSurface_Lock(lpDDS, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_READONLY, NULL);
  if(hr != DD_OK)
    return;

  tmp=(ui8 *)ddsd.lpSurface + PSTARTX + ddsd.lPitch*PSTARTY;
  for(y=0; y<HEIGHT; y++)
    memcpy(scr + y*320, tmp + y*ddsd.lPitch, 320);

  IDirectDrawSurface_Unlock(lpDDS, NULL);

  scrstate = 1;
}

void
paintscr(void)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  word y;
  ui8 *tmp;

  in_focus = 1;

  if(!lpDDS)
    return;
  if(!scrstate)
    return;

  hr = IDirectDrawSurface_IsLost(lpDDS);
  if(hr == DDERR_SURFACELOST)
    hr = IDirectDrawSurface_Restore(lpDDS);
  if(hr != DD_OK)
    return;

  ddsd.dwSize = sizeof(ddsd);
  hr = IDirectDrawSurface_Lock(lpDDS, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
  if(hr != DD_OK)
    return;

  tmp=(ui8 *)ddsd.lpSurface + PSTARTX + ddsd.lPitch*PSTARTY;
  for(y=0; y<HEIGHT; y++)
    memcpy(tmp + y*ddsd.lPitch, scr + y*320, 320);

  IDirectDrawSurface_Unlock(lpDDS, NULL);

  scrstate=0;
}

static HRESULT
lockscr(DDSURFACEDESC *ddsd)
{
  HRESULT hr;

  if(!lpDDS) {
    in_focus = 0;
    return S_FALSE;
  }

  if(scrstate) {
    in_focus = 0;
    ddsd->lpSurface = scr;
    ddsd->lPitch = 320;
    return S_OK;
  }
  
  hr = IDirectDrawSurface_IsLost(lpDDS);
  if(hr == DDERR_SURFACELOST)
    hr = IDirectDrawSurface_Restore(lpDDS);
  if(hr == DD_OK) {
    ddsd->dwSize = sizeof(*ddsd);
    hr = IDirectDrawSurface_Lock(lpDDS, NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
  }

  return DD_OK;
}

void
clearscr(void)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  word y;

  hr = lockscr(&ddsd);
  if(hr != DD_OK)
    return;

  for(y=0; y<HEIGHT; y++)
    memset((ui8 *)ddsd.lpSurface + y*ddsd.lPitch, 0, WIDTH);

  IDirectDrawSurface_Unlock(lpDDS, NULL);
}

void
putarea(ui8 *source,
	int x, int y, int width, int height, int bytesperline,
	int destx, int desty)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  ui8 *tmp;
  int res;
  int dy;

  hr = lockscr(&ddsd);
  if(hr != DD_OK)
    return;

  res=PSTARTX+destx+ddsd.lPitch*(PSTARTY+desty);
  if(bytesperline==320 && width==320 && WIDTH==320 && ddsd.lPitch==320 && x==0 && destx==0) {
    memcpy((ui8 *)ddsd.lpSurface + desty*WIDTH, source + y*width, height*width);
  }
  else {
    tmp=source+y*bytesperline+x;

    for(dy=y; dy<y+height; res+=ddsd.lPitch, dy++, tmp+=bytesperline) {
      memcpy((ui8 *)ddsd.lpSurface + res, tmp, width);
    }
  }

  IDirectDrawSurface_Unlock(lpDDS, NULL);
}

static void
hline(int x1, int y, int x2, ui8 color)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  int x;

  hr = lockscr(&ddsd);
  if(hr != DD_OK)
    return;

  for(x=min(x1, x2); x<=max(x1, x2); x++)
    *((ui8 *)ddsd.lpSurface + (PSTARTY+y)*ddsd.lPitch + PSTARTX+x) = color;

  IDirectDrawSurface_Unlock(lpDDS, NULL);
}

static void
vline(int x, int y1, int y2, ui8 color)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  int y;

  hr = lockscr(&ddsd);
  if(hr != DD_OK)
    return;

  for(y=min(y1, y2); y<=max(y1, y2); y++)
    *((ui8 *)ddsd.lpSurface + (PSTARTY+y)*ddsd.lPitch + PSTARTX+x) = color;

  IDirectDrawSurface_Unlock(lpDDS, NULL);
}

static void
systime(LONGLONG *now)
{
  FILETIME ft;
  LARGE_INTEGER li;
  GetSystemTimeAsFileTime(&ft);
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  *now = li.QuadPart;
}

static void
macrosleep(unsigned long us)
{
  LONGLONG start, last, now;
  LONGLONG wait;

  if(!us)
    return;

  wait = (LONGLONG)us * 10;
  systime(&start);
  last = start;
  while(last - start + 500000 <= wait) {
    usleep(50000UL);
    keypump();
    systime(&now);
    if(now < last)
      return;
    last = now;
  }

  if(last - start < wait - 10)
    usleep((wait - (last - start)) / 10);
}

void
putpixel(int x, int y, ui8 color)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;

  hr = lockscr(&ddsd);

  *((ui8 *)ddsd.lpSurface + (PSTARTY+y)*ddsd.lPitch + PSTARTX+x) = color;

  IDirectDrawSurface_Unlock(lpDDS, NULL);
}

void
syncscreen(unsigned long us)
{
  HRESULT hr;

  macrosleep(us);

  if(!lpDD)
    return;

  hr = IDirectDraw_WaitForVerticalBlank(lpDD, DDWAITVB_BLOCKBEGIN, NULL);
  if(hr == DDERR_UNSUPPORTED) {
    BOOL bIsInVB;
    do {
      hr = IDirectDraw_GetVerticalBlankStatus(lpDD, &bIsInVB);
      if(bIsInVB)
	continue;
    } while(hr == DD_OK);
  }
}

void
displayscreen(unsigned long us)
{
  macrosleep(us);
}

void
fadepalette(int first, int last, ui8 *RGBtable, int fade, int flag)
{
  static PALETTEENTRY tmpPal[256];
  int entries,i;
  PALETTEENTRY *c;
  ui8 *d;
  int used_first;
  int used_entries=0;

  entries=last-first+1;

  used_first = color_lookup[first];
  if(used_first == 0xff)
    printf("Aiee, bad fadepalette call.\n");
  c=tmpPal;
  d=RGBtable;
  i=0;

  while(i<entries) {
    if(color_lookup[first+i] != 0xff) {
      c->peRed   = (*d++ * fade) >> 6;
      c->peGreen = (*d++ * fade) >> 6;
      c->peBlue  = (*d * fade) >> 6;
      used_entries++;
    }
    else
      d += 2;
    if(++i<entries) {
      if(color_lookup[first+i-1] != 0xff)
        c++;
      d++;
    }
  }

  if(flag)
    syncscreen(0UL);

  IDirectDrawPalette_SetEntries(lpDDP, 0, used_first, used_entries, tmpPal);
}

void
fade_in(unsigned long us)
{
  int i;

  if(PSTARTY>0 && PSTARTX>0) {
    hline(-3, -3, PUSEX+2, 1);
    hline(-4, -4, PUSEX+3, 2);
    hline(-5, -5, PUSEX+4, 3);
    hline(-3, PUSEY+26, PUSEX+2, 3);
    hline(-4, PUSEY+27, PUSEX+3, 2);
    hline(-5, PUSEY+28, PUSEX+4, 1);
    vline(-3, -3, PUSEY+26, 1);
    vline(-4, -4, PUSEY+27, 2);
    vline(-5, -5, PUSEY+28, 3);
    vline(PUSEX+2, -3, PUSEY+26, 3);
    vline(PUSEX+3, -4, PUSEY+27, 2);
    vline(PUSEX+4, -5, PUSEY+28, 1);
  }
  else if(PSTARTY>0) {
    hline(0, -3, PUSEX-1, 1);
    hline(0, -4, PUSEX-1, 2);
    hline(0, -5, PUSEX-1, 3);
    hline(0, PUSEY+26, PUSEX-1, 3);
    hline(0, PUSEY+27, PUSEX-1, 2);
    hline(0, PUSEY+28, PUSEX-1, 1);
  }

  for(i=1; i<=64; i++)
    fadepalette(0, 255, bin_colors, i, 1);

  macrosleep(us);
}

void
fade_out(unsigned long us)
{
  int i;

  macrosleep(us);

  for(i=64; i; i--)
    fadepalette(0, 255, bin_colors, i, 1);
  clearscr();
  macrosleep(500000UL);
}



void
graphics_preinit(void)
{
}

int
graphicsinit(int argc, char **argv)
{
  HRESULT hr;
  DDSURFACEDESC ddsd;
  PALETTEENTRY ape[256];
  int i;
  MSG msg;

  hInstance = GetModuleHandle(NULL);
  
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THRUST));
  wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_NOCURSOR));
  wc.hbrBackground = GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ThrustMenu";
  wc.lpszClassName = "ThrustClass";
  wc.hIconSm = wc.hIcon;

  atomWndClass = RegisterClassEx(&wc);
  if(!atomWndClass)
    return 0;

  hWnd = CreateWindow("ThrustClass", "Thrust " PACKAGE_VERSION " by Peter Ekberg",
    WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME),
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, hInstance, NULL);
  if(!hWnd)
    return 0;

  ShowWindow(hWnd, SW_SHOWNORMAL);
  UpdateWindow(hWnd);

  hr = DirectDrawCreate(NULL, &lpDD, NULL);
  if(hr != DD_OK)
    return 0;
  /*
    LPDIRECTDRAW2 lpDD;
    hr = lpDD1->QueryInterface(IID_IDirectDraw2, (void **)&lpDD);
    if(hr != DD_OK)
    return 0;
    lpDD1->Release();
  */
  hr = IDirectDraw_SetCooperativeLevel(lpDD, hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
  if(hr != DD_OK)
    return 0;
  hr = IDirectDraw_SetDisplayMode(lpDD, 320, 200, 8);//, 0, 0);//DDSDM_STANDARDVGAMODE);
  if(hr != DD_OK) {
    hr = IDirectDraw_SetDisplayMode(lpDD, 640, 480, 8);//, 0 ,0);
    if(hr != DD_OK)
      return 0;
    WIDTH = 640;
    HEIGHT = 480;
  }
  else {
    WIDTH = 320;
    HEIGHT = 200;
  }

  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;// | DDSCAPS_LOCALVIDMEM;

  hr = IDirectDraw_CreateSurface(lpDD, &ddsd, &lpDDS, NULL);
  if(hr != DD_OK)
    return 0;

  for(i = 0; i<256; i++) {
    ape[i].peRed = 0;
    ape[i].peGreen = 0;
    ape[i].peBlue = 0;
  }
  hr = IDirectDraw_CreatePalette(lpDD, DDPCAPS_8BIT | DDPCAPS_ALLOW256, ape, &lpDDP, NULL);
  if(hr != DD_OK)
    return 0;

  hr = IDirectDrawSurface_SetPalette(lpDDS, lpDDP);
  if(hr != DD_OK)
    return 0;

  while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
    if(!GetMessage(&msg, NULL, 0, 0))
      break;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    continue;
  }

  fadepalette(0, 255, bin_colors, 1, 0);

  return 0;
}

int
graphicsclose(void)
{
  IDirectDrawPalette_Release(lpDDP);
  lpDDP = NULL;
  IDirectDrawSurface_Release(lpDDS);
  lpDDS = NULL;
  IDirectDraw_RestoreDisplayMode(lpDD);
  IDirectDraw_Release(lpDD);
  lpDD = NULL;

  DestroyWindow(hWnd);
  if(atomWndClass)
    UnregisterClass(MAKEINTATOM(atomWndClass), NULL);

  return 0;
}

char *
graphicsname(void)
{
  static char name[] = "DirectX";

  return name;
}
