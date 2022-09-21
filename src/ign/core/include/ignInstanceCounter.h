#ifndef _IGN_INSTANCE_COUNTER_H_
#define _IGN_INSTANCE_COUNTER_H_

#include <ignConfig.h>

//#include <typeinfo>
//#include <sstream>
//#include <string>

namespace ign {
	/// \brief compteur d'instance reposant sur le CRTP (inspire de faq C++ developpez)
	template < class T >
	class InstanceCounter {
	public:
		/// \brief constructeur par defaut
		InstanceCounter()
		{
			_num_created++;
			_num_alive++;
		}

		virtual ~InstanceCounter()
		{
			--_num_alive;
		}
		
		/// \brief renvoie le nombre d'objet en vie
		static int const& NumCreated()
		{
			return _num_created;
		}
		
		/// \brief renvoie le nombre d'objet cree
		static int const& NumAlive()
		{
			return _num_alive;
		}

	private:
		static int _num_created;
		static int _num_alive;
	};

	template <typename T> int InstanceCounter<T>::_num_created( 0 );
	template <typename T> int InstanceCounter<T>::_num_alive( 0 );
}


#endif

