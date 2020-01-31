
local current_dir = ...
local irrklang_root = project_root.."Dependencies\\Irrklang\\"

includedirs {
    irrklang_root.."Include\\",
    project_external_root.."irrklang\\inc\\",
}

libdirs {
    irrklang_root.."Libs\\",
}

links {
    "irrKlang",
    "irrklanglib"
}
