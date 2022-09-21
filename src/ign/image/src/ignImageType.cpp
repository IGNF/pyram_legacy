/*
 *  ImageIO.cpp
 *  ImageIO
 *
 *  Created by greg on 02/02/10.
 *  Copyright 2010 IGN. All rights reserved.
 *
 */
#include <boost/algorithm/string.hpp>

#include <ignImageType.h>

namespace ign {
	namespace image{
		///
		///
		///
		std::string TypeToString(eTypePixel type)
		{
			switch (type) {
				case numeric::eUnsignedChar:
					return std::string("8 bits non signes");
				case numeric::eSignedShort:
					return std::string("16 bits signes");
				case numeric::eUnsignedShort:
					return std::string("16 bits non signes");
				case numeric::eSignedInt:
					return std::string("32 bits signes");
				case numeric::eUnsignedInt:
					return std::string("32 bits non signes");
				case numeric::eFloat:
					return std::string("float");
				case numeric::eBool:
					return std::string("1 bit");
				case numeric::eOther:
					return std::string("autre");
			}
			return std::string("autre");
		}
		
		///
		///
		///
		size_t  TypeSize(eTypePixel typeIO)
		{
			//pour eviter un changement cassant, on garde la fonction ici...
			return ign::numeric::TypeSize(typeIO);
		}
		
		///
		///
		///
		bool  TypeIsSigned(eTypePixel typeIO)
		{
			switch (typeIO) {
				case numeric::eSignedShort:
				case numeric::eSignedInt:
					return true;
				case numeric::eOther:
				case numeric::eUnsignedChar:
				case numeric::eUnsignedShort:
				case numeric::eUnsignedInt:
				case numeric::eFloat:
				case numeric::eBool:
					return false;
			}
			return false;
		}

		std::string			ColorSetToString(const ColorSet cs)
			{
			switch (cs) 
				{
				case ColorSetRGB:
					return std::string ("RGB");
				case ColorSetNIR:
					return std::string ("NIR");
				case ColorSetCIR:
					return std::string ("CIR");
				case ColorSetRGBI:
					return std::string ("RGBI");
				case ColorSetPAN:
					return std::string ("PAN");
				case ColorSetMonoR:
					return std::string ("MonoR");
				case ColorSetMonoG:
					return std::string ("MonoG");
				case ColorSetMonoB:
					return std::string ("MonoB");
				case ColorSetOther:
					return std::string("autre");
				}
			return std::string("autre");
		}

		ColorSet			StringToColorSet(const std::string cs)
			{
			if (boost::iequals(cs,"RGB"))
				return ColorSetRGB;
			if (boost::iequals(cs,"NIR"))
				return ColorSetNIR;
			if (boost::iequals(cs,"CIR"))
				return ColorSetCIR;
			if (boost::iequals(cs,"RGBI"))
				return ColorSetRGBI;
			if (boost::iequals(cs,"PAN"))
				return ColorSetPAN;
			if (boost::iequals(cs,"MonoR"))
				return ColorSetMonoR;
			if (boost::iequals(cs,"MonoG"))
				return ColorSetMonoG;
			if (boost::iequals(cs,"MonoB"))
				return ColorSetMonoB;
			return ColorSetOther;
		}

		///
		/// renvoie le canal en position i (commencant a 0) dans la composition coloree cs
		///
		Channel				ChannelFromColorSet(const ColorSet cs, const int i)
			{
			switch(cs)
				{
				case ColorSetRGB:
					switch(i)
						{
						case 0: return ChannelR;
						case 1 : return ChannelG;
						case 2 : return ChannelB;
						default: return ChannelOther;
						}
					break;
				case ColorSetCIR:
					switch(i)
						{
						case 0: return ChannelI;
						case 1 : return ChannelR;
						case 2 : return ChannelG;
						default: return ChannelOther;
						}
					break;
				case ColorSetNIR:
					if(i==0)return ChannelI;
					else return ChannelOther;
					break;
				case ColorSetMonoR:
					if(i==0)return ChannelR;
					else return ChannelOther;
					break;
				case ColorSetMonoG:
					if(i==0)return ChannelG;
					else return ChannelOther;
					break;
				case ColorSetMonoB:
					if(i==0)return ChannelB;
					else return ChannelOther;
					break;
				case ColorSetPAN:
					if(i==0)return ChannelP;
					else return ChannelOther;
					break;
				case ColorSetRGBI:
					switch(i)
						{
						case 0: return ChannelR;
						case 1 : return ChannelG;
						case 2 : return ChannelB;
						case 3 : return ChannelI;
						default: return ChannelOther;
						}
					break;
				default:
					return ChannelOther;
				}
			return ChannelOther;
			}

		///
		///
		///
		std::string			ChannelToString(const Channel c)
			{
			switch (c) 
				{
				case ChannelR:
					return std::string ("R");
				case ChannelB:
					return std::string ("B");
				case ChannelG:
					return std::string ("G");
				case ChannelI:
					return std::string ("NIR");
				case ChannelP:
					return std::string ("PAN");
				case ChannelOther:
					return std::string("autre");
				}
			return std::string("autre");
		}

		///
		///
		///
		CameraRole			ChannelToCameraRole(const Channel ch)
			{
			switch (ch) 
				{
				case ChannelR:
					return CameraRoleR;
				case ChannelB:
					return CameraRoleB;
				case ChannelG:
					return CameraRoleG;
				case ChannelI:
					return CameraRoleI;
				case ChannelP:
				case ChannelOther:
				default:
					return CameraRoleOther;
				}
			return CameraRoleOther;
			}
		///
		///
		///
		Channel				StringToChannel(const std::string c)
			{
			if (boost::iequals(c,"R"))
				return ChannelR;
			if (boost::iequals(c,"G"))
				return ChannelG;
			if (boost::iequals(c,"B"))
				return ChannelB;
			if (boost::iequals(c,"NIR"))
				return ChannelI;
			if (boost::iequals(c,"PAN"))
				return ChannelP;
			return ChannelOther;
		}

		///
		///
		///
		std::string			CameraRoleToString(const CameraRole cr)
			{
			switch (cr) 
				{
				case CameraRoleR:
					return std::string ("Red");
				case CameraRoleB:
					return std::string ("Blue");
				case CameraRoleG:
					return std::string ("Green");
				case CameraRoleI:
					return std::string ("NIR");
				case CameraRolePAN1:
					return std::string ("PAN1");
				case CameraRolePAN2:
					return std::string ("PAN2");
				case CameraRolePAN3:
					return std::string ("PAN3");
				case CameraRolePAN4:
					return std::string ("PAN4");
				case CameraRoleOther:
					return std::string("autre");
				}
			return std::string("autre");
			}

		///
		///
		///
		Channel				CameraRoleToChannel(const CameraRole cr)
			{
			switch (cr) 
				{
				case CameraRoleR:
					return ChannelR;
				case CameraRoleB:
					return ChannelB;
				case CameraRoleG:
					return ChannelG;
				case CameraRoleI:
					return ChannelI;
				case CameraRolePAN1:
				case CameraRolePAN2:
				case CameraRolePAN3:
				case CameraRolePAN4:
					return ChannelP;
				case CameraRoleOther:
					return ChannelOther;
				}
			return ChannelOther;
			}

		///
		///
		///
		CameraRole			StringToCameraRole(const std::string cr)
			{
			if (boost::iequals(cr,"Red"))
				return CameraRoleR;
			if (boost::iequals(cr,"Green"))
				return CameraRoleG;
			if (boost::iequals(cr,"Blue"))
				return CameraRoleB;
			if (boost::iequals(cr,"NIR"))
				return CameraRoleI;
			if (boost::iequals(cr,"PAN1"))
				return CameraRolePAN1;
			if (boost::iequals(cr,"PAN2"))
				return CameraRolePAN2;
			if (boost::iequals(cr,"PAN3"))
				return CameraRolePAN3;
			if (boost::iequals(cr,"PAN4"))
				return CameraRolePAN4;
			return CameraRoleOther;
			}

		///
		///
		///
		double			MaxValueTypePixel(eTypePixel typePixel)
		{
			switch(typePixel)
			{
				case numeric::eUnsignedChar:
					//return ( (unsigned char) - 1 ) ;
					return std::numeric_limits<unsigned char>::max();
				case numeric::eUnsignedShort:
					//return ( (unsigned short) - 1);
					return std::numeric_limits<unsigned short>::max();
				case numeric::eSignedInt:
					//return ( (signed int) - 1);
					return std::numeric_limits<signed int>::max();
				case numeric::eUnsignedInt:
					//return ( (unsigned int) - 1);
					return std::numeric_limits<unsigned int>::max();
				case numeric::eBool:
					//return ( (bool) - 1);
					return std::numeric_limits<bool>::max();
				case numeric::eSignedShort:
					//return ( (signed short) - 1);	
					return std::numeric_limits<signed short>::max();
				case numeric::eFloat:
					//return ( (float) - 1);	
					return std::numeric_limits<float>::max();
				case numeric::eOther:
					return -1;
			}

			IGN_THROW_EXCEPTION("[MaxValueTypePixel] : type not accepted in the method ");
			
		}

		///
		///
		///
		std::string			BayerPatternToString(const BayerPattern bp)
			{
			switch (bp) 
				{
				case BayerRGGB:
					return std::string ("RGGB");
				case BayerBGGR:
					return std::string ("BGGR");
				case BayerGBRG:
					return std::string ("GBRG");
				case BayerGRBG:
					return std::string ("GRBG");
				case BayerRGBG:
					return std::string ("RGBG");
				case BayerNone:
					return std::string("aucun pattern Bayer");
				default:
					return std::string("motif Bayer non reconnu");
				}
			return std::string("motif Bayer non reconnu");
				}
	}
}





