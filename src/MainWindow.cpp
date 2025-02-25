#include "MainWindow.h"
#include "Button.h"
#include "WindowIDHandler.h"


static void* b1ID=NULL;
static void* b2ID=NULL;
static void* e1ID=NULL;

static HWND hButton;
static HWND hButton2;
// static HWND hEdit1;










// Window procedure
LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            {


                break;
            }

        case WM_COMMAND:
            {

                // If the GWLP_USERDATA pointer is set
                // then we have attached a WindowInterface instance to it

                // If the WM_COMMAND message source was a *control*, then wParam is split in two parts (Low and high words).
                // HI = Control-defined notification code. -- For instance BN_CLICKED
                // LO = The control's identifier
                // The lParam is the handle to the control window.
                if ( HIWORD(wParam) == BN_CLICKED ) {
                    int buttonID = LOWORD(wParam);
                    HWND hButton = (HWND)lParam;

                    // Button *btn = (Button *) GetWindowLongPtr(hButton, GWLP_USERDATA);

                    SendMessage(hButton, uMsg, wParam, lParam);
                }


                break;

            }
        case WM_KEYDOWN:
            // VK_ --> Virtual Keys
            // Mostly includes physical keyboard keys but also virtual ones like LMB, RMB, MMB
            if (wParam == VK_RETURN ) {
                MessageBox(hwnd, "Pressed ENTER", "Notification", MB_OK);
            }

            break;
        case WM_QUIT:
            MessageBox(hwnd, "Quitting now", "Notification", MB_OK);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
