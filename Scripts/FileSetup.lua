
local current_dir = ...
local current_dir_inc = current_dir.."inc\\"
local current_dir_src = current_dir.."src\\"

-- c++ files
files {
    current_dir.."**.h", current_dir.."**.hpp", current_dir.."**.cpp", current_dir.."**.inl",
}

-- dx11 files
files {
    current_dir.."**.hlsl",
}

-- opengl files
files {
    current_dir.."**.shader", current_dir.."**.vs", current_dir.."**.gs", current_dir.."**.ps", current_dir.."**.compute",
}

-- json files
files {
    current_dir.."**.json",
}

-- xml files
files {
    current_dir.."**.xml"
}
