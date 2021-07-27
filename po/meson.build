i18n = import('i18n')

i18n.gettext(meson.project_name(),
    args: ['--c++',
           '--directory=' + meson.source_root(),
           '--escape',
           '--from-code=utf-8',
           '--indent',
           '--no-wrap',
           '--package-version=' + meson.project_version(),
    ],
)
