
local current_dir = os.getcwd().."\\"

project "imgui"
    kind "StaticLib"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters
    
    includedirs {
        current_dir.."src\\",
    }
    
    filter "platforms:Dx11"
        dofile(project_scripts_root.."Dx11Includes.lua")    -- setup dx11 includes & libs

    filter "platforms:OpenGL"
        performfile(project_scripts_root.."OpenGLIncludes.lua", current_dir)  -- setup opengl includes & libs

    filter ""
    