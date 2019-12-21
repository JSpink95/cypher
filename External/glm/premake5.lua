
local current_dir = os.getcwd().."\\"

project "glm"
    kind "StaticLib"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters
    
    includedirs {
        current_dir.."inc\\",
    }
    