#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#define CONSOLE_LOG(format, ...) log(__FILE__, __func__,__LINE__, false, false, false, format, __VA_ARGS__);
#define CONSOLE_WARNING(format, ...) log(__FILE__, __func__,__LINE__, true, false, false, format, __VA_ARGS__);
#define CONSOLE_ERROR(format, ...) log(__FILE__, __func__, __LINE__, false, true, false, format, __VA_ARGS__);
#define CONSOLE_DEBUG(format, ...) log(__FILE__, __func__, __LINE__, false, false, true, format, __VA_ARGS__);

void log(const char file[], const char function[], int line, bool is_warning, bool is_error, bool is_debug, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846f
#define TWO_PI 6.28318530717958647692f
#define HALF_PI 1.57079632679489661923f
#define QUARTER_PI 0.78539816339744830961f
#define INV_PI 0.31830988618379067154f
#define INV_TWO_PI 0.15915494309189533576f


typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned int UID;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Align 16, use if you have math elemtns in your class like float4x4 or AABB
#define ALIGN_CLASS_TO_16 \
	void* operator new(size_t i) { return _aligned_malloc(i,16); }\
    void operator delete(void* p) { _aligned_free(p); }

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define DEFAULT_SCENE_TITLE "The Creator 3D - Untitled Scene"
#define SCENE_TITLE_PREFIX "The Creator 3D - "

#define PROJECT_FOLDER "./Project/"

#define ASSETS_FOLDER "./Project/Assets/"
#define ASSETS_FOLDER_PATH "./Project/Assets"
#define ASSETS_TEXTURES_FOLDER "./Project/Assets/Textures/"
#define ASSETS_TEXTURES_FOLDER_PATH "./Project/Assets/Textures"
#define ASSETS_PREFABS_FOLDER "./Project/Assets/Prefabs/"
#define ASSETS_PREFABS_FOLDER_PATH "./Project/Assets/Prefabs"
#define ASSETS_FBX_FOLDER "./Project/Assets/FBX/"
#define ASSETS_FBX_FOLDER_PATH "./Project/Assets/FBX"
#define ASSETS_SHADERS_FOLDER "./Project/Assets/Shaders/"
#define ASSETS_SHADERS_FOLDER_PATH "./Project/Assets/Shaders"

#define LIBRARY_FOLDER "./Project/Library/"
#define LIBRARY_FOLDER_PATH "./Project/Library"
#define LIBRARY_TEXTURES_FOLDER "./Project/Library/Textures/"
#define LIBRARY_TEXTURES_FOLDER_PATH "./Project/Library/Textures"
#define LIBRARY_MESHES_FOLDER "./Project/Library/Meshes/"
#define LIBRARY_MESHES_FOLDER_PATH "./Project/Library/Meshes"
#define LIBRARY_PREFABS_FOLDER "./Project/Library/Prefabs/"
#define LIBRARY_PREFABS_FOLDER_PATH "./Project/Library/Prefabs"
#define LIBRARY_MATERIALS_FOLDER "./Project/Library/Materials/"
#define LIBRARY_MATERIALS_FOLDER_PATH "./Project/Library/Materials"
#define LIBRARY_SCRIPTS_FOLDER "./Project/Library/Scripts/"
#define LIBRARY_SCRIPTS_FOLDER_PATH "./Project/Library/Scripts"
#define LIBRARY_SHADERS_FOLDER "./Project/Library/Shaders/"
#define LIBRARY_SHADERS_FOLDER_PATH "./Project/Library/Shaders"
#define LIBRARY_SKELETONS_FOLDER "./Project/Library/Skeletons/"
#define LIBRARY_SKELETONS_FOLDER_PATH "./Project/Library/Skeletons"

#define EDITOR_CONFIG_FILE "./Editor_Settings/Editor_Config.json"
#define EDITOR_LAYOUT_FILE "./Editor_Settings/Editor_Layout"
#define EDITOR_STYLE_FILE "./Editor_Settings/Editor_Style.json"
#define EDITOR_IMAGES_FOLDER "./Editor_Settings/Images/"
#define EDITOR_FONTS_FOLDER "./Editor_Settings/Fonts/"
#define EDITOR_SKYBOX_FOLDER "./Editor_Settings/Images/Sky_Box/"

#define LOGS_FOLDER "./Logs/"
#define LOGS_FOLDER_PATH "./Logs"
#define TMP_FOLDER "./Tmp/"
#define TMP_FOLDER_PATH "./Tmp"

#define REFERENCE_ASSEMBLIES_FOLDER "./Editor_Settings/Reference_Assemblies/"
#define CS_TEMPLATE_FILE_PATH "./Editor_Settings/Scripts_Templates/C#Script.txt"
#define LUA_TEMPLATE_FILE_PATH "./Editor_Settings/Scripts_Templates/LuaScript.txt"
#define VERTEX_SHADER_TEMPLATE_FILE_PATH "./Editor_Settings/Shaders_Templates/Vertex_Shader.txt"
#define FRAGMENT_SHADER_TEMPLATE_FILE_PATH "./Editor_Settings/Shaders_Templates/Fragment_Shader.txt"
#define SHADER_DEFAULT_FOLDER "./Editor_Settings/Default_Shaders/"
#define SHADER_DEFAULT_FOLDER_PATH "./Editor_Settings/Default_Shaders"
#define MATERIAL_DEFAULT_FOLDER "./Editor_Settings/Default_Materials/"
#define MATERIAL_DEFAULT_FOLDER_PATH "./Editor_Settings/Default_Materials"
