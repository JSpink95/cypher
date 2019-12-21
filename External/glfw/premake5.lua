
local current_dir = os.getcwd().."\\"
local opengl_root = project_root.."Dependencies\\OpenGL\\"

project "glfw"
    kind "StaticLib"

    performfile(project_scripts_root.."FileSetup.lua", current_dir)       -- setup file filters

    includedirs {
        opengl_root.."Include\\",
        current_dir.."inc\\",
        current_dir.."src\\",
    }
    
    libdirs {
        opengl_root.."Libs\\",
    }

    links {
        "glew32",
        "glfw3",
        "opengl32",
    }