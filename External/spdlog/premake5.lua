
local current_dir = os.getcwd().."\\"

project "spdlog"
    kind "StaticLib"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)
    
    includedirs {
        current_dir.."inc\\",
        current_dir.."src\\",
    }
    
    libdirs {
        project_files_root,
    }
