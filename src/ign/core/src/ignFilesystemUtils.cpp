#include <ignFilesystemUtils.h>
#include <ignFilesystemSysInfo.h>

#include <ignCommon.h>
#include <ignStringTools.h>

#include <fstream>

const std::string ign::filesystem::SIMPLE_QUOTE_MARK="%*%";
const std::string ign::filesystem::DOUBLE_QUOTE_MARK="%**%";
const std::string ign::filesystem::ENVIRONMENT_MARK="%%";

namespace boost
{
	namespace filesystem
	{
		//specialization locale du append pour make_relative
		template < >
		path& path::append<  path::iterator >( path::iterator begin, path::iterator end/*, const codecvt_type& cvt*/)
		{ 
			for( ; begin != end ; ++begin )	*this /= *begin;
			return *this;
		}
	}
}

namespace ign
{
	namespace filesystem
	{
		
		///
		///
		///
		void FindFilesByKeyName(const boost::filesystem::path & dir_path,			// in this directory,
								const std::string & keyName,						// search for this name,
								bool bSubDir,										//look also for subdirectory paths
								const std::set<std::string> & extensions,
								std::vector<boost::filesystem::path> & filelist)	// placing paths here if found
		{
			IGN_ASSERT (boost::filesystem::exists( dir_path ) );
			
			boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
			for ( boost::filesystem::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
			{
				if ( boost::filesystem::is_directory(itr->status()) )
				{
					ign::filesystem::FindFilesByKeyName( itr->path(), keyName, bSubDir, extensions, filelist);
				}
				else
				{
					std::string fileName = itr->path().filename().generic_string();
					//on regarde si la chaine keyName se trouve dans fileName
					if (ign::tools::StringManip::FindSubString(fileName, keyName))
					{
						if (!extensions.empty())//check extensions...
						{
							if (extensions.find(itr->path().extension().generic_string())!= extensions.end())
							{
								filelist.push_back(itr->path());
							}
						}
						else
						{
							filelist.push_back(itr->path());
						}
					}
				}
			}
		}
		
		///
		///
		///
		bool is_writable_file(const boost::filesystem::path& my_path)
		{
			bool verbose  = 0;
			//implementation simpliste: on verifie que le path existe et est un fichier regulier. 
			if (boost::filesystem::exists(my_path))
			{
				if (verbose) std::cout<<"[ign::filesystem::writable]: "<<my_path.generic_string()<<" exists"<<std::endl;
				if (boost::filesystem::is_regular_file(my_path))
				{
					//on essaie de l'ouvrir en lecture / ecriture
					if (verbose) std::cout<<"[ign::filesystem::writable]: "<<my_path.generic_string()<<" is regular file"<<std::endl;
					
					std::ofstream f( my_path.generic_string().c_str(), std::ios::app );
					return (f.good());
				}
			}
			else 
			{
				FILE* fic = fopen(my_path.generic_string().c_str(), "w");
				if (fic != 0)
				{
					if (verbose) std::cout<<"[ign::filesystem::writable]: "<<my_path.generic_string()<<" is writable in mod w"<<std::endl;
					fclose(fic);
					boost::filesystem::remove(my_path);
					return true;
				}
			}
			return false;
		}
		
		///
		///
		///
		bool is_readable_file(const boost::filesystem::path& my_path)
		{
			//implementation simpliste: on verifie que le path existe et est un fichier regulier. 
			if (boost::filesystem::exists(my_path))
			{
				if (boost::filesystem::is_regular_file(my_path))
				{
					//on essaie de l'ouvrir en lecture / ecriture
					FILE* fic = fopen(my_path.generic_string().c_str(), "r");
					if (fic != 0)
					{
						fclose(fic);
						return true;
					}
				}
			}
			return false;
		}
		
		///
		///
		///
		// Return path when appended to a_From will resolve to same as a_To
		
		boost::filesystem::path make_relative(boost::filesystem::path a_From, boost::filesystem::path a_To )
		{
			a_From = boost::filesystem::absolute( a_From ); a_To = boost::filesystem::absolute( a_To );
			boost::filesystem::path ret;
			boost::filesystem::path::iterator itrFrom(a_From.begin()), itrTo(a_To.begin()), itrEnd(a_To.end());
			// Find common base
			for( boost::filesystem::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo );
			// Navigate backwards in directory to reach previously found base
			for( boost::filesystem::path::iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom )
			{
				if( (*itrFrom) != "." )	ret /= "..";
			}
			// Now navigate down the directory branch
			ret.append(itrTo, itrEnd);
			return ret;
		}
        
        ///
        ///
        ///
        bool copy_file(const boost::filesystem::path& my_path_in,const boost::filesystem::path&  my_path_out, bool overwrite)
        {
            try
            {
				boost::filesystem::path pathOut (my_path_out);
				if (boost::filesystem::is_directory(my_path_out)) pathOut/=my_path_in.filename();
				
                if (overwrite)
                    boost::filesystem::copy_file(my_path_in,pathOut,boost::filesystem::copy_option::overwrite_if_exists);
                else
                    boost::filesystem::copy_file(my_path_in,pathOut,boost::filesystem::copy_option::fail_if_exists);
            }
            catch(...)
            {
                return false;
            }
            return true;
        }
        
        ///
        ///
        ///
        boost::filesystem::path NormalizePath(boost::filesystem::path const& myPath)
        {
            int verbose = 0;
            boost::filesystem::path out;
            for( boost::filesystem::path::iterator it( myPath.begin() ); it != myPath.end(); ++it)
            {
                if (verbose) std::cout<<"it: "<<it->filename().generic_string()<<std::endl;
                if (it->filename().generic_string() == "." ||
                    it->filename().generic_string() == "./" ||
                    it->filename().generic_string() == ".\\" ||
                    it->filename().generic_string() == "")
                    continue;
                if (it->filename().generic_string() == ".." ||
                    it->filename().generic_string() == "../" ||
                    it->filename().generic_string() == "..\\")
                    out = out.parent_path();
                else
                    out/=it->filename();
                
                if (verbose) std::cout<<"out: "<<out.generic_string()<<std::endl;
            }
            return out;
        }
        
        ///
        ///
        ///
        void copy_dir(boost::filesystem::path const & source, boost::filesystem::path const & destination)
        {
            bool verbose (0);
            
            if(!boost::filesystem::exists(source) || !boost::filesystem::is_directory(source))
                IGN_THROW_EXCEPTION("[FileTransfert] La source n'est pas un repertoire valide : "+source.generic_string());
            
            if(boost::filesystem::exists(destination))
            {
                if (verbose)
                    std::cout << "Destination directory " << destination.string() << " already exists." << std::endl;
            }
            
            if(!boost::filesystem::exists(destination))
            {
                boost::filesystem::create_directories(destination);
                 if(!boost::filesystem::exists(destination))
                     std::cout << "Echec a la creation du dossier : "<<destination.string()<<std::endl;
            }
            
            // Iteration du repertoire courant
            for(boost::filesystem::directory_iterator file(source);file != boost::filesystem::directory_iterator(); ++file)
            {
                boost::filesystem::path current(file->path());
                if(boost::filesystem::is_directory(current))
                {
                    // Found directory: Recursion
                    copy_dir( current, destination / current.filename());
                }
                else
                {
                    // Found file: Copy
                    // boost::filesystem::copy_file(current, destination / current.filename(), boost::filesystem::copy_option::overwrite_if_exists);
                    ign::filesystem::copy_file(current, destination / current.filename(), true);
                    
                }
            }
        }
    }
}
