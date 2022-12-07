win = import('windows')

# ModShot Windows resources
windows_resources_main = win.compile_resources(
    'resource-main.rc',
    depend_files: files(
        'icon.ico',
        'modshot.manifest',
        'resource.h',
        'resource-main.rc'
    )
)

# Steamshim Windows resources
if steamworks == true
    windows_resources_steamshim = win.compile_resources(
        'resource-steamshim.rc',
        depend_files: files(
            'icon.ico',
            'steamshim.manifest',
            'resource.h',
            'resource-steamshim.rc'
        )
    )
endif
