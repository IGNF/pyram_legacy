/*
 *  ImageIO.h
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */
#ifndef __IGN_IMAGE_IMAGETYPE_H__
#define __IGN_IMAGE_IMAGETYPE_H__

#include <utility>
#include <string>

#include <ignException.h>
#include <ignNumericTypes.h>

namespace ign
{
    namespace image
    {
		
		/**@warning: Type de pixel -> redescendu dans ign::numeric*/
		typedef ign::numeric::NumericType		eTypePixel;
		 
		///\brief taille image
		typedef std::pair<size_t, size_t> ImageSize;

        
        typedef enum
        {
            Undefined=0,
            GrayScale,
            RGB,
            CMYK,
            HLS
        } ColorSpace;
        
		///\brief Composition coloree 
		typedef enum 
		{
			ColorSetOther = 0,
			ColorSetRGB,
			ColorSetNIR,
			ColorSetCIR,
			ColorSetRGBI,
			ColorSetPAN,
			ColorSetMonoR,
			ColorSetMonoG,
			ColorSetMonoB
			
		} ColorSet;
		
		///\brief Canal
		typedef enum 
		{
			ChannelOther = 0,
			ChannelR,
			ChannelG,
			ChannelB,
			ChannelI,
			ChannelP
			
		} Channel;

		///\brief CameraRole
		typedef enum 
		{
			CameraRoleOther = 0,
			CameraRoleR,
			CameraRoleG,
			CameraRoleB,
			CameraRoleI,
			CameraRolePAN1,
			CameraRolePAN2,
			CameraRolePAN3,
			CameraRolePAN4
			
		} CameraRole;

		///\brief BayerPattern
		typedef enum
		{
			BayerNone = 0,
			BayerRGGB,
			BayerBGGR, 
			BayerGBRG,
			BayerGRBG, 
			BayerRGBG
		} BayerPattern;

		template
		<class T>
		class TypePixel
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eOther;
			}	
			
		};
		
		template
		<>
		class TypePixel<unsigned char>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eUnsignedChar;
			}	
			
		};
		
		template
		<>
		class TypePixel<signed short>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eSignedShort;
			}	
			
		};
		
		template
		<>
		class TypePixel<unsigned short>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eUnsignedShort;
			}	
			
		};

		template
		<>
		class TypePixel<signed int>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eSignedInt;
			}	
			
		};
		
		template
		<>
		class TypePixel<unsigned int>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eUnsignedInt;
			}	
			
		};
		
		template
		<>
		class TypePixel<float>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eFloat;
			}	
			
		};
		
		template
		<>
		class TypePixel<bool>
		{
		public:
			static eTypePixel GetType()
			{
				return numeric::eBool;
			}	
			
		};
		
		
		std::string			TypeToString(eTypePixel type);
		double				MaxValueTypePixel(eTypePixel typePixel);
		
		size_t				TypeSize(eTypePixel type);
		bool				TypeIsSigned(eTypePixel type);

		std::string			ColorSetToString(const ColorSet cs);
		ColorSet			StringToColorSet(const std::string cs);

		std::string			ChannelToString(const Channel c);
		Channel				StringToChannel(const std::string c);

		std::string			CameraRoleToString(const CameraRole cr);
		CameraRole			StringToCameraRole(const std::string cr);
		
		CameraRole			ChannelToCameraRole(const Channel ch);
		Channel				CameraRoleToChannel(const CameraRole cr);

		std::string			BayerPatternToString(const BayerPattern bp);
	}
}

#endif

