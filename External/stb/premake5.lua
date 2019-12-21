
local current_dir = os.getcwd().."\\"

project "stb"
    kind "StaticLib"
    
    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters

    includedirs {
        current_dir.."inc\\",
        current_dir.."src\\",
    }
