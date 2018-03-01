# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS   = plugin_core \
            plugin_devexplorer \
            plugin_phynetwork \
            plugin_sclmodel \
            plugin_baycopy \
            plugin_devlibrary \
            plugin_baytemplate \
            plugin_ssdautoconn \
            plugin_autoconn

plugin_core.subdir          = core

plugin_devexplorer.subdir   = devexplorer
plugin_devexplorer.depends  += plugin_core

plugin_phynetwork.subdir    = phynetwork
plugin_phynetwork.depends   += plugin_core

plugin_sclmodel.subdir      = sclmodel
plugin_sclmodel.depends     += plugin_core

plugin_baycopy.subdir       = baycopy
plugin_baycopy.depends      += plugin_core

plugin_devlibrary.subdir    = devlibrary
plugin_devlibrary.depends   += plugin_core

plugin_baytemplate.subdir   = baytemplate
plugin_baytemplate.depends  += plugin_core

plugin_ssdautoconn.subdir   = ssdautoconn
plugin_ssdautoconn.depends  += plugin_core

plugin_autoconn.subdir      = autoconn
plugin_autoconn.depends     += plugin_core
