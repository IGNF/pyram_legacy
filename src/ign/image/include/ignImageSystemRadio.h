#ifndef __SDI_RADIOMETRY_RADIOSYSTEM_H__
#define __SDI_RADIOMETRY_RADIOSYSTEM_H__

#include <ignTransformSystem.h>

namespace ign
{
	namespace image
	{
		namespace radiometry
		{
			class SystemRadio;
			typedef ign::shared_ptr<SystemRadio> SystemRadioPtr;

			///\brief represente un systeme pour une transformation radiometrique
			class SystemRadio : public ign::transform::System
			{
				//					static const std::string	_kSystemRadioType;
			protected:
				///\brief code de l'espace colorimetrique
				std::string					_colorSpace;
				///\brief type de pixels
				std::string					_typePixel;
				///\brief sous-code
				std::string					_subCode;
				
				
			public:
				/// \brief constructeur utilise dans CreateSystemRadio. preferez l'utilisation de CreateSystemRadio
				SystemRadio	(	const std::string& uuid, numeric::unit::SysUnitPtr const& unites);
				
				///\brief destructeur
				virtual						~SystemRadio(){;}
				
				///\brief Lecture du code d'espace colorimetrique
				std::string					getColorSpace() const;
				///\brief Modifie le code de l'espace colorimetrique
				///@param colorSpace nouveau code de l'espace colorimetrique du systeme.
				void						setColorSpace (const std::string& colorSpace);
				///\brief Lecture du type de pixels
				std::string					getTypePixel() const;
				///\brief Modifie le code de l'espace colorimetrique
				///@param colorSpace nouveau code de l'espace colorimetrique du systeme.
				void						setTypePixel(const std::string& typePixel);
				///\brief Lecture du sous-code
				std::string					getSubCode() const;
				///\brief Modifie le sous-code
				///@param colorSpace nouveau sous-code du systeme
				void						setSubCode(const std::string& subCode);
				///\brief renvoie une chaine unique correspondant au systeme
				std::string					toString() const ;
				
				/// \brief static method to use in order to create a SystemRadio
				static SystemRadioPtr  CreateSystemRadio(const std::string& uuid,
														const std::string& colorSpace,
														const std::string& typePixel,
														const std::string& subCode);
				
				
			private:
				
				
			};
		}
	}
}
#endif