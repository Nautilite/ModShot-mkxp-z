win = import('windows')

res = files(
    'icon.ico',
    'modshot.manifest',
    'resource.h',
    'resource.rc'
)

windows_resources = win.compile_resources(
    'resource.rc',
    depend_files: res
)
