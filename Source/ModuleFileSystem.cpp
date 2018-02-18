#include "ModuleFileSystem.h"

#include "Resource.h"

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleFileSystem::~ModuleFileSystem()
{
}

bool ModuleFileSystem::Create_Directory(std::string directory_path)
{
	return fs::create_directory(directory_path);
}

void ModuleFileSystem::DeleteDirectory(std::string directory_path)
{
	fs::remove_all(directory_path);
}

bool ModuleFileSystem::DirectoryExist(std::string directory_path)
{
	return fs::exists(directory_path);
}

bool ModuleFileSystem::IsDirectory(std::string directory_path)
{
	return fs::is_directory(directory_path);
}

bool ModuleFileSystem::DirectoryIsEmpty(std::string directory_path)
{
	return fs::is_empty(directory_path);
}

bool ModuleFileSystem::DirectoryHasSubDirectories(std::string directory_path)
{
	for (auto & p : fs::directory_iterator(directory_path))
	{
		if (fs::is_directory(p))
		{
			return true;
		}
	}
	return false;
}

bool ModuleFileSystem::DirectoryHasFiles(std::string directory_path)
{
	for (auto & p : fs::directory_iterator(directory_path))
	{
		if (!fs::is_directory(p))
		{
			return true;
		}
	}
	return false;
}

std::string ModuleFileSystem::GetDirectoryName(std::string directory_path)
{
	fs::path path(directory_path);
	if (fs::is_directory(path))
	{
		return path.filename().string();
	}
	return "";
}

std::vector<std::string> ModuleFileSystem::GetSubDirectories(std::string directory_path)
{
	std::vector<std::string> directories;
	for (auto & p : fs::directory_iterator(directory_path))
	{
		if (fs::is_directory(p))
		{
			directories.push_back(p.path().string());
		}
	}
	return directories;
}

std::vector<std::string> ModuleFileSystem::GetAllSubDirectoriesRecursively(std::string directory_path)
{
	std::vector<std::string> directories;
	for (auto & p : fs::recursive_directory_iterator(directory_path))
	{
		if (fs::is_directory(p))
		{
			directories.push_back(p.path().string());
		}
	}
	return directories;
}

bool ModuleFileSystem::FileExist(std::string file_path)
{
	return fs::exists(file_path);
}

bool ModuleFileSystem::Delete_File(std::string file_path)
{
	return fs::remove(file_path);
}

void ModuleFileSystem::Copy(std::string old_path, std::string new_path)
{
	fs::copy(old_path, new_path, fs::copy_options::update_existing);
}

std::string ModuleFileSystem::GetFileName(std::string file_path)
{
	fs::path path(file_path);
	return path.filename().string();
}

std::string ModuleFileSystem::GetFileNameWithoutExtension(std::string file_path)
{
	fs::path path(file_path);
	return path.stem().string();
}

std::string ModuleFileSystem::GetFileExtension(std::string file_path)
{
	fs::path path(file_path);
	return path.extension().string();
}

std::string ModuleFileSystem::GetFileDirectory(std::string file_path)
{
	fs::path path(file_path);
	return path.parent_path().string();
}

std::string ModuleFileSystem::GetFilePathInDirectory(std::string file_name, std::string directory_path, bool search_subdirectories)
{
	if (search_subdirectories)
	{
		for (auto & p : fs::recursive_directory_iterator(directory_path))
		{
			if (!fs::is_directory(p))
			{
				if (p.path().filename().string() == file_name) return p.path().string();
			}
		}
	}
	else
	{
		for (auto & p : fs::directory_iterator(directory_path))
		{
			if (!fs::is_directory(p))
			{
				if (p.path().filename().string() == file_name) return p.path().string();
			}
		}
	}
	return "";
}

std::vector<std::string> ModuleFileSystem::GetFilesInDirectory(std::string directory_path)
{
	std::vector<std::string> files;
	for (auto & p : fs::directory_iterator(directory_path))
	{
		if (!fs::is_directory(p))
		{
			files.push_back(p.path().string());
		}
	}
	return files;
}

std::vector<std::string> ModuleFileSystem::GetAllFilesRecursively(std::string directory_path)
{
	std::vector<std::string> files;
	for (auto & p : fs::recursive_directory_iterator(directory_path))
	{
		if (!fs::is_directory(p))
		{
			files.push_back(p.path().string());
		}
	}
	return files;
}

bool ModuleFileSystem::FileExistInDirectory(std::string file_name, std::string directory_path, bool search_subdirectories)
{
	if (search_subdirectories)
	{
		for (auto & p : fs::recursive_directory_iterator(directory_path))
		{
			if (!fs::is_directory(p))
			{
				if (p.path().filename().string() == file_name) return true;
			}
		}
	}
	else
	{
		for (auto & p : fs::directory_iterator(directory_path))
		{
			if (!fs::is_directory(p))
			{
				if (p.path().filename().string() == file_name) return true;
			}
		}
	}
	
	return false;
}

std::string ModuleFileSystem::ChangeFileExtension(std::string file_path, std::string new_extension)
{
	fs::path file(file_path);
	if(file.has_extension()) file.replace_extension(new_extension);
	else file += "." + new_extension;
	return file.string();
}

std::string ModuleFileSystem::GetFullPath(std::string file_name)
{
	fs::path path(file_name);
	fs::path full_path = fs::system_complete(path);
	return full_path.string();
}

bool ModuleFileSystem::CompareFilesTime(std::string file1_path, std::string file2_path)
{
	if (GetModifiedTime(file1_path) > GetModifiedTime(file2_path)) return true;
	return false;
}

fs::file_time_type ModuleFileSystem::GetModifiedTime(std::string file_path)
{
	fs::file_time_type time = fs::last_write_time(file_path);
	return time;
}

std::string ModuleFileSystem::StringToPathFormat(std::string path)
{
	fs::path new_path(path);
	return new_path.string();
}
