
local current_dir = os.getcwd().."\\"

project "bullet"
    kind "StaticLib"

    defines {
        "B3_USE_CLEW",
    }

    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters

    includedirs {
        current_dir.."src\\",
    }
    