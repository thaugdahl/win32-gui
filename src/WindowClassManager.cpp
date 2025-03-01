#include "WindowClassManager.h"

std::vector<WNDCLASS> WindowClassManager::classes{};
std::map<std::string, std::string> WindowClassManager::classNames{};
std::map<std::string, ATOM> WindowClassManager::classAtoms{};
