#include "WindowIDHandler.h"


bool WindowIDHandler::isSetup = false;
size_t WindowIDHandler::nextID = 1;
HANDLE WindowIDHandler::hMutex = nullptr;
bool WindowIDHandler::teardown() {
  CloseHandle(hMutex);
  return true;
}
bool WindowIDHandler::setup() {
  SECURITY_ATTRIBUTES secAttributes;
  secAttributes.bInheritHandle = 1;
  secAttributes.lpSecurityDescriptor = nullptr;
  secAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);

  hMutex = CreateMutex(&secAttributes, 1, "WindowIDHandler");
  isSetup = true;

  return hMutex != NULL;
}
size_t WindowIDHandler::getNew() {
  if (!isSetup) {
    // Panic
    MessageBox(NULL, TEXT("Window ID Handler was never set up!"), TEXT("ERROR"),
               MB_ICONERROR | MB_OK);
    ExitProcess(1);
  }

  DWORD mutexResult = WaitForSingleObject(hMutex, INFINITE);
  if (mutexResult != WAIT_OBJECT_0) {
    MessageBox(NULL, TEXT("Failed to acquire mutex"), TEXT("ERROR"),
               MB_ICONERROR | MB_OK);
    ExitProcess(1);
  }

  size_t result = nextID++;

  if (!ReleaseMutex(hMutex)) {
    MessageBox(NULL, TEXT("Failed to release mutex"), TEXT("ERROR"),
               MB_ICONERROR | MB_OK);
  }

  return result;
}
