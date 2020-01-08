
workspace_name = "Engine_".._ACTION
project_root = os.getcwd().."\\"
project_scripts_root = project_root.."Scripts\\"
project_files_root = project_root.."ProjectFiles\\".._ACTION.."\\"
project_external_root = project_root.."External\\"
project_engine_root = project_root.."Engine\\"

function performfile(filename, ...)
    assert(loadfile(filename))(...)
end

workspace(workspace_name)
    language "C++"
    cppdialect "C++17"
    targetdir("ProjectFiles\\".._ACTION)

    configurations {
        "Debug", "Release"
    }

    platforms {
        "OpenGL", "Dx11"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }

    filter "configurations:Release"
        defines { "RELEASE" }

    filter ""

    defines {
        "NOMINMAX",
        "RESOURCES_ROOT="..project_root.."Resources"
    }

    filter "platforms:Dx11"
        defines {
            "_DX11",
        }

    filter "platforms:OpenGL"
        defines {
            "_OPENGL",
        }

    filter ""

    group "External"
        include("External\\bullet")
        include("External\\glfw")
        include("External\\glm")
        include("External\\stb")
        include("External\\spdlog")
        include("External\\json")
        include("External\\pugixml")

    group "Engine"
        include("Engine\\Engine")

    group "Apps"
        include("Apps\\Sandbox")
        include("Apps\\ParticleEditor")

    group ""
