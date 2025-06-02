project('gexplorer', 'cpp',
    version: '0.1',
    default_options: [
        'cpp_std=c++23',
    ]
)

includes = include_directories('include')

gnome = import('gnome')

deps = [
    dependency('gtkmm-4.0'),
    dependency('boost'),
]

resources = gnome.compile_resources(
    'resources',
    'gexplorer.gresource.xml',
    source_dir: 'ui',
)

cpp_sources = files(
    'src/graph.cc',
    'src/graph_drawing_area.cc',
    'src/infofield.cc',
    'src/main.cc',
    'src/main_window.cc',
    'src/osm_parser.cc',
    'src/searchfield.cc',
)

executable(
    meson.project_name(),
    cpp_sources,
    resources,
    include_directories: includes,
    dependencies: deps,
    win_subsystem: 'windows'
)
