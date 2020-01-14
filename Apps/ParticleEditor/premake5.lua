
local current_dir = os.getcwd().."\\"
local opengl_root = project_root.."Dependencies\\OpenGL\\"

project "ParticleEditor"
    kind "ConsoleApp"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)     -- setup file filters
    
    libdirs {
        project_files_root,
    }

    includedirs {
        project_external_root.."glm\\inc\\",
        project_external_root.."stb\\inc\\",
        project_external_root.."spdlog\\inc\\",
        project_external_root.."json\\inc\\",
        project_external_root.."pugixml\\inc\\",
        project_external_root.."bullet\\src\\",
        project_external_root.."imgui\\src\\",
        
        project_engine_root.."Engine\\inc\\",

        current_dir.."inc\\",
        current_dir.."src\\",
    }
    
    links {
        "glm", "stb", "spdlog", "Engine", "pugixml", "bullet", "imgui"
    }

    filter "platforms:Dx11"
        dofile(project_scripts_root.."Dx11Includes.lua")    -- setup dx11 includes & libs

    filter "platforms:OpenGL"
        performfile(project_scripts_root.."OpenGLIncludes.lua", current_dir)

    filter ""