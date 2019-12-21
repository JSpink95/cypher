
local dxsdk_root = "" -- os.getenv("DXSDK_DIR")

includedirs {
    dxsdk_root.."Include\\",
}

libdirs {
    dxsdk_root.."Lib\\x86\\",
}

links {
    "d3d11",
    "d3dcompiler",
}
