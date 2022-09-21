#include <stdexcept>
#include <sstream>

#include <ignColorTable.h>

namespace ign {
	namespace image{
		
		ColorEntry::ColorEntry():c1(0),c2(0),c3(0),c4(0)
		{
		}
		
		///
		///
		///
		ColorEntry::~ColorEntry()
		{
		}
		
		///
		///
		///
		ColorEntry::ColorEntry(short aC1,short aC2, short aC3, short aC4)
		:c1(aC1),c2(aC2),c3(aC3),c4(aC4)
		{
		}
		
		///
		///
		///
		ColorTable::ColorTable()
		{
		}	
		
		///
		///
		///
		ColorTable::~ColorTable()
		{
		}
		
		///
		///
		///
		size_t ColorTable::numEntries() const
		{
			return _table.size();
		}
		
		///
		///
		///
		std::pair<const int, ColorEntry>& ColorTable::entryN(int n)
		{
			if ((n>(int)_table.size())||(n<0))
			{
				std::ostringstream oss;
				oss << __FILE__ << ":"<<__LINE__;
				throw std::out_of_range(oss.str().c_str());
			}
			std::map<int,ColorEntry>::iterator it = _table.begin();
			for(int i=0;i<n;++i) ++it;
			return (*it);
		}
		
		///
		///
		///
		std::pair<const int, ColorEntry>const& ColorTable::entryN(int n)const
		{
			if ((n>(int)_table.size())||(n<0))
			{
				std::ostringstream oss;
				oss << __FILE__ << ":"<<__LINE__;
				throw std::out_of_range(oss.str().c_str());
			}
			std::map<int,ColorEntry>::const_iterator it = _table.begin();
			for(int i=0;i<n;++i) ++it;
			return (*it);
		}
		
		///
		///
		///
		void ColorTable::getColor(int index, ColorEntry &val)const
		{
			std::map<int,ColorEntry>::const_iterator it = _table.find(index);
			if (it!=_table.end())
			{
				val = (*it).second;
			}
			else
			{
				// Couleur par defaut
				val.c1=0;
				val.c2=0;
				val.c3=0;
				val.c4=0;		
			}
		}
		
		///
		///
		///
		void ColorTable::setColor(int index,ColorEntry const &val)
		{
			_table.insert(std::pair<int,ColorEntry>(index,val));
		}
		
	}
}
