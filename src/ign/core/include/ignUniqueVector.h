#ifndef _IGN_TOOLS_UNIQUEVECTOR_H_
#define _IGN_TOOLS_UNIQUEVECTOR_H_

#include <vector>
#include <map>
#include <set>
#include <sstream>

#include <ignConfig.h>
#include <ignCommon.h>

namespace ign {
	namespace tools {

		/// \brief vecteur de valeurs assurant l'unite et permettant en acces en RandomAccess
		///
		/// Particulierement utile pour la suppression de doublon
		/// \see std::unique (algorithm)
		template < typename T, typename LessComparator = std::less< T > >
		class UniqueVector {
		private:
			typedef std::vector< T >							vector_t ;
			typedef std::map< T, size_t, LessComparator > 		index_map_t ;
		public:
			typedef typename vector_t::iterator 				iterator;
			typedef typename vector_t::const_iterator 			const_iterator;
			typedef typename vector_t::reverse_iterator 		reverse_iterator;
			typedef typename vector_t::const_reverse_iterator 	const_reverse_iterator;

			/// \brief default
			UniqueVector():
				_data(),
				_indexData()
			{

			}

			/// \brief copy constructor
			UniqueVector( UniqueVector const& other ):
				_data(other._data),
				_indexData(other._indexData)
			{

			}

			/// \brief assign
			UniqueVector & operator = ( UniqueVector const& other )
			{
				_data 			= other._data;
				_indexData		= other._indexData;
				return *this;
			}

			/// \brief recherche un element dans le vecteur
			/// \return -1 si non trouve, le rang sinon
			int					find( T const& item ) const
			{
				typename index_map_t::const_iterator it = _indexData.find( item );
				return (it != _indexData.end()) ? static_cast<int>(it->second) : -1;
			}


			/// \brief ajout d'un element sans doublon et renvoie de son rang
			size_t 				addItem( T const & item )
			{
				typename index_map_t::const_iterator it = _indexData.find( item );
				if ( it  != _indexData.end() ){
					return it->second;
				}else{
					size_t id = _data.size();
					_data.push_back( item );
					_indexData.insert( std::make_pair( item, id ) );
					return id;
				}
			}



			/// \brief renvoie le i-ieme element
			T const& 			getItem( size_t const & i ) const
			{
				IGN_SAFE_MODE_ASSERT( i < size() );
				return _data[i];
			}

			/// \brief update item and check that unique constraint is respected
			/// \warning convenience method, avoid to use it (slow)
			/// \throw ign::Exception if newValue already exists for a different index
			void 				setItem( size_t const & i, T const& newValue )
			{
				IGN_SAFE_MODE_ASSERT( i < size() );

				int posNewValue = find(newValue) ;
				if ( posNewValue != -1 && posNewValue != (int)i ){
					std::ostringstream oss;
					oss << "UniqueVector::setItem(" << i << ", newValue )" << " violates unique constraints" ;
					IGN_THROW_EXCEPTION( oss.str() );
				}
				_indexData.erase( _data[i] );
				_data[i] = newValue;
				_indexData.insert( std::make_pair( newValue, i ) );
			}

			/// \brief renvoie la taille
			size_t				size() const
			{
				return _data.size();
			}
			/// \brief true si vecteur vide
			bool				empty() const
			{
				return _data.empty();
			}

			/// \brief vide le vecteur
			void				clear()
			{
				_data.clear();
				_indexData.clear();
			}


			/// \brief renvoie le vecteur de donnees
			vector_t const&		data() const
			{
				return _data;
			}

			/**
			 * returns n-th item
			 */
			inline T const& operator [] ( size_t const& i ) const
			{
				IGN_SAFE_MODE_ASSERT( i < size() );
				return _data[i];
			}

		private:
			/// \brief le vecteur de donnees
			vector_t 										_data;
			/// \brief la map assurant l'unicite
			index_map_t										_indexData;
		};

	}
}




#endif
