#ifndef CORECONSTANTS_H
#define CORECONSTANTS_H

#include <QtGlobal>

namespace Core {
namespace Constants {

#define VERSION_MAJOR       2
#define VERSION_MINOR       1
#define VERSION_REVISION    1

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define APP_VERSION STRINGIFY(VERSION_MAJOR) \
                    "." STRINGIFY(VERSION_MINOR) \
                    "." STRINGIFY(VERSION_REVISION)

const char * const g_szVersion          = APP_VERSION;

#undef APP_VERSION
#undef STRINGIFY
#undef STRINGIFY_INTERNAL

// General
const char * const g_szGeneral_AppTitle                 = "Smart Designer";

// Context unique ID
const int          g_iUID_Context_Global                = 0;
const char * const g_szUID_Context_Global               = "Core.Context.Global";

// Menu IDs
const char * const g_szUID_Menu                         = "Core.Menu";
const char * const g_szUID_ContextMenu                  = "Core.ContextMenu";
const char * const g_szUID_ViewInModesMenu              = "Core.ViewInModesMenu";

// Menu group IDs
const char * const g_szUID_MenuGroup_Project            = "Core.MenuGroup.Project";
const char * const g_szUID_MenuGroup_Tools              = "Core.MenuGroup.Tools";
const char * const g_szUID_MenuGroup_Options            = "Core.MenuGroup.Options";
const char * const g_szUID_MenuGroup_Exit               = "Core.MenuGroup.Exit";

// Context Menu group IDs
const char * const g_szUID_ContextMenuGroup_Project     = "Core.MenuGroup.Project";
const char * const g_szUID_ContextMenuGroup_DevObject   = "Core.MenuGroup.DevObject";
const char * const g_szUID_ContextMenuGroup_View        = "Core.MenuGroup.View";
const char * const g_szUID_ContextMenuGroup_Export      = "Core.MenuGroup.Export";
const char * const g_szUID_ContextMenuGroup_TreeView    = "Core.MenuGroup.TreeView";

// Default group IDs
const char * const g_szUID_GroupDefaultOne              = "Core.Group.Default.One";
const char * const g_szUID_GroupDefaultTwo              = "Core.Group.Default.Two";
const char * const g_szUID_GroupDefaultThree            = "Core.Group.Default.Three";

} // namespace Constants
} // namespace Core

#endif // CORECONSTANTS_H
