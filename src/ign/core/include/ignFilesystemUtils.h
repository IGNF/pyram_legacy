#ifndef _IGN_FILESYSTEM_UTILS_H_
#define _IGN_FILESYSTEM_UTILS_H_

#include <ignConfig.h>

#include <set>
#include <vector>
#include <boost/filesystem.hpp>


namespace ign {
namespace filesystem {
    
    extern const std::string SIMPLE_QUOTE_MARK;
    extern const std::string DOUBLE_QUOTE_MARK;
    extern const std::string ENVIRONMENT_MARK;

     ///\brief The following function, given a directory path and a file name,
     ///recursively searches the directory and its sub-directories for the file name
     ///
     /// \param dir_path in this directory
     /// \param keyName search for this name
     /// \param bSubDir look also for subdirectory paths
     /// \param extensions look only for these fil extensions (all if empty vector)
     /// \param placing paths here if found
     IGN_FILESYSTEM_API void FindFilesByKeyName(
											const boost::filesystem::path & dir_path,
											const std::string & keyName,
											bool bSubDir,
											const std::set<std::string> & extensions,
											std::vector<boost::filesystem::path> & filelist
     );
    
    ///\brief normalize given path, removing /./, resolving /../, removing final /, cleaning double /, or \\ etc...
    IGN_FILESYSTEM_API boost::filesystem::path NormalizePath(boost::filesystem::path const& myPath);

    
	///completing boost::filesystem APIs
    IGN_FILESYSTEM_API bool is_writable_file(const boost::filesystem::path& my_path);
	
    IGN_FILESYSTEM_API bool is_readable_file(const boost::filesystem::path& my_path);

	// Return path when appended to a_From will resolve to same as a_To
    IGN_FILESYSTEM_API boost::filesystem::path make_relative( boost::filesystem::path a_From, boost::filesystem::path a_To );
	
	// Wraps boost for linking issues under MacOSX
    IGN_FILESYSTEM_API bool copy_file(const boost::filesystem::path& my_path_in,const boost::filesystem::path&  my_path_out, bool overwrite);

    // Copy of a directory
    IGN_FILESYSTEM_API void copy_dir(boost::filesystem::path const & source, boost::filesystem::path const & destination);

}
}

#endif
