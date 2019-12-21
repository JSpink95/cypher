
local current_dir = ...
local opengl_root = project_root.."Dependencies\\OpenGL\\"

includedirs {
    opengl_root.."Include\\",
    project_external_root.."glfw\\inc\\",
    current_dir.."glsl\\",
}

libdirs {
    opengl_root.."Libs\\",
}

links {
    "glew32",
    "glfw3",
    "opengl32",
    "glfw",
}
