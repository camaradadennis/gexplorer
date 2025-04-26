project('gexplorer', 'cpp',
    version: '0.1',
    default_options: [
        'cpp_std=c++23',
    ]
)

includes = include_directories('include')

gnome = import('gnome')
gtkmm_dep = dependency('gtkmm-4.0')

resources = gnome.compile_resources(
    'resources',
    'gexplorer.gresource.xml',
    source_dir: 'ui',
)

cpp_sources = files(
    'src/graph_drawing_area.cc',
    'src/main.cc',
    'src/main_window.cc',
)

executable(
    meson.project_name(),
    cpp_sources,
    resources,
    include_directories: includes,
    dependencies: gtkmm_dep,
    win_subsystem: 'windows'
)
