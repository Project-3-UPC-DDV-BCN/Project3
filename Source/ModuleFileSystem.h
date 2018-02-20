#pragma once
#include "Module.h"
#include <string>
#include <vector>
#include <experimental\filesystem>
namespace fs = std::experimental::filesystem;

class ModuleFileSystem :
	public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleFileSystem();

	bool Create_Directory(std::string directory_path);
	void DeleteDirectory(std::string directory_path);
	bool DirectoryExist(std::string directory_path);
	bool IsDirectory(std::string directory_path);
	bool DirectoryIsEmpty(std::string directory_path);
	bool DirectoryHasSubDirectories(std::string directory_path);
	bool DirectoryHasFiles(std::string directory_path);
	std::string GetDirectoryName(std::string directory_path);
	std::vector<std::string> GetSubDirectories(std::string directory_path);
	std::vector<std::string> GetAllSubDirectoriesRecursively(std::string directory_path);

	bool FileExist(std::string file_path);
	bool Delete_File(std::string file_path);
	void Copy(std::string old_path, std::string new_path);
	std::string GetFileName(std::string file_path);
	std::string GetFileNameWithoutExtension(std::string file_path);
	std::string GetFileExtension(std::string file_path);
	std::string GetFileDirectory(std::string file_path);
	std::string GetFilePathInDirectory(std::string file_name, std::string directory_path, bool search_subdirectories);
	std::vector<std::string> GetFilesInDirectory(std::string directory_path);
	std::vector<std::string> GetAllFilesRecursively(std::string directory_path);
	bool FileExistInDirectory(std::string file_name, std::string directory_path, bool search_subdirectories);
	std::string ChangeFileExtension(std::string file_path, std::string new_extension);
	std::string GetFullPath(std::string file_name);
	//Returns true if file1 time is greater than file2 time
	bool CompareFilesTime(std::string file1_path, std::string file2_path);
	fs::file_time_type GetModifiedTime(std::string file_path);

	std::string StringToPathFormat(std::string path);
};

