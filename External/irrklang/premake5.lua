
local current_dir = os.getcwd().."\\"
local irrklang_root = project_root.."Dependencies\\Irrklang\\"

project "irrklanglib"
    kind "StaticLib"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters

    includedirs {
        irrklang_root.."Include\\",
        current_dir.."inc\\",
    }
    
    libdirs {
        irrklang_root.."Libs\\",
    }

    links {
        "irrKlang",
    }