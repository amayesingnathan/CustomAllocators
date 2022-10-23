project "CustomAllocators"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

    files 
    { 
        "Allocators/src/**.h", 
        "Allocators/src/**.cpp",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "Allocators/src",
    }

	links
	{
    }